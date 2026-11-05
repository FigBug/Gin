# Breaking Changes

Behavioral and API changes that may affect plugins built on Gin. Most recent first.

## November 2026

### `WTOscillator::noteOn()` starts at a random phase

Commit `4b0858e01f` fixed a conditional in `WTOscillator::noteOn` that assigned to `phase`
on one side and threw the random value away on the other:

```cpp
p >= 0 ? phase = p : juce::Random::getSystemRandom().nextFloat();   // the random value went nowhere
```

Calling `noteOn()` with no phase therefore left the phase wherever it was — zero on a
fresh oscillator — so every voice started aligned. It now randomises the phase.

`StereoOscillator::noteOn` had the same typo and was fixed the same way in May 2026
(commit `0825695566`), which changed that oscillator's note-on in exactly this way;
`WTOscillator` was missed at the time.

This changes the sound of anything wavetable-based that lets the phase default:
`WTOscillator`, `WTVoicedStereoOscillator` (whose `noteOn()` forwards the default to
every voice) and so anything built on `gin::loadWavetables`. Unison voices no longer
start in phase with each other, so the note-on transient loses the coherent click and
comb that phase alignment gave it, and the output is no longer identical from run to
run. Pass an explicit phase — `noteOn (0.0f)`, or the `noteOn (float phases[])` overload
on the voiced oscillators — to get the old behaviour, and for a deterministic render in
a test.

### Image effects respect premultiplied alpha

A `juce::Image`'s ARGB components are premultiplied, so a component may never exceed its
pixel's alpha. Several effects read, changed and wrote the components as if they were
straight, which left colour in pixels whose alpha was zero for the compositor to add —
the halo of issue #24. Commits `ee75300b28` and `031a0dbd73` fixed:

- `applyInvert`, `applyBrightnessContrast` and `applyGradientMap` now bring each pixel
  out of its alpha before touching the colour and put it back after. This also puts the
  intensity they pick on the colour rather than on the colour times its alpha.
- `applyColour` premultiplies its fill colour. A translucent colour previously went in
  straight and so read back far brighter than asked for; it now fills at the opacity
  given.
- `applySoften` weights its neighbours by their alpha instead of dividing by their
  number. An invisible neighbour used to count as black and darken the edge of a solid
  region, and a transparent pixel beside one came out carrying colour.

Fully opaque pixels are unchanged in all of these — JUCE's premultiply and unpremultiply
both return early at an alpha of 255, and `applySoften` still reduces to the same plain
3x3 average — so RGB images and images with no transparency render exactly as before.
What changes is semi-transparent and fully transparent pixels: effects no longer leave a
visible box around artwork with transparent margins, and antialiased edges are composited
at the colour they should be. Code that worked around the old leak by re-masking the
image afterwards can drop the workaround; a translucent `applyColour` fill will need its
alpha revisited, since it was previously rendering heavier than requested.

### SIMD helpers follow the register width

`gin::math::minVal` and `gin::math::maxVal` reduced a register through a `float[4]` and
returned the smallest or largest of those four. On a build whose registers hold more —
any AVX build, which is now the default baseline on current Linux toolchains — this both
overran the array and ignored the remaining lanes. They use mipp's own reductions now
(commit `4dd8c57757`), so they cover every lane. Builds at four lanes wide (SSE, NEON)
are unaffected.

The same commit, with `67a4cd9259`, replaced the four-lane assumptions in
`BandLimitedLookupTable` and `WTOscillator` with the register's actual lane count. On an
AVX build these previously failed to compile (`static_assert (mipp::N<float>() == 4)`,
issue #56) or aborted at runtime on mipp's `initializer_list` size check. One audible
change comes with it: `WTOscillator`'s crossfade between wavetables mixes on the phase of
each sample rather than on the phase the register began at, so a table change crossfades
smoothly instead of in steps of four samples.

## August 2026

### `gin::Parameter` text functions replaced by bidirectional conversion functions

The `gin::Parameter` and `gin::SmoothedParameter` constructors no longer take a
`textFunction` (`std::function<juce::String (const Parameter&, float)>`). They instead
take a `Parameter::ConversionFunction`:

```cpp
std::function<std::variant<float, juce::String> (const Parameter&, const std::variant<float, juce::String>&)>
```

The conversion function works in both directions: called with a float it returns the
display text for that user value, called with a string it returns the user value for
that text. `Parameter::getValueForText` now uses it, so text typed into host automation
lanes and UI text editors (`Readout`) maps back to the correct value for parameters
with custom display scaling — previously the typed text was parsed as a plain float,
ignoring any custom mapping.

The `Processor` API is unchanged: the `addExtParam` / `addIntParam` / `createParam`
overloads taking a `textFunction` still exist and wrap it in a conversion function
that parses text with a plain float parse (the old behavior). New overloads take a
`Parameter::ConversionFunction` directly. Only code constructing `Parameter` /
`SmoothedParameter` directly with a text function needs updating.

### Wavetable oscillators are ~6 dB louder (`BandLimitedLookupTable::loadFromBuffer`)

Commit `b3393cb471` fixed a bug in the FFT-based `loadFromBuffer` where band-limited
mip levels were generated at half amplitude: only the positive-frequency bins were
zeroed when removing harmonics, so the spectrum lost its conjugate symmetry and taking
the real part of the inverse transform halved every retained harmonic. Since the mip
levels for virtually the whole playable range go through this path, wavetable
oscillators (e.g. anything using `gin::loadWavetables` / `WTOscillator`) now render at
the true level of the source wavetable — approximately 6 dB (2×) louder than before.

The new level is correct: it matches the source waveform and the unfiltered lowest mip
levels, and the 6 dB step between unfiltered and filtered mips is gone. However,
existing user projects were mixed against the old level. If you want to preserve the
previous loudness when updating Gin, apply a one-time 0.5× gain to your wavetable
oscillator output.

Related subtlety: DC was never halved by the old code, so wavetables with a DC offset
previously had 2× DC relative to their harmonic content. That inconsistency is also
gone.

### Square and pulse waves are band-limited correctly (`BandLimitedLookupTables`)

PR #62 fixed `processSquare` and `processPulse` selecting their mip-map table with the
wrong divisor (table count instead of `notesPerTable`). Previously, any note above the
bottom octave used a table containing far too many harmonics, producing heavy aliasing.
Square and pulse waveforms now sound cleaner (and duller, since the aliased energy is
gone) — patches relying on the old, brighter aliased sound will change character.

PR #62 also fixed the wavetable phase mapping in `reset()` (tables were generated over
`tableSize - 1` samples but read over `tableSize`), removing a small phase drift and a
flat interpolation segment at the wrap point of every generated waveform.

### `BandLimitedLookupTables::reset()` signature (added and then changed)

`reset (double sampleRate, int notesPerTable = -1, int tableSize = -1)` was added to
allow cheap construction with deferred table building (commit `97a30bdf06`, refining
PR #63). If you adopted the initial PR #63 version, note the defaults changed: passing
only a sample rate now *keeps* the current `notesPerTable` / `tableSize` instead of
resetting them to 3 / 2048. Pass all three arguments to change everything.
