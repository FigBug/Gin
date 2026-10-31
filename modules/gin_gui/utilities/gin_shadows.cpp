/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

static const juce::Identifier kShadowLayer      { "ginShadowLayer" };
static const juce::Identifier kShadowRenderer   { "ginShadowRenderer" };

static BoxShadow::Renderer& globalShadowRenderer()
{
    static BoxShadow::Renderer renderer;
    return renderer;
}

ShadowList Interpolator<ShadowList>::blend (const ShadowList& a, const ShadowList& b, float p)
{
    ShadowList res;

    for (size_t i = 0; i < std::max (a.size(), b.size()); i++)
    {
        auto from = i < a.size() ? a[i] : b[i];
        auto to = i < b.size() ? b[i] : a[i];

        if (i >= a.size())  from.colour = from.colour.withAlpha (0.0f);
        if (i >= b.size())  to.colour = to.colour.withAlpha (0.0f);

        ShadowSpec s;
        s.offset = from.offset + (to.offset - from.offset) * p;
        s.blur = from.blur + (to.blur - from.blur) * p;
        s.spread = from.spread + (to.spread - from.spread) * p;
        s.colour = from.colour.interpolatedWith (to.colour, p);

        res.push_back (s);
    }

    return res;
}

//==============================================================================
// Paints the shadows of its siblings, as the container's first child
class ShadowLayer : public juce::Component,
                    private juce::ComponentListener
{
public:
    static void ensure (juce::Component& container)
    {
        if (container.getProperties()[kShadowLayer].getObject() != nullptr)
            return;

        // Stored first: adding the layer re-enters ensure through the container's child states
        auto holder = new Holder();
        container.getProperties().set (kShadowLayer, juce::var (holder));
        holder->layer = std::make_unique<ShadowLayer> (container);
    }

    static ShadowLayer* find (const juce::Component& container)
    {
        if (auto h = dynamic_cast<Holder*> (container.getProperties()[kShadowLayer].getObject()))
            return h->layer.get();

        return nullptr;
    }

    explicit ShadowLayer (juce::Component& c)
        : container (&c)
    {
        setInterceptsMouseClicks (false, false);
        setAccessible (false);
        setWantsKeyboardFocus (false);
        getProperties().set (LayoutSupport::excludeFromLayout, true);

        c.addChildComponent (this, 0);
        setVisible (true);
        setBounds (c.getLocalBounds());

        c.addComponentListener (this);
        sync();
    }

    ~ShadowLayer() override
    {
        for (auto& w : watched)
            if (w != nullptr)
                w->removeComponentListener (this);

        if (auto c = container.getComponent())
        {
            c->removeComponentListener (this);
            c->removeChildComponent (this);
        }
    }

    void paint (juce::Graphics& g) override
    {
        if (auto c = container.getComponent())
            BoxShadow::paintChildShadows (g, *c);
    }

    void sync()
    {
        auto c = container.getComponent();
        if (c == nullptr)
            return;

        if (c->getIndexOfChildComponent (this) > 0)
            toBack();

        for (auto& w : watched)
            if (w != nullptr)
                w->removeComponentListener (this);

        watched.clear();

        for (auto child : c->getChildren())
        {
            if (child != this)
            {
                child->addComponentListener (this);
                watched.push_back (child);
            }
        }

        repaint();
    }

private:
    struct Holder : public juce::ReferenceCountedObject
    {
        std::unique_ptr<ShadowLayer> layer;
    };

    void componentMovedOrResized (juce::Component& c, bool, bool) override
    {
        if (&c == container.getComponent())
            setBounds (c.getLocalBounds());
        else
            repaint();
    }

    void componentVisibilityChanged (juce::Component&) override      { repaint(); }
    void componentChildrenChanged (juce::Component& c) override
    {
        if (&c == container.getComponent())
            sync();
    }

    void componentBeingDeleted (juce::Component& c) override
    {
        c.removeComponentListener (this);

        if (&c == container.getComponent())
            container = nullptr;
    }

    juce::Component::SafePointer<juce::Component> container;
    std::vector<juce::Component::SafePointer<juce::Component>> watched;
};

//==============================================================================
std::optional<ShadowList> BoxShadow::parse (const juce::var& shadow)
{
    juce::StringArray layers;

    if (shadow.isVoid())
        return ShadowList();

    if (shadow.isArray())
    {
        for (auto& l : *shadow.getArray())
            layers.add (l.toString());
    }
    else
    {
        layers = juce::StringArray::fromTokens (shadow.toString(), ",", "");
    }

    layers.trim();
    layers.removeEmptyStrings();

    ShadowList list;

    for (auto& layer : layers)
    {
        if (layer == "none")
            continue;

        auto tokens = juce::StringArray::fromTokens (layer, " \t", "");
        tokens.removeEmptyStrings();

        if (tokens.size() < 4 || tokens.size() > 6)
            return {};

        auto isNumber = [] (const juce::String& t) { return t.isNotEmpty() && t.containsOnly ("0123456789.-+"); };

        for (int i = 0; i < 4; i++)
            if (! isNumber (tokens[i]))
                return {};

        ShadowSpec s;
        s.offset = { tokens[0].getFloatValue(), tokens[1].getFloatValue() };
        s.blur = tokens[2].getFloatValue();

        auto colour = juce::Colours::black;
        auto next = 4;

        if (next < tokens.size() && isNumber (tokens[next]))
            s.spread = tokens[next++].getFloatValue();

        if (next < tokens.size())
        {
            const auto& c = tokens[next++];

            if (c.startsWithChar ('#') || c.containsOnly ("0123456789abcdefABCDEF"))
            {
                auto hex = c.trimCharactersAtStart ("#");
                colour = juce::Colour::fromString (hex.length() == 6 ? "ff" + hex : hex);
            }
            else
            {
                colour = juce::Colours::findColourForName (c, juce::Colours::transparentBlack);

                if (colour == juce::Colours::transparentBlack)
                    return {};
            }
        }

        if (next < tokens.size())
            return {};

        s.colour = colour.withMultipliedAlpha (juce::jlimit (0.0f, 1.0f, tokens[3].getFloatValue()));
        list.push_back (s);
    }

    return list;
}

void BoxShadow::setRenderer (Renderer renderer)
{
    globalShadowRenderer() = std::move (renderer);
}

struct ShadowRendererHolder : public juce::ReferenceCountedObject
{
    BoxShadow::Renderer renderer;
};

void BoxShadow::setRenderer (juce::Component& container, Renderer renderer)
{
    if (renderer == nullptr)
    {
        container.getProperties().remove (kShadowRenderer);
        return;
    }

    auto h = new ShadowRendererHolder();
    h->renderer = std::move (renderer);
    container.getProperties().set (kShadowRenderer, juce::var (h));
}

void BoxShadow::set (juce::Component& c, const juce::var& shadow)
{
    auto parsed = parse (shadow);

    if (! parsed)
    {
        DBG ("Invalid box shadow: " + shadow.toString());
        jassertfalse;
    }

    AnimatedProperties::get (c).setBaseShadow (parsed.value_or (ShadowList()), true);
}

void BoxShadow::setRadius (juce::Component& c, float radius)
{
    AnimatedProperties::get (c).setShadowRadius (radius);
}

void BoxShadow::paintChildShadows (juce::Graphics& g, juce::Component& container)
{
    auto renderer = globalShadowRenderer();

    if (auto h = dynamic_cast<ShadowRendererHolder*> (container.getProperties()[kShadowRenderer].getObject()))
        renderer = h->renderer;

    if (renderer == nullptr)
        return;

    for (auto child : container.getChildren())
    {
        if (! child->isVisible() || child->getAlpha() <= 0.0f)
            continue;

        auto props = AnimatedProperties::find (*child);
        if (props == nullptr || props->getShadow().empty())
            continue;

        juce::Path outline;
        outline.addRoundedRectangle (child->getBounds().toFloat(), props->getShadowRadius());
        outline.applyTransform (child->getTransform());

        for (auto s : props->getShadow())
        {
            s.colour = s.colour.withMultipliedAlpha (child->getAlpha());
            renderer (g, outline, s);
        }
    }
}
