/*==============================================================================
 
 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com
 
 ==============================================================================*/

#pragma once

//==============================================================================
template <typename T>
inline uint8 toByte (T v)
{
    if (v < 0)   return 0;
    if (v > 255) return 255;
    return T (v);
}

inline uint8 getIntensity (uint8 r, uint8 g, int8 b)
{
    return (uint8)((7471 * b + 38470 * g + 19595 * r) >> 16);
}

inline uint8 computeAlpha (uint8 la, uint8 ra)
{
    return (uint8)(((la * (256 - (ra + (ra >> 7)))) >> 8) + ra);
}

inline PixelARGB blend (const PixelARGB& c1, const PixelARGB& c2)
{
    int a = c1.getAlpha();
    int invA = 255 - a;
    
    int r = ((c2.getRed()   * invA) + (c1.getRed()   * a)) / 256;
    int g = ((c2.getGreen() * invA) + (c1.getGreen() * a)) / 256;
    int b = ((c2.getBlue()  * invA) + (c1.getBlue()  * a)) / 256;
    uint8 a2 = computeAlpha (c2.getAlpha(), c1.getAlpha());
    
    return PixelARGB (a2, r, g, b);
}

//==============================================================================
/** Apply vignette
 *
 \param amount Amount to darken outside of vignette. 0 no darkening. 1 is black.
 \param radius Size of vignette. 1 size of image. 0 is 0 size.
 \param falloff Relative size of inner boundry of vignette 0,1
 */
Image applyVignette (Image src, float amount, float radius, float falloff);

/** Make it look old */
Image applySepia (Image src);

/** Converts image to B/W, heavier weighting towards greens */
Image applyGreyScale (Image src);

/** Softens an image */
Image applySoften (Image src);

/** Sharpens an image */
Image applySharpen (Image src);

Image applyGamma (Image src, float gamma);

/** Inverts colours of an image */
Image applyInvert (Image src);

/** Adjust contrast of an image
 *
 \param contrast Amount to adjust contrast. Negative values increase, positive values increase
*/
Image applyContrast (Image src, float contrast);

/** Adjust brightness and contrast of an image
 *
 \param brightness Amount to adjust brightness -100,100
 \param contrast Amount to adjust contrast -100,100
 */
Image applyBrightnessContrast (Image src, float brightness, float contrast);

/** Adjust hue, saturation and lightness of an image
 *
 \param hue Amount to adjust hue -180,180
 \param saturation Amount to adjust saturation 0,200
 \param lightness Amount to adjust lightness -100,100
 */
Image applyHueSaturationLightness (Image src, float hue, float saturation, float lightness);
