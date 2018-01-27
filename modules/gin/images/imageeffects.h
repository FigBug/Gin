/*==============================================================================
 
 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com
 
 ==============================================================================*/

#pragma once

//==============================================================================
inline uint8 getIntensity (uint8 r, uint8 g, int8 b)
{
    return (uint8)((7471 * b + 38470 * g + 19595 * r) >> 16);
}

//==============================================================================
Image applyVignette (Image src, float amount, float radius, float falloff);

Image applySepia (Image src);

Image applyGreyScale (Image src);

Image applySoften (Image src);

Image applySharpen (Image src);

Image applyGamma (Image src, float gamma);

Image applyInvert (Image src);

Image applyContrast (Image src, float contrast);

Image applyBrightnessContrast (Image src, float brightness, float contrast);

Image applyHueSaturationLightness (Image src, float hue, float saturdation, float lightness);
