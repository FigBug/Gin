/*==============================================================================
 
 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com
 
 ==============================================================================*/

Image applyVignette (Image src, float amountIn, float radiusIn)
{
    const int w = src.getWidth();
    const int h = src.getHeight();
    
    jassert (src.getFormat() == Image::ARGB);
    if (src.getFormat() != Image::ARGB)
        return Image();
    
    Image dst (Image::ARGB, w, h, true);
    
    double fallOff = 0.5;
    
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
