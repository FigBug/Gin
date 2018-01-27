/*==============================================================================
 
 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com
 
 ==============================================================================*/

Image applyVignette (Image src, float amountIn, float radiusIn, float fallOff)
{
    const int w = src.getWidth();
    const int h = src.getHeight();
    
    jassert (src.getFormat() == Image::ARGB);
    if (src.getFormat() != Image::ARGB)
        return Image();
    
    Image dst (Image::ARGB, w, h, true);
    
    double outA = w * 0.5 * radiusIn;
    double outB = h * 0.5 * radiusIn;
    
    double inA = outA * fallOff;
    double inB = outB * fallOff;
    
    double cx = w * 0.5;
    double cy = h * 0.5;
    
    double amount = 1.0 - amountIn;

    Image::BitmapData srcData (src, Image::BitmapData::readOnly);
    Image::BitmapData dstData (dst, Image::BitmapData::writeOnly);
    
    Ellipse<double> outE { outA, outB };
    Ellipse<double> inE  { inA,  inB  };
    
    for (int y = 0; y < h; y++)
    {
        uint8* ps = srcData.getLinePointer (y);
        uint8* ds = dstData.getLinePointer (y);

        double dy = y - cy;
        
        for (int x = 0; x < w; x++)
        {
            double dx = x - cx;

            double outside = outE.isPointOutside ({dx, dy});
            double inside  = inE.isPointInside ({dx, dy});
            
            PixelARGB* s = (PixelARGB*)ps;
            PixelARGB* d = (PixelARGB*)ds;
            
            if (inside)
            {
                d->set (*s);
            }
            else if (outside)
            {
                uint8 r = (uint8)(0.5 + (s->getRed() * amount));
                uint8 g = (uint8)(0.5 + (s->getGreen() * amount));
                uint8 b = (uint8)(0.5 + (s->getBlue() * amount));
                uint8 a = s->getAlpha();
                
                d->setARGB (a, r, g, b);
            }
            else
            {
                double angle = std::atan2 (dy, dx);
                
                auto p1 = outE.pointAtAngle (angle);
                auto p2 = inE.pointAtAngle (angle);
                
                auto l1 = Line<double> ({dx,dy}, p2);
                auto l2 = Line<double> (p1, p2);
                
                double factor = 1.0 - (amountIn * jlimit (0.0, 1.0, l1.getLength() / l2.getLength()));
                
                uint8 r = (uint8)(0.5 + (s->getRed() * factor));
                uint8 g = (uint8)(0.5 + (s->getGreen() * factor));
                uint8 b = (uint8)(0.5 + (s->getBlue() * factor));
                uint8 a = s->getAlpha();
                
                d->setARGB (a, r, g, b);
            }
            
            ps += srcData.pixelStride;
            ds += srcData.pixelStride;
        }
    }
    return dst;
}

Image applySepia (Image src)
{
    const int w = src.getWidth();
    const int h = src.getHeight();
    
    jassert (src.getFormat() == Image::ARGB);
    if (src.getFormat() != Image::ARGB)
        return Image();
    
    Image dst (Image::ARGB, w, h, true);
    
    Image::BitmapData srcData (src, Image::BitmapData::readOnly);
    Image::BitmapData dstData (dst, Image::BitmapData::writeOnly);
    
    for (int y = 0; y < h; y++)
    {
        uint8* ps = srcData.getLinePointer (y);
        uint8* ds = dstData.getLinePointer (y);
        
        for (int x = 0; x < w; x++)
        {
            PixelARGB* s = (PixelARGB*)ps;
            PixelARGB* d = (PixelARGB*)ds;
            
            uint8 r = s->getRed();
            uint8 g = s->getGreen();
            uint8 b = s->getBlue();
            uint8 a = s->getAlpha();
            
            uint8 ro = jlimit (0.0, 255.0, (r * .393) + (g *.769) + (b * .189));
            uint8 go = jlimit (0.0, 255.0, (r * .349) + (g *.686) + (b * .168));
            uint8 bo = jlimit (0.0, 255.0, (r * .272) + (g *.534) + (b * .131));

            d->setARGB (a, ro, go, bo);
            
            ps += srcData.pixelStride;
            ds += srcData.pixelStride;
        }
    }
    return dst;
}

Image applyGreyScale (Image src)
{
    const int w = src.getWidth();
    const int h = src.getHeight();

    if (src.getFormat() != Image::ARGB)
        return Image();
    
    Image dst (Image::ARGB, w, h, true);
    
    Image::BitmapData srcData (src, Image::BitmapData::readOnly);
    Image::BitmapData dstData (dst, Image::BitmapData::writeOnly);
    
    for (int y = 0; y < h; y++)
    {
        uint8* ps = srcData.getLinePointer (y);
        uint8* ds = dstData.getLinePointer (y);
        
        for (int x = 0; x < w; x++)
        {
            PixelARGB* s = (PixelARGB*)ps;
            PixelARGB* d = (PixelARGB*)ds;
            
            uint8 r = s->getRed();
            uint8 g = s->getGreen();
            uint8 b = s->getBlue();
            uint8 a = s->getAlpha();
            
            uint8 ro = jlimit (0.0, 255.0, r * 0.30 + 0.5);
            uint8 go = jlimit (0.0, 255.0, g * 0.59 + 0.5);
            uint8 bo = jlimit (0.0, 255.0, b * 0.11 + 0.5);
            
            d->setARGB (a, ro, go, bo);
            
            ps += srcData.pixelStride;
            ds += srcData.pixelStride;
        }
    }
    return dst;
}

Image applySoften (Image src)
{
    const int w = src.getWidth();
    const int h = src.getHeight();

    if (src.getFormat() != Image::ARGB)
        return Image();
    
    Image dst (Image::ARGB, w, h, true);
    
    Image::BitmapData srcData (src, Image::BitmapData::readOnly);
    Image::BitmapData dstData (dst, Image::BitmapData::writeOnly);
    
    for (int y = 1; y < h - 1; y++)
    {
        for (int x = 1; x < w - 1; x++)
        {
            int ro = 0, go = 0, bo = 0;
            uint8 a = 0;
            
            for (int m = -1; m <= 1; m++)
            {
                for (int n = -1; n <= 1; n++)
                {
                    PixelARGB* s = (PixelARGB*) srcData.getPixelPointer (x + m, y + n);

                    ro += s->getRed();
                    go += s->getGreen();
                    bo += s->getBlue();
                }
            }
            
            PixelARGB* s = (PixelARGB*) srcData.getPixelPointer (x, y);
            a = s->getAlpha();
            
            PixelARGB* d = (PixelARGB*) dstData.getPixelPointer (x, y);
            
            d->setARGB (a, ro / 9, go / 9, bo / 9);
        }
    }
    return dst;
}

Image applySharpen (Image src)
{
    const int w = src.getWidth();
    const int h = src.getHeight();
    
    if (src.getFormat() != Image::ARGB)
        return Image();
    
    Image dst (Image::ARGB, w, h, true);
    
    Image::BitmapData srcData (src, Image::BitmapData::readOnly);
    Image::BitmapData dstData (dst, Image::BitmapData::writeOnly);
    
    for (int y = 1; y < h - 1; y++)
    {
        for (int x = 1; x < w - 1; x++)
        {
            int ro = 0, go = 0, bo = 0;
            uint8 ao = 0;
            
            PixelARGB* s = (PixelARGB*) srcData.getPixelPointer (x, y);
            
            ro = s->getRed()   * 5;
            go = s->getGreen() * 5;
            bo = s->getBlue()  * 5;
            ao = s->getAlpha();
            
            s = (PixelARGB*) srcData.getPixelPointer (x, y - 1);
            ro -= s->getRed();
            go -= s->getGreen();
            bo -= s->getBlue();

            s = (PixelARGB*) srcData.getPixelPointer (x - 1, y);
            ro -= s->getRed();
            go -= s->getGreen();
            bo -= s->getBlue();

            s = (PixelARGB*) srcData.getPixelPointer (x + 1, y);
            ro -= s->getRed();
            go -= s->getGreen();
            bo -= s->getBlue();

            s = (PixelARGB*) srcData.getPixelPointer (x, y + 1);
            ro -= s->getRed();
            go -= s->getGreen();
            bo -= s->getBlue();

            PixelARGB* d = (PixelARGB*) dstData.getPixelPointer (x, y);
            
            d->setARGB (ao, ro / 9, go / 9, bo / 9);
        }
    }
    return dst;
}

Image applyGamma (Image src, float gamma)
{
    const int w = src.getWidth();
    const int h = src.getHeight();

    if (src.getFormat() != Image::ARGB)
        return Image();
    
    Image dst (Image::ARGB, w, h, true);
    
    Image::BitmapData srcData (src, Image::BitmapData::readOnly);
    Image::BitmapData dstData (dst, Image::BitmapData::writeOnly);
    
    for (int y = 0; y < h; y++)
    {
        uint8* ps = srcData.getLinePointer (y);
        uint8* ds = dstData.getLinePointer (y);
        
        for (int x = 0; x < w; x++)
        {
            PixelARGB* s = (PixelARGB*)ps;
            PixelARGB* d = (PixelARGB*)ds;
            
            uint8 r = s->getRed();
            uint8 g = s->getGreen();
            uint8 b = s->getBlue();
            uint8 a = s->getAlpha();
            
            uint8 ro = std::pow (r / 255.0, gamma) * 255.0 + 0.5;
            uint8 go = std::pow (g / 255.0, gamma) * 255.0 + 0.5;
            uint8 bo = std::pow (b / 255.0, gamma) * 255.0 + 0.5;
            
            d->setARGB (a, ro, go, bo);
            
            ps += srcData.pixelStride;
            ds += srcData.pixelStride;
        }
    }
    return dst;
}

Image applyInvert (Image src)
{
    const int w = src.getWidth();
    const int h = src.getHeight();

    if (src.getFormat() != Image::ARGB)
        return Image();
    
    Image dst (Image::ARGB, w, h, true);
    
    Image::BitmapData srcData (src, Image::BitmapData::readOnly);
    Image::BitmapData dstData (dst, Image::BitmapData::writeOnly);
    
    for (int y = 0; y < h; y++)
    {
        uint8* ps = srcData.getLinePointer (y);
        uint8* ds = dstData.getLinePointer (y);
        
        for (int x = 0; x < w; x++)
        {
            PixelARGB* s = (PixelARGB*)ps;
            PixelARGB* d = (PixelARGB*)ds;
            
            uint8 r = s->getRed();
            uint8 g = s->getGreen();
            uint8 b = s->getBlue();
            uint8 a = s->getAlpha();
            
            uint8 ro = 255 - r;
            uint8 go = 255 - g;
            uint8 bo = 255 - b;
            
            d->setARGB (a, ro, go, bo);
            
            ps += srcData.pixelStride;
            ds += srcData.pixelStride;
        }
    }
    return dst;
}
