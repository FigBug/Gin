#pragma once

//==============================================================================
/**
    Identifier for a modulation source in the ModMatrix system.

    Wraps an integer ID with type safety to distinguish modulation sources
    from other identifiers in the system. Use isValid() to check if the ID
    represents a valid modulation source.

    @see IModMatrix, ModDstId
*/
struct ModSrcId
{
    ModSrcId () = default;
    explicit ModSrcId (int id_) : id (id_) {}
    ModSrcId (const ModSrcId& other) { id = other.id; }
    ModSrcId& operator= (const ModSrcId& other) { id = other.id; return *this; }
    bool operator== (const ModSrcId& other) const { return other.id == id; }
    bool isValid() const { return id >= 0; }

    int id = -1;
};

//==============================================================================
/**
    Identifier for a modulation destination in the ModMatrix system.

    Wraps an integer ID with type safety to distinguish modulation destinations
    (parameters that can be modulated) from other identifiers. Use isValid() to
    check if the ID represents a valid modulation destination.

    @see IModMatrix, ModSrcId
*/
struct ModDstId
{
    ModDstId () = default;
    explicit ModDstId (int id_) : id (id_)  {}
    ModDstId (const ModDstId& other) { id = other.id; }
    ModDstId& operator= (const ModDstId& other) { id = other.id; return *this; }
    bool operator== (const ModDstId& other) const { return other.id == id; }
    bool isValid() const { return id >= 0; }

    int id = -1;
};

//==============================================================================
/**
    Modulation shaping functions for non-linear modulation curves.

    ModFunction provides various easing curves to shape modulation between
    source and destination. These allow creative control over how modulation
    values are mapped, enabling smooth fades, exponential responses, and
    inverted behaviors.

    Curve Types:
    - linear: Direct 1:1 mapping (no curve)
    - quadratic: Smooth acceleration (parabolic)
    - sine: Smooth S-curve using sine function
    - exponential: Rapid acceleration (exponential)
    - inv*: Inverted versions (1 - f(x))

    Curve Timing:
    - *In: Ease in (slow start, fast end)
    - *Out: Ease out (fast start, slow end)
    - *InOut: Ease in and out (smooth both ends)

    These functions are applied after the modulation source value is
    retrieved and before it's scaled by depth and applied to the destination.

    @see IModMatrix
*/
enum class ModFunction
{
    linear,            ///< Linear (no shaping)

    quadraticIn,       ///< Quadratic ease in
    quadraticInOut,    ///< Quadratic ease in and out
    quadraticOut,      ///< Quadratic ease out

    sineIn,            ///< Sine ease in
    sineInOut,         ///< Sine ease in and out
    sineOut,           ///< Sine ease out

    exponentialIn,     ///< Exponential ease in
    exponentialInOut,  ///< Exponential ease in and out
    exponentialOut,    ///< Exponential ease out

    invLinear,         ///< Inverted linear

    invQuadraticIn,    ///< Inverted quadratic ease in
    invQuadraticInOut, ///< Inverted quadratic ease in and out
    invQuadraticOut,   ///< Inverted quadratic ease out

    invSineIn,         ///< Inverted sine ease in
    invSineInOut,      ///< Inverted sine ease in and out
    invSineOut,        ///< Inverted sine ease out

    invExponentialIn,  ///< Inverted exponential ease in
    invExponentialInOut, ///< Inverted exponential ease in and out
    invExponentialOut,   ///< Inverted exponential ease out
};

//==============================================================================
/**
    Interface for modulation matrix systems.

    IModMatrix provides the abstract interface for modulation routing systems
    commonly used in synthesizers and audio effects. It allows multiple modulation
    sources (LFOs, envelopes, velocity, etc.) to control multiple destination
    parameters with configurable depth, enable/disable, and curve shaping.

    Implementations:
    - gin::ModMatrix: Full-featured implementation with dynamic routing
    - tonic::ModMatrix: Fixed-slot implementation with parameter-based control

    Key Features:
    - Listener pattern for UI updates
    - Learning mode for quick modulation assignment
    - Query methods for modulation routing
    - Modulation depth, function, and enable control

    @see ModMatrix, ModSrcId, ModDstId, ModFunction
*/
class IModMatrix
{
public:
    virtual ~IModMatrix() = default;

    //==============================================================================
    /** Listener interface for modulation matrix changes */
    class Listener
    {
    public:
        virtual ~Listener() = default;
        virtual void modMatrixChanged()             {}
        virtual void learnSourceChanged (ModSrcId)  {}
    };

    virtual void addListener (Listener* l) = 0;
    virtual void removeListener (Listener* l) = 0;

    //==============================================================================
    // Source information
    virtual int getNumModSources() = 0;
    virtual juce::String getModSrcName (ModSrcId src) = 0;
    virtual bool getModSrcPoly (ModSrcId src) = 0;
    virtual bool getModSrcBipolar (ModSrcId src) = 0;

    //==============================================================================
    // Destination information
    virtual juce::String getModDstName (ModDstId dst) = 0;
    virtual Parameter* getParameter (ModDstId d) = 0;

    //==============================================================================
    // Learn mode
    virtual void enableLearn (ModSrcId source) = 0;
    virtual void disableLearn() = 0;
    virtual ModSrcId getLearn() = 0;

    //==============================================================================
    // Modulation queries
    virtual bool isModulated (ModDstId param) = 0;
    virtual juce::Array<ModSrcId> getModSources (gin::Parameter* p) = 0;

    //==============================================================================
    // Modulation depth
    virtual float getModDepth (ModSrcId src, ModDstId param) = 0;
    virtual std::vector<std::pair<ModDstId, float>> getModDepths (ModSrcId src) = 0;
    virtual std::vector<std::pair<ModSrcId, float>> getModDepths (ModDstId param) = 0;
    virtual void setModDepth (ModSrcId src, ModDstId param, float f) = 0;
    virtual void clearModDepth (ModSrcId src, ModDstId param) = 0;

    //==============================================================================
    // Modulation function/curve
    virtual ModFunction getModFunction (ModSrcId src, ModDstId param) = 0;
    virtual void setModFunction (ModSrcId src, ModDstId param, ModFunction f) = 0;

    //==============================================================================
    // Modulation enable
    virtual bool getModEnable (ModSrcId src, ModDstId param) = 0;
    virtual void setModEnable (ModSrcId src, ModDstId param, bool b) = 0;

    //==============================================================================
    // Bipolar mapping
    virtual bool getModBipolarMapping (ModSrcId src, ModDstId param) = 0;
    virtual void setModBipolarMapping (ModSrcId src, ModDstId param, bool b) = 0;

    //==============================================================================
    // Live values display
    virtual bool shouldShowLiveModValues() = 0;
    virtual juce::Array<float> getLiveValues (gin::Parameter* p) = 0;
};
