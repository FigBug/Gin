/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

#pragma once

//==============================================================================
/** Wrapper around juce::OwnedArray<BandLimitedLookupTable> for managing multiple wavetables
*/
class Wavetable
{
public:
    Wavetable() = default;

    /** Add a new BandLimitedLookupTable to the collection */
    void addTable (BandLimitedLookupTable* table)
    {
        tables.add (table);
    }

    /** Get the number of tables */
    int getNumTables() const
    {
        return tables.size();
    }

    /** Get a table by index */
    BandLimitedLookupTable* getTable (int index)
    {
        return tables[index];
    }

    /** Get a table by index (const version) */
    const BandLimitedLookupTable* getTable (int index) const
    {
        return tables[index];
    }

    /** Array subscript operator for compatibility with OwnedArray usage */
    BandLimitedLookupTable* operator[] (int index)
    {
        return tables[index];
    }

    /** Array subscript operator for compatibility with OwnedArray usage (const version) */
    const BandLimitedLookupTable* operator[] (int index) const
    {
        return tables[index];
    }

    /** Get unchecked access to a table (for compatibility with OwnedArray) */
    BandLimitedLookupTable* getUnchecked (int index)
    {
        return tables.getUnchecked (index);
    }

    /** Get unchecked access to a table (for compatibility with OwnedArray, const version) */
    const BandLimitedLookupTable* getUnchecked (int index) const
    {
        return tables.getUnchecked (index);
    }

    /** Get the number of tables (for compatibility with OwnedArray::size()) */
    int size() const
    {
        return tables.size();
    }

    /** Clear all tables */
    void clear()
    {
        tables.clear();
    }

    /** Process using a specific table index */
    inline float process (int tableIndex, float note, float phase)
    {
        if (tableIndex >= 0 && tableIndex < tables.size())
            return tables[tableIndex]->process (note, phase);
        return 0.0f;
    }

    /** Process with linear interpolation using a specific table index */
    inline float processLinear (int tableIndex, float note, float phase)
    {
        if (tableIndex >= 0 && tableIndex < tables.size())
            return tables[tableIndex]->processLinear (note, phase);
        return 0.0f;
    }

   #if GIN_HAS_SIMD
    /** Process using SIMD with a specific table index */
    inline mipp::Reg<float> process (int tableIndex, float note, mipp::Reg<float> phase)
    {
        if (tableIndex >= 0 && tableIndex < tables.size())
            return tables[tableIndex]->process (note, phase);
        return mipp::Reg<float> (0.0f);
    }

    /** Process with linear interpolation using SIMD with a specific table index */
    inline mipp::Reg<float> processLinear (int tableIndex, float note, mipp::Reg<float> phase)
    {
        if (tableIndex >= 0 && tableIndex < tables.size())
            return tables[tableIndex]->processLinear (note, phase);
        return mipp::Reg<float> (0.0f);
    }
   #endif

private:
    juce::OwnedArray<BandLimitedLookupTable> tables;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Wavetable)
};
