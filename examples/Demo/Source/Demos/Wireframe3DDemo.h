/*
  ==============================================================================

    Wireframe3DDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
struct Wireframe3DDemo : public DemoComponent,
                         private juce::Timer
{
    // Utah teapot - 306 control points for 32 Bezier patches (original Newell data)
    static constexpr float teapotVertices[][3] = {
        {1.4f,0.0f,2.4f}, {1.4f,-0.784f,2.4f}, {0.784f,-1.4f,2.4f}, {0.0f,-1.4f,2.4f},
        {1.3375f,0.0f,2.53125f}, {1.3375f,-0.749f,2.53125f}, {0.749f,-1.3375f,2.53125f}, {0.0f,-1.3375f,2.53125f},
        {1.4375f,0.0f,2.53125f}, {1.4375f,-0.805f,2.53125f}, {0.805f,-1.4375f,2.53125f}, {0.0f,-1.4375f,2.53125f},
        {1.5f,0.0f,2.4f}, {1.5f,-0.84f,2.4f}, {0.84f,-1.5f,2.4f}, {0.0f,-1.5f,2.4f},
        {-0.784f,-1.4f,2.4f}, {-1.4f,-0.784f,2.4f}, {-1.4f,0.0f,2.4f},
        {-0.749f,-1.3375f,2.53125f}, {-1.3375f,-0.749f,2.53125f}, {-1.3375f,0.0f,2.53125f},
        {-0.805f,-1.4375f,2.53125f}, {-1.4375f,-0.805f,2.53125f}, {-1.4375f,0.0f,2.53125f},
        {-0.84f,-1.5f,2.4f}, {-1.5f,-0.84f,2.4f}, {-1.5f,0.0f,2.4f},
        {-1.4f,0.784f,2.4f}, {-0.784f,1.4f,2.4f}, {0.0f,1.4f,2.4f},
        {-1.3375f,0.749f,2.53125f}, {-0.749f,1.3375f,2.53125f}, {0.0f,1.3375f,2.53125f},
        {-1.4375f,0.805f,2.53125f}, {-0.805f,1.4375f,2.53125f}, {0.0f,1.4375f,2.53125f},
        {-1.5f,0.84f,2.4f}, {-0.84f,1.5f,2.4f}, {0.0f,1.5f,2.4f},
        {0.784f,1.4f,2.4f}, {1.4f,0.784f,2.4f},
        {0.749f,1.3375f,2.53125f}, {1.3375f,0.749f,2.53125f},
        {0.805f,1.4375f,2.53125f}, {1.4375f,0.805f,2.53125f},
        {0.84f,1.5f,2.4f}, {1.5f,0.84f,2.4f},
        {1.75f,0.0f,1.875f}, {1.75f,-0.98f,1.875f}, {0.98f,-1.75f,1.875f}, {0.0f,-1.75f,1.875f},
        {2.0f,0.0f,1.35f}, {2.0f,-1.12f,1.35f}, {1.12f,-2.0f,1.35f}, {0.0f,-2.0f,1.35f},
        {2.0f,0.0f,0.9f}, {2.0f,-1.12f,0.9f}, {1.12f,-2.0f,0.9f}, {0.0f,-2.0f,0.9f},
        {-0.98f,-1.75f,1.875f}, {-1.75f,-0.98f,1.875f}, {-1.75f,0.0f,1.875f},
        {-1.12f,-2.0f,1.35f}, {-2.0f,-1.12f,1.35f}, {-2.0f,0.0f,1.35f},
        {-1.12f,-2.0f,0.9f}, {-2.0f,-1.12f,0.9f}, {-2.0f,0.0f,0.9f},
        {-1.75f,0.98f,1.875f}, {-0.98f,1.75f,1.875f}, {0.0f,1.75f,1.875f},
        {-2.0f,1.12f,1.35f}, {-1.12f,2.0f,1.35f}, {0.0f,2.0f,1.35f},
        {-2.0f,1.12f,0.9f}, {-1.12f,2.0f,0.9f}, {0.0f,2.0f,0.9f},
        {0.98f,1.75f,1.875f}, {1.75f,0.98f,1.875f},
        {1.12f,2.0f,1.35f}, {2.0f,1.12f,1.35f},
        {1.12f,2.0f,0.9f}, {2.0f,1.12f,0.9f},
        {2.0f,0.0f,0.45f}, {2.0f,-1.12f,0.45f}, {1.12f,-2.0f,0.45f}, {0.0f,-2.0f,0.45f},
        {1.5f,0.0f,0.225f}, {1.5f,-0.84f,0.225f}, {0.84f,-1.5f,0.225f}, {0.0f,-1.5f,0.225f},
        {1.5f,0.0f,0.15f}, {1.5f,-0.84f,0.15f}, {0.84f,-1.5f,0.15f}, {0.0f,-1.5f,0.15f},
        {-1.12f,-2.0f,0.45f}, {-2.0f,-1.12f,0.45f}, {-2.0f,0.0f,0.45f},
        {-0.84f,-1.5f,0.225f}, {-1.5f,-0.84f,0.225f}, {-1.5f,0.0f,0.225f},
        {-0.84f,-1.5f,0.15f}, {-1.5f,-0.84f,0.15f}, {-1.5f,0.0f,0.15f},
        {-2.0f,1.12f,0.45f}, {-1.12f,2.0f,0.45f}, {0.0f,2.0f,0.45f},
        {-1.5f,0.84f,0.225f}, {-0.84f,1.5f,0.225f}, {0.0f,1.5f,0.225f},
        {-1.5f,0.84f,0.15f}, {-0.84f,1.5f,0.15f}, {0.0f,1.5f,0.15f},
        {1.12f,2.0f,0.45f}, {2.0f,1.12f,0.45f},
        {0.84f,1.5f,0.225f}, {1.5f,0.84f,0.225f},
        {0.84f,1.5f,0.15f}, {1.5f,0.84f,0.15f},
        {-1.6f,0.0f,2.025f}, {-1.6f,-0.3f,2.025f}, {-1.5f,-0.3f,2.25f}, {-1.5f,0.0f,2.25f},
        {-2.3f,0.0f,2.025f}, {-2.3f,-0.3f,2.025f}, {-2.5f,-0.3f,2.25f}, {-2.5f,0.0f,2.25f},
        {-2.7f,0.0f,2.025f}, {-2.7f,-0.3f,2.025f}, {-3.0f,-0.3f,2.25f}, {-3.0f,0.0f,2.25f},
        {-2.7f,0.0f,1.8f}, {-2.7f,-0.3f,1.8f}, {-3.0f,-0.3f,1.8f}, {-3.0f,0.0f,1.8f},
        {-1.5f,0.3f,2.25f}, {-1.6f,0.3f,2.025f},
        {-2.5f,0.3f,2.25f}, {-2.3f,0.3f,2.025f},
        {-3.0f,0.3f,2.25f}, {-2.7f,0.3f,2.025f},
        {-3.0f,0.3f,1.8f}, {-2.7f,0.3f,1.8f},
        {-2.7f,0.0f,1.575f}, {-2.7f,-0.3f,1.575f}, {-3.0f,-0.3f,1.35f}, {-3.0f,0.0f,1.35f},
        {-2.5f,0.0f,1.125f}, {-2.5f,-0.3f,1.125f}, {-2.65f,-0.3f,0.9375f}, {-2.65f,0.0f,0.9375f},
        {-2.0f,-0.3f,0.9f}, {-1.9f,-0.3f,0.6f}, {-1.9f,0.0f,0.6f},
        {-3.0f,0.3f,1.35f}, {-2.7f,0.3f,1.575f},
        {-2.65f,0.3f,0.9375f}, {-2.5f,0.3f,1.125f},
        {-1.9f,0.3f,0.6f}, {-2.0f,0.3f,0.9f},
        {1.7f,0.0f,1.425f}, {1.7f,-0.66f,1.425f}, {1.7f,-0.66f,0.6f}, {1.7f,0.0f,0.6f},
        {2.6f,0.0f,1.425f}, {2.6f,-0.66f,1.425f}, {3.1f,-0.66f,0.825f}, {3.1f,0.0f,0.825f},
        {2.3f,0.0f,2.1f}, {2.3f,-0.25f,2.1f}, {2.4f,-0.25f,2.025f}, {2.4f,0.0f,2.025f},
        {2.7f,0.0f,2.4f}, {2.7f,-0.25f,2.4f}, {3.3f,-0.25f,2.4f}, {3.3f,0.0f,2.4f},
        {1.7f,0.66f,0.6f}, {1.7f,0.66f,1.425f},
        {3.1f,0.66f,0.825f}, {2.6f,0.66f,1.425f},
        {2.4f,0.25f,2.025f}, {2.3f,0.25f,2.1f},
        {3.3f,0.25f,2.4f}, {2.7f,0.25f,2.4f},
        {2.8f,0.0f,2.475f}, {2.8f,-0.25f,2.475f}, {3.525f,-0.25f,2.49375f}, {3.525f,0.0f,2.49375f},
        {2.9f,0.0f,2.475f}, {2.9f,-0.15f,2.475f}, {3.45f,-0.15f,2.5125f}, {3.45f,0.0f,2.5125f},
        {2.8f,0.0f,2.4f}, {2.8f,-0.15f,2.4f}, {3.2f,-0.15f,2.4f}, {3.2f,0.0f,2.4f},
        {3.525f,0.25f,2.49375f}, {2.8f,0.25f,2.475f},
        {3.45f,0.15f,2.5125f}, {2.9f,0.15f,2.475f},
        {3.2f,0.15f,2.4f}, {2.8f,0.15f,2.4f},
        {0.0f,0.0f,3.15f}, {0.0f,-0.002f,3.15f}, {0.002f,0.0f,3.15f},
        {0.8f,0.0f,3.15f}, {0.8f,-0.45f,3.15f}, {0.45f,-0.8f,3.15f}, {0.0f,-0.8f,3.15f},
        {0.0f,0.0f,2.85f},
        {0.2f,0.0f,2.7f}, {0.2f,-0.112f,2.7f}, {0.112f,-0.2f,2.7f}, {0.0f,-0.2f,2.7f},
        {-0.002f,0.0f,3.15f}, {-0.45f,-0.8f,3.15f}, {-0.8f,-0.45f,3.15f}, {-0.8f,0.0f,3.15f},
        {-0.112f,-0.2f,2.7f}, {-0.2f,-0.112f,2.7f}, {-0.2f,0.0f,2.7f},
        {0.0f,0.002f,3.15f}, {-0.8f,0.45f,3.15f}, {-0.45f,0.8f,3.15f}, {0.0f,0.8f,3.15f},
        {-0.2f,0.112f,2.7f}, {-0.112f,0.2f,2.7f}, {0.0f,0.2f,2.7f},
        {0.45f,0.8f,3.15f}, {0.8f,0.45f,3.15f},
        {0.112f,0.2f,2.7f}, {0.2f,0.112f,2.7f},
        {0.4f,0.0f,2.55f}, {0.4f,-0.224f,2.55f}, {0.224f,-0.4f,2.55f}, {0.0f,-0.4f,2.55f},
        {1.3f,0.0f,2.55f}, {1.3f,-0.728f,2.55f}, {0.728f,-1.3f,2.55f}, {0.0f,-1.3f,2.55f},
        {1.3f,0.0f,2.4f}, {1.3f,-0.728f,2.4f}, {0.728f,-1.3f,2.4f}, {0.0f,-1.3f,2.4f},
        {-0.224f,-0.4f,2.55f}, {-0.4f,-0.224f,2.55f}, {-0.4f,0.0f,2.55f},
        {-0.728f,-1.3f,2.55f}, {-1.3f,-0.728f,2.55f}, {-1.3f,0.0f,2.55f},
        {-0.728f,-1.3f,2.4f}, {-1.3f,-0.728f,2.4f}, {-1.3f,0.0f,2.4f},
        {-0.4f,0.224f,2.55f}, {-0.224f,0.4f,2.55f}, {0.0f,0.4f,2.55f},
        {-1.3f,0.728f,2.55f}, {-0.728f,1.3f,2.55f}, {0.0f,1.3f,2.55f},
        {-1.3f,0.728f,2.4f}, {-0.728f,1.3f,2.4f}, {0.0f,1.3f,2.4f},
        {0.224f,0.4f,2.55f}, {0.4f,0.224f,2.55f},
        {0.728f,1.3f,2.55f}, {1.3f,0.728f,2.55f},
        {0.728f,1.3f,2.4f}, {1.3f,0.728f,2.4f},
        {0.0f,0.0f,0.0f},
        {1.5f,0.0f,0.15f}, {1.5f,0.84f,0.15f}, {0.84f,1.5f,0.15f}, {0.0f,1.5f,0.15f},
        {1.5f,0.0f,0.075f}, {1.5f,0.84f,0.075f}, {0.84f,1.5f,0.075f}, {0.0f,1.5f,0.075f},
        {1.425f,0.0f,0.0f}, {1.425f,0.798f,0.0f}, {0.798f,1.425f,0.0f}, {0.0f,1.425f,0.0f},
        {-0.84f,1.5f,0.15f}, {-1.5f,0.84f,0.15f}, {-1.5f,0.0f,0.15f},
        {-0.84f,1.5f,0.075f}, {-1.5f,0.84f,0.075f}, {-1.5f,0.0f,0.075f},
        {-0.798f,1.425f,0.0f}, {-1.425f,0.798f,0.0f}, {-1.425f,0.0f,0.0f},
        {-1.5f,-0.84f,0.15f}, {-0.84f,-1.5f,0.15f}, {0.0f,-1.5f,0.15f},
        {-1.5f,-0.84f,0.075f}, {-0.84f,-1.5f,0.075f}, {0.0f,-1.5f,0.075f},
        {-1.425f,-0.798f,0.0f}, {-0.798f,-1.425f,0.0f}, {0.0f,-1.425f,0.0f},
        {0.84f,-1.5f,0.15f}, {1.5f,-0.84f,0.15f},
        {0.84f,-1.5f,0.075f}, {1.5f,-0.84f,0.075f},
        {0.798f,-1.425f,0.0f}, {1.425f,-0.798f,0.0f}
    };

    // 32 Bezier patches, each with 16 control point indices (1-based, will subtract 1)
    static constexpr int teapotPatches[32][16] = {
        {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16},
        {4,17,18,19,8,20,21,22,12,23,24,25,16,26,27,28},
        {19,29,30,31,22,32,33,34,25,35,36,37,28,38,39,40},
        {31,41,42,1,34,43,44,5,37,45,46,9,40,47,48,13},
        {13,14,15,16,49,50,51,52,53,54,55,56,57,58,59,60},
        {16,26,27,28,52,61,62,63,56,64,65,66,60,67,68,69},
        {28,38,39,40,63,70,71,72,66,73,74,75,69,76,77,78},
        {40,47,48,13,72,79,80,49,75,81,82,53,78,83,84,57},
        {57,58,59,60,85,86,87,88,89,90,91,92,93,94,95,96},
        {60,67,68,69,88,97,98,99,92,100,101,102,96,103,104,105},
        {69,76,77,78,99,106,107,108,102,109,110,111,105,112,113,114},
        {78,83,84,57,108,115,116,85,111,117,118,89,114,119,120,93},
        {121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136},
        {124,137,138,121,128,139,140,125,132,141,142,129,136,143,144,133},
        {133,134,135,136,145,146,147,148,149,150,151,152,69,153,154,155},
        {136,143,144,133,148,156,157,145,152,158,159,149,155,160,161,69},
        {162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177},
        {165,178,179,162,169,180,181,166,173,182,183,170,177,184,185,174},
        {174,175,176,177,186,187,188,189,190,191,192,193,194,195,196,197},
        {177,184,185,174,189,198,199,186,193,200,201,190,197,202,203,194},
        {204,204,204,204,207,208,209,210,211,211,211,211,212,213,214,215},
        {204,204,204,204,210,217,218,219,211,211,211,211,215,220,221,222},
        {204,204,204,204,219,224,225,226,211,211,211,211,222,227,228,229},
        {204,204,204,204,226,230,231,207,211,211,211,211,229,232,233,212},
        {212,213,214,215,234,235,236,237,238,239,240,241,242,243,244,245},
        {215,220,221,222,237,246,247,248,241,249,250,251,245,252,253,254},
        {222,227,228,229,248,255,256,257,251,258,259,260,254,261,262,263},
        {229,232,233,212,257,264,265,234,260,266,267,238,263,268,269,242},
        {270,270,270,270,279,280,281,282,275,276,277,278,271,272,273,274},
        {270,270,270,270,282,289,290,291,278,286,287,288,274,283,284,285},
        {270,270,270,270,291,298,299,300,288,295,296,297,285,292,293,294},
        {270,270,270,270,300,305,306,279,297,303,304,275,294,301,302,271}
    };

    // Evaluate a point on a Bezier patch at parameters (u, v)
    static gin::Vec3f evaluateBezierPatch (const int patchIndices[16], float u, float v)
    {
        // Bernstein polynomials for cubic Bezier
        auto bernstein = [] (float t, int i) -> float {
            float mt = 1.0f - t;
            switch (i) {
                case 0: return mt * mt * mt;
                case 1: return 3.0f * mt * mt * t;
                case 2: return 3.0f * mt * t * t;
                case 3: return t * t * t;
                default: return 0.0f;
            }
        };

        gin::Vec3f result { 0, 0, 0 };

        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                int idx = patchIndices[i * 4 + j] - 1;  // Convert 1-based to 0-based
                if (idx >= 0 && idx < 306)
                {
                    float weight = bernstein (u, j) * bernstein (v, i);
                    result.x += teapotVertices[idx][0] * weight;
                    result.y += teapotVertices[idx][1] * weight;
                    result.z += teapotVertices[idx][2] * weight;
                }
            }
        }

        return result;
    }

    static void addTeapot (gin::Object3D& obj, float scale, juce::Colour colour, int subdivisions = 8)
    {
        float offsetZ = 1.5f;

        for (int p = 0; p < 32; ++p)
        {
            for (int i = 0; i < subdivisions; ++i)
            {
                for (int j = 0; j < subdivisions; ++j)
                {
                    float u0 = float (i) / float (subdivisions);
                    float u1 = float (i + 1) / float (subdivisions);
                    float v0 = float (j) / float (subdivisions);
                    float v1 = float (j + 1) / float (subdivisions);

                    auto p00 = evaluateBezierPatch (teapotPatches[p], u0, v0);
                    auto p10 = evaluateBezierPatch (teapotPatches[p], u1, v0);
                    auto p01 = evaluateBezierPatch (teapotPatches[p], u0, v1);

                    // Transform: swap Y and Z, center vertically, apply scale
                    gin::Vec3f v1pt { p00.x * scale, (p00.z - offsetZ) * scale, p00.y * scale };
                    gin::Vec3f v2pt { p10.x * scale, (p10.z - offsetZ) * scale, p10.y * scale };
                    gin::Vec3f v3pt { p01.x * scale, (p01.z - offsetZ) * scale, p01.y * scale };

                    // Add horizontal and vertical lines of the grid
                    obj.addLine (v1pt, v2pt, colour);
                    obj.addLine (v1pt, v3pt, colour);
                }
            }
        }
    }

    Wireframe3DDemo()
    {
        setName ("Wireframe 3D");

        // Set up the camera
        scene.getCamera().setPosition (0, 2, 6);
        scene.getCamera().setTarget (0, 0, 0);
        scene.getCamera().setPerspective (45.0f, 1.0f, 0.1f, 100.0f);

        // Add a grid
        auto* grid = scene.addObject();
        grid->addGrid (10.0f, 10, juce::Colours::grey.withAlpha (0.3f));

        // Add axes
        auto* axes = scene.addObject();
        axes->addAxes (2.0f);

        // Add the teapot
        teapotObj = scene.addObject();
        addTeapot (*teapotObj, 0.5f, juce::Colours::cyan);

        startTimerHz (60);
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colour (0xff1a1a2e));

        // Update camera aspect ratio
        scene.getCamera().setPerspective (45.0f, getWidth() / (float) getHeight(), 0.1f, 100.0f);

        // Render the scene
        renderer.render (g, scene, getLocalBounds());

        // Draw instructions
        g.setColour (juce::Colours::white.withAlpha (0.7f));
        g.setFont (12.0f);
        g.drawText ("Drag to orbit, scroll to zoom, shift+drag to pan",
                    getLocalBounds().removeFromBottom (25), juce::Justification::centred);
    }

    void timerCallback() override
    {
        // Auto-rotate the teapot
        if (! isDragging)
        {
            rotation += 0.01f;
            teapotObj->setTransform (gin::Mat4f::rotationY (rotation));
            repaint();
        }
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
        {
            // Pan
            scene.getCamera().pan (-delta.x * 0.01f, delta.y * 0.01f);
        }
        else
        {
            // Orbit
            scene.getCamera().orbit (-delta.x * 0.01f, -delta.y * 0.01f);
        }
        repaint();
    }

    void mouseWheelMove (const juce::MouseEvent&, const juce::MouseWheelDetails& wheel) override
    {
        scene.getCamera().zoom (wheel.deltaY * 2.0f);
        repaint();
    }

    gin::Scene3D scene;
    gin::Renderer3D renderer;
    gin::Object3D* teapotObj = nullptr;
    float rotation = 0.0f;
    juce::Point<float> lastMousePos;
    bool isDragging = false;
};
