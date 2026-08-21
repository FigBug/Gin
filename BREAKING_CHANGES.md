# Breaking Changes

Behavioral and API changes that may affect plugins built on Gin. Most recent first.

## August 2026

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
