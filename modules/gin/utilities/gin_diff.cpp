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

    // Simple diff using dynamic programming to find LCS, then derive edits
    std::vector<Edit> computeDiff (const juce::StringArray& oldLines, const juce::StringArray& newLines)
    {
        int n = oldLines.size();
        int m = newLines.size();

        // Build LCS table
        std::vector<std::vector<int>> dp ((size_t) (n + 1), std::vector<int> ((size_t) (m + 1), 0));

        for (int i = 1; i <= n; i++)
        {
            for (int j = 1; j <= m; j++)
            {
                if (oldLines[i - 1] == newLines[j - 1])
                    dp[(size_t) i][(size_t) j] = dp[(size_t) (i - 1)][(size_t) (j - 1)] + 1;
                else
                    dp[(size_t) i][(size_t) j] = std::max (dp[(size_t) (i - 1)][(size_t) j], dp[(size_t) i][(size_t) (j - 1)]);
            }
        }

        // Backtrack to find edits
        std::vector<Edit> edits;
        int i = n, j = m;

        while (i > 0 || j > 0)
        {
            if (i > 0 && j > 0 && oldLines[i - 1] == newLines[j - 1])
            {
                edits.push_back ({ Edit::Type::equal, i - 1, j - 1, oldLines[i - 1] });
                i--;
                j--;
            }
            else if (j > 0 && (i == 0 || dp[(size_t) i][(size_t) (j - 1)] >= dp[(size_t) (i - 1)][(size_t) j]))
            {
                edits.push_back ({ Edit::Type::insert, -1, j - 1, newLines[j - 1] });
                j--;
            }
            else
            {
                edits.push_back ({ Edit::Type::remove, i - 1, -1, oldLines[i - 1] });
                i--;
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
