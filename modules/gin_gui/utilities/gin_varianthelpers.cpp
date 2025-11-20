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

juce::String removeJsonComments (const juce::String& input)
{
    const auto text = input.toStdString();
    std::string out;
    out.reserve(text.size());

    enum class State {
        Normal,
        InString,
        InLineComment,
        InBlockComment
    } state = State::Normal;

    bool escape = false;

    for (size_t i = 0; i < text.size(); ++i)
    {
        char c = text[i];
        char next = (i + 1 < text.size() ? text[i + 1] : '\0');

        switch (state)
        {
            case State::Normal:
                if (c == '"' && !escape)
                {
                    state = State::InString;
                    out += c;
                }
                else if (c == '/' && next == '/')  // start line comment
                {
                    state = State::InLineComment;
                    ++i; // skip next
                }
                else if (c == '/' && next == '*') // start block comment
                {
                    state = State::InBlockComment;
                    ++i; // skip next
                }
                else
                {
                    out += c;
                }
                break;

            case State::InString:
                out += c;
                if (!escape && c == '"')
                    state = State::Normal;

                escape = (!escape && c == '\\'); // detect \"
                break;

            case State::InLineComment:
                if (c == '\n')
                {
                    out += c; // keep the newline
                    state = State::Normal;
                }
                break;

            case State::InBlockComment:
                if (c == '*' && next == '/')
                {
                    state = State::Normal;
                    ++i; // skip '/'
                }
                break;
        }
    }

    return juce::String(out);
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

