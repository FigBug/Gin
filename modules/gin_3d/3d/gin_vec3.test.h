/*==============================================================================

 Copyright 2018 - 2025 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#if GIN_UNIT_TESTS

class Vec3Tests : public juce::UnitTest
{
public:
    Vec3Tests() : juce::UnitTest ("Vec3", "gin_3d") {}

    void runTest() override
    {
        testConstruction();
        testArithmetic();
        testCompoundAssignment();
        testComparison();
        testDotProduct();
        testCrossProduct();
        testLength();
        testNormalization();
        testLerp();
    }

private:
    void testConstruction()
    {
        beginTest ("Construction");

        Vec3f defaultVec;
        expectEquals (defaultVec.x, 0.0f, "Default x should be 0");
        expectEquals (defaultVec.y, 0.0f, "Default y should be 0");
        expectEquals (defaultVec.z, 0.0f, "Default z should be 0");

        Vec3f vec (1.0f, 2.0f, 3.0f);
        expectEquals (vec.x, 1.0f, "x should be 1");
        expectEquals (vec.y, 2.0f, "y should be 2");
        expectEquals (vec.z, 3.0f, "z should be 3");

        Vec3d doubleVec (1.5, 2.5, 3.5);
        expectWithinAbsoluteError (doubleVec.x, 1.5, 0.0001, "Double x");
        expectWithinAbsoluteError (doubleVec.y, 2.5, 0.0001, "Double y");
        expectWithinAbsoluteError (doubleVec.z, 3.5, 0.0001, "Double z");
    }

    void testArithmetic()
    {
        beginTest ("Arithmetic");

        Vec3f a (1.0f, 2.0f, 3.0f);
        Vec3f b (4.0f, 5.0f, 6.0f);

        auto sum = a + b;
        expectEquals (sum.x, 5.0f, "Sum x");
        expectEquals (sum.y, 7.0f, "Sum y");
        expectEquals (sum.z, 9.0f, "Sum z");

        auto diff = b - a;
        expectEquals (diff.x, 3.0f, "Diff x");
        expectEquals (diff.y, 3.0f, "Diff y");
        expectEquals (diff.z, 3.0f, "Diff z");

        auto scaled = a * 2.0f;
        expectEquals (scaled.x, 2.0f, "Scaled x");
        expectEquals (scaled.y, 4.0f, "Scaled y");
        expectEquals (scaled.z, 6.0f, "Scaled z");

        auto divided = b / 2.0f;
        expectEquals (divided.x, 2.0f, "Divided x");
        expectEquals (divided.y, 2.5f, "Divided y");
        expectEquals (divided.z, 3.0f, "Divided z");

        auto negated = -a;
        expectEquals (negated.x, -1.0f, "Negated x");
        expectEquals (negated.y, -2.0f, "Negated y");
        expectEquals (negated.z, -3.0f, "Negated z");
    }

    void testCompoundAssignment()
    {
        beginTest ("Compound Assignment");

        Vec3f v (1.0f, 2.0f, 3.0f);

        v += Vec3f (1.0f, 1.0f, 1.0f);
        expectEquals (v.x, 2.0f, "+= x");
        expectEquals (v.y, 3.0f, "+= y");
        expectEquals (v.z, 4.0f, "+= z");

        v -= Vec3f (0.5f, 0.5f, 0.5f);
        expectWithinAbsoluteError (v.x, 1.5f, 0.0001f, "-= x");
        expectWithinAbsoluteError (v.y, 2.5f, 0.0001f, "-= y");
        expectWithinAbsoluteError (v.z, 3.5f, 0.0001f, "-= z");

        v *= 2.0f;
        expectWithinAbsoluteError (v.x, 3.0f, 0.0001f, "*= x");
        expectWithinAbsoluteError (v.y, 5.0f, 0.0001f, "*= y");
        expectWithinAbsoluteError (v.z, 7.0f, 0.0001f, "*= z");

        v /= 2.0f;
        expectWithinAbsoluteError (v.x, 1.5f, 0.0001f, "/= x");
        expectWithinAbsoluteError (v.y, 2.5f, 0.0001f, "/= y");
        expectWithinAbsoluteError (v.z, 3.5f, 0.0001f, "/= z");
    }

    void testComparison()
    {
        beginTest ("Comparison");

        Vec3f a (1.0f, 2.0f, 3.0f);
        Vec3f b (1.0f, 2.0f, 3.0f);
        Vec3f c (1.0f, 2.0f, 4.0f);

        expect (a == b, "Identical vectors should be equal");
        expect (!(a == c), "Different vectors should not be equal");
        expect (a != c, "Different vectors should be not-equal");
        expect (!(a != b), "Identical vectors should not be not-equal");
    }

    void testDotProduct()
    {
        beginTest ("Dot Product");

        Vec3f a (1.0f, 0.0f, 0.0f);
        Vec3f b (0.0f, 1.0f, 0.0f);
        expectEquals (a.dot (b), 0.0f, "Perpendicular vectors dot product should be 0");

        Vec3f c (1.0f, 2.0f, 3.0f);
        Vec3f d (4.0f, 5.0f, 6.0f);
        expectEquals (c.dot (d), 32.0f, "Dot product of (1,2,3) and (4,5,6)");

        Vec3f e (1.0f, 0.0f, 0.0f);
        expectEquals (e.dot (e), 1.0f, "Unit vector dot with itself should be 1");
    }

    void testCrossProduct()
    {
        beginTest ("Cross Product");

        Vec3f x (1.0f, 0.0f, 0.0f);
        Vec3f y (0.0f, 1.0f, 0.0f);
        Vec3f z = x.cross (y);

        expectEquals (z.x, 0.0f, "X cross Y should give Z axis (x)");
        expectEquals (z.y, 0.0f, "X cross Y should give Z axis (y)");
        expectEquals (z.z, 1.0f, "X cross Y should give Z axis (z)");

        Vec3f negZ = y.cross (x);
        expectEquals (negZ.z, -1.0f, "Y cross X should give -Z axis");

        Vec3f self = x.cross (x);
        expectEquals (self.x, 0.0f, "Cross product with self x");
        expectEquals (self.y, 0.0f, "Cross product with self y");
        expectEquals (self.z, 0.0f, "Cross product with self z");
    }

    void testLength()
    {
        beginTest ("Length");

        Vec3f unitX (1.0f, 0.0f, 0.0f);
        expectWithinAbsoluteError (unitX.length(), 1.0f, 0.0001f, "Unit vector length");

        Vec3f v (3.0f, 4.0f, 0.0f);
        expectWithinAbsoluteError (v.length(), 5.0f, 0.0001f, "3-4-5 triangle");

        Vec3f v2 (1.0f, 2.0f, 2.0f);
        expectWithinAbsoluteError (v2.length(), 3.0f, 0.0001f, "1-2-2 vector length is 3");

        expectWithinAbsoluteError (v.lengthSquared(), 25.0f, 0.0001f, "Length squared of (3,4,0)");
    }

    void testNormalization()
    {
        beginTest ("Normalization");

        Vec3f v (3.0f, 4.0f, 0.0f);
        Vec3f normalized = v.normalized();

        expectWithinAbsoluteError (normalized.length(), 1.0f, 0.0001f, "Normalized length should be 1");
        expectWithinAbsoluteError (normalized.x, 0.6f, 0.0001f, "Normalized x");
        expectWithinAbsoluteError (normalized.y, 0.8f, 0.0001f, "Normalized y");

        Vec3f v2 (0.0f, 0.0f, 5.0f);
        v2.normalize();
        expectWithinAbsoluteError (v2.z, 1.0f, 0.0001f, "In-place normalize z");
        expectWithinAbsoluteError (v2.length(), 1.0f, 0.0001f, "In-place normalized length");

        Vec3f zero (0.0f, 0.0f, 0.0f);
        Vec3f normalizedZero = zero.normalized();
        expectEquals (normalizedZero.x, 0.0f, "Normalizing zero vector should give zero");
    }

    void testLerp()
    {
        beginTest ("Lerp");

        Vec3f a (0.0f, 0.0f, 0.0f);
        Vec3f b (10.0f, 20.0f, 30.0f);

        Vec3f mid = a.lerp (b, 0.5f);
        expectEquals (mid.x, 5.0f, "Lerp 0.5 x");
        expectEquals (mid.y, 10.0f, "Lerp 0.5 y");
        expectEquals (mid.z, 15.0f, "Lerp 0.5 z");

        Vec3f start = a.lerp (b, 0.0f);
        expectEquals (start.x, 0.0f, "Lerp 0 should give start");

        Vec3f end = a.lerp (b, 1.0f);
        expectEquals (end.x, 10.0f, "Lerp 1 should give end x");
        expectEquals (end.y, 20.0f, "Lerp 1 should give end y");

        Vec3f quarter = a.lerp (b, 0.25f);
        expectWithinAbsoluteError (quarter.x, 2.5f, 0.0001f, "Lerp 0.25 x");
    }
};

static Vec3Tests vec3Tests;

#endif
