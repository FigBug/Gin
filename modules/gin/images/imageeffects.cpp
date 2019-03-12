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

inline PixelARGB blend (const PixelARGB& c1, const PixelARGB& c2)
{
    int a = c1.getAlpha();
    int invA = 255 - a;

    int r = ((c2.getRed()   * invA) + (c1.getRed()   * a)) / 256;
    int g = ((c2.getGreen() * invA) + (c1.getGreen() * a)) / 256;
    int b = ((c2.getBlue()  * invA) + (c1.getBlue()  * a)) / 256;
    uint8 a2 = computeAlpha (c2.getAlpha(), c1.getAlpha());

    return PixelARGB (a2, toByte (r), toByte (g), toByte (b));
}

//==============================================================================
void applyVignette (Image& src, float amountIn, float radiusIn, float fallOff)
{
    const int w = src.getWidth();
    const int h = src.getHeight();

    jassert (src.getFormat() == Image::ARGB);
    if (src.getFormat() != Image::ARGB)
        return;

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

    multiThreadedFor<int> (0, h, 1, [&] (int y)
    {
        uint8* p = data.getLinePointer (y);

        double dy = y - cy;

        for (int x = 0; x < w; x++)
        {
            double dx = x - cx;

            bool outside = outE.isPointOutside ({dx, dy});
            bool inside  = inE.isPointInside ({dx, dy});

            PixelARGB* s = (PixelARGB*)p;

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

void applySepia (Image& src)
{
    const int w = src.getWidth();
    const int h = src.getHeight();

    jassert (src.getFormat() == Image::ARGB);
    if (src.getFormat() != Image::ARGB)
        return;

    Image::BitmapData data (src, Image::BitmapData::readWrite);

    multiThreadedFor<int> (0, h, 1, [&] (int y)
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

void applyGreyScale (Image& src)
{
    const int w = src.getWidth();
    const int h = src.getHeight();

    if (src.getFormat() != Image::ARGB)
        return;

    Image::BitmapData data (src, Image::BitmapData::readWrite);

    multiThreadedFor<int> (0, h, 1, [&] (int y)
    {
        uint8* p = data.getLinePointer (y);

        for (int x = 0; x < w; x++)
        {
            PixelARGB* s = (PixelARGB*)p;

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

void applySoften (Image& src)
{
    const int w = src.getWidth();
    const int h = src.getHeight();

    if (src.getFormat() != Image::ARGB)
        return;

    Image dst (Image::ARGB, w, h, true);

    Image::BitmapData srcData (src, Image::BitmapData::readOnly);
    Image::BitmapData dstData (dst, Image::BitmapData::writeOnly);

    multiThreadedFor<int> (0, h, 1, [&] (int y)
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

                    PixelARGB* s = (PixelARGB*) srcData.getPixelPointer (cx, cy);

                    ro += s->getRed();
                    go += s->getGreen();
                    bo += s->getBlue();
                }
            }

            PixelARGB* s = (PixelARGB*) srcData.getPixelPointer (x, y);
            a = s->getAlpha();

            PixelARGB* d = (PixelARGB*) dstData.getPixelPointer (x, y);

            d->setARGB (a, toByte (ro / 9), toByte (go / 9), toByte (bo / 9));
        }
    });
    src = dst;
}

void applySharpen (Image& src)
{
    const int w = src.getWidth();
    const int h = src.getHeight();

    if (src.getFormat() != Image::ARGB)
        return;

    Image dst (Image::ARGB, w, h, true);

    Image::BitmapData srcData (src, Image::BitmapData::readOnly);
    Image::BitmapData dstData (dst, Image::BitmapData::writeOnly);

    multiThreadedFor<int> (0, h, 1, [&] (int y)
    {
        for (int x = 0; x < w; x++)
        {
            auto getPixelPointer = [&] (int cx, int cy) -> PixelARGB*
            {
                cx = jlimit (0, w - 1, cx);
                cy = jlimit (0, h - 1, cy);

                return (PixelARGB*) srcData.getPixelPointer (cx, cy);
            };

            int ro = 0, go = 0, bo = 0;
            uint8 ao = 0;

            PixelARGB* s = getPixelPointer (x, y);

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

            PixelARGB* d = (PixelARGB*) dstData.getPixelPointer (x, y);

            d->setARGB (ao, toByte (ro), toByte (go), toByte (bo));
        }
    });
    src = dst;
}

void applyGamma (Image& src, float gamma)
{
    const int w = src.getWidth();
    const int h = src.getHeight();

    if (src.getFormat() != Image::ARGB)
        return;

    Image::BitmapData data (src, Image::BitmapData::readWrite);

    multiThreadedFor<int> (0, h, 1, [&] (int y)
    {
        uint8* p = data.getLinePointer (y);

        for (int x = 0; x < w; x++)
        {
            PixelARGB* s = (PixelARGB*)p;

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

void applyInvert (Image& src)
{
    const int w = src.getWidth();
    const int h = src.getHeight();

    if (src.getFormat() != Image::ARGB)
        return;

    Image::BitmapData data (src, Image::BitmapData::readWrite);

    multiThreadedFor<int> (0, h, 1, [&] (int y)
    {
        uint8* p = data.getLinePointer (y);

        for (int x = 0; x < w; x++)
        {
            PixelARGB* s = (PixelARGB*)p;

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

void applyContrast (Image& src, float contrast)
{
    const int w = src.getWidth();
    const int h = src.getHeight();

    if (src.getFormat() != Image::ARGB)
        return;

    contrast = (100.0f + contrast) / 100.0f;
    contrast = square (contrast);

    Image::BitmapData data (src, Image::BitmapData::readWrite);

    multiThreadedFor<int> (0, h, 1, [&] (int y)
    {
        uint8* p = data.getLinePointer (y);

        for (int x = 0; x < w; x++)
        {
            PixelARGB* s = (PixelARGB*)p;

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

void applyBrightnessContrast (Image& src, float brightness, float contrast)
{
    const int w = src.getWidth();
    const int h = src.getHeight();

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

    multiThreadedFor<int> (0, h, 1, [&] (int y)
    {
        uint8* p = data.getLinePointer (y);

        for (int x = 0; x < w; x++)
        {
            PixelARGB* s = (PixelARGB*)p;

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

void applyHueSaturationLightness (Image& src, float hueIn, float saturation, float lightness)
{
    const int w = src.getWidth();
    const int h = src.getHeight();

    if (src.getFormat() != Image::ARGB)
        return;

    if (saturation > 100)
        saturation = ((saturation - 100) * 3) + 100;
    saturation = (saturation * 1024) / 100;

    hueIn /= 360.0f;

    Image::BitmapData data (src, Image::BitmapData::readWrite);

    multiThreadedFor<int> (0, h, 1, [&] (int y)
    {
        uint8* p = data.getLinePointer (y);

        for (int x = 0; x < w; x++)
        {
            PixelARGB* s = (PixelARGB*)p;

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
                *s = blend (PixelARGB (toByte ((lightness * 255) / 100), 255, 255, 255), *s);
            else if (lightness < 0)
                *s = blend (PixelARGB (toByte ((-lightness * 255) / 100), 0, 0, 0), *s);

            p += data.pixelStride;
        }
    });
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
    unsigned int w1 = w * 1;
    unsigned int div = (unsigned int)(radius * 2) + 1;
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
                src_ptr += w1;        // +stride
            
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
                src_ptr += w1;            // stride
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
    unsigned int w3 = w * 3;
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
                src_ptr += w3;        // +stride
            
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
                src_ptr += w3;            // stride
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
    unsigned int w4 = w * 4;
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
                src_ptr += w4;        // +stride
            
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
                src_ptr += w4;            // stride
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
