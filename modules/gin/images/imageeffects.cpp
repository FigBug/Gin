/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#if JUCE_INTEL
 #define USE_SSE 1
#endif

template <typename T>
inline uint8 toByte (T v)
{
    if (v < 0)   return 0;
    if (v > 255) return 255;
    return uint8 (v);
}

inline uint8 getIntensity (uint8 r, uint8 g, uint8 b)
{
    return (uint8)((7471 * b + 38470 * g + 19595 * r) >> 16);
}

inline uint8 computeAlpha (uint8 la, uint8 ra)
{
    return (uint8)(((la * (256 - (ra + (ra >> 7)))) >> 8) + ra);
}

template <class T>
inline T blend (const T& c1, const T& c2)
{
    int a = c1.getAlpha();
    int invA = 255 - a;

    int r = ((c2.getRed()   * invA) + (c1.getRed()   * a)) / 256;
    int g = ((c2.getGreen() * invA) + (c1.getGreen() * a)) / 256;
    int b = ((c2.getBlue()  * invA) + (c1.getBlue()  * a)) / 256;
    uint8 a2 = computeAlpha (c2.getAlpha(), c1.getAlpha());

    T res;
    res.setARGB (a2, toByte (r), toByte (g), toByte (b));
    return res;
}

template <class T1, class T2>
inline T2 convert (const T1& in)
{
    T2 out;
    out.setARGB (in.getAlpha(), in.getRed(), in.getGreen(), in.getBlue());
    return out;
}

//==============================================================================
template <class T>
void applyVignette (Image& img, float amountIn, float radiusIn, float fallOff, ThreadPool* threadPool)
{
    const int w = img.getWidth();
    const int h = img.getHeight();
    threadPool = (w >= 256 || h >= 256) ? threadPool : nullptr;

    double outA = w * 0.5 * radiusIn;
    double outB = h * 0.5 * radiusIn;

    double inA = outA * fallOff;
    double inB = outB * fallOff;

    double cx = w * 0.5;
    double cy = h * 0.5;

    double amount = 1.0 - amountIn;

    Image::BitmapData data (img, Image::BitmapData::readWrite);

    Ellipse<double> outE { outA, outB };
    Ellipse<double> inE  { inA,  inB  };

    multiThreadedFor<int> (0, h, 1, threadPool, [&] (int y)
    {
        uint8* p = data.getLinePointer (y);

        double dy = y - cy;

        for (int x = 0; x < w; x++)
        {
            double dx = x - cx;

            bool outside = outE.isPointOutside ({dx, dy});
            bool inside  = inE.isPointInside ({dx, dy});

            T* s = (T*)p;

            if (outside)
            {
                uint8 r = toByte (0.5 + (s->getRed() * amount));
                uint8 g = toByte (0.5 + (s->getGreen() * amount));
                uint8 b = toByte (0.5 + (s->getBlue() * amount));
                uint8 a = s->getAlpha();

                s->setARGB (a, r, g, b);
            }
            else if (! inside)
            {
                double angle = std::atan2 (dy, dx);

                auto p1 = outE.pointAtAngle (angle);
                auto p2 = inE.pointAtAngle (angle);

                auto l1 = Line<double> ({dx,dy}, p2);
                auto l2 = Line<double> (p1, p2);

                double factor = 1.0 - (amountIn * jlimit (0.0, 1.0, l1.getLength() / l2.getLength()));

                uint8 r = toByte (0.5 + (s->getRed()   * factor));
                uint8 g = toByte (0.5 + (s->getGreen() * factor));
                uint8 b = toByte (0.5 + (s->getBlue()  * factor));
                uint8 a = s->getAlpha();

                s->setARGB (a, r, g, b);
            }

            p += data.pixelStride;
        }
    });
}

void applyVignette (Image& img, float amountIn, float radiusIn, float fallOff, ThreadPool* threadPool)
{
    if (img.getFormat() == Image::ARGB)          applyVignette<PixelARGB> (img, amountIn, radiusIn, fallOff, threadPool);
    else if (img.getFormat() == Image::RGB)      applyVignette<PixelRGB>  (img, amountIn, radiusIn, fallOff, threadPool);
    else jassertfalse;
}

template <class T>
void applySepia (Image& img, ThreadPool* threadPool)
{
    const int w = img.getWidth();
    const int h = img.getHeight();
    threadPool = (w >= 256 || h >= 256) ? threadPool : nullptr;

    Image::BitmapData data (img, Image::BitmapData::readWrite);

    multiThreadedFor<int> (0, h, 1, threadPool, [&] (int y)
    {
        uint8* p = data.getLinePointer (y);

        for (int x = 0; x < w; x++)
        {
            PixelARGB* s = (PixelARGB*)p;

            uint8 r = s->getRed();
            uint8 g = s->getGreen();
            uint8 b = s->getBlue();
            uint8 a = s->getAlpha();

            uint8 ro = toByte ((r * .393) + (g *.769) + (b * .189));
            uint8 go = toByte ((r * .349) + (g *.686) + (b * .168));
            uint8 bo = toByte ((r * .272) + (g *.534) + (b * .131));

            s->setARGB (a, ro, go, bo);

            p += data.pixelStride;
        }
    });
}

void applySepia (Image& img, ThreadPool* threadPool)
{
    if (img.getFormat() == Image::ARGB)          applySepia<PixelARGB> (img, threadPool);
    else if (img.getFormat() == Image::RGB)      applySepia<PixelRGB>  (img, threadPool);
    else jassertfalse;
}

template <class T>
void applyGreyScale (Image& img, ThreadPool* threadPool)
{
    const int w = img.getWidth();
    const int h = img.getHeight();
    threadPool = (w >= 256 || h >= 256) ? threadPool : nullptr;

    Image::BitmapData data (img, Image::BitmapData::readWrite);

    multiThreadedFor<int> (0, h, 1, threadPool, [&] (int y)
    {
        uint8* p = data.getLinePointer (y);

        for (int x = 0; x < w; x++)
        {
            T* s = (T*)p;

            uint8 r = s->getRed();
            uint8 g = s->getGreen();
            uint8 b = s->getBlue();
            uint8 a = s->getAlpha();

            uint8 ro = toByte (r * 0.30 + 0.5);
            uint8 go = toByte (g * 0.59 + 0.5);
            uint8 bo = toByte (b * 0.11 + 0.5);

            s->setARGB (a,
                        toByte (ro + go + bo),
                        toByte (ro + go + bo),
                        toByte (ro + go + bo));

            p += data.pixelStride;
        }
    });
}

void applyGreyScale (Image& img, ThreadPool* threadPool)
{
    if (img.getFormat() == Image::ARGB)          applyGreyScale<PixelARGB> (img, threadPool);
    else if (img.getFormat() == Image::RGB)      applyGreyScale<PixelRGB>  (img, threadPool);
    else jassertfalse;
}

template <class T>
void applySoften (Image& img, ThreadPool* threadPool)
{
    const int w = img.getWidth();
    const int h = img.getHeight();
    threadPool = (w >= 256 || h >= 256) ? threadPool : nullptr;

    Image dst (img.getFormat(), w, h, true);

    Image::BitmapData srcData (img, Image::BitmapData::readOnly);
    Image::BitmapData dstData (dst, Image::BitmapData::writeOnly);

    multiThreadedFor<int> (0, h, 1, threadPool, [&] (int y)
    {
        for (int x = 0; x < w; x++)
        {
            int ro = 0, go = 0, bo = 0;
            uint8 a = 0;

            for (int m = -1; m <= 1; m++)
            {
                for (int n = -1; n <= 1; n++)
                {
                    int cx = jlimit (0, w - 1, x + m);
                    int cy = jlimit (0, h - 1, y + n);

                    T* s = (T*) srcData.getPixelPointer (cx, cy);

                    ro += s->getRed();
                    go += s->getGreen();
                    bo += s->getBlue();
                }
            }

            T* s = (T*) srcData.getPixelPointer (x, y);
            a = s->getAlpha();

            T* d = (T*) dstData.getPixelPointer (x, y);

            d->setARGB (a, toByte (ro / 9), toByte (go / 9), toByte (bo / 9));
        }
    });
    img = dst;
}

void applySoften (Image& img, ThreadPool* threadPool)
{
    if (img.getFormat() == Image::ARGB)          applySoften<PixelARGB> (img, threadPool);
    else if (img.getFormat() == Image::RGB)      applySoften<PixelRGB>  (img, threadPool);
    else jassertfalse;
}

template <class T>
void applySharpen (Image& img, ThreadPool* threadPool)
{
    const int w = img.getWidth();
    const int h = img.getHeight();
    threadPool = (w >= 256 || h >= 256) ? threadPool : nullptr;

    Image dst (img.getFormat(), w, h, true);

    Image::BitmapData srcData (img, Image::BitmapData::readOnly);
    Image::BitmapData dstData (dst, Image::BitmapData::writeOnly);

    multiThreadedFor<int> (0, h, 1, threadPool, [&] (int y)
    {
        for (int x = 0; x < w; x++)
        {
            auto getPixelPointer = [&] (int cx, int cy) -> T*
            {
                cx = jlimit (0, w - 1, cx);
                cy = jlimit (0, h - 1, cy);

                return (T*) srcData.getPixelPointer (cx, cy);
            };

            int ro = 0, go = 0, bo = 0;
            uint8 ao = 0;

            T* s = getPixelPointer (x, y);

            ro = s->getRed()   * 5;
            go = s->getGreen() * 5;
            bo = s->getBlue()  * 5;
            ao = s->getAlpha();

            s = getPixelPointer (x, y - 1);
            ro -= s->getRed();
            go -= s->getGreen();
            bo -= s->getBlue();

            s = getPixelPointer (x - 1, y);
            ro -= s->getRed();
            go -= s->getGreen();
            bo -= s->getBlue();

            s = getPixelPointer (x + 1, y);
            ro -= s->getRed();
            go -= s->getGreen();
            bo -= s->getBlue();

            s = getPixelPointer (x, y + 1);
            ro -= s->getRed();
            go -= s->getGreen();
            bo -= s->getBlue();

            T* d = (T*) dstData.getPixelPointer (x, y);

            d->setARGB (ao, toByte (ro), toByte (go), toByte (bo));
        }
    });
    img = dst;
}

void applySharpen (Image& img, ThreadPool* threadPool)
{
    if (img.getFormat() == Image::ARGB)          applySharpen<PixelARGB> (img, threadPool);
    else if (img.getFormat() == Image::RGB)      applySharpen<PixelRGB>  (img, threadPool);
    else jassertfalse;
}

template <class T>
void applyGamma (Image& img, float gamma, ThreadPool* threadPool)
{
    const int w = img.getWidth();
    const int h = img.getHeight();
    threadPool = (w >= 256 || h >= 256) ? threadPool : nullptr;

    Image::BitmapData data (img, Image::BitmapData::readWrite);

    multiThreadedFor<int> (0, h, 1, threadPool, [&] (int y)
    {
        uint8* p = data.getLinePointer (y);

        for (int x = 0; x < w; x++)
        {
            T* s = (T*)p;

            uint8 r = s->getRed();
            uint8 g = s->getGreen();
            uint8 b = s->getBlue();
            uint8 a = s->getAlpha();

            uint8 ro = toByte (std::pow (r / 255.0, gamma) * 255.0 + 0.5);
            uint8 go = toByte (std::pow (g / 255.0, gamma) * 255.0 + 0.5);
            uint8 bo = toByte (std::pow (b / 255.0, gamma) * 255.0 + 0.5);

            s->setARGB (a, ro, go, bo);

            p += data.pixelStride;
        }
    });
}

void applyGamma (Image& img, float gamma, ThreadPool* threadPool)
{
    if (img.getFormat() == Image::ARGB)          applyGamma<PixelARGB> (img, gamma, threadPool);
    else if (img.getFormat() == Image::RGB)      applyGamma<PixelRGB>  (img, gamma, threadPool);
    else jassertfalse;
}

template <class T>
void applyInvert (Image& img, ThreadPool* threadPool)
{
    const int w = img.getWidth();
    const int h = img.getHeight();
    threadPool = (w >= 256 || h >= 256) ? threadPool : nullptr;

    Image::BitmapData data (img, Image::BitmapData::readWrite);

    multiThreadedFor<int> (0, h, 1, threadPool, [&] (int y)
    {
        uint8* p = data.getLinePointer (y);

        for (int x = 0; x < w; x++)
        {
            T* s = (T*)p;

            uint8 r = s->getRed();
            uint8 g = s->getGreen();
            uint8 b = s->getBlue();
            uint8 a = s->getAlpha();

            uint8 ro = 255 - r;
            uint8 go = 255 - g;
            uint8 bo = 255 - b;

            s->setARGB (a, ro, go, bo);

            p += data.pixelStride;
        }
    });
}

void applyInvert (Image& img, ThreadPool* threadPool)
{
    if (img.getFormat() == Image::ARGB)          applyInvert<PixelARGB> (img, threadPool);
    else if (img.getFormat() == Image::RGB)      applyInvert<PixelRGB>  (img, threadPool);
    else jassertfalse;
}

template <class T>
void applyContrast (Image& img, float contrast, ThreadPool* threadPool)
{
    const int w = img.getWidth();
    const int h = img.getHeight();
    threadPool = (w >= 256 || h >= 256) ? threadPool : nullptr;

    contrast = (100.0f + contrast) / 100.0f;
    contrast = square (contrast);

    Image::BitmapData data (img, Image::BitmapData::readWrite);

    multiThreadedFor<int> (0, h, 1, threadPool, [&] (int y)
    {
        uint8* p = data.getLinePointer (y);

        for (int x = 0; x < w; x++)
        {
            T* s = (T*)p;

            uint8 r = s->getRed();
            uint8 g = s->getGreen();
            uint8 b = s->getBlue();
            uint8 a = s->getAlpha();

            double ro = (double) r / 255.0;
            ro = ro - 0.5;
            ro = ro * contrast;
            ro = ro + 0.5;
            ro = ro * 255.0;

            double go = (double) g / 255.0;
            go = go - 0.5;
            go = go * contrast;
            go = go + 0.5;
            go = go * 255.0;

            double bo = (double) b / 255.0;
            bo = bo - 0.5;
            bo = bo * contrast;
            bo = bo + 0.5;
            bo = bo * 255.0;

            ro = toByte (ro);
            go = toByte (go);
            bo = toByte (bo);

            s->setARGB (a, toByte (ro), toByte (go), toByte (bo));

            p += data.pixelStride;
        }
    });
}

void applyContrast (Image& img, float contrast, ThreadPool* threadPool)
{
    if (img.getFormat() == Image::ARGB)          applyContrast<PixelARGB> (img, contrast, threadPool);
    else if (img.getFormat() == Image::RGB)      applyContrast<PixelRGB>  (img, contrast, threadPool);
    else jassertfalse;
}

template <class T>
void applyBrightnessContrast (Image& img, float brightness, float contrast, ThreadPool* threadPool)
{
    const int w = img.getWidth();
    const int h = img.getHeight();
    threadPool = (w >= 256 || h >= 256) ? threadPool : nullptr;

    Image::BitmapData data (img, Image::BitmapData::readWrite);

    double multiply = 1;
    double divide = 1;

    if (contrast < 0)
    {
        multiply = contrast + 100;
        divide = 100;
    }
    else if (contrast > 0)
    {
        multiply = 100;
        divide = 100 - contrast;
    }
    else
    {
        multiply = 1;
        divide = 1;
    }

    uint8* rgbTable = new uint8[65536];

    if (divide == 0)
    {
        for (int intensity = 0; intensity < 256; intensity++)
        {
            if (intensity + brightness < 128)
                rgbTable[intensity] = 0;
            else
                rgbTable[intensity] = 255;
        }
    }
    else if (divide == 100)
    {
        for (int intensity = 0; intensity < 256; intensity++)
        {
            int shift = int ((intensity - 127) * multiply / divide + 127 - intensity + brightness);

            for (int col = 0; col < 256; col++)
            {
                int index = (intensity * 256) + col;
                rgbTable[index] = toByte (col + shift);
            }
        }
    }
    else
    {
        for (int intensity = 0; intensity < 256; intensity++)
        {
            int shift = int ((intensity - 127 + brightness) * multiply / divide + 127 - intensity);

            for (int col = 0; col < 256; col++)
            {
                int index = (intensity * 256) + col;
                rgbTable[index] = toByte (col + shift);
            }
        }
    }

    multiThreadedFor<int> (0, h, 1, threadPool, [&] (int y)
    {
        uint8* p = data.getLinePointer (y);

        for (int x = 0; x < w; x++)
        {
            T* s = (T*)p;

            uint8 r = s->getRed();
            uint8 g = s->getGreen();
            uint8 b = s->getBlue();
            uint8 a = s->getAlpha();

            if (divide == 0)
            {
                int i = getIntensity (toByte (r), toByte (g), toByte (b));
                uint8 c = rgbTable[i];

                s->setARGB (a, c, c, c);
            }
            else
            {
                int i = getIntensity (toByte (r), toByte (g), toByte (b));
                int shiftIndex = i * 256;

                uint8 ro = rgbTable[shiftIndex + r];
                uint8 go = rgbTable[shiftIndex + g];
                uint8 bo = rgbTable[shiftIndex + b];

                ro = toByte (ro);
                go = toByte (go);
                bo = toByte (bo);

                s->setARGB (a, ro, go, bo);
            }

            p += data.pixelStride;
        }
    });

    delete[] rgbTable;
}

void applyBrightnessContrast (Image& img, float brightness, float contrast, ThreadPool* threadPool)
{
    if (img.getFormat() == Image::ARGB)          applyBrightnessContrast<PixelARGB> (img, brightness, contrast, threadPool);
    else if (img.getFormat() == Image::RGB)      applyBrightnessContrast<PixelRGB>  (img, brightness, contrast, threadPool);
    else jassertfalse;
}

template <class T>
void applyHueSaturationLightness (Image& img, float hueIn, float saturation, float lightness, ThreadPool* threadPool)
{
    const int w = img.getWidth();
    const int h = img.getHeight();
    threadPool = (w >= 256 || h >= 256) ? threadPool : nullptr;

    if (saturation > 100)
        saturation = ((saturation - 100) * 3) + 100;
    saturation = (saturation * 1024) / 100;

    hueIn /= 360.0f;

    Image::BitmapData data (img, Image::BitmapData::readWrite);

    multiThreadedFor<int> (0, h, 1, threadPool, [&] (int y)
    {
        uint8* p = data.getLinePointer (y);

        for (int x = 0; x < w; x++)
        {
            T* s = (T*)p;

            uint8 r = s->getRed();
            uint8 g = s->getGreen();
            uint8 b = s->getBlue();
            uint8 a = s->getAlpha();

            int intensity = getIntensity (toByte (r), toByte (g), toByte (b));
            int ro = toByte (int (intensity * 1024 + (r - intensity) * saturation) >> 10);
            int go = toByte (int (intensity * 1024 + (g - intensity) * saturation) >> 10);
            int bo = toByte (int (intensity * 1024 + (b - intensity) * saturation) >> 10);

            Colour c (toByte (ro), toByte (go), toByte (bo));
            float hue = c.getHue();
            hue += hueIn;

            while (hue < 0.0f)  hue += 1.0f;
            while (hue >= 1.0f) hue -= 1.0f;

            c = Colour::fromHSV (hue, c.getSaturation(), c.getBrightness(), float (a));
            ro = c.getRed();
            go = c.getGreen();
            bo = c.getBlue();

            ro = toByte (ro);
            go = toByte (go);
            bo = toByte (bo);

            s->setARGB (a, toByte (ro), toByte (go), toByte (bo));

            if (lightness > 0)
            {
                auto blended = blend (PixelARGB (toByte ((lightness * 255) / 100 * (a / 255.0)), 255, 255, 255), convert<T, PixelARGB> (*s));
                *s = convert<PixelARGB, T> (blended);
            }
            else if (lightness < 0)
            {
                auto blended = blend (PixelARGB (toByte ((-lightness * 255) / 100 * (a / 255.0)), 0, 0, 0), convert<T, PixelARGB> (*s));
                *s = convert<PixelARGB, T> (blended);
            }

            p += data.pixelStride;
        }
    });
}

void applyHueSaturationLightness (Image& img, float hue, float saturation, float lightness, ThreadPool* threadPool)
{
    if (img.getFormat() == Image::ARGB)          applyHueSaturationLightness<PixelARGB> (img, hue, saturation, lightness, threadPool);
    else if (img.getFormat() == Image::RGB)      applyHueSaturationLightness<PixelRGB>  (img, hue, saturation, lightness, threadPool);
    else jassertfalse;
}

Image applyResize (const Image& src, int width, int height)
{
    Image dst (src.getFormat(), width, height, true);

    Image::BitmapData srcData (src, Image::BitmapData::readOnly);
    Image::BitmapData dstData (dst, Image::BitmapData::readWrite);

    int channels = 0;
    if (src.getFormat() == Image::ARGB)                 channels = 4;
    else if (src.getFormat() == Image::RGB)             channels = 3;
    else if (src.getFormat() == Image::SingleChannel)   channels = 1;
    else                                                return {};

    // JUCE images may have padding at the end of each scan line.
    // Avir expects the image data to be packed. So we need to
    // pack and unpack the image data before and after resizing.
    HeapBlock<uint8> srcPacked (src.getWidth() * src.getHeight() * channels);
    HeapBlock<uint8> dstPacked (dst.getWidth() * dst.getHeight() * channels);

    uint8* rawSrc = srcPacked.getData();
    uint8* rawDst = dstPacked.getData();

    for (int y = 0; y < src.getHeight(); y++)
        memcpy (rawSrc + y * src.getWidth() * channels,
                srcData.getLinePointer (y),
                (size_t) (src.getWidth() * channels));

   #if USE_SSE
    avir::CImageResizer<avir::fpclass_float4> imageResizer (8);
    imageResizer.resizeImage (rawSrc, src.getWidth(), src.getHeight(), 0,
                                rawDst, dst.getWidth(), dst.getHeight(), channels, 0);
   #else
    avir::CImageResizer<> imageResizer (8);
    imageResizer.resizeImage (rawSrc, src.getWidth(), src.getHeight(), 0,
                                    rawDst, dst.getWidth(), dst.getHeight(), channels, 0);
   #endif

    for (int y = 0; y < dst.getHeight(); y++)
        memcpy (dstData.getLinePointer (y),
                rawDst + y * dst.getWidth() * channels,
                (size_t) (dst.getWidth() * channels));

    return dst;
}

Image applyResize (const Image& src, float factor)
{
    return applyResize (src,
                        roundToInt (factor * src.getWidth()),
                        roundToInt (factor * src.getHeight()));
}

template <class T>
void applyGradientMap (Image& img, const ColourGradient& gradient, ThreadPool* threadPool)
{
    const int w = img.getWidth();
    const int h = img.getHeight();
    threadPool = (w >= 256 || h >= 256) ? threadPool : nullptr;

    Image::BitmapData data (img, Image::BitmapData::readWrite);

    multiThreadedFor<int> (0, h, 1, threadPool, [&] (int y)
                           {
                               uint8* p = data.getLinePointer (y);

                               for (int x = 0; x < w; x++)
                               {
                                   T* s = (T*)p;

                                   uint8 r = s->getRed();
                                   uint8 g = s->getGreen();
                                   uint8 b = s->getBlue();
                                   uint8 a = s->getAlpha();

                                   uint8 ro = toByte (r * 0.30 + 0.5);
                                   uint8 go = toByte (g * 0.59 + 0.5);
                                   uint8 bo = toByte (b * 0.11 + 0.5);

                                   float proportion = float (ro + go + bo) / 256.0f;

                                   auto c = gradient.getColourAtPosition (proportion);

                                   s->setARGB (a,
                                               c.getRed(),
                                               c.getGreen(),
                                               c.getBlue());

                                   p += data.pixelStride;
                               }
                           });
}

void applyGradientMap (Image& img, const ColourGradient& gradient, ThreadPool* threadPool)
{
    if (img.getFormat() == Image::ARGB)          applyGradientMap<PixelARGB> (img, gradient, threadPool);
    else if (img.getFormat() == Image::RGB)      applyGradientMap<PixelRGB>  (img, gradient, threadPool);
    else jassertfalse;
}

void applyGradientMap (Image& img, const Colour c1, const Colour c2, ThreadPool* threadPool)
{
    ColourGradient g;
    g.addColour (0.0, c1);
    g.addColour (1.0, c2);

    applyGradientMap (img, g, threadPool);
}

template <class T>
void applyColour (Image& img, Colour c, ThreadPool* threadPool)
{
    const int w = img.getWidth();
    const int h = img.getHeight();
    threadPool = (w >= 256 || h >= 256) ? threadPool : nullptr;

    uint8 r = c.getRed();
    uint8 g = c.getGreen();
    uint8 b = c.getBlue();
    uint8 a = c.getAlpha();

    Image::BitmapData data (img, Image::BitmapData::readWrite);

    multiThreadedFor<int> (0, h, 1, threadPool, [&] (int y)
                           {
                               uint8* p = data.getLinePointer (y);

                               for (int x = 0; x < w; x++)
                               {
                                   T* s = (T*)p;
                                   s->setARGB (a, r, g, b);
                                   p += data.pixelStride;
                               }
                           });
}

void applyColour (Image& img, Colour c, ThreadPool* threadPool)
{
    if (img.getFormat() == Image::ARGB)          applyColour<PixelARGB> (img, c, threadPool);
    else if (img.getFormat() == Image::RGB)      applyColour<PixelRGB>  (img, c, threadPool);
    else jassertfalse;
}

