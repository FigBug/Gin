/*==============================================================================
 
 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com
 
 ==============================================================================*/

#pragma once

//==============================================================================
Image applyVignette (Image src, float amount, float radius, float falloff);

Image applySepia (Image src);

Image applyGreyScale (Image src);

Image applySoften (Image src);

Image applySharpen (Image src);

Image applyGamma (Image src, float gamma);

Image applyInvert (Image src);
