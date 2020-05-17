/*==============================================================================

 Copyright 2019 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

juce::Image rasterizeSVG (juce::String svgText, int w, int h)
{
    Image img (Image::ARGB, w, h, true);

    if (auto svg = XmlDocument::parse (svgText))
    {
        const MessageManagerLock mmLock;

        auto drawable = Drawable::createFromSVG (*svg);

        Graphics g (img);
        drawable->drawWithin (g, Rectangle<float>(float (w), float (h)), 0, 1.0f);
    }

    return img;
}

juce::Path parseSVGPath ( const String& text )
{
    auto pathFromPoints = [] (String pointsText) -> Path
    {
        auto points = StringArray::fromTokens (pointsText, " ,", "");
        points.removeEmptyStrings();

        jassert (points.size() % 2 == 0);

        Path p;

        for (int i = 0; i < points.size() / 2; i++)
        {
            auto x = points[i * 2].getFloatValue();
            auto y = points[i * 2 + 1].getFloatValue();

            if (i == 0)
                p.startNewSubPath ({ x, y });
            else
                p.lineTo ({ x, y });
        }

        p.closeSubPath();

        return p;
    };

    auto path = Drawable::parseSVGPath (text);

    if (path.isEmpty())
        path = pathFromPoints (text);

    return path;
}
