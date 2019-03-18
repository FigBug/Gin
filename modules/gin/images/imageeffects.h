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
void applyVignette (Image& src, float amount, float radius, float falloff, int maxThreads = -1);

/** Make it look old */
void applySepia (Image& src, int maxThreads = -1);

/** Converts image to B/W, heavier weighting towards greens */
void applyGreyScale (Image& src, int maxThreads = -1);

/** Softens an image */
void applySoften (Image& src, int maxThreads = -1);

/** Sharpens an image */
void applySharpen (Image& src, int maxThreads = -1);

void applyGamma (Image& src, float gamma, int maxThreads = -1);

/** Inverts colours of an image */
void applyInvert (Image& src, int maxThreads = -1);

/** Adjust contrast of an image
 *
 \param contrast Amount to adjust contrast. Negative values increase, positive values increase
*/
void applyContrast (Image& src, float contrast, int maxThreads = -1);

/** Adjust brightness and contrast of an image
 *
 \param brightness Amount to adjust brightness -100,100
 \param contrast Amount to adjust contrast -100,100
 */
void applyBrightnessContrast (Image& src, float brightness, float contrast, int maxThreads = -1);

/** Adjust hue, saturation and lightness of an image
 *
 \param hue Amount to adjust hue -180,180
 \param saturation Amount to adjust saturation 0,200
 \param lightness Amount to adjust lightness -100,100
 */
void applyHueSaturationLightness (Image& src, float hue, float saturation, float lightness, int maxThreads = -1);

/** A very fast blur. This is a compromise between Gaussian Blur and Box blur.
    It creates much better looking blurs than Box Blur, but is 7x faster than some Gaussian Blur
    implementations.
 *
 \param radius from 2 to 254
 */
void applyStackBlur (Image& src, unsigned int radius);
