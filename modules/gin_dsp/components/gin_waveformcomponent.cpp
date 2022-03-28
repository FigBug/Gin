/*
  ==============================================================================

  This file is based on part of the dRowAudio JUCE module
  Copyright 2004-13 by dRowAudio.
  dRowAudio is provided under the terms of The MIT License (MIT):

  ==============================================================================
*/


WaveformComponent::WaveformComponent()
{
    for (int i = 0; i < 32; i++)
        setColour (traceColourId + i, juce::Colours::white);
    
    for (int i = 0; i < 32; i++)
        setColour (envelopeColourId + i, juce::Colours::white.withAlpha (0.5f));
}

WaveformComponent::~WaveformComponent()
{
}

void WaveformComponent::setHorizontalZoom (float zoom_)
{
    zoom = zoom_;
    needsUpdate = true;
    repaint();
}

void WaveformComponent::setHorizontalOffset (float offset_)
{
    offset = offset_;
    needsUpdate = true;
    repaint();
}

void WaveformComponent::setBuffer (juce::AudioSampleBuffer& buffer_)
{
    buffer = &buffer_;
    needsUpdate = true;
    repaint();
}

//==============================================================================

void WaveformComponent::paint (juce::Graphics& g)
{
    if (needsUpdate)
    {
        needsUpdate = false;
        processPendingSamples();
    }
    
    render (g);
    
    g.setColour (findColour (lineColourId));
    g.drawRect (getLocalBounds());
}

//==============================================================================
void WaveformComponent::processPendingSamples()
{
    if (buffer == nullptr)
    {
        channels.clear();
        return;
    }
    
    while (channels.size() < buffer->getNumChannels())
        channels.add (new Channel());
    while (channels.size() > buffer->getNumChannels())
        channels.removeLast();

    int sz = getWidth();
    
    for (auto c : channels)
        c->setSize (sz);
    
    int ch = 0;
    for (auto c : channels)
    {
        const int numSamples = buffer->getNumSamples();

        float numSamplesPerPixel = float ( numSamples ) / float (getWidth()) / zoom;
        float currentMin = 999999.0f;
        float currentMax = -999999.0f;
        float currentAve = 0.0;
        int numAveraged = 0;
        int bufferWritePos = 0;
        int numLeftToAverage = (int) std::max (1.0f, numSamplesPerPixel);
        
        auto samples = buffer->getReadPointer (ch);
        int samplesTodo = juce::roundToInt (float ( numSamples ) / zoom);
        int i = juce::roundToInt ( float ( numSamples ) * offset);
        
        while (--samplesTodo >= 0)
        {
            const float currentSample = (i < numSamples) ? samples[i] : 0.0f;
            i++;

            if (currentSample < currentMin) currentMin = currentSample;
            if (currentSample > currentMax) currentMax = currentSample;
            
            currentAve += currentSample;
            numAveraged++;

            if (--numLeftToAverage <= 0)
            {
                c->posBuffer.set (bufferWritePos, currentAve / float ( numAveraged) );
                c->minBuffer.set (bufferWritePos, currentMin);
                c->maxBuffer.set (bufferWritePos, currentMax);

                currentMax = -999999.0f;
                currentMin = 999999.0f;
                currentAve = 0.0;

                bufferWritePos++;
                numLeftToAverage += (int) std::max (1.0f, numSamplesPerPixel);
                numAveraged = 0;
            }
            bufferWritePos++;
        }
        ch++;
    }
}

void WaveformComponent::render (juce::Graphics& g)
{
    const int w = getWidth();
    const int h = getHeight();

    int bufferReadPos = 0;
        
    int ch = 0;
    for (auto c : channels)
    {
        auto traceColour = findColour (traceColourId + ch);
        auto envelopeColour = findColour (envelopeColourId + ch);
        
        bool drawTrace = ! traceColour.isTransparent ();
        bool drawEnvelope = ! envelopeColour.isTransparent ();

        juce::Path p;
        
        int pos = bufferReadPos;
        int currentX = 0;
        
        g.setColour (envelopeColour);
        
        while (currentX < w)
        {
            ++pos;
            if (pos == c->bufferSize)
                pos = 0;

            const float top    = (1.0f - (0.5f + (0.5f * (c->maxBuffer[pos])))) * float ( h );
            const float bottom = (1.0f - (0.5f + (0.5f * (c->minBuffer[pos])))) * float ( h );
            const float mid    = (1.0f - (0.5f + (0.5f * (c->posBuffer[pos])))) * float ( h );
                        
            if (drawEnvelope && bottom - top > 2)
                g.drawVerticalLine (currentX, top, bottom);

            if (drawTrace)
            {
                if (currentX == 0)
                    p.startNewSubPath ( float ( currentX ), mid);
                else
                    p.lineTo ( float ( currentX ), mid);
            }
            
            currentX++;
        }
        
        if (drawTrace)
        {
            g.setColour (traceColour);
            g.strokePath (p, juce::PathStrokeType (1.5f));
        }
        
        ch++;
    }
}
