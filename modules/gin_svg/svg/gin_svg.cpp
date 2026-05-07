/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

struct ParsedSVG
{
    ParsedSVG (const juce::String& svgText)
        : data (svgText.toStdString())
    {
        image = nsvgParse (data.data(), "px", 96.0f);
    }

    ~ParsedSVG()
    {
        if (image != nullptr)
            nsvgDelete (image);
    }

    bool isValid() const { return image != nullptr && image->shapes != nullptr; }

    std::string data;
    NSVGimage* image = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParsedSVG)
};

static juce::Colour nsvgColour (unsigned int c, float opacity)
{
    return juce::Colour (juce::uint8 ((c >> 0) & 0xFF),
                         juce::uint8 ((c >> 8) & 0xFF),
                         juce::uint8 ((c >> 16) & 0xFF),
                         juce::uint8 (((c >> 24) & 0xFF) * opacity));
}

static juce::Path buildPath (NSVGshape* shape)
{
    juce::Path result;

    for (auto* path = shape->paths; path != nullptr; path = path->next)
    {
        if (path->npts < 1)
            continue;

        result.startNewSubPath (path->pts[0], path->pts[1]);

        for (int i = 1; i < path->npts - 2; i += 3)
        {
            auto* p = &path->pts[i * 2];
            result.cubicTo (p[0], p[1], p[2], p[3], p[4], p[5]);
        }

        if (path->closed)
            result.closeSubPath();
    }

    if (shape->fillRule == NSVG_FILLRULE_EVENODD)
        result.setUsingNonZeroWinding (false);

    return result;
}

static juce::ColourGradient buildGradient (const NSVGpaint& paint)
{
    auto* grad = paint.gradient;

    auto inverseXform = [](const float* t, float* inv)
    {
        auto det = t[0] * t[3] - t[2] * t[1];
        if (std::abs (det) < 1e-6f)
        {
            std::memset (inv, 0, 6 * sizeof (float));
            return;
        }
        auto invdet = 1.0f / det;
        inv[0] =  t[3] * invdet;
        inv[2] = -t[2] * invdet;
        inv[4] = (t[2] * t[5] - t[3] * t[4]) * invdet;
        inv[1] = -t[1] * invdet;
        inv[3] =  t[0] * invdet;
        inv[5] = (t[1] * t[4] - t[0] * t[5]) * invdet;
    };

    float inv[6];
    inverseXform (grad->xform, inv);

    auto transformPoint = [&](float x, float y) -> juce::Point<float>
    {
        return { inv[0] * x + inv[2] * y + inv[4],
                 inv[1] * x + inv[3] * y + inv[5] };
    };

    juce::ColourGradient cg;

    if (paint.type == NSVG_PAINT_LINEAR_GRADIENT)
    {
        auto p1 = transformPoint (0.0f, 0.0f);
        auto p2 = transformPoint (0.0f, 1.0f);
        cg = juce::ColourGradient (juce::Colours::black, p1, juce::Colours::black, p2, false);
    }
    else
    {
        auto centre = transformPoint (grad->fx, grad->fy);
        auto edge   = transformPoint (grad->fx + 1.0f, grad->fy);
        auto radius = centre.getDistanceFrom (edge);
        cg = juce::ColourGradient (juce::Colours::black, centre, juce::Colours::black,
                                   { centre.x + radius, centre.y }, true);
    }

    cg.clearColours();

    for (int i = 0; i < grad->nstops; i++)
    {
        auto& stop = grad->stops[i];
        cg.addColour (double (stop.offset), nsvgColour (stop.color, 1.0f));
    }

    return cg;
}

static void applyPaint (juce::Graphics& g, const NSVGpaint& paint, float opacity)
{
    switch (paint.type)
    {
        case NSVG_PAINT_COLOR:
            g.setColour (nsvgColour (paint.color, opacity));
            break;
        case NSVG_PAINT_LINEAR_GRADIENT:
        case NSVG_PAINT_RADIAL_GRADIENT:
            g.setGradientFill (buildGradient (paint));
            g.setOpacity (opacity);
            break;
        default:
            break;
    }
}

static juce::PathStrokeType buildStrokeType (const NSVGshape* shape)
{
    auto join = juce::PathStrokeType::mitered;
    switch (shape->strokeLineJoin)
    {
        case NSVG_JOIN_ROUND: join = juce::PathStrokeType::curved; break;
        case NSVG_JOIN_BEVEL: join = juce::PathStrokeType::beveled; break;
        default: break;
    }

    auto cap = juce::PathStrokeType::butt;
    switch (shape->strokeLineCap)
    {
        case NSVG_CAP_ROUND:  cap = juce::PathStrokeType::rounded; break;
        case NSVG_CAP_SQUARE: cap = juce::PathStrokeType::square; break;
        default: break;
    }

    return juce::PathStrokeType (shape->strokeWidth, join, cap);
}

static void fillShape (juce::Graphics& g, const NSVGshape* shape, const juce::Path& path)
{
    if (shape->fill.type == NSVG_PAINT_NONE)
        return;

    applyPaint (g, shape->fill, shape->opacity);
    g.fillPath (path);
}

static void strokeShape (juce::Graphics& g, const NSVGshape* shape, const juce::Path& path)
{
    if (shape->stroke.type == NSVG_PAINT_NONE)
        return;

    applyPaint (g, shape->stroke, shape->opacity);

    if (shape->strokeDashCount > 0)
    {
        juce::Path dashed;
        float dashes[8];
        for (int i = 0; i < shape->strokeDashCount; i++)
            dashes[i] = shape->strokeDashArray[i];

        auto strokeType = buildStrokeType (shape);
        strokeType.createDashedStroke (dashed, path, dashes, shape->strokeDashCount);
        g.fillPath (dashed);
    }
    else
    {
        g.strokePath (path, buildStrokeType (shape));
    }
}

static void renderWithNanoSVG (const ParsedSVG& svg, juce::Image& img)
{
    auto width = img.getWidth();
    auto height = img.getHeight();

    auto scaleX = float (width) / svg.image->width;
    auto scaleY = float (height) / svg.image->height;
    auto scale = std::min (scaleX, scaleY);
    auto scaledW = svg.image->width * scale;
    auto scaledH = svg.image->height * scale;
    auto tx = (float (width) - scaledW) * 0.5f;
    auto ty = (float (height) - scaledH) * 0.5f;

    std::vector<unsigned char> rgba (size_t (width * height * 4), 0);

    auto* rast = nsvgCreateRasterizer();
    nsvgRasterize (rast, svg.image, tx, ty, scale, rgba.data(), width, height, width * 4);
    nsvgDeleteRasterizer (rast);

    juce::Image::BitmapData bitmap (img, juce::Image::BitmapData::writeOnly);

    for (int y = 0; y < height; y++)
    {
        auto* dst = reinterpret_cast<juce::PixelARGB*> (bitmap.getLinePointer (y));
        auto* src = rgba.data() + size_t (y * width * 4);

        for (int x = 0; x < width; x++)
        {
            auto r = src[0];
            auto g = src[1];
            auto b = src[2];
            auto a = src[3];

            dst->setARGB (a, r, g, b);

            dst++;
            src += 4;
        }
    }
}

static void renderWithJUCE (const ParsedSVG& svg, juce::Graphics& g)
{
    for (auto* shape = svg.image->shapes; shape != nullptr; shape = shape->next)
    {
        if (! (shape->flags & NSVG_FLAGS_VISIBLE))
            continue;

        auto path = buildPath (shape);

        auto order = shape->paintOrder;
        for (int i = 0; i < 3; i++)
        {
            auto op = (order >> (i * 2)) & 0x03;
            if (op == NSVG_PAINT_FILL)
                fillShape (g, shape, path);
            else if (op == NSVG_PAINT_STROKE)
                strokeShape (g, shape, path);
        }
    }
}

static juce::AffineTransform fitTransform (const ParsedSVG& svg, int width, int height)
{
    auto scaleX = float (width) / svg.image->width;
    auto scaleY = float (height) / svg.image->height;
    auto scale = std::min (scaleX, scaleY);
    auto scaledW = svg.image->width * scale;
    auto scaledH = svg.image->height * scale;
    auto tx = (float (width) - scaledW) * 0.5f;
    auto ty = (float (height) - scaledH) * 0.5f;

    return juce::AffineTransform::scale (scale).translated (tx, ty);
}

static juce::AffineTransform fitTransform (const ParsedSVG& svg, juce::Rectangle<float> targetBounds)
{
    auto svgBounds = juce::Rectangle<float> (0.0f, 0.0f, svg.image->width, svg.image->height);
    return juce::RectanglePlacement (juce::RectanglePlacement::centred)
               .getTransformToFit (svgBounds, targetBounds);
}

//==============================================================================

juce::Image SVG::renderToImage (const juce::String& svgText, int width, int height, Engine engine)
{
    return renderToImage (svgText, width, height, juce::Colours::transparentBlack, engine);
}

juce::Image SVG::renderToImage (const juce::String& svgText, int width, int height,
                                juce::Colour backgroundColour, Engine engine)
{
    juce::Image img (juce::Image::ARGB, width, height, true);

    ParsedSVG svg (svgText);
    if (! svg.isValid() || width <= 0 || height <= 0)
        return img;

    if (backgroundColour != juce::Colours::transparentBlack)
    {
        juce::Graphics g (img);
        g.fillAll (backgroundColour);
    }

    if (engine == Engine::nanosvg)
    {
        renderWithNanoSVG (svg, img);
    }
    else
    {
        juce::Graphics g (img);
        g.addTransform (fitTransform (svg, width, height));
        renderWithJUCE (svg, g);
    }

    return img;
}

juce::Path SVG::renderToPath (const juce::String& svgText)
{
    juce::Path result;

    ParsedSVG svg (svgText);
    if (! svg.isValid())
        return result;

    for (auto* shape = svg.image->shapes; shape != nullptr; shape = shape->next)
    {
        if (! (shape->flags & NSVG_FLAGS_VISIBLE))
            continue;

        result.addPath (buildPath (shape));
    }

    return result;
}

juce::Path SVG::renderToPath (const juce::String& svgText, juce::Rectangle<float> targetBounds)
{
    ParsedSVG svg (svgText);
    if (! svg.isValid())
        return {};

    auto path = renderToPath (svgText);
    if (path.isEmpty())
        return path;

    path.applyTransform (fitTransform (svg, targetBounds));
    return path;
}

void SVG::render (const juce::String& svgText, juce::Graphics& g,
                  juce::Rectangle<float> targetBounds, Engine engine)
{
    ParsedSVG svg (svgText);
    if (! svg.isValid())
        return;

    if (engine == Engine::juce)
    {
        juce::Graphics::ScopedSaveState saveState (g);
        g.addTransform (fitTransform (svg, targetBounds));
        renderWithJUCE (svg, g);
    }
    else
    {
        auto scaleFactor = g.getInternalContext().getPhysicalPixelScaleFactor();
        auto pixelW = juce::roundToInt (targetBounds.getWidth() * scaleFactor);
        auto pixelH = juce::roundToInt (targetBounds.getHeight() * scaleFactor);

        if (pixelW <= 0 || pixelH <= 0)
            return;

        auto img = renderToImage (svgText, pixelW, pixelH, Engine::nanosvg);
        g.drawImage (img, targetBounds);
    }
}
