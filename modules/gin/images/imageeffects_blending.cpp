/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

inline uint8 channelBlendNormal (int A, int)          { return ((uint8)(A)); }
inline uint8 channelBlendLighten (int A, int B)       { return ((uint8)((B > A) ? B : A)); }
inline uint8 channelBlendDarken (int A, int B)        { return ((uint8)((B > A) ? A : B)); }
inline uint8 channelBlendMultiply (int A, int B)      { return ((uint8)((A * B) / 255)); }
inline uint8 channelBlendAverage (int A, int B)       { return ((uint8)((A + B) / 2)); }
inline uint8 channelBlendAdd (int A, int B)           { return ((uint8)(jmin (255, (A + B)))); }
inline uint8 channelBlendSubtract (int A, int B)      { return ((uint8)((A + B < 255) ? 0 : (A + B - 255))); }
inline uint8 channelBlendDifference (int A, int B)    { return ((uint8)(std::abs (A - B))); }
inline uint8 channelBlendNegation (int A, int B)      { return ((uint8)(255 - std::abs (255 - A - B))); }
inline uint8 channelBlendScreen (int A, int B)        { return ((uint8)(255 - (((255 - A) * (255 - B)) >> 8))); }
inline uint8 channelBlendExclusion (int A, int B)     { return ((uint8)(A + B - 2 * A * B / 255)); }
inline uint8 channelBlendOverlay (int A, int B)       { return ((uint8)((B < 128) ? (2 * A * B / 255) : (255 - 2 * (255 - A) * (255 - B) / 255))); }
inline uint8 channelBlendSoftLight (int A, int B)     { return ((uint8)((B < 128) ? (2 * ((A >> 1) + 64)) * ((float)B / 255) : (255 - (2 * (255 - ((A >> 1) + 64)) * (float)(255 - B) / 255)))); }
inline uint8 channelBlendHardLight (int A, int B)     { return (channelBlendOverlay (B,A)); }
inline uint8 channelBlendColorDodge (int A, int B)    { return ((uint8)((B == 255) ? B : jmin (255, ((A << 8 ) / (255 - B))))); }
inline uint8 channelBlendColorBurn (int A, int B)     { return ((uint8)((B == 0) ? B : jmax (0, (255 - ((255 - A) << 8 ) / B)))); }
inline uint8 channelBlendLinearDodge (int A, int B)   { return (channelBlendAdd (A, B)); }
inline uint8 channelBlendLinearBurn (int A, int B)    { return (channelBlendSubtract (A, B)); }
inline uint8 channelBlendLinearLight (int A, int B)   { return ((uint8)(B < 128) ? channelBlendLinearBurn (A,(2 * B)) : channelBlendLinearDodge (A, (2 * (B - 128)))); }
inline uint8 channelBlendVividLight (int A, int B)    { return ((uint8)(B < 128) ? channelBlendColorBurn (A,(2 * B)) : channelBlendColorDodge (A, (2 * (B - 128)))); }
inline uint8 channelBlendPinLight (int A, int B)      { return ((uint8)(B < 128) ? channelBlendDarken (A,(2 * B)) : channelBlendLighten (A, (2 * (B - 128)))); }
inline uint8 channelBlendHardMix (int A, int B)       { return ((uint8)((channelBlendVividLight (A, B) < 128) ? 0:255)); }
inline uint8 channelBlendReflect (int A, int B)       { return ((uint8)((B == 255) ? B : jmin (255, (A * A / (255 - B))))); }
inline uint8 channelBlendGlow (int A, int B)          { return (channelBlendReflect (B, A)); }
inline uint8 channelBlendPhoenix (int A, int B)       { return ((uint8)(jmin (A, B) - jmax (A, B) + 255)); }

inline uint8 channelBlendAlpha (uint8 A, uint8 B, float O)
{
    return ((uint8)(O * A + (1 - O) * B));
}

template <class T, uint8 (*F)(int, int)>
void applyBlend (Image& dst, const Image& src, float alpha, Point<int> position, int maxThreads)
{
    auto rcLower = Rectangle<int> (0, 0, dst.getWidth(), dst.getHeight());
    auto rcUpper = Rectangle<int> (position.x, position.y, src.getWidth(), src.getHeight());

    auto rcOverlap = rcLower.getIntersection (rcUpper);
    if (rcOverlap.isEmpty())
        return;

    int w = rcOverlap.getWidth();
    int h = rcOverlap.getHeight();

    int cropX = position.x < 0 ? -position.x : 0;
    int cropY = position.y < 0 ? -position.y : 0;

    const int numThreads = (w >= 256 || h >= 256) ? maxThreads : 1;

    Image::BitmapData srcData (src, Image::BitmapData::readOnly);
    Image::BitmapData dstData (dst, Image::BitmapData::readWrite);

    multiThreadedFor<int> (0, h, 1, numThreads, [&] (int y)
                           {
                               uint8* pSrc = srcData.getLinePointer (cropY + y);
                               uint8* pDst = dstData.getLinePointer (rcOverlap.getY() + y);

                               pSrc += srcData.pixelStride * cropX;
                               pDst += dstData.pixelStride * rcOverlap.getX();

                               for (int x = 0; x < w; x++)
                               {
                                   T* ac = (T*)pSrc;
                                   T* bc = (T*)pDst;

                                   uint8 ar = ac->getRed();
                                   uint8 ag = ac->getGreen();
                                   uint8 ab = ac->getBlue();
                                   uint8 aa = ac->getAlpha();

                                   uint8 br = bc->getRed();
                                   uint8 bg = bc->getGreen();
                                   uint8 bb = bc->getBlue();
                                   uint8 ba = bc->getAlpha();

                                   if (ba == 255)
                                   {
                                       float pixelAlpha = alpha * aa / 255.0f;

                                       br = channelBlendAlpha (F (ar, br), br, pixelAlpha);
                                       bg = channelBlendAlpha (F (ag, bg), bg, pixelAlpha);
                                       bb = channelBlendAlpha (F (ab, bb), bb, pixelAlpha);
                                   }
                                   else
                                   {
                                       float srcAlpha = alpha * aa / 255.0f;
                                       float dstAlpha = ba / 255.0f;

                                       float outAlpha = srcAlpha + dstAlpha * (1.0f - srcAlpha);

                                       if (outAlpha == 0.0)
                                       {
                                           br = 0;
                                           bg = 0;
                                           bb = 0;
                                       }
                                       else
                                       {
                                           uint8 r = F (ar, br);
                                           uint8 g = F (ag, bg);
                                           uint8 b = F (ab, bb);

                                           br = uint8 ((r * srcAlpha + br * dstAlpha * (1.0f - srcAlpha)) / outAlpha);
                                           bg = uint8 ((g * srcAlpha + bg * dstAlpha * (1.0f - srcAlpha)) / outAlpha);
                                           bb = uint8 ((b * srcAlpha + bb * dstAlpha * (1.0f - srcAlpha)) / outAlpha);
                                       }
                                   }

                                   bc->setARGB (ba, br, bg, bb);

                                   pSrc += srcData.pixelStride;
                                   pDst += dstData.pixelStride;
                               }
                           });
}

template <class T>
void applyBlend (Image& dst, const Image& src, BlendMode mode, float alpha, Point<int> position, int maxThreads)
{
    switch (mode)
    {
        case Normal: applyBlend<T, channelBlendNormal> (dst, src, alpha, position, maxThreads); break;
        case Lighten: applyBlend<T, channelBlendLighten> (dst, src, alpha, position, maxThreads); break;
        case Darken: applyBlend<T, channelBlendDarken> (dst, src, alpha, position, maxThreads); break;
        case Multiply: applyBlend<T, channelBlendMultiply> (dst, src, alpha, position, maxThreads); break;
        case Average: applyBlend<T, channelBlendAverage> (dst, src, alpha, position, maxThreads); break;
        case Add: applyBlend<T, channelBlendAdd> (dst, src, alpha, position, maxThreads); break;
        case Subtract: applyBlend<T, channelBlendSubtract> (dst, src, alpha, position, maxThreads); break;
        case Difference: applyBlend<T, channelBlendDifference> (dst, src, alpha, position, maxThreads); break;
        case Negation: applyBlend<T, channelBlendNegation> (dst, src, alpha, position, maxThreads); break;
        case Screen: applyBlend<T, channelBlendScreen> (dst, src, alpha, position, maxThreads); break;
        case Exclusion: applyBlend<T, channelBlendExclusion> (dst, src, alpha, position, maxThreads); break;
        case Overlay: applyBlend<T, channelBlendOverlay> (dst, src, alpha, position, maxThreads); break;
        case SoftLight: applyBlend<T, channelBlendSoftLight> (dst, src, alpha, position, maxThreads); break;
        case HardLight: applyBlend<T, channelBlendHardLight> (dst, src, alpha, position, maxThreads); break;
        case ColorDodge: applyBlend<T, channelBlendColorDodge> (dst, src, alpha, position, maxThreads); break;
        case ColorBurn: applyBlend<T, channelBlendColorBurn> (dst, src, alpha, position, maxThreads); break;
        case LinearDodge: applyBlend<T, channelBlendLinearDodge> (dst, src, alpha, position, maxThreads); break;
        case LinearBurn: applyBlend<T, channelBlendLinearBurn> (dst, src, alpha, position, maxThreads); break;
        case LinearLight: applyBlend<T, channelBlendLinearLight> (dst, src, alpha, position, maxThreads); break;
        case VividLight: applyBlend<T, channelBlendVividLight> (dst, src, alpha, position, maxThreads); break;
        case PinLight: applyBlend<T, channelBlendPinLight> (dst, src, alpha, position, maxThreads); break;
        case HardMix: applyBlend<T, channelBlendHardMix> (dst, src, alpha, position, maxThreads); break;
        case Reflect: applyBlend<T, channelBlendReflect> (dst, src, alpha, position, maxThreads); break;
        case Glow: applyBlend<T, channelBlendGlow> (dst, src, alpha, position, maxThreads); break;
        case Phoenix: applyBlend<T, channelBlendPhoenix> (dst, src, alpha, position, maxThreads); break;
    }
}

void applyBlend (Image& dst, const Image& src, BlendMode mode, float alpha, Point<int> position, int maxThreads)
{
    if (src.getFormat() != dst.getFormat())
    {
        Image copy = src.createCopy();
        copy = copy.convertedToFormat (dst.getFormat());

        if (src.getFormat() == Image::ARGB)          applyBlend<PixelARGB> (dst, copy, mode, alpha, position, maxThreads);
        else if (src.getFormat() == Image::RGB)      applyBlend<PixelRGB>  (dst, copy, mode, alpha, position, maxThreads);
        else jassertfalse;
    }
    else
    {
        if (src.getFormat() == Image::ARGB)          applyBlend<PixelARGB> (dst, src, mode, alpha, position, maxThreads);
        else if (src.getFormat() == Image::RGB)      applyBlend<PixelRGB>  (dst, src, mode, alpha, position, maxThreads);
        else jassertfalse;
    }
}

template <class T, uint8 (*F)(int, int)>
void applyBlend (Image& dst, Colour c, int maxThreads)
{
    int w = dst.getWidth();
    int h = dst.getHeight();

    const int numThreads = (w >= 256 || h >= 256) ? maxThreads : 1;

    Image::BitmapData dstData (dst, Image::BitmapData::readWrite);

    uint8 ar = c.getRed();
    uint8 ag = c.getGreen();
    uint8 ab = c.getBlue();
    uint8 aa = c.getAlpha();

    multiThreadedFor<int> (0, h, 1, numThreads, [&] (int y)
                           {
                               uint8* pDst = dstData.getLinePointer (y);

                               for (int x = 0; x < w; x++)
                               {
                                   T* bc = (T*)pDst;

                                   uint8 br = bc->getRed();
                                   uint8 bg = bc->getGreen();
                                   uint8 bb = bc->getBlue();
                                   uint8 ba = bc->getAlpha();

                                   if (ba == 255)
                                   {
                                       float pixelAlpha = aa / 255.0f;

                                       br = channelBlendAlpha (F (ar, br), br, pixelAlpha);
                                       bg = channelBlendAlpha (F (ag, bg), bg, pixelAlpha);
                                       bb = channelBlendAlpha (F (ab, bb), bb, pixelAlpha);
                                   }
                                   else
                                   {
                                       float srcAlpha = aa / 255.0f;
                                       float dstAlpha = ba / 255.0f;

                                       float outAlpha = srcAlpha + dstAlpha * (1.0f - srcAlpha);

                                       if (outAlpha == 0.0)
                                       {
                                           br = 0;
                                           bg = 0;
                                           bb = 0;
                                       }
                                       else
                                       {
                                           uint8 r = F (ar, br);
                                           uint8 g = F (ag, bg);
                                           uint8 b = F (ab, bb);

                                           br = uint8 ((r * srcAlpha + br * dstAlpha * (1.0f - srcAlpha)) / outAlpha);
                                           bg = uint8 ((g * srcAlpha + bg * dstAlpha * (1.0f - srcAlpha)) / outAlpha);
                                           bb = uint8 ((b * srcAlpha + bb * dstAlpha * (1.0f - srcAlpha)) / outAlpha);
                                       }
                                   }

                                   bc->setARGB (ba, br, bg, bb);

                                   pDst += dstData.pixelStride;
                               }
                           });
}

template <class T>
void applyBlend (Image& dst, BlendMode mode, Colour c, int maxThreads)
{
    switch (mode)
    {
        case Normal: applyBlend<T, channelBlendNormal> (dst, c, maxThreads); break;
        case Lighten: applyBlend<T, channelBlendLighten> (dst, c, maxThreads); break;
        case Darken: applyBlend<T, channelBlendDarken> (dst, c, maxThreads); break;
        case Multiply: applyBlend<T, channelBlendMultiply> (dst, c, maxThreads); break;
        case Average: applyBlend<T, channelBlendAverage> (dst, c, maxThreads); break;
        case Add: applyBlend<T, channelBlendAdd> (dst, c, maxThreads); break;
        case Subtract: applyBlend<T, channelBlendSubtract> (dst, c, maxThreads); break;
        case Difference: applyBlend<T, channelBlendDifference> (dst, c, maxThreads); break;
        case Negation: applyBlend<T, channelBlendNegation> (dst, c, maxThreads); break;
        case Screen: applyBlend<T, channelBlendScreen> (dst, c, maxThreads); break;
        case Exclusion: applyBlend<T, channelBlendExclusion> (dst, c, maxThreads); break;
        case Overlay: applyBlend<T, channelBlendOverlay> (dst, c, maxThreads); break;
        case SoftLight: applyBlend<T, channelBlendSoftLight> (dst, c, maxThreads); break;
        case HardLight: applyBlend<T, channelBlendHardLight> (dst, c, maxThreads); break;
        case ColorDodge: applyBlend<T, channelBlendColorDodge> (dst, c, maxThreads); break;
        case ColorBurn: applyBlend<T, channelBlendColorBurn> (dst, c, maxThreads); break;
        case LinearDodge: applyBlend<T, channelBlendLinearDodge> (dst, c, maxThreads); break;
        case LinearBurn: applyBlend<T, channelBlendLinearBurn> (dst, c, maxThreads); break;
        case LinearLight: applyBlend<T, channelBlendLinearLight> (dst, c, maxThreads); break;
        case VividLight: applyBlend<T, channelBlendVividLight> (dst, c, maxThreads); break;
        case PinLight: applyBlend<T, channelBlendPinLight> (dst, c, maxThreads); break;
        case HardMix: applyBlend<T, channelBlendHardMix> (dst, c, maxThreads); break;
        case Reflect: applyBlend<T, channelBlendReflect> (dst, c, maxThreads); break;
        case Glow: applyBlend<T, channelBlendGlow> (dst, c, maxThreads); break;
        case Phoenix: applyBlend<T, channelBlendPhoenix> (dst, c, maxThreads); break;
    }
}

void applyBlend (Image& dst, BlendMode mode, Colour c, int maxThreads)
{
    if (dst.getFormat() == Image::ARGB)          applyBlend<PixelARGB> (dst, mode, c, maxThreads);
    else if (dst.getFormat() == Image::RGB)      applyBlend<PixelRGB>  (dst, mode, c, maxThreads);
    else jassertfalse;
}
