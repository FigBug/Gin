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
    
    double xCentreOffset = w * 0.5;
    double yCentreOffset = h * 0.5;
    
    double radius = jmax (w, h) * 0.5;
    radius *= radiusIn;
    radius *= radius;
    
    double radiusR = double_Pi / (8 * radius);
    double amount = 1.0 - amountIn;

    Image::BitmapData srcData (src, Image::BitmapData::readOnly);
    Image::BitmapData dstData (dst, Image::BitmapData::writeOnly);
    
    for (int y = 0; y < h; y++)
    {
        uint8* ps = srcData.getLinePointer (y);
        uint8* ds = dstData.getLinePointer (y);
        
        double iy2 = y - yCentreOffset;
        iy2 *= iy2;
        
        for (int x = 0; x < w; x++)
        {
            double ix = x - xCentreOffset;
            double dd = (iy2 + (ix * ix)) * radiusR;
            double factor = std::cos (dd);
            
            PixelARGB* s = (PixelARGB*)ps;
            PixelARGB* d = (PixelARGB*)ds;
            
            if (factor <= 0 || dd > double_Pi)
            {
                uint8 r = (uint8)(0.5 + (s->getRed() * amount));
                uint8 g = (uint8)(0.5 + (s->getGreen() * amount));
                uint8 b = (uint8)(0.5 + (s->getBlue() * amount));
                uint8 a = s->getAlpha();
                
                d->setARGB (a, r, g, b);
            }
            else
            {
                factor *= factor;
                factor *= factor;
                factor = amount + (amountIn * factor);
                
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
