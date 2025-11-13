/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/** Binary diff/patch utilities using the bsdiff algorithm.

    These functions provide efficient binary differencing and patching for strings.
    The bsdiff algorithm creates compact patches by finding matching sequences
    between the old and new data.

    @see bsDiff, bsApplyPatch
*/
namespace Diff
{
    /** Creates a binary patch that transforms s1 into s2.

        Uses the bsdiff algorithm to generate a compact binary patch representing
        the differences between two strings. The resulting patch can be applied
        to s1 using bsApplyPatch() to recreate s2.

        @param s1  The original string
        @param s2  The target string
        @returns   A binary patch as a vector of bytes
    */
    std::vector<uint8_t> bsDiff (const juce::String& s1, const juce::String& s2);

    /** Applies a binary patch to a string.

        Applies a patch created by bsDiff() to transform the original string
        into the target string.

        @param s      The original string to patch
        @param patch  The binary patch data created by bsDiff()
        @returns      The resulting string after applying the patch
    */
    juce::String bsApplyPatch (const juce::String& s, const std::vector<uint8_t>& patch);
}
