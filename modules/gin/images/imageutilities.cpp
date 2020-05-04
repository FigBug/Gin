/*==============================================================================

 Copyright 2019 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

juce::Image rasterizeSVG ( juce::String svgText, int w, int h )
{
	Image       img ( Image::ARGB, w, h, true );

	if ( auto svg = XmlDocument::parse ( svgText ) )
	{
		const MessageManagerLock	mmLock;

		auto	drawable = Drawable::createFromSVG ( *svg );

		Graphics    g ( img );
		drawable->drawWithin ( g, Rectangle<float> ( float ( w ), float ( h ) ), 0, 1.f );
	}

	return img;
}
