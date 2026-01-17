/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
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

    //==============================================================================
    /** A compact text patch storing only the differences between two texts.

        This stores only inserted and removed lines with their line numbers,
        making it memory efficient while remaining human readable.
    */
    struct Patch
    {
        struct Hunk
        {
            int oldLine;         /**< Line number in old text (for removes) or -1 */
            int newLine;         /**< Line number in new text (for inserts) or -1 */
            bool isInsert;       /**< true = insert, false = remove */
            juce::String text;   /**< The line content */
        };

        std::vector<Hunk> hunks;

        /** Serializes the patch to a human-readable string. */
        juce::String toString() const;

        /** Parses a patch from its string representation. */
        static Patch fromString (const juce::String& s);
    };

    /** Creates a compact patch that can transform newText back into oldText.

        Uses the Myers diff algorithm to find the minimal differences.
        Only stores the changes (inserts/removes), not unchanged lines.

        @param oldText  The original text
        @param newText  The modified text
        @returns        A compact Patch containing only the differences
    */
    Patch createPatch (const juce::String& oldText, const juce::String& newText);

    /** Applies a patch to transform text back to its original form.

        @param newText  The modified text
        @param patch    The patch created by createPatch()
        @returns        The original text
    */
    juce::String applyPatch (const juce::String& newText, const Patch& patch);
}
