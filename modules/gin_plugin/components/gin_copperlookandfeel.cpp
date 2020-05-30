//==============================================================================
CopperLookAndFeel::CopperLookAndFeel()
{
    typeface = Typeface::createSystemTypefaceFor (Resources::InterRegular_otf, Resources::InterRegular_otfSize);

	setColour (whiteColourId, Colour (0xffFFFFFF));
	setColour (blackColourId, Colour (0xff000000));
	setColour (displayColourId, Colour (0xff0D0E0F));
	setColour (backgroundColourId, Colour (0xff16171A));
	setColour (groupColourId, Colour (0xff28292E));
	setColour (lightGroupColourId, Colour (0xff35373D));
	setColour (grey30ColourId, Colour (0xff585A62));
	setColour (grey45ColourId, Colour (0xff797C84));
	setColour (grey60ColourId, Colour (0xff9B9EA5));
	setColour (grey90ColourId, Colour (0xffE6E6E9));
	setColour (glass1ColourId, Colour (0xff0f1012));
	setColour (glass2ColourId, Colour (0xff0F1012));
	setColour (matte1ColourId, Colour (0xff18191C));
	setColour (matte2ColourId, Colour (0xff131417));
	setColour (title1ColourId, Colour (0xff2A2C30));
	setColour (title2ColourId, Colour (0xff25272B));
	setColour (accentColourId, Colour (0xffCC8866));
}

Typeface::Ptr CopperLookAndFeel::getTypefaceForFont (const Font& font)
{
    if (font.getTypefaceName() == Font::getDefaultSansSerifFontName())
        return typeface;
    
    return LookAndFeel_V4::getTypefaceForFont (font);
}

//==============================================================================
CopperLookAndFeelWrapper::CopperLookAndFeelWrapper()
{
    LookAndFeel::setDefaultLookAndFeel (this);
}

CopperLookAndFeelWrapper::~CopperLookAndFeelWrapper()
{
    LookAndFeel::setDefaultLookAndFeel (nullptr);
}
