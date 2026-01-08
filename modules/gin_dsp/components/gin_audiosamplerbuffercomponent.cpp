
namespace
{
constexpr auto minZoom = 10;
}

AudioSamplerBufferComponent::AudioSamplerBufferComponent()
{
}

AudioSamplerBufferComponent::~AudioSamplerBufferComponent()
{
}

juce::MouseCursor AudioSamplerBufferComponent::getMouseCursor()
{
    if (dragScrolling)
        return juce::MouseCursor::DraggingHandCursor;

    return juce::Component::getMouseCursor();
}

void AudioSamplerBufferComponent::mouseDown (const juce::MouseEvent& e)
{
    mouseDownSample = xToSample (e.position.x);
    dragScrolling = true;
}

void AudioSamplerBufferComponent::mouseDrag (const juce::MouseEvent& e)
{
    auto sampleUnderMouse = xToSample (e.position.x);
    auto delta = sampleUnderMouse - mouseDownSample;

    setViewBounds (getViewLeft() - delta, getViewRight() - delta);
}

void AudioSamplerBufferComponent::mouseUp (const juce::MouseEvent&)
{
    dragScrolling = false;
}

void AudioSamplerBufferComponent::paintBackground (juce::Graphics& g)
{
    g.fillAll (bkColour);

    if (buffer.getNumSamples() == 0)
        return;

    if (pixelCacheDirty)
    {
        if (hiRes())
            updatePixelCacheHiRes();
        else
            updatePixelCache();

        pixelCacheDirty = false;
    }
}

void AudioSamplerBufferComponent::paintPlayheads (juce::Graphics& g)
{
    for (auto p : playheads)
    {
        const auto x = sampleToX (p);
        const auto h = static_cast<float> (getHeight());

        g.setColour (juce::Colours::black.withAlpha (0.5f));
        g.fillRect (x - 0.5f, 0.0f, 3.0f, h);

        g.setColour (lineColour.brighter());
        g.fillRect (x, 0.0f, 2.0f, h);
    }
}

void AudioSamplerBufferComponent::paint (juce::Graphics& g)
{
    paintBackground (g);
    paintWaveform (g);
    paintPlayheads (g);
}

void AudioSamplerBufferComponent::paintWaveform (juce::Graphics& g)
{
    auto rc = getLocalBounds().toFloat ();

    for (auto ch = 0; ch < channels.size(); ch++)
    {
        auto r = rc.removeFromTop (float (getHeight()) / float (channels.size()));
        paintChannelData (g, ch, r);
        ++ch;
    }
}

void AudioSamplerBufferComponent::paintChannelData (juce::Graphics& g, int ch, const juce::Rectangle<float>& r)
{
    const auto& c = channels[ch];

    g.setColour (lineColour);

    if (hiRes())
        g.strokePath (c->path, juce::PathStrokeType (2.0f), juce::AffineTransform::scale (1.0f, -(r.getHeight() / 2.0f)).translated (0.0f, r.getCentreY()));
    else
        g.fillPath (c->path, juce::AffineTransform::scale (1.0f, -(r.getHeight () / 2.0f)).translated (0.0f, r.getCentreY()));

    if (auto spp = getNumSamplesPerPixel (); spp < 1.0f)
    {
        g.setColour (lineColour.withAlpha (juce::jmap (std::max (spp, 0.05f), 0.05f, 1.0f, 1.0f, 0.0f)));

        auto v = 0.0f;
        auto dx = 3.0f;

        auto samples = buffer.getReadPointer (ch);

        for (auto samp = int (viewLeft) - 1; samp <= std::min (getViewRight () + 1.0f, buffer.getNumSamples() - 1.0f); samp++)
        {
            v = samples[samp];
            auto x = sampleToX (samp);

            auto y = juce::jmap (v, -1.0f, 1.0f, r.getBottom(), r.getY());

            g.fillEllipse (float (x - dx / 2.0f), float (y - dx / 2.0f), dx, dx);
        }
    }

    g.setColour (lineColour.withMultipliedAlpha (0.7f));
    g.fillRect (r.withY (r.getCentreY()).withHeight (0.0f).expanded (0.0f, 0.5f));
}

void AudioSamplerBufferComponent::setPlayheads (const std::vector<int>& playheads_)
{
    if (playheads != playheads_)
    {
        playheads = playheads_;
        repaint ();
    }
}

void AudioSamplerBufferComponent::setViewLeft (float left)
{
    auto right = viewLeft + viewWidth;
    viewLeft = std::clamp (left, std::max(0.0f, right - maxZoom), right - minZoom);
    viewWidth = right - viewLeft;

    pixelCacheDirty = true;
    repaint ();
}

void AudioSamplerBufferComponent::setViewRight (float right)
{
    auto viewRight = std::clamp (right, viewLeft + minZoom, std::min (viewLeft + maxZoom, float (buffer.getNumSamples())));
    viewWidth = viewRight - viewLeft;

    pixelCacheDirty = true;
    repaint ();
}

void AudioSamplerBufferComponent::updatePixelCache()
{
    auto minLen = 5.0f / float (getHeight());
    auto rc = getLocalBounds ().toFloat ();

    for (auto ch = 0; auto c : channels)
    {
        c->setSize (int (viewWidth / getNumSamplesPerPixel()));

        auto    numSamples = int (viewWidth);
        auto    samples = buffer.getReadPointer (ch, int (viewLeft));

        auto    numLeftToAverage = std::max (1.0f, getNumSamplesPerPixel());
        auto    writePos = 0;

        while (numSamples > 0)
        {
            auto todo = std::min (int (numLeftToAverage), numSamples);

            auto minMax = juce::FloatVectorOperations::findMinAndMax (samples, todo);
            auto currentMin = minMax.getStart();
            auto currentMax = minMax.getEnd();

            if (std::isinf (currentMin) || std::isnan (currentMin))
                currentMin = 0.0f;

            if (std::isinf (currentMax) || std::isnan (currentMax))
                currentMax = 0.0f;

            // If min/max are of the same sign, force one of them to be zero
            if (currentMin > 0.0f)
                currentMin = 0.0f;
            else if (currentMax < 0.0f)
                currentMax = 0.0f;

            // Enforce minimum thickness
            if ((currentMax - currentMin) < minLen)
            {
                currentMax += minLen * 0.5f;
                currentMin -= minLen * 0.5f;
            }

            c->minBuffer.set (writePos, currentMin);
            c->maxBuffer.set (writePos, currentMax);

            numLeftToAverage = numLeftToAverage - todo + getNumSamplesPerPixel();

            writePos++;
            numSamples -= todo;
            samples += todo;
        }

        // Convert min/max buffer into path
        auto& p = c->path;
        p.clear();
        p.startNewSubPath (-2.0f, 0.0f);

        auto currentX = 0.0f;
        while (currentX < rc.getWidth() && int (currentX) < c->bufferSize)
        {
            const auto top = c->maxBuffer[int (currentX)];
            p.lineTo (currentX, top);
            currentX += 1.0f;
        }

        p.lineTo (currentX + 1.0f, 0.0f);

        currentX -= 1.0f;
        while (currentX >= 0.0f && int (currentX) < c->bufferSize)
        {
            const auto bottom = c->minBuffer[int (currentX)];
            p.lineTo (currentX, bottom);
            currentX -= 1.0f;
        }

        p.closeSubPath();

        ch++;
    }
}

void AudioSamplerBufferComponent::updatePixelCacheHiRes()
{
    for (auto ch = 0; auto c : channels)
    {
        c->setSize (0);

        auto& p = c->path;

        p.clear();
        p.startNewSubPath (-2.0f, 0.0f);

        auto v = 0.0f;
        auto samples = buffer.getReadPointer (ch);

        for (auto samp = int (viewLeft) - 1; samp <= std::min (getViewRight () + 1.0f, buffer.getNumSamples() - 1.0f); samp++)
        {
            v = ( samp >= 0 && samp < buffer.getNumSamples () ) ? samples[samp] : 0.0f;
            auto x = sampleToX (samp);
            if (samp == int (viewLeft) - 1)
            {
                p.lineTo (-1, v);
                p.lineTo (float (x), v);
            }
            else
            {
                p.lineTo (float (x), v);
            }
        }

        p.lineTo (getWidth () + 2.0f, v);
        p.lineTo (getWidth () + 2.0f, 0.0f);

        ch++;
    }
}

float AudioSamplerBufferComponent::getViewLeft()
{
    return viewLeft;
}

float AudioSamplerBufferComponent::getViewRight()
{
    return viewLeft + viewWidth;
}

bool AudioSamplerBufferComponent::hiRes()
{
    if (hiResDisabled)
        return false;

    return getNumSamplesPerPixel () <= 15.0f;
}

void AudioSamplerBufferComponent::clear()
{
    viewLeft    = 0.0;
    viewWidth   = 0.0;
    maxZoom     = 0.0;

    channels.clear();
    buffer.setSize (0, 0);

    pixelCacheDirty = true;
    repaint();
}

void AudioSamplerBufferComponent::zoom (float center, float factor)
{
    auto ratio = (center - viewLeft) / viewWidth;

    auto newWidth = viewWidth * factor;
    auto newViewLeft = center - ratio * newWidth;

    setViewBounds (newViewLeft, newViewLeft + newWidth);
}

void AudioSamplerBufferComponent::setViewBounds (float left, float right)
{
    if (right - left >= minZoom && right - left <= maxZoom)
    {
        if (left < 0.0)
        {
            right = (right - left);
            left  = 0.0;
        }

        if (right > buffer.getNumSamples())
        {
            left = buffer.getNumSamples() - (right - left);
            right = float (buffer.getNumSamples());

            if (left < 0.0)
                left = 0.0;
        }

        if (! juce::approximatelyEqual (left, getViewLeft()) || ! juce::approximatelyEqual (right, getViewRight()))
        {
            viewLeft  = left;
            viewWidth = right - left;
        }

        pixelCacheDirty = true;
        repaint ();
    }
}

void AudioSamplerBufferComponent::setMaxZoom (float mz)
{
    maxZoom = mz;
    if (viewWidth > mz)
        viewWidth = mz;
}

void AudioSamplerBufferComponent::mouseWheelMove (const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel)
{
    if (! scrollable)
        return;

    auto dx = wheel.deltaX;
    auto dy = wheel.deltaY;

   #if JUCE_MAC
    if (e.mods.isShiftDown () && juce::approximatelyEqual (dy, 0.0f) && ! juce::approximatelyEqual (dx, 0.0f))
        std::swap (dx, dy);
   #endif

    if (! juce::approximatelyEqual (dx, 0.0f))
        scrollViewBounds (-0.15f * viewWidth * dx);

    if (! juce::approximatelyEqual (dy, 0.0f))
    {
        const auto willScroll = e.mods.isShiftDown();

        if (willScroll)
            scrollViewBounds (-0.15f * viewWidth * dy);
        else
            zoom (xToSample (e.position.x), juce::jlimit (0.2f, 1.9f, 1.0f - dy * 0.7f));
    }
}

void AudioSamplerBufferComponent::scrollViewBounds (float delta)
{
    if (! juce::approximatelyEqual (delta, 0.0f))
    {
        setViewBounds (getViewLeft() + delta, getViewRight() + delta);

        pixelCacheDirty = true;
        repaint ();
    }
}

float AudioSamplerBufferComponent::getNumSamplesPerPixel()
{
    return float (viewWidth / getWidth());
}

float AudioSamplerBufferComponent::sampleToX (int sample)
{
    return (sample - viewLeft) / (viewWidth / getWidth());
}

float AudioSamplerBufferComponent::xToSample (float x)
{
    return viewLeft + (x * viewWidth) / getWidth();
}

void AudioSamplerBufferComponent::resized()
{
    pixelCacheDirty = true;
    repaint ();
}

void AudioSamplerBufferComponent::setBuffer (const juce::AudioBuffer<float>& bufferIn)
{
    buffer = bufferIn;

    viewLeft    = 0.0;
    viewWidth   = buffer.getNumSamples() - 1.0f;
    maxZoom     = float (buffer.getNumSamples());

    setNumChannels (buffer.getNumChannels());

    pixelCacheDirty = true;
    repaint ();
}

void AudioSamplerBufferComponent::setNumChannels (int num)
{
    if (num != channels.size())
    {
        channels.clear();
        while (channels.size() < num)
            channels.add (new Channel());
    }
}
