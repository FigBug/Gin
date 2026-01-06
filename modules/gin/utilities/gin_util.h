/*==============================================================================

 Copyright 2018 - 2025 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================
/** Checks if a boolean flag is set, and if so, clears it and returns true.
    This is useful for handling one-shot flags that need to be checked and reset atomically.

    @param flag  The boolean flag to check and reset
    @returns     true if the flag was set (and has now been cleared), false if it was already clear
*/
inline bool compareAndReset (bool& flag)
{
    if (flag)
    {
        flag = false;
        return true;
    }
    return false;
}

/** Calculates the Root Mean Square (RMS) of an array of values.
    RMS is the square root of the mean of the squares of the values, commonly used
    to measure the magnitude of a varying signal.

    @param values  Pointer to an array of float values
    @param n       Number of values in the array
    @returns       The RMS value
*/
inline float calculateRMS (const float* values, int n)
{
    float rms = 0;

    for (int i = 0; i < n; i++)
        rms += values[i] * values[i];

    return std::sqrt ((1.0f / float ( n )) * rms);
}

/** Calculates the median value of an array of values.
    The median is the middle value when the values are sorted. For an even number
    of values, it returns the average of the two middle values.

    @param values  Pointer to an array of float values
    @param n       Number of values in the array
    @returns       The median value
*/
inline float calculateMedian (const float* values, int n)
{
    juce::Array<float> f;
    f.insertArray (0, values, n);
    f.sort();

    if (f.size() % 2 == 0)
        return (f[f.size() / 2] + f[f.size() / 2 - 1]) / 2.0f;

    return f[f.size() / 2];
}

//==============================================================================
/** Randomly shuffles the elements of an array using the Fisher-Yates algorithm.
    This produces an unbiased random permutation where each possible ordering
    has equal probability of occurring.

    @param r      Reference to a juce::Random object used for generating random indices
    @param array  The array to shuffle (must support size() and swap() methods)
*/
template <typename T>
void shuffleArray (juce::Random& r, T& array)
{
    const int n = array.size();
    for (int i = n - 1; i >= 1; i--)
    {
        int j = r.nextInt (i + 1);
        array.swap (i, j);
    }
}

//==============================================================================
/** Generates Perlin noise - a type of gradient noise with natural-looking characteristics.

    Perlin noise is widely used in procedural generation for creating textures, terrain,
    and other organic-looking random variations. It produces smooth, coherent noise with
    controllable frequency and amplitude.

    Based on the reference implementation by Ken Perlin.
    See: http://mrl.nyu.edu/~perlin/paper445.pdf

    @see noise
*/
template <class T>
class PerlinNoise
{
public:
    /** Creates a PerlinNoise generator with the default permutation table. */
    PerlinNoise()
    {
        p = { 151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
            8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
            35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,
            134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
            55,46,245,40,244,102,143,54, 65,25,63,161,1,216,80,73,209,76,132,187,208, 89,
            18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,
            250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
            189,28,42,223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167,
            43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,
            97,228,251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,
            107,49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
            138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180 };

        for (auto i = 0; i < 256; i++)
            p.add (p[i]);
    }

    /** Creates a PerlinNoise generator with a randomized permutation table.

        @param seed  The random seed used to generate the permutation table
    */
    PerlinNoise (unsigned int seed)
    {
        juce::Random r (seed);

        for (int i = 0; i <= 255; i++)
            p.add (i);

        shuffleArray (r, p);

        for (auto i = 0; i < 256; i++)
            p.add (p[i]);
    }

    /** Generates a Perlin noise value at the specified coordinates.

        @param x  The x coordinate
        @param y  The y coordinate (defaults to 0 for 1D or 2D noise)
        @param z  The z coordinate (defaults to 0 for 1D or 2D noise)
        @returns  A noise value in the range [0, 1]
    */
    T noise (T x, T y = 0, T z = 0)
    {
        jassert (p.size() == 512);
        
        int X = (int) std::floor (x) & 255;
        int Y = (int) std::floor (y) & 255;
        int Z = (int) std::floor (z) & 255;

        x -= std::floor (x);
        y -= std::floor (y);
        z -= std::floor (z);

        T u = fade (x);
        T v = fade (y);
        T w = fade (z);

        int A  = p[X] + Y;
        int AA = p[A] + Z;
        int AB = p[A + 1] + Z;
        int B  = p[X + 1] + Y;
        int BA = p[B] + Z;
        int BB = p[B + 1] + Z;

        // Add blended results from 8 corners of cube
        T res = lerp (w, lerp (v, lerp (u, grad (p[AA  ], x,   y,   z   ),
                                           grad (p[BA  ], x-1, y,   z   )),
                                  lerp (u, grad (p[AB  ], x,   y-1, z   ),
                                           grad (p[BB  ], x-1, y-1, z   ))),
                         lerp (v, lerp (u, grad (p[AA+1], x,   y,   z-1 ),
                                           grad (p[BA+1], x-1, y,   z-1 )),
                                  lerp (u, grad (p[AB+1], x,   y-1, z-1 ),
                                           grad (p[BB+1], x-1, y-1, z-1 ))));

        return T ((res + 1.0) / 2.0);
    }

private:
    T fade (T t)
    {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    T lerp (T t, T a, T b)
    {
        return a + t * (b - a);
    }

    T grad (int hash, T x, T y, T z)
    {
        int h = hash & 15;
        T u = h < 8 ? x : y, v = h < 4 ? y : h == 12 || h == 14 ? x : z;
        return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    }

    juce::Array<int> p;
};

//==============================================================================
/** Maintains a rolling average of a series of numbers.

    This class computes a running average that incorporates new values while maintaining
    a weighted influence from previous values. Useful for smoothing time-series data.

    @see average, getAverage, setAverage
*/
class RollingAverage
{
public:
    /** Creates a RollingAverage with the specified weighting factor.

        @param numVals_  The number of values to weight the current average by when
                         incorporating new values. Higher values give more weight to
                         the existing average.
    */
    RollingAverage (int numVals_)
      : numVals (numVals_)
    {
    }

    /** Updates the rolling average with a new value and returns the new average.

        @param nextValue  The new value to incorporate into the average
        @returns          The updated rolling average
    */
    double average (double nextValue)
    {
        currAvg = (nextValue + numVals * currAvg) / (double)(numVals + 1);
        return currAvg;
    }

    /** Returns the current rolling average value.

        @returns  The current average
    */
    double getAverage()
    {
        return currAvg;
    }

    /** Sets the current rolling average to a specific value.

        @param avg  The new average value
    */
    void setAverage (double avg)
    {
        currAvg = avg;
    }

private:
    int numVals = 0;
    double currAvg = 0.0;
};

//==============================================================================
/** A simple RAII-based time profiler for measuring execution duration.

    Create an instance at the start of a scope to measure how long that scope takes
    to execute. When the instance is destroyed, it prints the elapsed time to the
    debug console.

    Example usage:
    @code
    {
        TimeProfiler timer ("MyFunction");
        // ... code to profile ...
    } // Prints "MyFunction 1.23s" to debug output
    @endcode
*/
class TimeProfiler
{
public:
    /** Creates a TimeProfiler with the specified name.

        @param name_  The name to display in the debug output
    */
    TimeProfiler (const juce::String& name_) :
    name (name_), start (juce::Time::getMillisecondCounterHiRes()) {}

    /** Destructor that outputs the elapsed time to the debug console. */
    ~TimeProfiler()
    {
        DBG (name + juce::String::formatted (" %.2fs", (juce::Time::getMillisecondCounterHiRes() - start) / 1000.0));
    }

private:
    juce::String name;
    double start;
};

/** Converts a version string to an integer for comparison purposes.

    Parses version strings in the format "major.minor.patch" and converts them
    to a single integer value that can be compared numerically.

    @param versionString  The version string to convert (e.g., "1.2.3")
    @returns              An integer representation of the version
*/
int versionStringToInt (const juce::String& versionString);

//==============================================================================
/** Executes a lambda function after a specified delay.

    The callback will be executed on the message thread after the specified delay.
    This is useful for deferred execution of code that needs to run later.

    @param callback  The function to execute after the delay
    @param delayMS   The delay in milliseconds before executing the callback
*/
void delayedLambda (std::function<void ()> callback, int delayMS);

/** Formats a number as a string with appropriate decimal precision.

    Automatically chooses the number of decimal places based on the magnitude
    of the value:
    - Values >= 10: No decimal places (rounded to integer)
    - Values >= 1: 1 decimal place
    - Values >= 0.1: 2 decimal places
    - Values < 0.1: 3 decimal places
    - Values approximately equal to 0: Returns "0"

    @param v  The numeric value to format
    @returns  A formatted string representation of the number
*/
template <typename T>
juce::String formatNumber (T v)
{
    if (juce::approximatelyEqual (v, T(0.0)))
        return "0";
    
    int dec = 0;
    if (std::abs (v) < 10)   dec = 1;
    if (std::abs (v) < 1)    dec = 2;
    if (std::abs (v) < 0.1)  dec = 3;
    
    if (dec == 0)
        return juce::String (juce::roundToInt (v));

    return juce::String (v, dec);
}

/** A utility class that adapts an ActionBroadcaster to use a lambda callback.

    This class provides a convenient way to listen to ActionBroadcaster messages
    using a lambda or std::function instead of implementing the ActionListener
    interface directly.

    The listener is automatically registered on construction and unregistered
    on destruction.

    Example usage:
    @code
    juce::ActionBroadcaster broadcaster;

    ActionMessageLambda listener (broadcaster, [] (const juce::String& message)
    {
        DBG ("Received: " + message);
    });

    broadcaster.sendActionMessage ("Hello");  // Lambda will be called
    @endcode

    @see juce::ActionBroadcaster, juce::ActionListener
*/
class ActionMessageLambda : private juce::ActionListener
{
public:
    /** Creates an ActionMessageLambda that listens to the given broadcaster.

        @param src_      The ActionBroadcaster to listen to. Must remain valid
                         for the lifetime of this object.
        @param handler_  A function to call when an action message is received.
                         The function receives the message string as its parameter.
    */
    ActionMessageLambda (juce::ActionBroadcaster& src_, std::function<void(juce::String)> handler_)
        : src (src_), handler (handler_)
    {
        src.addActionListener (this);
    }

    ~ActionMessageLambda() override
    {
        src.removeActionListener (this);
    }

    void actionListenerCallback (const juce::String& message) override
    {
        handler (message);
    }

    juce::ActionBroadcaster&            src;
    std::function<void(juce::String)>   handler;
};
