/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================
/** Apply vignette
 *
 \param amount Amount to darken outside of vignette. 0 no darkening. 1 is black.
 \param radius Size of vignette. 1 size of image. 0 is 0 size.
 \param falloff Relative size of inner boundry of vignette 0,1
 */
void applyVignette (Image& img, float amount, float radius, float falloff, ThreadPool* threadPool = nullptr);

/** Make it look old */
void applySepia (Image& img, ThreadPool* threadPool = nullptr);

/** Converts image to B/W, heavier weighting towards greens */
void applyGreyScale (Image& img, ThreadPool* threadPool = nullptr);

/** Softens an image */
void applySoften (Image& img, ThreadPool* threadPool = nullptr);

/** Sharpens an image */
void applySharpen (Image& img, ThreadPool* threadPool = nullptr);

void applyGamma (Image& img, float gamma, ThreadPool* threadPool = nullptr);

/** Inverts colours of an image */
void applyInvert (Image& img, ThreadPool* threadPool = nullptr);

/** Adjust contrast of an image
 *
 \param contrast Amount to adjust contrast. Negative values increase, positive values increase
*/
void applyContrast (Image& img, float contrast, ThreadPool* threadPool = nullptr);

/** Adjust brightness and contrast of an image
 *
 \param brightness Amount to adjust brightness -100,100
 \param contrast Amount to adjust contrast -100,100
 */
void applyBrightnessContrast (Image& img, float brightness, float contrast, ThreadPool* threadPool = nullptr);

/** Adjust hue, saturation and lightness of an image
 *
 \param hue Amount to adjust hue -180,180
 \param saturation Amount to adjust saturation 0,200
 \param lightness Amount to adjust lightness -100,100
 */
void applyHueSaturationLightness (Image& img, float hue, float saturation, float lightness, ThreadPool* threadPool = nullptr);

/** A very fast blur. This is a compromise between Gaussian Blur and Box blur.
    It creates much better looking blurs than Box Blur, but is 7x faster than some Gaussian Blur
    implementations.
 *
 \param radius from 2 to 254
 */
void applyStackBlur (Image& img, int radius);

/** A very high quality image resize using a bank of sinc
 *  function-based fractional delay filters */
Image applyResize (const Image& img, int width, int height);

Image applyResize (const Image& img, float factor);

/** GradientMap a image. Brightness gets remapped to colour on a gradient.
  */
void applyGradientMap (Image& img, const ColourGradient& gradient, ThreadPool* threadPool = nullptr);

void applyGradientMap (Image& img, const Colour c1, const Colour c2, ThreadPool* threadPool = nullptr);

/** Set an image to a solid colour
  */
void applyColour (Image& img, Colour c, ThreadPool* threadPool = nullptr);

/** Blend two images
  */

enum BlendMode
{
    Normal,
    Lighten,
    Darken,
    Multiply,
    Average,
    Add,
    Subtract,
    Difference,
    Negation,
    Screen,
    Exclusion,
    Overlay,
    SoftLight,
    HardLight,
    ColorDodge,
    ColorBurn,
    LinearDodge,
    LinearBurn,
    LinearLight,
    VividLight,
    PinLight,
    HardMix,
    Reflect,
    Glow,
    Phoenix,
};

void applyBlend (Image& dst, const Image& src, BlendMode mode, float alpha = 1.0f, juce::Point<int> position = {}, ThreadPool* threadPool = nullptr);

void applyBlend (Image& dst, BlendMode mode, Colour c, ThreadPool* threadPool = nullptr);


