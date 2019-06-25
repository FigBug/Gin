/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

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
void applyVignette (Image& src, float amountIn, float radiusIn, float fallOff, int maxThreads)
{
    const int w = src.getWidth();
    const int h = src.getHeight();
    const int numThreads = (w >= 256 || h >= 256) ? maxThreads : 1;

    double outA = w * 0.5 * radiusIn;
    double outB = h * 0.5 * radiusIn;

    double inA = outA * fallOff;
    double inB = outB * fallOff;

    double cx = w * 0.5;
    double cy = h * 0.5;

    double amount = 1.0 - amountIn;

    Image::BitmapData data (src, Image::BitmapData::readWrite);

    Ellipse<double> outE { outA, outB };
    Ellipse<double> inE  { inA,  inB  };

    multiThreadedFor<int> (0, h, 1, numThreads, [&] (int y)
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

void applyVignette (Image& src, float amountIn, float radiusIn, float fallOff, int maxThreads)
{
    if (src.getFormat() == Image::ARGB)          applyVignette<PixelARGB> (src, amountIn, radiusIn, fallOff, maxThreads);
    else if (src.getFormat() == Image::RGB)      applyVignette<PixelRGB>  (src, amountIn, radiusIn, fallOff, maxThreads);
    else jassertfalse;
}

template <class T>
void applySepia (Image& src, int maxThreads)
{
    const int w = src.getWidth();
    const int h = src.getHeight();
    const int numThreads = (w >= 256 || h >= 256) ? maxThreads : 1;

    Image::BitmapData data (src, Image::BitmapData::readWrite);

    multiThreadedFor<int> (0, h, 1, numThreads, [&] (int y)
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

void applySepia (Image& src, int maxThreads)
{
    if (src.getFormat() == Image::ARGB)          applySepia<PixelARGB> (src, maxThreads);
    else if (src.getFormat() == Image::RGB)      applySepia<PixelRGB>  (src, maxThreads);
    else jassertfalse;
}

template <class T>
void applyGreyScale (Image& src, int maxThreads)
{
    const int w = src.getWidth();
    const int h = src.getHeight();
    const int numThreads = (w >= 256 || h >= 256) ? maxThreads : 1;

    Image::BitmapData data (src, Image::BitmapData::readWrite);

    multiThreadedFor<int> (0, h, 1, numThreads, [&] (int y)
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

void applyGreyScale (Image& src, int maxThreads)
{
    if (src.getFormat() == Image::ARGB)          applyGreyScale<PixelARGB> (src, maxThreads);
    else if (src.getFormat() == Image::RGB)      applyGreyScale<PixelRGB>  (src, maxThreads);
    else jassertfalse;
}

template <class T>
void applySoften (Image& src, int maxThreads)
{
    const int w = src.getWidth();
    const int h = src.getHeight();
    const int numThreads = (w >= 256 || h >= 256) ? maxThreads : 1;

    Image dst (src.getFormat(), w, h, true);

    Image::BitmapData srcData (src, Image::BitmapData::readOnly);
    Image::BitmapData dstData (dst, Image::BitmapData::writeOnly);

    multiThreadedFor<int> (0, h, 1, numThreads, [&] (int y)
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
    src = dst;
}

void applySoften (Image& src, int maxThreads)
{
    if (src.getFormat() == Image::ARGB)          applySoften<PixelARGB> (src, maxThreads);
    else if (src.getFormat() == Image::RGB)      applySoften<PixelRGB>  (src, maxThreads);
    else jassertfalse;
}

template <class T>
void applySharpen (Image& src, int maxThreads)
{
    const int w = src.getWidth();
    const int h = src.getHeight();
    const int numThreads = (w >= 256 || h >= 256) ? maxThreads : 1;

    Image dst (src.getFormat(), w, h, true);

    Image::BitmapData srcData (src, Image::BitmapData::readOnly);
    Image::BitmapData dstData (dst, Image::BitmapData::writeOnly);

    multiThreadedFor<int> (0, h, 1, numThreads, [&] (int y)
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
    src = dst;
}

void applySharpen (Image& src, int maxThreads)
{
    if (src.getFormat() == Image::ARGB)          applySharpen<PixelARGB> (src, maxThreads);
    else if (src.getFormat() == Image::RGB)      applySharpen<PixelRGB>  (src, maxThreads);
    else jassertfalse;
}

template <class T>
void applyGamma (Image& src, float gamma, int maxThreads)
{
    const int w = src.getWidth();
    const int h = src.getHeight();
    const int numThreads = (w >= 256 || h >= 256) ? maxThreads : 1;

    if (src.getFormat() != Image::ARGB)
        return;

    Image::BitmapData data (src, Image::BitmapData::readWrite);

    multiThreadedFor<int> (0, h, 1, numThreads, [&] (int y)
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

void applyGamma (Image& src, float gamma, int maxThreads)
{
    if (src.getFormat() == Image::ARGB)          applyGamma<PixelARGB> (src, gamma, maxThreads);
    else if (src.getFormat() == Image::RGB)      applyGamma<PixelRGB>  (src, gamma, maxThreads);
    else jassertfalse;
}

template <class T>
void applyInvert (Image& src, int maxThreads)
{
    const int w = src.getWidth();
    const int h = src.getHeight();
    const int numThreads = (w >= 256 || h >= 256) ? maxThreads : 1;

    if (src.getFormat() != Image::ARGB)
        return;

    Image::BitmapData data (src, Image::BitmapData::readWrite);

    multiThreadedFor<int> (0, h, 1, numThreads, [&] (int y)
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

void applyInvert (Image& src, int maxThreads)
{
    if (src.getFormat() == Image::ARGB)          applyInvert<PixelARGB> (src, maxThreads);
    else if (src.getFormat() == Image::RGB)      applyInvert<PixelRGB>  (src, maxThreads);
    else jassertfalse;
}

template <class T>
void applyContrast (Image& src, float contrast, int maxThreads)
{
    const int w = src.getWidth();
    const int h = src.getHeight();
    const int numThreads = (w >= 256 || h >= 256) ? maxThreads : 1;

    if (src.getFormat() != Image::ARGB)
        return;

    contrast = (100.0f + contrast) / 100.0f;
    contrast = square (contrast);

    Image::BitmapData data (src, Image::BitmapData::readWrite);

    multiThreadedFor<int> (0, h, 1, numThreads, [&] (int y)
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

void applyContrast (Image& src, float contrast, int maxThreads)
{
    if (src.getFormat() == Image::ARGB)          applyContrast<PixelARGB> (src, contrast, maxThreads);
    else if (src.getFormat() == Image::RGB)      applyContrast<PixelRGB>  (src, contrast, maxThreads);
    else jassertfalse;
}

template <class T>
void applyBrightnessContrast (Image& src, float brightness, float contrast, int maxThreads)
{
    const int w = src.getWidth();
    const int h = src.getHeight();
    const int numThreads = (w >= 256 || h >= 256) ? maxThreads : 1;

    if (src.getFormat() != Image::ARGB)
        return;

    Image::BitmapData data (src, Image::BitmapData::readWrite);

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

    multiThreadedFor<int> (0, h, 1, numThreads, [&] (int y)
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

void applyBrightnessContrast (Image& src, float brightness, float contrast, int maxThreads)
{
    if (src.getFormat() == Image::ARGB)          applyBrightnessContrast<PixelARGB> (src, brightness, contrast, maxThreads);
    else if (src.getFormat() == Image::RGB)      applyBrightnessContrast<PixelRGB>  (src, brightness, contrast, maxThreads);
    else jassertfalse;
}

template <class T>
void applyHueSaturationLightness (Image& src, float hueIn, float saturation, float lightness, int maxThreads)
{
    const int w = src.getWidth();
    const int h = src.getHeight();
    const int numThreads = (w >= 256 || h >= 256) ? maxThreads : 1;

    if (src.getFormat() != Image::ARGB)
        return;

    if (saturation > 100)
        saturation = ((saturation - 100) * 3) + 100;
    saturation = (saturation * 1024) / 100;

    hueIn /= 360.0f;

    Image::BitmapData data (src, Image::BitmapData::readWrite);

    multiThreadedFor<int> (0, h, 1, numThreads, [&] (int y)
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
                auto blended = blend (PixelARGB (toByte ((lightness * 255) / 100), 255, 255, 255), convert<T, PixelARGB> (*s));
                *s = convert<PixelARGB, T> (blended);
            }
            else if (lightness < 0)
            {
                auto blended = blend (PixelARGB (toByte ((-lightness * 255) / 100), 0, 0, 0), convert<T, PixelARGB> (*s));
                *s = convert<PixelARGB, T> (blended);
            }

            p += data.pixelStride;
        }
    });
}

void applyHueSaturationLightness (Image& src, float hue, float saturation, float lightness, int maxThreads)
{
    if (src.getFormat() == Image::ARGB)          applyHueSaturationLightness<PixelARGB> (src, hue, saturation, lightness, maxThreads);
    else if (src.getFormat() == Image::RGB)      applyHueSaturationLightness<PixelRGB>  (src, hue, saturation, lightness, maxThreads);
    else jassertfalse;
}

static unsigned short const stackblur_mul[255] =
{
    512,512,456,512,328,456,335,512,405,328,271,456,388,335,292,512,
    454,405,364,328,298,271,496,456,420,388,360,335,312,292,273,512,
    482,454,428,405,383,364,345,328,312,298,284,271,259,496,475,456,
    437,420,404,388,374,360,347,335,323,312,302,292,282,273,265,512,
    497,482,468,454,441,428,417,405,394,383,373,364,354,345,337,328,
    320,312,305,298,291,284,278,271,265,259,507,496,485,475,465,456,
    446,437,428,420,412,404,396,388,381,374,367,360,354,347,341,335,
    329,323,318,312,307,302,297,292,287,282,278,273,269,265,261,512,
    505,497,489,482,475,468,461,454,447,441,435,428,422,417,411,405,
    399,394,389,383,378,373,368,364,359,354,350,345,341,337,332,328,
    324,320,316,312,309,305,301,298,294,291,287,284,281,278,274,271,
    268,265,262,259,257,507,501,496,491,485,480,475,470,465,460,456,
    451,446,442,437,433,428,424,420,416,412,408,404,400,396,392,388,
    385,381,377,374,370,367,363,360,357,354,350,347,344,341,338,335,
    332,329,326,323,320,318,315,312,310,307,304,302,299,297,294,292,
    289,287,285,282,280,278,275,273,271,269,267,265,263,261,259
};

static unsigned char const stackblur_shr[255] =
{
    9, 11, 12, 13, 13, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 17,
    17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19,
    19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20,
    20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21,
    21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
    21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22,
    22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
    22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23,
    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
    23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
    24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
    24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
    24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
    24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24
};

static void applyStackBlurBW (Image& src, unsigned int radius)
{
    const unsigned int w = (unsigned int)src.getWidth();
    const unsigned int h = (unsigned int)src.getHeight();

    Image::BitmapData data (src, Image::BitmapData::readWrite);

    radius = jlimit (2u, 254u, radius);

    unsigned char    stack[(254 * 2 + 1) * 1];

    unsigned int x, y, xp, yp, i, sp, stack_start;

    unsigned char* stack_ptr = nullptr;
    unsigned char* src_ptr = nullptr;
    unsigned char* dst_ptr = nullptr;

    unsigned long sum, sum_in, sum_out;

    unsigned int wm = w - 1;
    unsigned int hm = h - 1;
    unsigned int w1 = (unsigned int) data.lineStride;
    unsigned int div = (unsigned int) (radius * 2) + 1;
    unsigned int mul_sum = stackblur_mul[radius];
    unsigned char shr_sum = stackblur_shr[radius];

    for (y = 0; y < h; ++y)
    {
        sum = sum_in = sum_out = 0;

        src_ptr = data.getLinePointer (int (y));

        for (i = 0; i <= radius; ++i)
        {
            stack_ptr    = &stack[i];
            stack_ptr[0] = src_ptr[0];
            sum += src_ptr[0] * (i + 1);
            sum_out += src_ptr[0];
        }

        for (i = 1; i <= radius; ++i)
        {
            if (i <= wm)
                src_ptr += 1;

            stack_ptr = &stack[1 * (i + radius)];
            stack_ptr[0] = src_ptr[0];
            sum += src_ptr[0] * (radius + 1 - i);
            sum_in += src_ptr[0];
        }

        sp = radius;
        xp = radius;
        if (xp > wm)
            xp = wm;

        src_ptr = data.getLinePointer (int (y)) + (unsigned int)data.pixelStride * xp;
        dst_ptr = data.getLinePointer (int (y));

        for (x = 0; x < w; ++x)
        {
            dst_ptr[0] = (unsigned char)((sum * mul_sum) >> shr_sum);
            dst_ptr += 1;

            sum -= sum_out;

            stack_start = sp + div - radius;

            if (stack_start >= div)
                stack_start -= div;

            stack_ptr = &stack[1 * stack_start];

            sum_out -= stack_ptr[0];

            if (xp < wm)
            {
                src_ptr += 1;
                ++xp;
            }

            stack_ptr[0] = src_ptr[0];

            sum_in += src_ptr[0];
            sum    += sum_in;

            ++sp;
            if (sp >= div)
                sp = 0;

            stack_ptr = &stack[sp*1];

            sum_out += stack_ptr[0];
            sum_in  -= stack_ptr[0];
        }
    }

    for (x = 0; x < w; ++x)
    {
        sum = sum_in = sum_out = 0;

        src_ptr = data.getLinePointer (0) + (unsigned int)data.pixelStride * x;

        for (i = 0; i <= radius; ++i)
        {
            stack_ptr    = &stack[i * 1];
            stack_ptr[0] = src_ptr[0];
            sum           += src_ptr[0] * (i + 1);
            sum_out       += src_ptr[0];
        }

        for (i = 1; i <= radius; ++i)
        {
            if (i <= hm)
                src_ptr += w1;

            stack_ptr = &stack[1 * (i + radius)];
            stack_ptr[0] = src_ptr[0];
            sum += src_ptr[0] * (radius + 1 - i);
            sum_in += src_ptr[0];
        }

        sp = radius;
        yp = radius;
        if (yp > hm)
            yp = hm;

        src_ptr = data.getLinePointer (int (yp)) + (unsigned int)data.pixelStride * x;
        dst_ptr = data.getLinePointer (0) + (unsigned int)data.pixelStride * x;

        for (y = 0; y < h; ++y)
        {
            dst_ptr[0] = (unsigned char)((sum * mul_sum) >> shr_sum);
            dst_ptr += w1;

            sum -= sum_out;

            stack_start = sp + div - radius;
            if (stack_start >= div)
                stack_start -= div;

            stack_ptr = &stack[1 * stack_start];

            sum_out -= stack_ptr[0];

            if (yp < hm)
            {
                src_ptr += w1;
                ++yp;
            }

            stack_ptr[0] = src_ptr[0];

            sum_in += src_ptr[0];
            sum    += sum_in;

            ++sp;
            if (sp >= div)
                sp = 0;

            stack_ptr = &stack[sp * 1];

            sum_out += stack_ptr[0];
            sum_in  -= stack_ptr[0];
        }
    }
}

static void applyStackBlurRGB (Image& src, unsigned int radius)
{
    const unsigned int w = (unsigned int)src.getWidth();
    const unsigned int h = (unsigned int)src.getHeight();

    Image::BitmapData data (src, Image::BitmapData::readWrite);

    radius = jlimit (2u, 254u, radius);

    unsigned char    stack[(254 * 2 + 1) * 3];

    unsigned int x, y, xp, yp, i, sp, stack_start;

    unsigned char* stack_ptr = nullptr;
    unsigned char* src_ptr = nullptr;
    unsigned char* dst_ptr = nullptr;

    unsigned long sum_r, sum_g, sum_b, sum_in_r, sum_in_g, sum_in_b,
                  sum_out_r, sum_out_g, sum_out_b;

    unsigned int wm = w - 1;
    unsigned int hm = h - 1;
    unsigned int w3 = (unsigned int) data.lineStride;
    unsigned int div = (unsigned int)(radius * 2) + 1;
    unsigned int mul_sum = stackblur_mul[radius];
    unsigned char shr_sum = stackblur_shr[radius];

    for (y = 0; y < h; ++y)
    {
        sum_r = sum_g = sum_b =
        sum_in_r = sum_in_g = sum_in_b =
        sum_out_r = sum_out_g = sum_out_b = 0;

        src_ptr = data.getLinePointer (int (y));

        for (i = 0; i <= radius; ++i)
        {
            stack_ptr    = &stack[3 * i];
            stack_ptr[0] = src_ptr[0];
            stack_ptr[1] = src_ptr[1];
            stack_ptr[2] = src_ptr[2];
            sum_r += src_ptr[0] * (i + 1);
            sum_g += src_ptr[1] * (i + 1);
            sum_b += src_ptr[2] * (i + 1);
            sum_out_r += src_ptr[0];
            sum_out_g += src_ptr[1];
            sum_out_b += src_ptr[2];
        }

        for (i = 1; i <= radius; ++i)
        {
            if (i <= wm)
                src_ptr += 3;

            stack_ptr = &stack[3 * (i + radius)];
            stack_ptr[0] = src_ptr[0];
            stack_ptr[1] = src_ptr[1];
            stack_ptr[2] = src_ptr[2];
            sum_r += src_ptr[0] * (radius + 1 - i);
            sum_g += src_ptr[1] * (radius + 1 - i);
            sum_b += src_ptr[2] * (radius + 1 - i);
            sum_in_r += src_ptr[0];
            sum_in_g += src_ptr[1];
            sum_in_b += src_ptr[2];
        }

        sp = radius;
        xp = radius;
        if (xp > wm)
            xp = wm;

        src_ptr = data.getLinePointer (int (y)) + (unsigned int)data.pixelStride * xp;
        dst_ptr = data.getLinePointer (int (y));

        for (x = 0; x < w; ++x)
        {
            dst_ptr[0] = (unsigned char)((sum_r * mul_sum) >> shr_sum);
            dst_ptr[1] = (unsigned char)((sum_g * mul_sum) >> shr_sum);
            dst_ptr[2] = (unsigned char)((sum_b * mul_sum) >> shr_sum);
            dst_ptr += 3;

            sum_r -= sum_out_r;
            sum_g -= sum_out_g;
            sum_b -= sum_out_b;

            stack_start = sp + div - radius;

            if (stack_start >= div)
                stack_start -= div;

            stack_ptr = &stack[3 * stack_start];

            sum_out_r -= stack_ptr[0];
            sum_out_g -= stack_ptr[1];
            sum_out_b -= stack_ptr[2];

            if (xp < wm)
            {
                src_ptr += 3;
                ++xp;
            }

            stack_ptr[0] = src_ptr[0];
            stack_ptr[1] = src_ptr[1];
            stack_ptr[2] = src_ptr[2];

            sum_in_r += src_ptr[0];
            sum_in_g += src_ptr[1];
            sum_in_b += src_ptr[2];
            sum_r    += sum_in_r;
            sum_g    += sum_in_g;
            sum_b    += sum_in_b;

            ++sp;
            if (sp >= div)
                sp = 0;

            stack_ptr = &stack[sp*3];

            sum_out_r += stack_ptr[0];
            sum_out_g += stack_ptr[1];
            sum_out_b += stack_ptr[2];
            sum_in_r  -= stack_ptr[0];
            sum_in_g  -= stack_ptr[1];
            sum_in_b  -= stack_ptr[2];
        }
    }

    for (x = 0; x < w; ++x)
    {
        sum_r =    sum_g =    sum_b =
        sum_in_r = sum_in_g = sum_in_b =
        sum_out_r = sum_out_g = sum_out_b = 0;

        src_ptr = data.getLinePointer (0) + (unsigned int)data.pixelStride * x;

        for (i = 0; i <= radius; ++i)
        {
            stack_ptr    = &stack[i * 3];
            stack_ptr[0] = src_ptr[0];
            stack_ptr[1] = src_ptr[1];
            stack_ptr[2] = src_ptr[2];
            sum_r           += src_ptr[0] * (i + 1);
            sum_g           += src_ptr[1] * (i + 1);
            sum_b           += src_ptr[2] * (i + 1);
            sum_out_r       += src_ptr[0];
            sum_out_g       += src_ptr[1];
            sum_out_b       += src_ptr[2];
        }

        for (i = 1; i <= radius; ++i)
        {
            if (i <= hm)
                src_ptr += w3;

            stack_ptr = &stack[3 * (i + radius)];
            stack_ptr[0] = src_ptr[0];
            stack_ptr[1] = src_ptr[1];
            stack_ptr[2] = src_ptr[2];
            sum_r += src_ptr[0] * (radius + 1 - i);
            sum_g += src_ptr[1] * (radius + 1 - i);
            sum_b += src_ptr[2] * (radius + 1 - i);
            sum_in_r += src_ptr[0];
            sum_in_g += src_ptr[1];
            sum_in_b += src_ptr[2];
        }

        sp = radius;
        yp = radius;
        if (yp > hm)
            yp = hm;

        src_ptr = data.getLinePointer (int (yp)) + (unsigned int)data.pixelStride * x;
        dst_ptr = data.getLinePointer (0) + (unsigned int)data.pixelStride * x;

        for (y = 0; y < h; ++y)
        {
            dst_ptr[0] = (unsigned char)((sum_r * mul_sum) >> shr_sum);
            dst_ptr[1] = (unsigned char)((sum_g * mul_sum) >> shr_sum);
            dst_ptr[2] = (unsigned char)((sum_b * mul_sum) >> shr_sum);
            dst_ptr += w3;

            sum_r -= sum_out_r;
            sum_g -= sum_out_g;
            sum_b -= sum_out_b;

            stack_start = sp + div - radius;
            if (stack_start >= div)
                stack_start -= div;

            stack_ptr = &stack[3 * stack_start];

            sum_out_r -= stack_ptr[0];
            sum_out_g -= stack_ptr[1];
            sum_out_b -= stack_ptr[2];

            if (yp < hm)
            {
                src_ptr += w3;
                ++yp;
            }

            stack_ptr[0] = src_ptr[0];
            stack_ptr[1] = src_ptr[1];
            stack_ptr[2] = src_ptr[2];

            sum_in_r += src_ptr[0];
            sum_in_g += src_ptr[1];
            sum_in_b += src_ptr[2];
            sum_r    += sum_in_r;
            sum_g    += sum_in_g;
            sum_b    += sum_in_b;

            ++sp;
            if (sp >= div)
                sp = 0;

            stack_ptr = &stack[sp * 3];

            sum_out_r += stack_ptr[0];
            sum_out_g += stack_ptr[1];
            sum_out_b += stack_ptr[2];
            sum_in_r  -= stack_ptr[0];
            sum_in_g  -= stack_ptr[1];
            sum_in_b  -= stack_ptr[2];
        }
    }
}

static void applyStackBlurARGB (Image& src, unsigned int radius)
{
    const unsigned int w = (unsigned int)src.getWidth();
    const unsigned int h = (unsigned int)src.getHeight();

    Image::BitmapData data (src, Image::BitmapData::readWrite);

    radius = jlimit (2u, 254u, radius);

    unsigned char    stack[(254 * 2 + 1) * 4];

    unsigned int x, y, xp, yp, i, sp, stack_start;

    unsigned char* stack_ptr = nullptr;
    unsigned char* src_ptr = nullptr;
    unsigned char* dst_ptr = nullptr;

    unsigned long sum_r, sum_g, sum_b, sum_a, sum_in_r, sum_in_g, sum_in_b, sum_in_a,
    sum_out_r, sum_out_g, sum_out_b, sum_out_a;

    unsigned int wm = w - 1;
    unsigned int hm = h - 1;
    unsigned int w4 = (unsigned int) data.lineStride;
    unsigned int div = (unsigned int)(radius * 2) + 1;
    unsigned int mul_sum = stackblur_mul[radius];
    unsigned char shr_sum = stackblur_shr[radius];

    for (y = 0; y < h; ++y)
    {
        sum_r = sum_g = sum_b = sum_a =
        sum_in_r = sum_in_g = sum_in_b = sum_in_a =
        sum_out_r = sum_out_g = sum_out_b = sum_out_a = 0;

        src_ptr = data.getLinePointer (int (y));

        for (i = 0; i <= radius; ++i)
        {
            stack_ptr    = &stack[4 * i];
            stack_ptr[0] = src_ptr[0];
            stack_ptr[1] = src_ptr[1];
            stack_ptr[2] = src_ptr[2];
            stack_ptr[3] = src_ptr[3];
            sum_r += src_ptr[0] * (i + 1);
            sum_g += src_ptr[1] * (i + 1);
            sum_b += src_ptr[2] * (i + 1);
            sum_a += src_ptr[3] * (i + 1);
            sum_out_r += src_ptr[0];
            sum_out_g += src_ptr[1];
            sum_out_b += src_ptr[2];
            sum_out_a += src_ptr[3];
        }

        for (i = 1; i <= radius; ++i)
        {
            if (i <= wm)
                src_ptr += 4;

            stack_ptr = &stack[4 * (i + radius)];
            stack_ptr[0] = src_ptr[0];
            stack_ptr[1] = src_ptr[1];
            stack_ptr[2] = src_ptr[2];
            stack_ptr[3] = src_ptr[3];
            sum_r += src_ptr[0] * (radius + 1 - i);
            sum_g += src_ptr[1] * (radius + 1 - i);
            sum_b += src_ptr[2] * (radius + 1 - i);
            sum_a += src_ptr[3] * (radius + 1 - i);
            sum_in_r += src_ptr[0];
            sum_in_g += src_ptr[1];
            sum_in_b += src_ptr[2];
            sum_in_a += src_ptr[3];
        }

        sp = radius;
        xp = radius;
        if (xp > wm)
            xp = wm;

        src_ptr = data.getLinePointer (int (y)) + (unsigned int)data.pixelStride * xp;
        dst_ptr = data.getLinePointer (int (y));

        for (x = 0; x < w; ++x)
        {
            dst_ptr[0] = (unsigned char)((sum_r * mul_sum) >> shr_sum);
            dst_ptr[1] = (unsigned char)((sum_g * mul_sum) >> shr_sum);
            dst_ptr[2] = (unsigned char)((sum_b * mul_sum) >> shr_sum);
            dst_ptr[3] = (unsigned char)((sum_a * mul_sum) >> shr_sum);
            dst_ptr += 4;

            sum_r -= sum_out_r;
            sum_g -= sum_out_g;
            sum_b -= sum_out_b;
            sum_a -= sum_out_a;

            stack_start = sp + div - radius;

            if (stack_start >= div)
                stack_start -= div;

            stack_ptr = &stack[4 * stack_start];

            sum_out_r -= stack_ptr[0];
            sum_out_g -= stack_ptr[1];
            sum_out_b -= stack_ptr[2];
            sum_out_a -= stack_ptr[3];

            if (xp < wm)
            {
                src_ptr += 4;
                ++xp;
            }

            stack_ptr[0] = src_ptr[0];
            stack_ptr[1] = src_ptr[1];
            stack_ptr[2] = src_ptr[2];
            stack_ptr[3] = src_ptr[3];

            sum_in_r += src_ptr[0];
            sum_in_g += src_ptr[1];
            sum_in_b += src_ptr[2];
            sum_in_a += src_ptr[3];
            sum_r    += sum_in_r;
            sum_g    += sum_in_g;
            sum_b    += sum_in_b;
            sum_a    += sum_in_a;

            ++sp;
            if (sp >= div)
                sp = 0;

            stack_ptr = &stack[sp*4];

            sum_out_r += stack_ptr[0];
            sum_out_g += stack_ptr[1];
            sum_out_b += stack_ptr[2];
            sum_out_a += stack_ptr[3];
            sum_in_r  -= stack_ptr[0];
            sum_in_g  -= stack_ptr[1];
            sum_in_b  -= stack_ptr[2];
            sum_in_a  -= stack_ptr[3];
        }
    }

    for (x = 0; x < w; ++x)
    {
        sum_r =    sum_g =    sum_b =    sum_a =
        sum_in_r = sum_in_g = sum_in_b = sum_in_a =
        sum_out_r = sum_out_g = sum_out_b = sum_out_a = 0;

        src_ptr = data.getLinePointer (0) + (unsigned int)data.pixelStride * x;

        for (i = 0; i <= radius; ++i)
        {
            stack_ptr    = &stack[i * 4];
            stack_ptr[0] = src_ptr[0];
            stack_ptr[1] = src_ptr[1];
            stack_ptr[2] = src_ptr[2];
            stack_ptr[3] = src_ptr[3];
            sum_r           += src_ptr[0] * (i + 1);
            sum_g           += src_ptr[1] * (i + 1);
            sum_b           += src_ptr[2] * (i + 1);
            sum_a           += src_ptr[3] * (i + 1);
            sum_out_r       += src_ptr[0];
            sum_out_g       += src_ptr[1];
            sum_out_b       += src_ptr[2];
            sum_out_a       += src_ptr[3];
        }

        for (i = 1; i <= radius; ++i)
        {
            if (i <= hm)
                src_ptr += w4;

            stack_ptr = &stack[4 * (i + radius)];
            stack_ptr[0] = src_ptr[0];
            stack_ptr[1] = src_ptr[1];
            stack_ptr[2] = src_ptr[2];
            stack_ptr[3] = src_ptr[3];
            sum_r += src_ptr[0] * (radius + 1 - i);
            sum_g += src_ptr[1] * (radius + 1 - i);
            sum_b += src_ptr[2] * (radius + 1 - i);
            sum_a += src_ptr[3] * (radius + 1 - i);
            sum_in_r += src_ptr[0];
            sum_in_g += src_ptr[1];
            sum_in_b += src_ptr[2];
            sum_in_a += src_ptr[3];
        }

        sp = radius;
        yp = radius;
        if (yp > hm)
            yp = hm;

        src_ptr = data.getLinePointer (int (yp)) + (unsigned int)data.pixelStride * x;
        dst_ptr = data.getLinePointer (0) + (unsigned int)data.pixelStride * x;

        for (y = 0; y < h; ++y)
        {
            dst_ptr[0] = (unsigned char)((sum_r * mul_sum) >> shr_sum);
            dst_ptr[1] = (unsigned char)((sum_g * mul_sum) >> shr_sum);
            dst_ptr[2] = (unsigned char)((sum_b * mul_sum) >> shr_sum);
            dst_ptr[3] = (unsigned char)((sum_a * mul_sum) >> shr_sum);
            dst_ptr += w4;

            sum_r -= sum_out_r;
            sum_g -= sum_out_g;
            sum_b -= sum_out_b;
            sum_a -= sum_out_a;

            stack_start = sp + div - radius;
            if (stack_start >= div)
                stack_start -= div;

            stack_ptr = &stack[4 * stack_start];

            sum_out_r -= stack_ptr[0];
            sum_out_g -= stack_ptr[1];
            sum_out_b -= stack_ptr[2];
            sum_out_a -= stack_ptr[3];

            if (yp < hm)
            {
                src_ptr += w4;
                ++yp;
            }

            stack_ptr[0] = src_ptr[0];
            stack_ptr[1] = src_ptr[1];
            stack_ptr[2] = src_ptr[2];
            stack_ptr[3] = src_ptr[3];

            sum_in_r += src_ptr[0];
            sum_in_g += src_ptr[1];
            sum_in_b += src_ptr[2];
            sum_in_a += src_ptr[3];
            sum_r    += sum_in_r;
            sum_g    += sum_in_g;
            sum_b    += sum_in_b;
            sum_a    += sum_in_a;

            ++sp;
            if (sp >= div)
                sp = 0;

            stack_ptr = &stack[sp * 4];

            sum_out_r += stack_ptr[0];
            sum_out_g += stack_ptr[1];
            sum_out_b += stack_ptr[2];
            sum_out_a += stack_ptr[3];
            sum_in_r  -= stack_ptr[0];
            sum_in_g  -= stack_ptr[1];
            sum_in_b  -= stack_ptr[2];
            sum_in_a  -= stack_ptr[3];
        }
    }
}

// The Stack Blur Algorithm was invented by Mario Klingemann,
// mario@quasimondo.com and described here:
// http://incubator.quasimondo.com/processing/fast_blur_deluxe.php

// Stackblur algorithm by Mario Klingemann
// Details here:
// http://www.quasimondo.com/StackBlurForCanvas/StackBlurDemo.html
// C++ implemenation base from:
// https://gist.github.com/benjamin9999/3809142
// http://www.antigrain.com/__code/include/agg_blur.h.html
void applyStackBlur (Image& src, unsigned int radius)
{
    if (src.getFormat() == Image::ARGB)          applyStackBlurARGB (src, radius);
    if (src.getFormat() == Image::RGB)           applyStackBlurRGB (src, radius);
    if (src.getFormat() == Image::SingleChannel) applyStackBlurBW (src, radius);
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

   #ifdef JUCE_MAC
    if (SystemStats::hasSSE())
    {
        avir::CImageResizer<avir::fpclass_float4> imageResizer (8);
        imageResizer.resizeImage (rawSrc, src.getWidth(), src.getHeight(), 0,
                                    rawDst, dst.getWidth(), dst.getHeight(), channels, 0);
    }
    else
   #endif
    {
        avir::CImageResizer<> imageResizer (8);
        imageResizer.resizeImage (rawSrc, src.getWidth(), src.getHeight(), 0,
                                    rawDst, dst.getWidth(), dst.getHeight(), channels, 0);
    }

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
void applyGradientMap (Image& src, const ColourGradient& gradient, int maxThreads = -1)
{
    const int w = src.getWidth();
    const int h = src.getHeight();
    const int numThreads = (w >= 256 || h >= 256) ? maxThreads : 1;

    Image::BitmapData data (src, Image::BitmapData::readWrite);

    multiThreadedFor<int> (0, h, 1, numThreads, [&] (int y)
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

void applyGradientMap (Image& src, const ColourGradient& gradient, int maxThreads)
{
    if (src.getFormat() == Image::ARGB)          applyGradientMap<PixelARGB> (src, gradient, maxThreads);
    else if (src.getFormat() == Image::RGB)      applyGradientMap<PixelRGB>  (src, gradient, maxThreads);
    else jassertfalse;
}

void applyGradientMap (Image& src, const Colour c1, const Colour c2, int maxThreads)
{
    ColourGradient g;
    g.addColour (0.0, c1);
    g.addColour (1.0, c2);

    applyGradientMap (src, g, maxThreads);
}

//==============================================================================

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
void applyBlend (Image& dst, const Image& src, float alpha, int maxThreads)
{
    const int w = jmin (src.getWidth(), dst.getWidth());
    const int h = jmin (src.getHeight(), dst.getWidth());
    
    const int numThreads = (w >= 256 || h >= 256) ? maxThreads : 1;

    Image::BitmapData srcData (src, Image::BitmapData::readOnly);
    Image::BitmapData dstData (dst, Image::BitmapData::readWrite);

    multiThreadedFor<int> (0, h, 1, numThreads, [&] (int y)
                           {
                               uint8* pSrc = srcData.getLinePointer (y);
                               uint8* pDst = dstData.getLinePointer (y);

                               for (int x = 0; x < w; x++)
                               {
                                   T* a = (T*)pSrc;
                                   T* b = (T*)pDst;

                                   uint8 ar = a->getRed();
                                   uint8 ag = a->getGreen();
                                   uint8 ab = a->getBlue();
                                   uint8 aa = a->getAlpha();

                                   ignoreUnused (aa);

                                   uint8 br = b->getRed();
                                   uint8 bg = b->getGreen();
                                   uint8 bb = b->getBlue();
                                   uint8 ba = b->getAlpha();

                                   br = channelBlendAlpha (F (ar, br), br, alpha);
                                   bg = channelBlendAlpha (F (ag, bg), bg, alpha);
                                   bb = channelBlendAlpha (F (ab, bb), bb, alpha);

                                   b->setARGB (ba, br, bg, bb);

                                   pSrc += srcData.pixelStride;
                                   pDst += dstData.pixelStride;
                               }
                           });
}

template <class T>
void applyBlend (Image& dst, const Image& src, BlendMode mode, float alpha, int maxThreads)
{
    switch (mode)
    {
        case Normal: applyBlend<T, channelBlendNormal> (dst, src, alpha, maxThreads); break;
        case Lighten: applyBlend<T, channelBlendLighten> (dst, src, alpha, maxThreads); break;
        case Darken: applyBlend<T, channelBlendDarken> (dst, src, alpha, maxThreads); break;
        case Multiply: applyBlend<T, channelBlendMultiply> (dst, src, alpha, maxThreads); break;
        case Average: applyBlend<T, channelBlendAverage> (dst, src, alpha, maxThreads); break;
        case Add: applyBlend<T, channelBlendAdd> (dst, src, alpha, maxThreads); break;
        case Subtract: applyBlend<T, channelBlendSubtract> (dst, src, alpha, maxThreads); break;
        case Difference: applyBlend<T, channelBlendDifference> (dst, src, alpha, maxThreads); break;
        case Negation: applyBlend<T, channelBlendNegation> (dst, src, alpha, maxThreads); break;
        case Screen: applyBlend<T, channelBlendScreen> (dst, src, alpha, maxThreads); break;
        case Exclusion: applyBlend<T, channelBlendExclusion> (dst, src, alpha, maxThreads); break;
        case Overlay: applyBlend<T, channelBlendOverlay> (dst, src, alpha, maxThreads); break;
        case SoftLight: applyBlend<T, channelBlendSoftLight> (dst, src, alpha, maxThreads); break;
        case HardLight: applyBlend<T, channelBlendHardLight> (dst, src, alpha, maxThreads); break;
        case ColorDodge: applyBlend<T, channelBlendColorDodge> (dst, src, alpha, maxThreads); break;
        case ColorBurn: applyBlend<T, channelBlendColorBurn> (dst, src, alpha, maxThreads); break;
        case LinearDodge: applyBlend<T, channelBlendLinearDodge> (dst, src, alpha, maxThreads); break;
        case LinearBurn: applyBlend<T, channelBlendLinearBurn> (dst, src, alpha, maxThreads); break;
        case LinearLight: applyBlend<T, channelBlendLinearLight> (dst, src, alpha, maxThreads); break;
        case VividLight: applyBlend<T, channelBlendVividLight> (dst, src, alpha, maxThreads); break;
        case PinLight: applyBlend<T, channelBlendPinLight> (dst, src, alpha, maxThreads); break;
        case HardMix: applyBlend<T, channelBlendHardMix> (dst, src, alpha, maxThreads); break;
        case Reflect: applyBlend<T, channelBlendReflect> (dst, src, alpha, maxThreads); break;
        case Glow: applyBlend<T, channelBlendGlow> (dst, src, alpha, maxThreads); break;
        case Phoenix: applyBlend<T, channelBlendPhoenix> (dst, src, alpha, maxThreads); break;
    }
}

void applyBlend (Image& dst, const Image& src, BlendMode mode, float alpha, int maxThreads)
{
    if (src.getFormat() != dst.getFormat())
        dst = dst.convertedToFormat (src.getFormat());

    if (src.getFormat() == Image::ARGB)          applyBlend<PixelARGB> (dst, src, mode, alpha, maxThreads);
    else if (src.getFormat() == Image::RGB)      applyBlend<PixelRGB>  (dst, src, mode, alpha, maxThreads);
    else jassertfalse;
}
