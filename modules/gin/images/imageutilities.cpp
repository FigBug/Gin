/*==============================================================================

 Copyright 2019 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

juce::Image rasterizeSVG (juce::String svgText, int w, int h)
{
    Image img (Image::ARGB, w, h, true);
    Graphics g (img);

    if (auto svg = XmlDocument::parse (svgText))
    {
        std::unique_ptr<Drawable> drawable;

        {
            const MessageManagerLock mmLock;
            drawable = Drawable::createFromSVG (*svg);
            drawable->setTransformToFit (Rectangle<float> (0.0f, 0.0f, float (w), float (h)), RectanglePlacement::centred);
            drawable->draw (g, 1.f);
        }

    }

    return img;
}
