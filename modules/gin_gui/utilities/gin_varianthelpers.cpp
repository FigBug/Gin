bool setJSONPointer (juce::var& v, juce::String pointer, const juce::var& newValue)
{
    if (pointer.isEmpty())
        return false;

    if (! pointer.startsWith ("/"))
    {
        // This is not a well-formed JSON pointer
        jassertfalse;
        return {};
    }

    const auto findResult = pointer.indexOfChar (1, '/');
    const auto pos = findResult < 0 ? pointer.length() : findResult;
    const juce::String head (pointer.begin() + 1, pointer.begin() + pos);
    const juce::String tail (pointer.begin() + pos, pointer.end());

    const auto unescaped = head.replace ("~1", "/").replace ("~0", "~");

    if (auto* object = v.getDynamicObject())
    {
        if (tail.isEmpty())
        {
            object->setProperty (unescaped, newValue);
            return true;
        }

        auto p = object->getProperty (unescaped);
        if (p.isVoid())
        {
            object->setProperty (unescaped, juce::var (new juce::DynamicObject()));
            p = object->getProperty (unescaped);
        }

        return setJSONPointer (p, tail, newValue);
    }
    else if (auto* array = v.getArray())
    {
        const auto index = [&]() -> size_t
        {
            if (unescaped == "-")
                return (size_t) array->size();

            if (unescaped == "0")
                return 0;

            if (! unescaped.startsWith ("0"))
                return (size_t) unescaped.getLargeIntValue();

            return std::numeric_limits<size_t>::max();
        }();

        if (tail.isEmpty())
        {
            if (juce::isPositiveAndBelow (index, array->size()))
            {
                array->set (int (index), newValue);
                return true;
            }

            if (int (index) == array->size())
            {
                array->add (newValue);
                return true;
            }
        }

        auto a = (*array)[(int) index];
        return setJSONPointer (a, tail, newValue);
    }

    return false;
}

juce::var getJSONPointer (const juce::var& v, juce::String pointer, const juce::var& defaultValue)
{
    if (pointer.isEmpty())
        return defaultValue;

    if (! pointer.startsWith ("/"))
    {
        // This is not a well-formed JSON pointer
        jassertfalse;
        return {};
    }

    const auto findResult = pointer.indexOfChar (1, '/');
    const auto pos = findResult < 0 ? pointer.length() : findResult;
    const juce::String head (pointer.begin() + 1, pointer.begin() + pos);
    const juce::String tail (pointer.begin() + pos, pointer.end());

    const auto unescaped = head.replace ("~1", "/").replace ("~0", "~");

    if (auto* object = v.getDynamicObject())
    {
        if (tail.isEmpty())
            return object->hasProperty (unescaped) ? object->getProperty (unescaped) : defaultValue;
        else
            return getJSONPointer (object->getProperty (unescaped), tail, defaultValue);
    }
    else if (auto* array = v.getArray())
    {
        const auto index = [&]() -> size_t
        {
            if (unescaped == "-")
                return (size_t) array->size();

            if (unescaped == "0")
                return 0;

            if (! unescaped.startsWith ("0"))
                return (size_t) unescaped.getLargeIntValue();

            return std::numeric_limits<size_t>::max();
        }();

        if (tail.isEmpty())
            return juce::isPositiveAndBelow (index, array->size()) ? (*array)[int (index)] : defaultValue;
        else
            return getJSONPointer ((*array)[(int) index], tail, defaultValue);
    }

    return defaultValue;
}

bool hasJSONPointer (const juce::var& v, juce::String pointer)
{
    if (pointer.isEmpty())
        return false;

    if (! pointer.startsWith ("/"))
    {
        // This is not a well-formed JSON pointer
        jassertfalse;
        return false;
    }

    const auto findResult = pointer.indexOfChar (1, '/');
    const auto pos = findResult < 0 ? pointer.length() : findResult;
    const juce::String head (pointer.begin() + 1, pointer.begin() + pos);
    const juce::String tail (pointer.begin() + pos, pointer.end());

    const auto unescaped = head.replace ("~1", "/").replace ("~0", "~");

    if (auto* object = v.getDynamicObject())
    {
        if (tail.isEmpty())
            return object->hasProperty (unescaped);
        else
            return hasJSONPointer (object->getProperty (unescaped), tail);
    }
    else if (auto* array = v.getArray())
    {
        const auto index = [&]() -> size_t
        {
            if (unescaped == "-")
                return (size_t) array->size();

            if (unescaped == "0")
                return 0;

            if (! unescaped.startsWith ("0"))
                return (size_t) unescaped.getLargeIntValue();

            return std::numeric_limits<size_t>::max();
        }();

        if (tail.isEmpty())
            return juce::isPositiveAndBelow (index, array->size());
        else
            return hasJSONPointer ((*array)[(int) index], tail);
    }

    return false;
}

juce::String removeJsonComments (const juce::String& input)
{
    const auto text = input.toStdString();
    std::string out;
    out.reserve (text.size());  // Pre-allocate to avoid reallocations

    // State machine to track our position in the JSON text
    enum class State
    {
        Normal,          // Normal JSON content
        InString,        // Inside a quoted string literal
        InLineComment,   // Inside a // line comment
        InBlockComment   // Inside a /* block comment */
    } state = State::Normal;

    bool escape = false;  // Track if we're on an escape character in a string

    for (size_t i = 0; i < text.size(); ++i)
    {
        char c = text[i];
        char next = (i + 1 < text.size() ? text[i + 1] : '\0');

        switch (state)
        {
            case State::Normal:
                // We're in normal JSON content (not in a string or comment)
                if (c == '"' && ! escape)
                {
                    // Start of a string literal - preserve everything inside
                    state = State::InString;
                    out += c;
                }
                else if (c == '/' && next == '/')
                {
                    // Start of line comment - skip it
                    state = State::InLineComment;
                    ++i;  // Skip the second '/'
                }
                else if (c == '/' && next == '*')
                {
                    // Start of block comment - skip it
                    state = State::InBlockComment;
                    ++i;  // Skip the '*'
                }
                else
                {
                    // Normal JSON content - keep it
                    out += c;
                }
                break;

            case State::InString:
                // We're inside a string literal - preserve everything, including comment-like text
                out += c;

                // Check for end of string (unescaped quote)
                if (! escape && c == '"')
                    state = State::Normal;

                // Track escape sequences (\" should not end the string)
                escape = (! escape && c == '\\');
                break;

            case State::InLineComment:
                // We're in a line comment - skip everything until newline
                if (c == '\n')
                {
                    out += c;  // Preserve the newline for formatting
                    state = State::Normal;
                }
                // All other characters in line comments are skipped
                break;

            case State::InBlockComment:
                // We're in a block comment - skip everything until */
                if (c == '*' && next == '/')
                {
                    state = State::Normal;
                    ++i;  // Skip the '/'
                }
                // All characters in block comments are skipped (including newlines)
                break;
        }
    }

    return juce::String (out);
}


}  // namespace gin

//==============================================================================
namespace juce
{

VarIterator::VarIterator (const juce::var& v_, bool isEnd)
    : v (v_)
{
    if (isEnd)
    {
        if (v.isArray())
            index = v.getArray()->size();
        else if (auto o = v.getDynamicObject())
            itr = o->getProperties().end();
    }
    else
    {
        if (v.isArray())
            index = 0;
        else if (auto o = v.getDynamicObject())
            itr = o->getProperties().begin();
    }
}

VarIterator& VarIterator::operator++()
{
    if (v.isArray())
    {
        index++;
    }
    else if (v.getDynamicObject())
    {
        auto i = (const juce::NamedValueSet::NamedValue*)itr;
        i++;
        itr = i;
    }
    return *this;
}

bool VarIterator::operator== (const VarIterator& other) const
{
    return index == other.index && itr == other.itr;
}

bool VarIterator::operator!= (const VarIterator& other) const
{
    return ! (*this == other);
}

VarIterator::NamedValue VarIterator::operator*() const
{
    if (v.isArray())
    {
        return { index, (*v.getArray())[index] };
    }
    else if (v.getDynamicObject())
    {
        auto i = (const juce::NamedValueSet::NamedValue*)itr;
        return { i->name.toString(), i->value };
    }
    return {};
}

VarIterator begin (const juce::var& v)
{
    return VarIterator (v, false);
}

VarIterator end (const juce::var& v)
{
    return VarIterator (v, true);
}

}  // namespace juce

namespace gin
{

