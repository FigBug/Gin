/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================
/** @file gin_imageeffects.h
    Image processing effects and filters.

    This file provides a collection of image processing functions for applying
    various visual effects, adjustments, and filters to JUCE images. Most functions
    modify images in-place and support optional multithreading via ThreadPool.

    Effects categories:
    - Color adjustments: sepia, greyscale, invert, gamma, brightness/contrast, HSL
    - Blur and sharpening: stack blur, soften, sharpen
    - Artistic effects: vignette, gradient map, blend modes
    - Geometry: high-quality resizing with multiple algorithms

    @see applyBlend for compositing images with blend modes
    @see applyResize for high-quality image scaling
*/

//==============================================================================
/** Applies a vignette effect to an image.

    Darkens the edges of the image, creating a focus effect towards the center.
    Commonly used for artistic photography effects.

    @param img        The image to modify (modified in-place)
    @param amount     Darkness amount [0, 1]. 0 = no darkening, 1 = black edges
    @param radius     Vignette size [0, 1]. 1 = full image size, 0 = no vignette
    @param falloff    Transition size [0, 1]. Controls softness of vignette edge
    @param threadPool Optional thread pool for parallel processing
*/
void applyVignette (juce::Image& img, float amount, float radius, float falloff, juce::ThreadPool* threadPool = nullptr);

/** Applies a sepia tone effect to an image.

    Converts the image to warm brown tones, creating a vintage/aged photograph look.

    @param img        The image to modify (modified in-place)
    @param threadPool Optional thread pool for parallel processing
*/
void applySepia (juce::Image& img, juce::ThreadPool* threadPool = nullptr);

/** Converts an image to greyscale.

    Uses weighted RGB conversion with heavier weighting towards green channel,
    which better matches human luminance perception.

    @param img        The image to modify (modified in-place)
    @param threadPool Optional thread pool for parallel processing
*/
void applyGreyScale (juce::Image& img, juce::ThreadPool* threadPool = nullptr);

/** Softens an image by reducing detail.

    Applies a subtle blur to reduce image sharpness and detail.

    @param img        The image to modify (modified in-place)
    @param threadPool Optional thread pool for parallel processing
*/
void applySoften (juce::Image& img, juce::ThreadPool* threadPool = nullptr);

/** Sharpens an image by enhancing edges.

    Increases image sharpness and edge definition using convolution.

    @param img        The image to modify (modified in-place)
    @param threadPool Optional thread pool for parallel processing
*/
void applySharpen (juce::Image& img, juce::ThreadPool* threadPool = nullptr);

/** Applies gamma correction to an image.

    Adjusts the brightness curve of the image non-linearly. Gamma < 1 brightens,
    gamma > 1 darkens.

    @param img        The image to modify (modified in-place)
    @param gamma      Gamma value (typically 0.1 to 10.0, 1.0 = no change)
    @param threadPool Optional thread pool for parallel processing
*/
void applyGamma (juce::Image& img, float gamma, juce::ThreadPool* threadPool = nullptr);

/** Inverts all colors in an image.

    Creates a negative/inverse effect by inverting RGB values.

    @param img        The image to modify (modified in-place)
    @param threadPool Optional thread pool for parallel processing
*/
void applyInvert (juce::Image& img, juce::ThreadPool* threadPool = nullptr);

/** Adjusts the contrast of an image.

    Modifies the difference between light and dark areas.

    @param img        The image to modify (modified in-place)
    @param contrast   Contrast adjustment [-100, 100]. Negative = decrease, positive = increase
    @param threadPool Optional thread pool for parallel processing
*/
void applyContrast (juce::Image& img, float contrast, juce::ThreadPool* threadPool = nullptr);

/** Adjusts brightness and contrast of an image.

    Combines brightness and contrast adjustments in a single pass.

    @param img        The image to modify (modified in-place)
    @param brightness Brightness adjustment [-100, 100]. Negative = darker, positive = brighter
    @param contrast   Contrast adjustment [-100, 100]. Negative = decrease, positive = increase
    @param threadPool Optional thread pool for parallel processing
*/
void applyBrightnessContrast (juce::Image& img, float brightness, float contrast, juce::ThreadPool* threadPool = nullptr);

/** Adjusts hue, saturation and lightness of an image.

    Modifies color properties in HSL color space, allowing independent control
    of hue (color), saturation (intensity), and lightness (brightness).

    @param img        The image to modify (modified in-place)
    @param hue        Hue shift [-180, 180] degrees. 0 = no change
    @param saturation Saturation adjustment [0, 200]. 100 = no change, 0 = greyscale
    @param lightness  Lightness adjustment [-100, 100]. 0 = no change
    @param threadPool Optional thread pool for parallel processing
*/
void applyHueSaturationLightness (juce::Image& img, float hue, float saturation, float lightness, juce::ThreadPool* threadPool = nullptr);

/** Applies a fast, high-quality blur to an image.

    Uses stack blur algorithm - a compromise between Gaussian blur quality and
    box blur speed. Produces much better results than box blur while being ~7x
    faster than traditional Gaussian blur implementations.

    @param img    The image to modify (modified in-place)
    @param radius Blur radius [2, 254]. Larger values = more blur
*/
void applyStackBlur (juce::Image& img, int radius);

/** Image resize algorithms for high-quality scaling.

    Both algorithms provide superior quality compared to basic interpolation,
    using sinc function-based fractional delay filters.

    @see applyResize
*/
enum class ResizeAlgorirm
{
    avir,     ///< AVIR algorithm - high quality, balanced speed/quality
    lanczos,  ///< Lanczos algorithm - excellent quality, slightly slower
};

/** Resizes an image to specific dimensions using high-quality algorithm.

    Uses sinc function-based fractional delay filters for superior quality
    compared to basic bilinear/bicubic interpolation.

    @param img             The source image (not modified)
    @param width           Target width in pixels
    @param height          Target height in pixels
    @param resizeAlgorirm  Algorithm to use (default: avir)
    @returns               New resized image
*/
juce::Image applyResize (const juce::Image& img, int width, int height, ResizeAlgorirm resizeAlgorirm = ResizeAlgorirm::avir);

/** Resizes an image by a scaling factor.

    Convenience overload that scales proportionally by a factor.

    @param img             The source image (not modified)
    @param factor          Scale factor (e.g., 0.5 = half size, 2.0 = double size)
    @param resizeAlgorirm  Algorithm to use (default: avir)
    @returns               New resized image
*/
juce::Image applyResize (const juce::Image& img, float factor, ResizeAlgorirm resizeAlgorirm = ResizeAlgorirm::avir);

/** Applies a gradient map to an image.

    Remaps the brightness of each pixel to a color from the gradient. Black pixels
    map to the start of the gradient, white pixels to the end, with smooth
    interpolation between.

    @param img        The image to modify (modified in-place)
    @param gradient   The color gradient to map brightness values to
    @param threadPool Optional thread pool for parallel processing
*/
void applyGradientMap (juce::Image& img, const juce::ColourGradient& gradient, juce::ThreadPool* threadPool = nullptr);

/** Applies a two-color gradient map to an image.

    Convenience overload that creates a linear gradient between two colors.

    @param img        The image to modify (modified in-place)
    @param c1         Color for black/dark pixels
    @param c2         Color for white/bright pixels
    @param threadPool Optional thread pool for parallel processing
*/
void applyGradientMap (juce::Image& img, const juce::Colour c1, const juce::Colour c2, juce::ThreadPool* threadPool = nullptr);

/** Sets all pixels in an image to a solid color.

    Fills the entire image with the specified color, preserving alpha if present.

    @param img        The image to modify (modified in-place)
    @param c          The color to fill with
    @param threadPool Optional thread pool for parallel processing
*/
void applyColour (juce::Image& img, juce::Colour c, juce::ThreadPool* threadPool = nullptr);

//==============================================================================
/** Blending modes for image compositing.

    Defines how two images are combined when using applyBlend. Each mode produces
    different visual effects based on how source and destination pixel values interact.

    Common modes:
    - Normal: Standard alpha compositing
    - Multiply: Darkens image, useful for shadows
    - Screen: Lightens image, useful for highlights
    - Overlay: Combines Multiply and Screen based on luminance
    - Add: Adds colors together, creates bright effects

    @see applyBlend
*/
enum BlendMode
{
    Normal,        ///< Standard alpha blending
    Lighten,       ///< Selects lighter of source and destination
    Darken,        ///< Selects darker of source and destination
    Multiply,      ///< Multiplies colors (darkens)
    Average,       ///< Averages source and destination
    Add,           ///< Adds colors together (brightens)
    Subtract,      ///< Subtracts source from destination
    Difference,    ///< Absolute difference between colors
    Negation,      ///< Inverted difference
    Screen,        ///< Inverse multiply (lightens)
    Exclusion,     ///< Similar to Difference but lower contrast
    Overlay,       ///< Combines Multiply and Screen based on destination
    SoftLight,     ///< Subtle lighting effect
    HardLight,     ///< Strong lighting effect
    ColorDodge,    ///< Brightens based on source
    ColorBurn,     ///< Darkens based on source
    LinearDodge,   ///< Same as Add
    LinearBurn,    ///< Linear burn effect
    LinearLight,   ///< Combination of LinearDodge and LinearBurn
    VividLight,    ///< Combination of ColorDodge and ColorBurn
    PinLight,      ///< Replaces colors based on blend color
    HardMix,       ///< Posterizes to limited colors
    Reflect,       ///< Brightening reflect effect
    Glow,          ///< Inverse reflect
    Phoenix,       ///< Similar to glow with different formula
};

/** Blends two images using the specified blend mode.

    Composites a source image onto a destination image using various blending
    algorithms. The destination image is modified in-place.

    @param dst        Destination image (modified in-place)
    @param src        Source image to blend on top
    @param mode       Blending mode to use
    @param alpha      Opacity of the blend [0, 1]. 0 = invisible, 1 = fully opaque
    @param position   Position to place source image (default: 0,0)
    @param threadPool Optional thread pool for parallel processing
*/
void applyBlend (juce::Image& dst, const juce::Image& src, BlendMode mode, float alpha = 1.0f, juce::Point<int> position = {}, juce::ThreadPool* threadPool = nullptr);

/** Blends a solid color onto an image using the specified blend mode.

    Applies a blend mode with a solid color across the entire image.

    @param dst        Destination image (modified in-place)
    @param mode       Blending mode to use
    @param c          Color to blend with
    @param threadPool Optional thread pool for parallel processing
*/
void applyBlend (juce::Image& dst, BlendMode mode, juce::Colour c, juce::ThreadPool* threadPool = nullptr);

inline uint8_t channelBlendNormal (int A, int)          { return ((uint8_t)(A)); }
inline uint8_t channelBlendLighten (int A, int B)       { return ((uint8_t)((B > A) ? B : A)); }
inline uint8_t channelBlendDarken (int A, int B)        { return ((uint8_t)((B > A) ? A : B)); }
inline uint8_t channelBlendMultiply (int A, int B)      { return ((uint8_t)((A * B) / 255)); }
inline uint8_t channelBlendAverage (int A, int B)       { return ((uint8_t)((A + B) / 2)); }
inline uint8_t channelBlendAdd (int A, int B)           { return ((uint8_t)(juce::jmin (255, (A + B)))); }
inline uint8_t channelBlendSubtract (int A, int B)      { return ((uint8_t)((A + B < 255) ? 0 : (A + B - 255))); }
inline uint8_t channelBlendDifference (int A, int B)    { return ((uint8_t)(::std::abs (A - B))); }
inline uint8_t channelBlendNegation (int A, int B)      { return ((uint8_t)(255 - ::std::abs (255 - A - B))); }
inline uint8_t channelBlendScreen (int A, int B)        { return ((uint8_t)(255 - (((255 - A) * (255 - B)) >> 8))); }
inline uint8_t channelBlendExclusion (int A, int B)     { return ((uint8_t)(A + B - 2 * A * B / 255)); }
inline uint8_t channelBlendOverlay (int A, int B)       { return ((uint8_t)((B < 128) ? (2 * A * B / 255) : (255 - 2 * (255 - A) * (255 - B) / 255))); }
inline uint8_t channelBlendSoftLight (int A, int B)     { return ((uint8_t)((B < 128) ? (2 * ((A >> 1) + 64)) * ((float)B / 255) : (255 - (2 * (255 - ((A >> 1) + 64)) * (float)(255 - B) / 255)))); }
inline uint8_t channelBlendHardLight (int A, int B)     { return (channelBlendOverlay (B,A)); }
inline uint8_t channelBlendColorDodge (int A, int B)    { return ((uint8_t)((B == 255) ? B : juce::jmin (255, ((A << 8 ) / (255 - B))))); }
inline uint8_t channelBlendColorBurn (int A, int B)     { return ((uint8_t)((B == 0) ? B : juce::jmax (0, (255 - ((255 - A) << 8 ) / B)))); }
inline uint8_t channelBlendLinearDodge (int A, int B)   { return (channelBlendAdd (A, B)); }
inline uint8_t channelBlendLinearBurn (int A, int B)    { return (channelBlendSubtract (A, B)); }
inline uint8_t channelBlendLinearLight (int A, int B)   { return ((uint8_t)(B < 128) ? channelBlendLinearBurn (A,(2 * B)) : channelBlendLinearDodge (A, (2 * (B - 128)))); }
inline uint8_t channelBlendVividLight (int A, int B)    { return ((uint8_t)(B < 128) ? channelBlendColorBurn (A,(2 * B)) : channelBlendColorDodge (A, (2 * (B - 128)))); }
inline uint8_t channelBlendPinLight (int A, int B)      { return ((uint8_t)(B < 128) ? channelBlendDarken (A,(2 * B)) : channelBlendLighten (A, (2 * (B - 128)))); }
inline uint8_t channelBlendHardMix (int A, int B)       { return ((uint8_t)((channelBlendVividLight (A, B) < 128) ? 0:255)); }
inline uint8_t channelBlendReflect (int A, int B)       { return ((uint8_t)((B == 255) ? B : juce::jmin (255, (A * A / (255 - B))))); }
inline uint8_t channelBlendGlow (int A, int B)          { return (channelBlendReflect (B, A)); }
inline uint8_t channelBlendPhoenix (int A, int B)       { return ((uint8_t)(juce::jmin (A, B) - juce::jmax (A, B) + 255)); }

inline uint8_t channelBlendAlpha (uint8_t A, uint8_t B, float O)
{
    return ((uint8_t)(O * A + (1 - O) * B));
}

