#pragma once
#include <vector>
#include <algorithm>
#include <assert.h>

/*
*   In this file the basic structs for filtering are defined. While parameters are always calculated 
*   in double precision the actual filter parameters and processing can also be done in
*   single precision (float datatype).
*/

namespace AudioFilter
{

/*
*   Basic biquad parameters include additional FIR parameters for MZTi compensation.
*   This is used for analog (s-domain) as well as digital (z-domain) biquad stages.
*   The compensation parameters is only used in the digital domain.
*/
struct BiquadParam
{
    double b0 = 1., b1 = 0., b2 = 0.; // Biquad input params
    double a1 = 0., a2 = 0.;          // Biquad output params
    double c0 = 1., c1 = 0., c2 = 0.; // FIR params for MZTi compensation.
    bool useCompensation = false;
};

//==============================================================================
/*
*   An array class with preallocated memory. This is used to avoid memory allocation
*   when changing filters.
*/
template <class Class>
class PreallocatedArray
{
public:

    /*
    *   ctor. Takes the maximum size as argument.
    */
    PreallocatedArray(int maxSize)
        : elements(static_cast<size_t> (maxSize))
    {
    }

    /*
    *   Access to array element. The index parameter must be lower than the used elements.
    */
    Class& operator[] (size_t index)
    {
        assert(index < used);
        return elements[index];
    }

    /*
    *   Const access to array element. The index parameter must be lower than the used elements.
    */
    const Class& operator[] (size_t index) const
    {
        assert(index < used);
        return elements[index];
    }

    /*
    *   Method used for ranged-based for loops.
    */
    const Class* begin() const
    {
        return elements.data();
    }

    /*
    *   Method used for ranged-based for loops.
    */
    const Class* end() const
    {
        return elements.data() + used;
    }

    /*
    *   Set array size. This doesn't allocate memory but resets the size of used elements.
    *   The newSize parameter must be lower than the maxSize value given in the ctor.
    */
    void resize(size_t newSize)
    {
        // Can't resize to more elements than it was initialized with.
        assert(newSize <= elements.size());
        used = std::min(newSize, elements.size());
    }

    /*
    *   Adds parameter element to the end of the array.
    */
    void push_back(const Class& element)
    {
        // If no more free elements are available within the array nothing happens.
        assert(used < elements.size());

        if (used < elements.size()/* - 1*/)
            elements[used++] = element;
    }

    /*
    *   Returns the number of accessible elements. This can be lower than the maximum array size.
    */
    size_t size() const
    {
        return used;
    }

    /*
    *   Returns the actual maximum array size.
    */
    size_t maxSize() const
    {
        return elements.size();
    }

private:

    size_t used = 0;
    std::vector<Class> elements;
};

//==============================================================================
/*
*   This class represents the state of a (SVF) biquad stage including the state of an
*   additional FIR filter for MZTi compensation.
*/
template <class DataType>
struct FilterState
{
    void clear()
    {
        z1 = static_cast<DataType> (0);
        z2 = static_cast<DataType> (0);
        x1 = static_cast<DataType> (0);
        x2 = static_cast<DataType> (0);
    }

    DataType z1 = static_cast<DataType> (0), z2 = static_cast<DataType> (0);
    DataType x1 = static_cast<DataType> (0), x2 = static_cast<DataType> (0);
};

//==============================================================================
/*
*   This class holds the (SVF) filter parameters for a biquad stage as well as the FIR
*   parameters of an optional MZTi compensation stage.
*/
template <class DataType>
struct FilterParams
{
    void setFromBiquadParams(const BiquadParam& params)
    {
        f = static_cast<DataType> (std::sqrt((1 + params.a1 + params.a2) / (1 - params.a1 + params.a2)));
        r = static_cast<DataType> (-2 * (params.a2 - 1) / sqrt(params.a2*params.a2 + 2 * params.a2 - params.a1 * params.a1 + 1));
        const double a0 = (f*(f + r) + 1);
        g = static_cast<DataType> (1 / a0);

        double r2f = 0.5 / f;   // 1 / (2*f) from above

        h = static_cast<DataType> ((params.b0 - params.b1 + params.b2) * 0.25 * a0);
        b = static_cast<DataType> ((params.b0 - params.b2) * r2f * a0);
        l = static_cast<DataType> ((params.b0 + params.b1 + params.b2) * (r2f * r2f) * a0);

        c0 = static_cast<DataType> (params.c0);
        c1 = static_cast<DataType> (params.c1);
        c2 = static_cast<DataType> (params.c2);
        useCompensation = params.useCompensation;
    }

    DataType f = static_cast<DataType> (1), r = static_cast<DataType> (2), g = static_cast<DataType> (0.25f);
    DataType h = static_cast<DataType> (1), b = static_cast<DataType> (2), l = static_cast<DataType> (1);

    DataType c0 = static_cast<DataType> (1), c1 = static_cast<DataType> (0), c2 = static_cast<DataType> (0);
    bool useCompensation = false; // If true the FIR compensation should be calculated.
};

// Cascade of BiquadParam class using a PreallocatedArray.
using BiquadParamCascade = PreallocatedArray<BiquadParam>;

// Cascade of FilterParams class using a PreallocatedArray.
template <class DataType>
using FilterParamsCascade = PreallocatedArray<FilterParams<DataType> >;

// Cascade of FilterState class using a PreallocatedArray.
template <class DataType>
using FilterStateCascade = PreallocatedArray<FilterState<DataType> >;


} // namespace AudioFilter