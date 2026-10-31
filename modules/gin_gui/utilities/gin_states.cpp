/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

static const juce::Identifier kAnimatedProperties   { "ginAnimatedProperties" };
static const juce::Identifier kStateTracker         { "ginStateTracker" };
static const juce::Identifier kChildStates          { "ginChildStates" };

static const juce::Identifier kStateAlpha           { "alpha" };
static const juce::Identifier kStateScale           { "scale" };
static const juce::Identifier kStateImageFilter     { "imageFilter" };
static const juce::Identifier kStateBoxShadow       { "boxShadow" };

template <typename T>
static T* findAttached (const juce::Component& c, const juce::Identifier& id)
{
    return dynamic_cast<T*> (c.getProperties()[id].getObject());
}

//==============================================================================
AnimatedProperties& AnimatedProperties::get (juce::Component& c)
{
    if (auto p = find (c))
        return *p;

    auto p = new AnimatedProperties (c);
    c.getProperties().set (kAnimatedProperties, juce::var (p));
    return *p;
}

AnimatedProperties* AnimatedProperties::find (const juce::Component& c)
{
    return findAttached<AnimatedProperties> (c, kAnimatedProperties);
}

AnimatedProperties::AnimatedProperties (juce::Component& c)
    : owner (&c)
{
    parentWatch.owner = this;
}

AnimatedProperties::~AnimatedProperties()
{
    if (auto c = owner.getComponent())
        c->removeComponentListener (&parentWatch);
}

void AnimatedProperties::ParentWatch::componentParentHierarchyChanged (juce::Component& c)
{
    if (auto p = c.getParentComponent(); p != nullptr && owner->shadow != nullptr)
        ShadowLayer::ensure (*p);

    owner->repaintShadow();
}

void AnimatedProperties::setBaseShadow (const ShadowList& newShadow, bool animate)
{
    baseShadow = newShadow;
    applyShadow (animate);
}

void AnimatedProperties::setShadowRadius (float radius)
{
    if (juce::exactlyEqual (radius, shadowRadius))
        return;

    shadowRadius = radius;
    repaintShadow();
}

const ShadowList& AnimatedProperties::getShadow() const
{
    static const ShadowList none;
    return shadow != nullptr ? shadow->get() : none;
}

void AnimatedProperties::repaintShadow()
{
    if (auto c = owner.getComponent())
        if (auto p = c->getParentComponent())
            if (auto layer = ShadowLayer::find (*p))
                layer->repaint();
}

void AnimatedProperties::applyShadow (bool animate)
{
    auto c = owner.getComponent();
    if (c == nullptr)
        return;

    auto target = baseShadow.value_or (ShadowList());

    if (stateValues.hasProperty (kStateBoxShadow))
    {
        auto parsed = BoxShadow::parse (stateValues[kStateBoxShadow]);
        jassert (parsed.has_value());
        target = parsed.value_or (ShadowList());
    }

    if (shadow == nullptr && target.empty())
        return;

    if (shadow == nullptr)
    {
        shadow = std::make_unique<Animated<ShadowList>> (*c, kStateBoxShadow.toString(), ShadowList());
        shadow->onChange = [this]
        {
            repaintShadow();

            if (auto o = owner.getComponent(); o != nullptr && onStep)
                onStep (*o);
        };

        c->addComponentListener (&parentWatch);
    }

    if (auto p = c->getParentComponent())
        ShadowLayer::ensure (*p);

    if (animate)
        shadow->setTarget (target);
    else
        shadow->setTarget (target, Transition());

    repaintShadow();
}

void AnimatedProperties::setBaseAlpha (float newAlpha, bool animate)
{
    baseAlpha = newAlpha;
    applyAlpha (animate);
}

void AnimatedProperties::setBaseScale (float newScale, juce::Point<float> newOrigin, bool animate)
{
    baseScale = newScale;
    origin = newOrigin;

    if (scale != nullptr)
        scale->setOrigin (origin);

    applyScale (animate);
}

void AnimatedProperties::setStateValues (const juce::var& values)
{
    stateValues = values;

    applyAlpha (true);
    applyScale (true);
    applyImageFilters();
    applyShadow (true);
}

void AnimatedProperties::addImageFilter (AnimatedImageFilter& f)
{
    imageFilters.push_back (&f);
}

void AnimatedProperties::removeImageFilter (AnimatedImageFilter& f)
{
    imageFilters.erase (std::remove (imageFilters.begin(), imageFilters.end(), &f), imageFilters.end());
}

void AnimatedProperties::applyAlpha (bool animate)
{
    auto c = owner.getComponent();
    if (c == nullptr)
        return;

    if (! stateValues.hasProperty (kStateAlpha) && ! baseAlpha.has_value() && alpha == nullptr)
        return;

    const auto target = juce::jlimit (0.0f, 1.0f, stateValues.hasProperty (kStateAlpha) ? float (stateValues[kStateAlpha])
                                                                                       : baseAlpha.value_or (1.0f));

    if (alpha == nullptr)
    {
        alpha = std::make_unique<Animated<float>> (*c, kStateAlpha.toString(), c->getAlpha());
        alpha->onChange = [this]
        {
            if (auto o = owner.getComponent())
            {
                o->setAlpha (alpha->get());

                if (! alpha->isAnimating() && alpha->get() <= 0.0f)
                    o->setVisible (false);

                repaintShadow();

                if (onStep)
                    onStep (*o);
            }
        };
    }

    auto& anim = *alpha;

    if (! anim.isAnimating())
        anim.setValue (c->isVisible() ? c->getAlpha() : 0.0f);

    if (target > 0.0f && ! c->isVisible())
    {
        c->setAlpha (anim.get());
        c->setVisible (true);
    }

    // A component fading out takes no clicks, as if it were already gone
    if (target > 0.0f && clicksSuppressed)
    {
        c->setInterceptsMouseClicks (selfClicks, childClicks);
        clicksSuppressed = false;
    }

    if (animate)
        anim.setTarget (target);
    else
        anim.setTarget (target, Transition());

    if (target <= 0.0f && anim.isAnimating() && ! clicksSuppressed)
    {
        c->getInterceptsMouseClicks (selfClicks, childClicks);
        c->setInterceptsMouseClicks (false, false);
        clicksSuppressed = true;
    }

    if (target <= 0.0f && ! anim.isAnimating())
        c->setVisible (false);
}

void AnimatedProperties::applyScale (bool animate)
{
    auto c = owner.getComponent();
    if (c == nullptr)
        return;

    const auto target = stateValues.hasProperty (kStateScale) ? float (stateValues[kStateScale]) : baseScale.value_or (1.0f);

    // A component that never scales keeps a transform of its own
    if (scale == nullptr && juce::exactlyEqual (target, 1.0f))
        return;

    if (scale == nullptr)
    {
        scale = std::make_unique<AnimatedScale> (*c, kStateScale.toString(), 1.0f);
        scale->setOrigin (origin);
        scale->onChange = [this]
        {
            if (auto o = owner.getComponent(); o != nullptr && onStep)
                onStep (*o);
        };
    }

    if (animate)
        scale->setTarget (target);
    else
        scale->setTarget (target, Transition());
}

void AnimatedProperties::applyImageFilters()
{
    const auto spec = stateValues[kStateImageFilter].toString();

    for (auto f : std::vector<AnimatedImageFilter*> (imageFilters))
    {
        if (spec.isNotEmpty())
        {
            f->setFilter (spec);
            f->amount.setTarget (1.0f);
        }
        else
        {
            f->amount.setTarget (0.0f);
        }
    }
}

//==============================================================================
// Follows the mouse over one component and gives its AnimatedProperties the values of
// the states that apply
class StateTracker : public juce::ReferenceCountedObject,
                     private juce::MouseListener,
                     private juce::ComponentListener
{
public:
    static StateTracker& get (juce::Component& c)
    {
        if (auto t = findAttached<StateTracker> (c, kStateTracker))
            return *t;

        auto t = new StateTracker (c);
        c.getProperties().set (kStateTracker, juce::var (t));
        return *t;
    }

    ~StateTracker() override
    {
        detach();
    }

    void setStates (const juce::var& newStates, bool fromParent)
    {
        states = newStates;
        parentStates = fromParent;
        update();
    }

    juce::var states;
    bool parentStates = false;

private:
    explicit StateTracker (juce::Component& c)
        : owner (&c)
    {
        c.addMouseListener (this, true);
        c.addComponentListener (this);
    }

    void detach()
    {
        if (auto c = owner.getComponent())
        {
            c->removeMouseListener (this);
            c->removeComponentListener (this);
        }

        owner = nullptr;
    }

    void update (std::optional<bool> pressedNow = {})
    {
        if (auto c = owner.getComponent())
        {
            const auto hovered = c->isMouseOver (true);
            const auto pressed = pressedNow.value_or (c->isMouseButtonDown (true));
            const auto values = States::merge (states, hovered, pressed, ! c->isEnabled());

            if (values.isVoid() && AnimatedProperties::find (*c) == nullptr)
                return;

            AnimatedProperties::get (*c).setStateValues (values);
        }
    }

    void mouseEnter (const juce::MouseEvent&) override                  { update(); }
    void mouseExit (const juce::MouseEvent&) override                   { update(); }
    void mouseDown (const juce::MouseEvent&) override                   { update (true); }
    void mouseUp (const juce::MouseEvent&) override                     { update (false); }

    void componentEnablementChanged (juce::Component&) override         { update(); }
    void componentBeingDeleted (juce::Component&) override              { detach(); }

    // JUCE tells a container about a removed child only while the child is showing
    void componentParentHierarchyChanged (juce::Component&) override;

    juce::Component::SafePointer<juce::Component> owner;
};

//==============================================================================
// Gives the container's direct children, current and later ones, a StateTracker with the
// container's child states, unless a child has states of its own
class ChildStates : public juce::ReferenceCountedObject,
                    private juce::ComponentListener
{
public:
    static ChildStates& get (juce::Component& c)
    {
        if (auto s = findAttached<ChildStates> (c, kChildStates))
            return *s;

        auto s = new ChildStates (c);
        c.getProperties().set (kChildStates, juce::var (s));
        return *s;
    }

    ~ChildStates() override
    {
        if (auto c = owner.getComponent())
            c->removeComponentListener (this);
    }

    void setStates (const juce::var& newStates)
    {
        states = newStates;
        sync();
    }

    void sync()
    {
        auto c = owner.getComponent();
        if (c == nullptr)
            return;

        // Children that left the container drop its states
        for (auto& child : attached)
            if (child != nullptr && child->getParentComponent() != c)
                if (auto t = findAttached<StateTracker> (*child, kStateTracker); t != nullptr && t->parentStates)
                    t->setStates ({}, true);

        attached.clear();

        for (auto child : c->getChildren())
        {
            auto existing = findAttached<StateTracker> (*child, kStateTracker);

            if (existing != nullptr && ! existing->parentStates && ! existing->states.isVoid())
                continue;

            if (existing == nullptr && states.isVoid())
                continue;

            StateTracker::get (*child).setStates (states, true);
            attached.push_back (child);
        }
    }

    juce::var states;

private:
    explicit ChildStates (juce::Component& c)
        : owner (&c)
    {
        c.addComponentListener (this);
    }

    void componentChildrenChanged (juce::Component&) override       { sync(); }
    void componentBeingDeleted (juce::Component& c) override        { c.removeComponentListener (this); owner = nullptr; }

    juce::Component::SafePointer<juce::Component> owner;
    std::vector<juce::Component::SafePointer<juce::Component>> attached;
};

void StateTracker::componentParentHierarchyChanged (juce::Component&)
{
    auto c = owner.getComponent();

    if (c == nullptr || ! parentStates)
        return;

    auto parent = c->getParentComponent();
    auto cs = parent != nullptr ? findAttached<ChildStates> (*parent, kChildStates) : nullptr;
    auto newStates = cs != nullptr ? cs->states : juce::var();

    if (newStates != states)
        setStates (newStates, true);
}

//==============================================================================
void States::set (juce::Component& c, const juce::var& states)
{
    auto existing = findAttached<StateTracker> (c, kStateTracker);

    if (states.isVoid())
    {
        // Back to the parent's child states, if it has any
        if (existing != nullptr && ! existing->parentStates)
        {
            existing->setStates ({}, false);

            if (auto p = c.getParentComponent())
                if (auto cs = findAttached<ChildStates> (*p, kChildStates))
                    cs->sync();
        }

        return;
    }

    StateTracker::get (c).setStates (states, false);
}

juce::var States::get (const juce::Component& c)
{
    if (auto t = findAttached<StateTracker> (c, kStateTracker); t != nullptr && ! t->parentStates)
        return t->states;

    return {};
}

void States::setForChildren (juce::Component& container, const juce::var& states)
{
    if (states.isVoid() && findAttached<ChildStates> (container, kChildStates) == nullptr)
        return;

    ChildStates::get (container).setStates (states);
}

juce::var States::getForChildren (const juce::Component& container)
{
    if (auto cs = findAttached<ChildStates> (container, kChildStates))
        return cs->states;

    return {};
}

juce::var States::merge (const juce::var& states, bool hovered, bool pressed, bool disabled)
{
    if (! states.isObject())
        return {};

    auto merged = jsonObject();
    auto any = false;

    auto add = [&] (const char* name)
    {
        const auto values = states[name];

        if (! values.isObject())
            return;

        for (auto& itm : getPropertySet (values))
            setProperty (merged, itm.key, itm.value);

        any = true;
    };

    if (disabled)
    {
        add ("disabled");
    }
    else
    {
        if (hovered)    add ("hover");
        if (pressed)    add ("pressed");
    }

    return any ? merged : juce::var();
}
