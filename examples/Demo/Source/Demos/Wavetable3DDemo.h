/*
  ==============================================================================

    Wavetable3DDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
struct Wavetable3DDemo : public DemoComponent,
                         private juce::Timer
{
    Wavetable3DDemo()
    {
        setName ("Wavetable 3D");

        // Set up the camera for a nice viewing angle
        scene.getCamera().setPosition (5, 3, 5);
        scene.getCamera().setTarget (0, 0, 0);
        scene.getCamera().setPerspective (45.0f, 1.0f, 0.1f, 100.0f);

        // Generate 64 wavetables morphing from sine to square
        generateWavetables();

        startTimerHz (60);
    }

    void generateWavetables()
    {
        gin::BandLimitedLookupTables tables { 44100, 12, 2048 };

        const float tableSpacing = 0.035f;

        juce::Colour colour = juce::Colours::green.withAlpha (0.6f);

        for (int t = 0; t < numTables; ++t)
        {
            auto* obj = scene.addObject();
            float morph = float (t) / float (numTables - 1);
            float zPos = (float (t) - float (numTables) / 2.0f) * tableSpacing;

            // Generate points for this wavetable by interpolating sine and square
            wavetablePoints[t].reserve (samplesPerTable);

            for (int i = 0; i < samplesPerTable; ++i)
            {
                float phase = float (i) / float (samplesPerTable);
                float xPos = (phase - 0.5f) * waveWidth;

                // Get sine and square values and interpolate
                float sineVal = tables.processSine (phase);
                float squareVal = tables.processSquare (60.0f, phase);  // Use middle note
                float yVal = (sineVal * (1.0f - morph) + squareVal * morph) * waveHeight;

                wavetablePoints[t].push_back ({ xPos, yVal, zPos });
            }

            // Connect points with lines (don't close the loop)
            for (int i = 0; i < samplesPerTable - 1; ++i)
                obj->addLine (wavetablePoints[t][size_t (i)], wavetablePoints[t][size_t (i + 1)], colour);
        }

        // Create the tracing point object
        tracingPointObj = scene.addObject();
        tracingPointObj->addPoint ({ 0, 0, 0 }, 8.0f, juce::Colours::yellow);
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colour (0xff1a1a2e));

        scene.getCamera().setPerspective (45.0f, getWidth() / (float) getHeight(), 0.1f, 100.0f);
        renderer.render (g, scene, getLocalBounds());

        g.setColour (juce::Colours::white.withAlpha (0.7f));
        g.setFont (12.0f);
        g.drawText ("Drag to orbit, scroll to zoom, shift+drag to pan",
                    getLocalBounds().removeFromBottom (25), juce::Justification::centred);
    }

    void timerCallback() override
    {
        // Update tracing point position (0.25 Hz per table)
        const float cyclesPerSecond = 0.25f;
        const float secondsPerTable = 1.0f / cyclesPerSecond;
        const float totalCycleTime = secondsPerTable * numTables;

        traceTime += 1.0f / 60.0f;  // Timer runs at 60 Hz
        if (traceTime >= totalCycleTime)
            traceTime -= totalCycleTime;

        // Calculate which table and which sample we're on
        float tableProgress = traceTime / secondsPerTable;
        int currentTable = int (tableProgress) % numTables;
        float sampleProgress = (tableProgress - float (currentTable)) * samplesPerTable;
        int sampleIndex = int (sampleProgress) % samplesPerTable;

        // Update the tracing point position and highlight current table
        if (tracingPointObj != nullptr && currentTable < numTables)
        {
            tracingPointObj->clear();
            tracingPointObj->addPoint (wavetablePoints[currentTable][size_t (sampleIndex)], 4.0f, juce::Colours::yellow);

            // Highlight the current wavetable in yellow
            if (currentTable != lastHighlightedTable)
            {
                // Reset previous table to green
                if (lastHighlightedTable >= 0)
                    updateTableColour (lastHighlightedTable, juce::Colours::green.withAlpha (0.6f));

                // Set current table to yellow
                updateTableColour (currentTable, juce::Colours::yellow);
                lastHighlightedTable = currentTable;
            }
        }

        if (! isDragging)
        {
            rotation += 0.005f;

            // Rotate all wavetable objects around Y axis
            for (size_t i = 0; i < scene.getNumObjects(); ++i)
                scene.getObject (i)->setTransform (gin::Mat4f::rotationY (rotation));
        }

        repaint();
    }

    void updateTableColour (int tableIndex, juce::Colour colour)
    {
        auto* obj = scene.getObject (size_t (tableIndex));
        if (obj == nullptr)
            return;

        obj->clear();
        for (int i = 0; i < samplesPerTable - 1; ++i)
            obj->addLine (wavetablePoints[tableIndex][size_t (i)], wavetablePoints[tableIndex][size_t (i + 1)], colour);
    }

    void mouseDown (const juce::MouseEvent& e) override
    {
        lastMousePos = e.position;
        isDragging = true;
    }

    void mouseUp (const juce::MouseEvent&) override
    {
        isDragging = false;
    }

    void mouseDrag (const juce::MouseEvent& e) override
    {
        auto delta = e.position - lastMousePos;
        lastMousePos = e.position;

        if (e.mods.isShiftDown())
            scene.getCamera().pan (-delta.x * 0.01f, delta.y * 0.01f);
        else
            scene.getCamera().orbit (-delta.x * 0.01f, -delta.y * 0.01f);

        repaint();
    }

    void mouseWheelMove (const juce::MouseEvent&, const juce::MouseWheelDetails& wheel) override
    {
        scene.getCamera().zoom (wheel.deltaY * 2.0f);
        repaint();
    }

    static constexpr int numTables = 64;
    static constexpr int samplesPerTable = 128;
    static constexpr float waveHeight = 0.4f;
    static constexpr float waveWidth = 2.0f;

    gin::Scene3D scene;
    gin::Renderer3D renderer;
    gin::Object3D* tracingPointObj = nullptr;
    std::array<std::vector<gin::Vec3f>, numTables> wavetablePoints;
    float rotation = 0.0f;
    float traceTime = 0.0f;
    int lastHighlightedTable = -1;
    juce::Point<float> lastMousePos;
    bool isDragging = false;
};
