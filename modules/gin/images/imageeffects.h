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

/** A very fast blur. This is a compromise between Gaussian Blur and Box blur.
    It creates much better looking blurs than Box Blur, but is 7x faster than some Gaussian Blur
    implementations. 
 */
Image applyStackBlur (Image src, unsigned int radius);
