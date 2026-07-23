namespace Diff
{

//==============================================================================
namespace
{
    juce::StringArray splitLines (const juce::String& text)
    {
        juce::StringArray lines;
        int start = 0;
        int pos = 0;
        auto chars = text.toRawUTF8();
        int len = text.length();

        while (pos < len)
        {
            if (chars[pos] == '\n')
            {
                lines.add (text.substring (start, pos + 1));
                start = pos + 1;
            }
            pos++;
        }

        if (start < len)
            lines.add (text.substring (start));

        return lines;
    }

    struct Edit
    {
        enum class Type { equal, insert, remove };
        Type type;
        int oldLineNum;
        int newLineNum;
        juce::String text;
    };

    // Myers O(ND) diff: greedy search for the shortest edit script, keeping a
    // trace of the furthest-reaching x on each diagonal per step, then
    // backtracking through the trace to recover the edits
    std::vector<Edit> computeDiff (const juce::StringArray& oldLines, const juce::StringArray& newLines)
    {
        const int n = oldLines.size();
        const int m = newLines.size();
        const int maxD = n + m;

        if (maxD == 0)
            return {};

        // v[k + offset] = furthest x reached on diagonal k (where k = x - y)
        const int offset = maxD;
        std::vector<int> v ((size_t) (2 * maxD + 1), 0);
        std::vector<std::vector<int>> trace;

        int foundD = -1;

        for (int d = 0; d <= maxD && foundD < 0; d++)
        {
            trace.push_back (v);

            for (int k = -d; k <= d; k += 2)
            {
                int x;
                if (k == -d || (k != d && v[(size_t) (k - 1 + offset)] < v[(size_t) (k + 1 + offset)]))
                    x = v[(size_t) (k + 1 + offset)];
                else
                    x = v[(size_t) (k - 1 + offset)] + 1;

                int y = x - k;

                while (x < n && y < m && oldLines[x] == newLines[y])
                {
                    x++;
                    y++;
                }

                v[(size_t) (k + offset)] = x;

                if (x >= n && y >= m)
                {
                    foundD = d;
                    break;
                }
            }
        }

        jassert (foundD >= 0);

        // Backtrack from (n, m) to (0, 0), emitting edits in reverse
        std::vector<Edit> edits;
        int x = n, y = m;

        for (int d = foundD; d >= 0; d--)
        {
            const auto& vd = trace[(size_t) d];
            const int k = x - y;

            int prevK;
            if (k == -d || (k != d && vd[(size_t) (k - 1 + offset)] < vd[(size_t) (k + 1 + offset)]))
                prevK = k + 1;
            else
                prevK = k - 1;

            const int prevX = vd[(size_t) (prevK + offset)];
            const int prevY = prevX - prevK;

            while (x > prevX && y > prevY)
            {
                edits.push_back ({ Edit::Type::equal, x - 1, y - 1, oldLines[x - 1] });
                x--;
                y--;
            }

            if (d > 0)
            {
                if (x == prevX)
                    edits.push_back ({ Edit::Type::insert, -1, prevY, newLines[prevY] });
                else
                    edits.push_back ({ Edit::Type::remove, prevX, -1, oldLines[prevX] });

                x = prevX;
                y = prevY;
            }
        }

        std::reverse (edits.begin(), edits.end());
        return edits;
    }
}

//==============================================================================
std::vector<uint8_t> bsDiff (const juce::String& s1, const juce::String& s2)
{
    std::vector<uint8_t> result;

    uint32_t sz = uint32_t (s2.getNumBytesAsUTF8());
    result.insert (result.end(), (uint8_t*)&sz, (uint8_t*)&sz + sizeof(uint32_t));

    bsdiff_stream stream;

    stream.opaque = &result;
    stream.malloc = malloc;
    stream.free   = free;
    stream.write  = [] (struct bsdiff_stream* param, const void* buffer, int size)
    {
        auto& buf = *((std::vector<uint8_t>*)param->opaque);

        buf.insert (buf.end(), (uint8_t*)buffer, (uint8_t*)buffer + size);
        return 0;
    };

    [[maybe_unused]] auto err = bsdiff ((const uint8_t*)s1.toRawUTF8(), int64_t (s1.getNumBytesAsUTF8()), (const uint8_t*)s2.toRawUTF8(), int64_t (s2.getNumBytesAsUTF8()), &stream);
    jassert (err == 0);

    return result;
}

juce::String bsApplyPatch (const juce::String& s, const std::vector<uint8_t>& patch)
{
    uint32_t sz;
    memcpy (&sz, patch.data(), sizeof (sz));

    juce::MemoryBlock mb (sz + 1, true);

    juce::MemoryInputStream input (patch.data() + sizeof (uint32_t), patch.size() - 4, false);

    bspatch_stream stream;
    stream.opaque = &input;
    stream.read   = [] (const struct bspatch_stream* param, void* buffer, int length)
    {
        auto& is = *(juce::MemoryInputStream*)param->opaque;

        if (is.read (buffer, length) == length)
            return 0;

        return -1;
    };

    [[maybe_unused]] auto err = bspatch ((const uint8_t*)s.toRawUTF8(), int64_t (s.getNumBytesAsUTF8()), (uint8_t*)mb.getData(), sz, &stream);
    jassert (err == 0);

    return juce::String::fromUTF8 ((const char*)mb.getData());
}

//==============================================================================
juce::String Patch::toString() const
{
    juce::String result;

    for (const auto& hunk : hunks)
    {
        if (hunk.isInsert)
            result += "+" + juce::String (hunk.newLine) + ":" + hunk.text;
        else
            result += "-" + juce::String (hunk.oldLine) + ":" + hunk.text;

        if (! hunk.text.endsWithChar ('\n'))
            result += "\n";
    }

    return result;
}

Patch Patch::fromString (const juce::String& s)
{
    Patch patch;
    auto lines = splitLines (s);

    for (const auto& line : lines)
    {
        if (line.isEmpty())
            continue;

        bool isInsert = line[0] == '+';
        bool isRemove = line[0] == '-';

        if (! isInsert && ! isRemove)
            continue;

        int colonPos = line.indexOfChar (':');
        if (colonPos < 0)
            continue;

        int lineNum = line.substring (1, colonPos).getIntValue();
        juce::String text = line.substring (colonPos + 1);

        Patch::Hunk hunk;
        hunk.isInsert = isInsert;
        hunk.oldLine = isInsert ? -1 : lineNum;
        hunk.newLine = isInsert ? lineNum : -1;
        hunk.text = text;

        patch.hunks.push_back (hunk);
    }

    return patch;
}

Patch createPatch (const juce::String& oldText, const juce::String& newText)
{
    auto oldLines = splitLines (oldText);
    auto newLines = splitLines (newText);

    auto edits = computeDiff (oldLines, newLines);

    // Convert to compact patch - only store inserts and removes
    Patch patch;
    for (const auto& edit : edits)
    {
        if (edit.type == Edit::Type::insert)
            patch.hunks.push_back ({ -1, edit.newLineNum, true, edit.text });
        else if (edit.type == Edit::Type::remove)
            patch.hunks.push_back ({ edit.oldLineNum, -1, false, edit.text });
    }

    return patch;
}

juce::String applyPatch (const juce::String& newText, const Patch& patch)
{
    auto newLines = splitLines (newText);

    // Build maps for quick lookup
    std::set<int> insertedNewLines;
    std::map<int, juce::String> removedOldLines;

    for (const auto& hunk : patch.hunks)
    {
        if (hunk.isInsert)
            insertedNewLines.insert (hunk.newLine);
        else
            removedOldLines[hunk.oldLine] = hunk.text;
    }

    // Reconstruct old text by:
    // - Walking through old line numbers in order
    // - For each old line: either it was removed (get from patch) or equal (get from new)
    // - Skip new lines that were inserted

    juce::String result;
    int oldLine = 0;
    int newLine = 0;

    // Find max old line number to know when we're done
    int maxOldLine = -1;
    for (const auto& hunk : patch.hunks)
        if (! hunk.isInsert)
            maxOldLine = std::max (maxOldLine, hunk.oldLine);

    // Also need to account for equal lines after all removes
    int totalOldLines = maxOldLine + 1;
    int removedCount = (int) removedOldLines.size();
    int insertedCount = (int) insertedNewLines.size();
    int equalCount = newLines.size() - insertedCount;
    totalOldLines = std::max (totalOldLines, removedCount + equalCount);

    while (oldLine < totalOldLines)
    {
        if (removedOldLines.count (oldLine))
        {
            // This line was removed - add it back from patch
            result += removedOldLines[oldLine];
            oldLine++;
        }
        else
        {
            // This line was equal - find next non-inserted new line
            while (newLine < newLines.size() && insertedNewLines.count (newLine))
                newLine++;

            if (newLine < newLines.size())
            {
                result += newLines[newLine];
                newLine++;
            }
            oldLine++;
        }
    }

    return result;
}

}
