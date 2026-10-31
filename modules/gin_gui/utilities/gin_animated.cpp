
static const juce::String kNone     { "none" };
static const juce::String kDefault  { "default" };
static const juce::Identifier kDescendants { "descendants" };

static const std::array<std::pair<const char*, Transition::Easing>, 31> easings
{{
    { "linear",         easeLinear<float> },
    { "inQuad",         easeQuadraticIn<float> },
    { "outQuad",        easeQuadraticOut<float> },
    { "inOutQuad",      easeQuadraticInOut<float> },
    { "inCubic",        easeCubicIn<float> },
    { "outCubic",       easeCubicOut<float> },
    { "inOutCubic",     easeCubicInOut<float> },
    { "inQuart",        easeQuarticIn<float> },
    { "outQuart",       easeQuarticOut<float> },
    { "inOutQuart",     easeQuarticInOut<float> },
    { "inQuint",        easeQuinticIn<float> },
    { "outQuint",       easeQuinticOut<float> },
    { "inOutQuint",     easeQuinticInOut<float> },
    { "inSine",         easeSineIn<float> },
    { "outSine",        easeSineOut<float> },
    { "inOutSine",      easeSineInOut<float> },
    { "inCirc",         easeCircularIn<float> },
    { "outCirc",        easeCircularOut<float> },
    { "inOutCirc",      easeCircularInOut<float> },
    { "inExpo",         easeExponentialIn<float> },
    { "outExpo",        easeExponentialOut<float> },
    { "inOutExpo",      easeExponentialInOut<float> },
    { "inBack",         easeBackIn<float> },
    { "outBack",        easeBackOut<float> },
    { "inOutBack",      easeBackInOut<float> },
    { "inElastic",      easeElasticIn<float> },
    { "outElastic",     easeElasticOut<float> },
    { "inOutElastic",   easeElasticInOut<float> },
    { "inBounce",       easeBounceIn<float> },
    { "outBounce",      easeBounceOut<float> },
    { "inOutBounce",    easeBounceInOut<float> },
}};

//==============================================================================
Transition::Transition (double durationMs_, Easing easing_)
    : durationMs (durationMs_), easing (easing_)
{
}

Transition::Easing Transition::findEasing (const juce::String& name)
{
    for (const auto& [n, e] : easings)
        if (name == n)
            return e;

    return nullptr;
}

std::optional<Transition> Transition::fromSpec (const juce::String& spec)
{
    auto tokens = juce::StringArray::fromTokens (spec.trim(), " \t", "");
    tokens.removeEmptyStrings();

    if (tokens.size() == 1 && tokens[0] == kNone)
        return Transition();

    if (tokens.isEmpty() || tokens.size() > 2 || ! tokens[0].containsOnly ("0123456789."))
        return {};

    auto easing = tokens.size() == 2 ? findEasing (tokens[1]) : easeLinear<float>;
    if (easing == nullptr)
        return {};

    return Transition (tokens[0].getDoubleValue(), easing);
}

Transition Transition::resolve (const juce::Component& c, const juce::String& nameOrSpec)
{
    if (auto t = fromSpec (nameOrSpec))
        return *t;

    if (nameOrSpec.isEmpty())
        return {};

    for (auto p = &c; p != nullptr; p = p->getParentComponent())
    {
        auto presets = p->getProperties()[presetsProperty];

        if (presets.hasProperty (nameOrSpec))
        {
            if (auto t = fromSpec (presets[juce::Identifier (nameOrSpec)].toString()))
                return *t;

            break;
        }
    }

    DBG ("Unknown transition: " + nameOrSpec);
    jassertfalse;
    return {};
}

Transition Transition::forProperty (const juce::Component& c, const juce::String& property)
{
    for (auto p = &c; p != nullptr; p = p->getParentComponent())
    {
        auto v = p->getProperties()[componentProperty];

        if (p != &c && v.hasProperty (kDescendants))
            v = v[kDescendants];

        if (v.isString())
            return resolve (*p, v.toString());

        if (property.isNotEmpty() && v.hasProperty (property))
            return resolve (*p, v[juce::Identifier (property)].toString());
    }

    for (auto p = &c; p != nullptr; p = p->getParentComponent())
        if (p->getProperties()[presetsProperty].hasProperty (kDefault))
            return resolve (*p, kDefault);

    return {};
}

void Transition::setPresets (juce::Component& c, const juce::var& presets)
{
    auto object = presets;

    if (presets.isString())
        object = juce::JSON::parse (presets.toString());

    if (presets.isVoid() || ! object.isObject())
    {
        // A string that isn't a JSON object, or a number, array...
        jassert (presets.isVoid());
        c.getProperties().remove (presetsProperty);
        return;
    }

    for (auto& itm : getPropertySet (object))
    {
        if (! fromSpec (itm.value.toString()).has_value())
        {
            DBG ("Invalid transition: " + itm.key + " " + itm.value.toString());
            jassertfalse;
        }
    }

    c.getProperties().set (presetsProperty, object);
}

juce::var Transition::getPresets (const juce::Component& c)
{
    return c.getProperties()[presetsProperty];
}

void Transition::set (juce::Component& c, const juce::var& transition)
{
    if (transition.isVoid())
        c.getProperties().remove (componentProperty);
    else
        c.getProperties().set (componentProperty, transition);
}

juce::var Transition::get (const juce::Component& c)
{
    return c.getProperties()[componentProperty];
}

static std::atomic<bool> transitionsEnabled { GIN_TRANSITIONS_ENABLED_BY_DEFAULT != 0 };

void Transition::setGlobalEnabled (bool enabled)
{
    JUCE_ASSERT_MESSAGE_THREAD

    if (transitionsEnabled.exchange (enabled) == enabled || enabled)
        return;

    juce::SharedResourcePointer<AnimationDriver> driver;
    driver->finishAll();
}

bool Transition::isGlobalEnabled()
{
    return transitionsEnabled;
}

float Transition::getProgress (double elapsedMs) const
{
    if (isInstant() || elapsedMs >= durationMs)
        return 1.0f;

    return easing (float (std::max (0.0, elapsedMs) / durationMs));
}

//==============================================================================
AnimationDriver::~AnimationDriver()
{
    cancelPendingUpdate();

    for (auto& [w, a] : windows)
        w->removeComponentListener (this);
}

void AnimationDriver::add (AnimatedBase& a)
{
    if (std::find (running.begin(), running.end(), &a) == running.end())
        running.push_back (&a);

    if (windows.find (a.window) == windows.end())
    {
        auto w = a.window;

        w->addComponentListener (this);
        windows[w] = std::make_unique<juce::VBlankAttachment> (w, [this, w] { tick (w); });
    }
}

void AnimationDriver::remove (AnimatedBase& a)
{
    running.erase (std::remove (running.begin(), running.end(), &a), running.end());

    if (a.window == nullptr || isRunningIn (a.window))
        return;

    // Never delete an attachment from inside its own callback
    if (a.window == ticking)
        triggerAsyncUpdate();
    else
        detach (a.window);
}

void AnimationDriver::finishAll()
{
    // Each step may stop, start or delete other values
    for (auto a : std::vector<AnimatedBase*> (running))
        if (std::find (running.begin(), running.end(), a) != running.end())
            a->tick (std::numeric_limits<double>::max());
}

bool AnimationDriver::isRunningIn (juce::Component* window) const
{
    return std::any_of (running.begin(), running.end(), [window] (AnimatedBase* a) { return a->window == window; });
}

void AnimationDriver::tick (juce::Component* window)
{
    juce::ScopedValueSetter<juce::Component*> svs (ticking, window);

    const auto now = juce::Time::getMillisecondCounterHiRes();

    // A step may start, stop or delete other values
    const auto snapshot = running;

    for (auto a : snapshot)
        if (std::find (running.begin(), running.end(), a) != running.end() && a->window == window)
            a->tick (now);
}

void AnimationDriver::detach (juce::Component* window)
{
    if (auto itr = windows.find (window); itr != windows.end())
    {
        window->removeComponentListener (this);
        windows.erase (itr);
    }
}

void AnimationDriver::handleAsyncUpdate()
{
    std::vector<juce::Component*> idle;

    for (auto& [w, a] : windows)
        if (! isRunningIn (w))
            idle.push_back (w);

    for (auto w : idle)
        detach (w);
}

void AnimationDriver::componentBeingDeleted (juce::Component& c)
{
    for (auto a : std::vector<AnimatedBase*> (running))
    {
        if (a->window == &c)
        {
            a->animating = false;
            a->window = nullptr;
            running.erase (std::remove (running.begin(), running.end(), a), running.end());
        }
    }

    detach (&c);
}

//==============================================================================
AnimatedBase::AnimatedBase (juce::Component& owner_, const juce::String& property_)
    : owner (&owner_), property (property_)
{
}

AnimatedBase::~AnimatedBase()
{
    stop();
}

Transition AnimatedBase::getTransition() const
{
    if (auto c = getOwner())
        return Transition::forProperty (*c, property);

    return {};
}

bool AnimatedBase::start (const Transition& t)
{
    auto c = getOwner();

    if (t.isInstant() || c == nullptr || ! c->isShowing() || ! Transition::isGlobalEnabled())
        return false;

    if (animating && window != c->getTopLevelComponent())
        stop();

    transition = t;
    startMs = juce::Time::getMillisecondCounterHiRes();
    lastProgress = 0.0f;
    animating = true;
    window = c->getTopLevelComponent();

    driver->add (*this);
    return true;
}

void AnimatedBase::stop()
{
    if (! animating)
        return;

    animating = false;
    driver->remove (*this);
    window = nullptr;
}

void AnimatedBase::changed()
{
    if (auto c = getOwner())
        c->repaint();

    valueChanged();

    if (onChange)
        onChange();
}

void AnimatedBase::tick (double nowMs)
{
    if (getOwner() == nullptr)
    {
        stop();
        return;
    }

    const auto progress = transition.getProgress (nowMs - startMs);

    lastProgress = progress;
    update (progress);

    if (progress >= 1.0f)
        stop();

    changed();
}

//==============================================================================
juce::Rectangle<float> Interpolator<juce::Rectangle<float>>::blend (juce::Rectangle<float> a, juce::Rectangle<float> b, float p)
{
    using F = Interpolator<float>;

    return juce::Rectangle<float>::leftTopRightBottom (F::blend (a.getX(), b.getX(), p),
                                                      F::blend (a.getY(), b.getY(), p),
                                                      F::blend (a.getRight(), b.getRight(), p),
                                                      F::blend (a.getBottom(), b.getBottom(), p));
}

juce::Rectangle<int> Interpolator<juce::Rectangle<int>>::blend (juce::Rectangle<int> a, juce::Rectangle<int> b, float p)
{
    return Interpolator<juce::Rectangle<float>>::blend (a.toFloat(), b.toFloat(), p).toNearestIntEdges();
}

juce::Font Interpolator<juce::Font>::blend (const juce::Font& a, const juce::Font& b, float p)
{
    return b.withHeight (Interpolator<float>::blend (a.getHeight(), b.getHeight(), p));
}

juce::ColourGradient Interpolator<juce::ColourGradient>::blend (const juce::ColourGradient& a, const juce::ColourGradient& b, float p)
{
    if (a.getNumColours() == 0 || b.getNumColours() == 0)
        return b;

    std::vector<double> positions;

    for (auto g : { &a, &b })
        for (int i = 0; i < g->getNumColours(); i++)
            positions.push_back (g->getColourPosition (i));

    std::sort (positions.begin(), positions.end());
    positions.erase (std::unique (positions.begin(), positions.end(), [] (double x, double y) { return juce::approximatelyEqual (x, y); }), positions.end());

    juce::ColourGradient res;
    res.point1 = a.point1 + (b.point1 - a.point1) * p;
    res.point2 = a.point2 + (b.point2 - a.point2) * p;
    res.isRadial = b.isRadial;

    for (auto pos : positions)
        res.addColour (pos, a.getColourAtPosition (pos).interpolatedWith (b.getColourAtPosition (pos), p));

    return res;
}

//==============================================================================
std::optional<juce::Point<float>> parseTransformOrigin (const juce::String& origin)
{
    auto tokens = juce::StringArray::fromTokens (origin.trim().toLowerCase(), " \t", "");
    tokens.removeEmptyStrings();

    if (tokens.isEmpty() || tokens.size() > 2)
        return {};

    std::optional<float> x, y, either;

    for (auto t : tokens)
    {
        if (t == "left")                        { if (x) return {}; x = 0.0f; }
        else if (t == "right")                  { if (x) return {}; x = 1.0f; }
        else if (t == "top")                    { if (y) return {}; y = 0.0f; }
        else if (t == "bottom")                 { if (y) return {}; y = 1.0f; }
        else if (t == "center" || t == "centre")
        {
            either = 0.5f;
        }
        else if (t.trimCharactersAtEnd ("%").containsOnly ("0123456789.-") && t.trimCharactersAtEnd ("%").isNotEmpty())
        {
            auto v = t.trimCharactersAtEnd ("%").getFloatValue() / (t.endsWithChar ('%') ? 100.0f : 1.0f);

            // Numbers go x first, then y
            if (! x)        x = v;
            else if (! y)   y = v;
            else            return {};

            if (tokens.size() == 1)
                y = v;
        }
        else
        {
            return {};
        }
    }

    if (either)
    {
        if (! x)        x = *either;
        else if (! y)   y = *either;
    }

    return juce::Point<float> (x.value_or (0.5f), y.value_or (0.5f));
}

juce::AffineTransform scaleTransform (juce::Rectangle<int> bounds, float s, juce::Point<float> origin)
{
    if (juce::exactlyEqual (s, 1.0f))
        return {};

    const auto b = bounds.toFloat();

    return juce::AffineTransform::scale (s, s, b.getX() + b.getWidth() * origin.x, b.getY() + b.getHeight() * origin.y);
}

AnimatedScale::AnimatedScale (juce::Component& owner_, const juce::String& property_, float initial)
    : Animated<float> (owner_, property_, initial)
{
    owner_.addComponentListener (this);
    apply();
}

AnimatedScale::~AnimatedScale()
{
    if (auto c = getOwner())
        c->removeComponentListener (this);
}

void AnimatedScale::setOrigin (juce::Point<float> relativeOrigin)
{
    if (relativeOrigin == origin)
        return;

    origin = relativeOrigin;
    apply();
}

void AnimatedScale::apply()
{
    if (auto c = getOwner())
        c->setTransform (scaleTransform (c->getBounds(), get(), origin));
}

void AnimatedScale::componentMovedOrResized (juce::Component&, bool wasMoved, bool wasResized)
{
    if (wasMoved || wasResized)
        apply();
}

//==============================================================================
void fillGradient (juce::Graphics& g, juce::Rectangle<float> area, float cornerRadius, const juce::ColourGradient& relativeGradient)
{
    if (relativeGradient.getNumColours() == 0)
        return;

    auto gradient = relativeGradient;
    gradient.point1 = area.getRelativePoint (relativeGradient.point1.x, relativeGradient.point1.y);
    gradient.point2 = area.getRelativePoint (relativeGradient.point2.x, relativeGradient.point2.y);

    g.setGradientFill (gradient);

    if (cornerRadius > 0.0f)
        g.fillRoundedRectangle (area, cornerRadius);
    else
        g.fillRect (area);
}

void drawText (juce::Graphics& g, const Crossfade<juce::String>& text, juce::Rectangle<float> area,
               const juce::Font& font, juce::Colour colour, juce::Justification justification)
{
    g.setFont (font);

    text.draw ([&] (const juce::String& s, float opacity)
    {
        if (s.isNotEmpty())
        {
            g.setColour (colour.withMultipliedAlpha (opacity));
            g.drawText (s, area, justification, true);
        }
    });
}

void drawIcon (juce::Graphics& g, const juce::Path& relativePath, juce::Rectangle<float> area, juce::Colour colour)
{
    const auto side = std::min (area.getWidth(), area.getHeight());
    const auto square = area.withSizeKeepingCentre (side, side);

    g.setColour (colour);
    g.fillPath (relativePath, juce::AffineTransform::scale (side).translated (square.getX(), square.getY()));
}

void drawIcon (juce::Graphics& g, const Crossfade<juce::Path>& icon, juce::Rectangle<float> area, juce::Colour colour)
{
    icon.draw ([&] (const juce::Path& p, float opacity)
    {
        drawIcon (g, p, area, colour.withMultipliedAlpha (opacity));
    });
}

void drawImage (juce::Graphics& g, const Crossfade<MipMap>& image, juce::Rectangle<float> area,
                juce::RectanglePlacement placement)
{
    drawBlend (g, image.getPrevious(), image.getCurrent(), image.getProgress(), area, placement);
}

//==============================================================================
std::optional<ImageFilter> ImageFilter::fromSpec (const juce::String& spec)
{
    static const juce::StringArray withValue { "brightness", "contrast", "hue", "saturation", "lightness", "gamma" };
    static const juce::StringArray withoutValue { "sharpen", "soften", "greyscale", "sepia", "invert" };

    auto tokens = juce::StringArray::fromTokens (spec.trim(), " \t", "");
    tokens.removeEmptyStrings();

    ImageFilter filter;

    for (int i = 0; i < tokens.size(); i++)
    {
        const auto& name = tokens[i];

        if (withoutValue.contains (name))
        {
            filter.steps.push_back ({ name, 0.0f });
        }
        else if (withValue.contains (name))
        {
            if (i + 1 >= tokens.size() || ! tokens[i + 1].containsOnly ("0123456789.-+") || tokens[i + 1].isEmpty())
                return {};

            const auto value = tokens[++i].getFloatValue();

            if (name == "gamma" && value <= 0.0f)
                return {};

            filter.steps.push_back ({ name, value });
        }
        else
        {
            return {};
        }
    }

    return filter;
}

void ImageFilter::apply (juce::Image& img) const
{
    for (const auto& [name, v] : steps)
    {
        if (name == "brightness")       applyBrightnessContrast (img, v, 0.0f);
        else if (name == "contrast")    applyBrightnessContrast (img, 0.0f, v);
        else if (name == "hue")         applyHueSaturationLightness (img, v, 100.0f, 0.0f);
        else if (name == "saturation")  applyHueSaturationLightness (img, 0.0f, v, 0.0f);
        else if (name == "lightness")   applyHueSaturationLightness (img, 0.0f, 100.0f, v);
        else if (name == "gamma")       applyGamma (img, v);
        else if (name == "sharpen")     applySharpen (img);
        else if (name == "soften")      applySoften (img);
        else if (name == "greyscale")   applyGreyScale (img);
        else if (name == "sepia")       applySepia (img);
        else if (name == "invert")      applyInvert (img);
        else                            jassertfalse;
    }
}

AnimatedImageFilter::AnimatedImageFilter (juce::Component& owner_, const juce::String& property_)
    : amount (owner_, property_, 0.0f), owner (&owner_)
{
    AnimatedProperties::get (owner_).addImageFilter (*this);
}

AnimatedImageFilter::~AnimatedImageFilter()
{
    if (auto c = owner.getComponent())
        if (auto p = AnimatedProperties::find (*c))
            p->removeImageFilter (*this);
}

void AnimatedImageFilter::setImage (const MipMap& newImage)
{
    if (newImage == image)
        return;

    image = newImage;
    rebuild();
}

void AnimatedImageFilter::setFilter (const ImageFilter& newFilter)
{
    if (newFilter == filter)
        return;

    filter = newFilter;
    rebuild();
}

void AnimatedImageFilter::setFilter (const juce::String& spec)
{
    auto parsed = ImageFilter::fromSpec (spec);

    if (! parsed)
    {
        DBG ("Invalid image filter: " + spec);
        jassertfalse;
    }

    setFilter (parsed.value_or (ImageFilter()));
}

void AnimatedImageFilter::rebuild()
{
    if (image.isNull() || filter.isEmpty())
    {
        filtered = image;
    }
    else
    {
        auto full = image.getImage().createCopy();
        filter.apply (full);
        filtered.setImage (full, image.getEnhance());
    }

    if (auto c = amount.getOwner())
        c->repaint();
}

void AnimatedImageFilter::draw (juce::Graphics& g, juce::Rectangle<float> area, juce::RectanglePlacement placement) const
{
    drawBlend (g, image, filtered, amount, area, placement);
}
