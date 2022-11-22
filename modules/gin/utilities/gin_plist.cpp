/*==============================================================================

 Copyright 2019 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

static juce::var parseData (const juce::XmlElement& e)
{
    if (e.hasTagName ("string"))
    {
        return e.getAllSubText();
    }
    else if (e.hasTagName ("array"))
    {
        juce::Array<juce::var> res;

        auto c = e.getFirstChildElement();
        while (c != nullptr)
        {
            res.add (parseData (*c));
            c = c->getNextElement();
        }

        return res;
    }
    else if (e.hasTagName ("dict"))
    {
        auto obj = new juce::DynamicObject();
        for (int i = 0; i < e.getNumChildElements(); i += 2)
        {
            auto key = e.getChildElement (i + 0);
            auto val = e.getChildElement (i + 1);

            if (key != nullptr && val != nullptr)
                obj->setProperty (key->getAllSubText(), parseData (*val));
        }

        return juce::var (obj);
    }
    else if (e.hasTagName ("data"))
    {
        juce::MemoryBlock mb;

        {
            auto text = e.getAllSubText();
            text = text.removeCharacters ("\t\n\r ");

            juce::MemoryOutputStream os (mb, true);
            juce::Base64::convertFromBase64 (os, text);
        }

        return mb;
    }
    else if (e.hasTagName ("date"))
    {
        return e.getAllSubText();
    }
    else if (e.hasTagName ("real"))
    {
        return e.getAllSubText().getDoubleValue();
    }
    else if (e.hasTagName ("integer"))
    {
        return e.getAllSubText().getIntValue();
    }
    else if (e.hasTagName ("true"))
    {
        return true;
    }
    else if (e.hasTagName ("false"))
    {
        return false;
    }

    jassertfalse;
    return {};
}

juce::var parsePlist (const juce::File& f)
{
    juce::XmlDocument doc (f);
    if (auto e = doc.getDocumentElement())
        return parsePlist (*e);
    return {};
}

juce::var parsePlist (const juce::String& s)
{
    juce::XmlDocument doc (s);
    if (auto e = doc.getDocumentElement())
        return parsePlist (*e);
    return {};
}

juce::var parsePlist (const juce::XmlElement& e)
{
    if (auto dict = e.getChildByName ("dict"))
    {
        auto obj = new juce::DynamicObject();
        for (int i = 0; i < dict->getNumChildElements(); i += 2)
        {
            auto key = dict->getChildElement (i + 0);
            auto val = dict->getChildElement (i + 1);

            if (key != nullptr && val != nullptr)
                obj->setProperty (key->getAllSubText(), parseData (*val));
        }

        return juce::var (obj);
    }
    return {};
}
