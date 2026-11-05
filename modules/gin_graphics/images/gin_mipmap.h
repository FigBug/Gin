/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/** Post-processing of a MipMap's reduced levels, off by default. 1.0 is the strength
    tuned for small pixel-art thumbnails, higher values are allowed. Lives outside the
    class so it is complete when MipMap's default arguments need it.
*/
struct MipMapEnhance
{
    /** Sharpens the levels of 128 px and below, which the box filter softens */
    float sharpen = 0.0f;

    /** Lifts the saturation of every level from the second halving on, against the grey
        that averaging dithered colours gives
    */
    float saturation = 0.0f;

    // an exact comparison of the float members is what is wanted here
    bool operator== (const MipMapEnhance& other) const
    {
        return juce::exactlyEqual (sharpen, other.sharpen)
            && juce::exactlyEqual (saturation, other.saturation);
    }

    bool operator!= (const MipMapEnhance& other) const  { return ! operator== (other); }
};

//==============================================================================
/** An image with its halved levels down to 16 px, each halving a rounded 2x2 box
    average, so drawing it smaller picks the nearest larger level and stays sharp
    without aliasing.

    draw() picks the level for the physical pixel size (HiDPI included) and draws it
    with low resampling quality when the level matches the size exactly, high otherwise.

    Copies are cheap: the levels are juce::Image objects, which share their pixels.

    @code
    gin::MipMap cover { juce::ImageFileFormat::loadFrom (file) };

    void paint (juce::Graphics& g) override
    {
        cover.draw (g, getLocalBounds().toFloat(), juce::RectanglePlacement::centred);
    }
    @endcode

    @see drawBlend, MipMapEnhance
*/
class MipMap final
{
public:
    using Enhance = MipMapEnhance;

    MipMap() = default;
    MipMap (const juce::Image& src, const Enhance& enhance = {})   { setImage (src, enhance); }

    /** Builds the levels from an image; a null image leaves the MipMap empty */
    void setImage (const juce::Image& src, const Enhance& enhance = {});
    void setImage (const juce::File& f, const Enhance& enhance = {});
    void setImage (const void* rawData, size_t numBytesOfData, const Enhance& enhance = {});

    /** Draws the level that fits rc at the context's physical pixel scale */
    void draw (juce::Graphics& g, juce::Rectangle<float> rc, juce::RectanglePlacement placement = 0) const;

    /** The level for a size in pixels: the smallest one at least that big, the largest for an empty size */
    juce::Image getImageFor (const int width, const int height) const;

    /** The full size source level */
    juce::Image getImage() const                { return isValid() ? images[0] : juce::Image(); }

    juce::Rectangle<int> getBounds() const;

    int numMipMaps() const                      { return int (images.size()); }
    bool isValid() const                        { return numMipMaps() > 0; }
    bool isNull() const                         { return ! isValid(); }
    int getWidth() const                        { return isValid() ? images[0].getWidth() : 0; }
    int getHeight() const                       { return isValid() ? images[0].getHeight() : 0; }

    /** The pixel memory of all levels */
    int getNumBytesOfData() const;
    const Enhance& getEnhance() const           { return enhance; }

    /** The same levels: equal when built from the same setImage() call or a copy of it */
    bool operator== (const MipMap& other) const { return images == other.images && enhance == other.enhance; }

private:
    int getIndexFor (int width, int height) const;

    std::vector<juce::Image> images;
    Enhance enhance;
};

/** Draws a blend of two MipMaps in the same rectangle, each through MipMap::draw: a at
    full opacity, then b on top at progress. 0 draws only a, 1 only b. An empty side is
    skipped.
*/
void drawBlend (juce::Graphics& g, const MipMap& a, const MipMap& b, float progress,
                juce::Rectangle<float> rc, juce::RectanglePlacement placement = 0);
