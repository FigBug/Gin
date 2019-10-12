/*==============================================================================

 Copyright 2019 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

var parseData (const XmlElement& e)
{
    if (e.hasTagName ("string"))
    {
        return e.getAllSubText();
    }
    else if (e.hasTagName ("array"))
    {
        Array<var> res;
        
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
        auto obj = new DynamicObject();
        for (int i = 0; i < e.getNumChildElements(); i += 2)
        {
            auto key = e.getChildElement (i + 0);
            auto val = e.getChildElement (i + 1);

            if (key != nullptr && val != nullptr)
                obj->setProperty (key->getAllSubText(), parseData (*val));
        }

        return var (obj);
    }
    else if (e.hasTagName ("data"))
    {
        MemoryBlock mb;
        
        {
            MemoryOutputStream os (mb, true);
            Base64::convertFromBase64 (os, e.getAllSubText());
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

var parsePlist (const File& f)
{
    XmlDocument doc (f);
    if (auto e = doc.getDocumentElement())
        return parsePlist (*e);
    return {};
}

var parsePlist (const String& s)
{
    XmlDocument doc (s);
    if (auto e = doc.getDocumentElement())
        return parsePlist (*e);
    return {};
}

var parsePlist (const XmlElement& e)
{
    if (auto dict = e.getChildByName ("dict"))
    {
        auto obj = new DynamicObject();
        for (int i = 0; i < dict->getNumChildElements(); i += 2)
        {
            auto key = dict->getChildElement (i + 0);
            auto val = dict->getChildElement (i + 1);
         
            if (key != nullptr && val != nullptr)
                obj->setProperty (key->getAllSubText(), parseData (*val));
        }
        
        return var (obj);
    }
    return {};
}
