/*==============================================================================

 Copyright 2018 - 2025 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#if GIN_UNIT_TESTS

class Mat4Tests : public juce::UnitTest
{
public:
    Mat4Tests() : juce::UnitTest ("Mat4", "gin_3d") {}

    void runTest() override
    {
        testIdentity();
        testTranslation();
        testScale();
        testRotation();
        testMultiplication();
        testTransformPoint();
        testTransformVector();
        testLookAt();
        testPerspective();
    }

private:
    void testIdentity()
    {
        beginTest ("Identity");

        Mat4f identity = Mat4f::identity();

        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                float expected = (i == j) ? 1.0f : 0.0f;
                expectWithinAbsoluteError (identity.m[i][j], expected,
                    0.0001f, "Identity matrix element [" + juce::String(i) + "][" + juce::String(j) + "]");
            }
        }

        Vec3f point (1.0f, 2.0f, 3.0f);
        Vec3f transformed = identity.transformPoint (point);
        expectWithinAbsoluteError (transformed.x, point.x, 0.0001f, "Identity transform x");
        expectWithinAbsoluteError (transformed.y, point.y, 0.0001f, "Identity transform y");
        expectWithinAbsoluteError (transformed.z, point.z, 0.0001f, "Identity transform z");
    }

    void testTranslation()
    {
        beginTest ("Translation");

        Mat4f trans = Mat4f::translation (5.0f, 10.0f, 15.0f);

        Vec3f origin (0.0f, 0.0f, 0.0f);
        Vec3f translated = trans.transformPoint (origin);

        expectWithinAbsoluteError (translated.x, 5.0f, 0.0001f, "Translate origin x");
        expectWithinAbsoluteError (translated.y, 10.0f, 0.0001f, "Translate origin y");
        expectWithinAbsoluteError (translated.z, 15.0f, 0.0001f, "Translate origin z");

        Vec3f point (1.0f, 2.0f, 3.0f);
        Vec3f translatedPoint = trans.transformPoint (point);

        expectWithinAbsoluteError (translatedPoint.x, 6.0f, 0.0001f, "Translate point x");
        expectWithinAbsoluteError (translatedPoint.y, 12.0f, 0.0001f, "Translate point y");
        expectWithinAbsoluteError (translatedPoint.z, 18.0f, 0.0001f, "Translate point z");

        Mat4f transVec = Mat4f::translation (Vec3f (5.0f, 10.0f, 15.0f));
        Vec3f translatedVec = transVec.transformPoint (origin);
        expectWithinAbsoluteError (translatedVec.x, 5.0f, 0.0001f, "Translate with Vec3 x");
    }

    void testScale()
    {
        beginTest ("Scale");

        Mat4f scaleMatrix = Mat4f::scale (2.0f, 3.0f, 4.0f);

        Vec3f point (1.0f, 1.0f, 1.0f);
        Vec3f scaled = scaleMatrix.transformPoint (point);

        expectWithinAbsoluteError (scaled.x, 2.0f, 0.0001f, "Scale x");
        expectWithinAbsoluteError (scaled.y, 3.0f, 0.0001f, "Scale y");
        expectWithinAbsoluteError (scaled.z, 4.0f, 0.0001f, "Scale z");

        Mat4f uniformScale = Mat4f::scale (5.0f);
        Vec3f uniformScaled = uniformScale.transformPoint (point);

        expectWithinAbsoluteError (uniformScaled.x, 5.0f, 0.0001f, "Uniform scale x");
        expectWithinAbsoluteError (uniformScaled.y, 5.0f, 0.0001f, "Uniform scale y");
        expectWithinAbsoluteError (uniformScaled.z, 5.0f, 0.0001f, "Uniform scale z");
    }

    void testRotation()
    {
        beginTest ("Rotation");

        float pi = juce::MathConstants<float>::pi;

        // Rotate around Z by 90 degrees: (1,0,0) -> (0,1,0)
        Mat4f rotZ = Mat4f::rotationZ (pi / 2.0f);
        Vec3f pointX (1.0f, 0.0f, 0.0f);
        Vec3f rotatedZ = rotZ.transformPoint (pointX);

        expectWithinAbsoluteError (rotatedZ.x, 0.0f, 0.0001f, "Rotate Z 90: x");
        expectWithinAbsoluteError (rotatedZ.y, 1.0f, 0.0001f, "Rotate Z 90: y");
        expectWithinAbsoluteError (rotatedZ.z, 0.0f, 0.0001f, "Rotate Z 90: z");

        // Rotate around Y by 90 degrees: (1,0,0) -> (0,0,-1)
        Mat4f rotY = Mat4f::rotationY (pi / 2.0f);
        Vec3f rotatedY = rotY.transformPoint (pointX);

        expectWithinAbsoluteError (rotatedY.x, 0.0f, 0.0001f, "Rotate Y 90: x");
        expectWithinAbsoluteError (rotatedY.y, 0.0f, 0.0001f, "Rotate Y 90: y");
        expectWithinAbsoluteError (rotatedY.z, -1.0f, 0.0001f, "Rotate Y 90: z");

        // Rotate around X by 90 degrees: (0,1,0) -> (0,0,1)
        Mat4f rotX = Mat4f::rotationX (pi / 2.0f);
        Vec3f pointY (0.0f, 1.0f, 0.0f);
        Vec3f rotatedX = rotX.transformPoint (pointY);

        expectWithinAbsoluteError (rotatedX.x, 0.0f, 0.0001f, "Rotate X 90: x");
        expectWithinAbsoluteError (rotatedX.y, 0.0f, 0.0001f, "Rotate X 90: y");
        expectWithinAbsoluteError (rotatedX.z, 1.0f, 0.0001f, "Rotate X 90: z");

        // Rotate 360 degrees should return to original
        Mat4f rot360 = Mat4f::rotationZ (2.0f * pi);
        Vec3f rotated360 = rot360.transformPoint (pointX);

        expectWithinAbsoluteError (rotated360.x, 1.0f, 0.0001f, "Rotate 360: x");
        expectWithinAbsoluteError (rotated360.y, 0.0f, 0.0001f, "Rotate 360: y");
    }

    void testMultiplication()
    {
        beginTest ("Multiplication");

        Mat4f trans = Mat4f::translation (10.0f, 0.0f, 0.0f);
        Mat4f scale = Mat4f::scale (2.0f);

        // Scale then translate
        Mat4f combined = trans * scale;

        Vec3f point (1.0f, 0.0f, 0.0f);
        Vec3f result = combined.transformPoint (point);

        // Point is scaled to (2,0,0) then translated to (12,0,0)
        expectWithinAbsoluteError (result.x, 12.0f, 0.0001f, "Scale then translate x");

        // Identity multiplication
        Mat4f identity = Mat4f::identity();
        Mat4f result2 = trans * identity;

        Vec3f origin (0.0f, 0.0f, 0.0f);
        Vec3f transOrigin = result2.transformPoint (origin);
        expectWithinAbsoluteError (transOrigin.x, 10.0f, 0.0001f, "Multiply by identity");
    }

    void testTransformPoint()
    {
        beginTest ("Transform Point");

        Mat4f combined = Mat4f::translation (1.0f, 2.0f, 3.0f) * Mat4f::scale (2.0f);

        Vec3f point (1.0f, 1.0f, 1.0f);
        Vec3f result = combined.transformPoint (point);

        // Scale to (2,2,2), then translate to (3,4,5)
        expectWithinAbsoluteError (result.x, 3.0f, 0.0001f, "Combined transform x");
        expectWithinAbsoluteError (result.y, 4.0f, 0.0001f, "Combined transform y");
        expectWithinAbsoluteError (result.z, 5.0f, 0.0001f, "Combined transform z");
    }

    void testTransformVector()
    {
        beginTest ("Transform Vector");

        Mat4f trans = Mat4f::translation (100.0f, 100.0f, 100.0f);

        Vec3f direction (1.0f, 0.0f, 0.0f);
        Vec3f transformed = trans.transformVector (direction);

        // Translation should not affect vectors (only directions)
        expectWithinAbsoluteError (transformed.x, 1.0f, 0.0001f, "Vector unaffected by translation x");
        expectWithinAbsoluteError (transformed.y, 0.0f, 0.0001f, "Vector unaffected by translation y");
        expectWithinAbsoluteError (transformed.z, 0.0f, 0.0001f, "Vector unaffected by translation z");

        Mat4f scale = Mat4f::scale (3.0f);
        Vec3f scaledDir = scale.transformVector (direction);

        expectWithinAbsoluteError (scaledDir.x, 3.0f, 0.0001f, "Vector scaled x");
    }

    void testLookAt()
    {
        beginTest ("Look At");

        Vec3f eye (0.0f, 0.0f, 5.0f);
        Vec3f target (0.0f, 0.0f, 0.0f);
        Vec3f up (0.0f, 1.0f, 0.0f);

        Mat4f view = Mat4f::lookAt (eye, target, up);

        // Transform the eye position - it should be at origin in view space
        Vec3f eyeInView = view.transformPoint (eye);
        expectWithinAbsoluteError (eyeInView.x, 0.0f, 0.0001f, "Eye at view origin x");
        expectWithinAbsoluteError (eyeInView.y, 0.0f, 0.0001f, "Eye at view origin y");
        expectWithinAbsoluteError (eyeInView.z, 0.0f, 0.0001f, "Eye at view origin z");

        // Target should be in front of camera (negative Z in view space)
        Vec3f targetInView = view.transformPoint (target);
        expect (targetInView.z < 0.0f, "Target should be in front of camera");
    }

    void testPerspective()
    {
        beginTest ("Perspective");

        float fov = juce::MathConstants<float>::pi / 4.0f; // 45 degrees
        float aspect = 16.0f / 9.0f;
        float nearZ = 0.1f;
        float farZ = 100.0f;

        Mat4f proj = Mat4f::perspective (fov, aspect, nearZ, farZ);

        // Point at origin should remain at origin after perspective (before divide)
        Vec3f origin (0.0f, 0.0f, 0.0f);
        auto clipOrigin = proj.project (origin);

        expectWithinAbsoluteError (clipOrigin[0], 0.0f, 0.0001f, "Origin clip x");
        expectWithinAbsoluteError (clipOrigin[1], 0.0f, 0.0001f, "Origin clip y");

        // Point on the near plane along -Z
        Vec3f nearPoint (0.0f, 0.0f, -nearZ);
        auto clipNear = proj.project (nearPoint);
        float ndcZ = clipNear[2] / clipNear[3];

        // Near plane should map to NDC z = -1
        expectWithinAbsoluteError (ndcZ, -1.0f, 0.01f, "Near plane maps to -1");

        // Point on the far plane along -Z
        Vec3f farPoint (0.0f, 0.0f, -farZ);
        auto clipFar = proj.project (farPoint);
        float farNdcZ = clipFar[2] / clipFar[3];

        // Far plane should map to NDC z = 1
        expectWithinAbsoluteError (farNdcZ, 1.0f, 0.01f, "Far plane maps to 1");
    }
};

static Mat4Tests mat4Tests;

#endif
