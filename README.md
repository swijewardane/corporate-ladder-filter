# Corporate Ladder Filter

*A digital emulation of the classic analog Moog ladder filter, built as a VST3/Standalone plugin. Named after the 'corporate ladder' and styled after a 70s retro-futurist aesthetic. Think Lumon Industries from Severance — clean, cold, corporate.*

[![Build](https://github.com/swijewardane/corporate-ladder-filter/actions/workflows/build.yml/badge.svg)](https://github.com/swijewardane/corporate-ladder-filter/actions)


![Corporate Ladder Filter GUI](Figures/plugin_in_DAW.png)

## Overview

Corporate Ladder Filter is a 4-pole Moog-style ladder filter plugin with cutoff, resonance, drive, and gain controls, built in JUCE/C++. It targets VST3 and Standalone formats on Windows.

The distinguishing feature is **antiderivative antialiasing (ADAA)** applied to the tanh saturation stages on both the drive and feedback paths — this is what keeps the resonant, saturating filter free of aliasing artifacts at high resonance and drive settings, where a naive implementation would introduce audible digital distortion.

## Features

- Cutoff (20 Hz – 20 kHz, logarithmic) and resonance (0–~4, self-oscillating at the top of the range) knobs
- Drive/gain stage with tanh saturation
- ADAA toggle — compare aliased vs. antialiased saturation directly
- Plugin bypass
- Custom Lumon/*Severance*-inspired GUI with filmstrip-animated controls

## Signal Processing

The filter is a discrete-time 4-pole ladder structure with nonlinear feedback. Two places where naive digital implementation would alias:

1. **Drive stage** — tanh saturation applied directly to the signal
2. **Feedback path** — tanh bounds the feedback signal to (-1, 1), preventing runaway gain as resonance `k` approaches self-oscillation

Naively applying `tanh(x)` at each sample introduces aliasing because it's a nonlinear operation on a discrete-time signal. ADAA fixes this by computing the finite difference of the antiderivative of tanh — `F(x) = log(cosh(x))` — instead of the function directly:

```
y[n] = (F(x[n]) - F(x[n-1])) / (x[n] - x[n-1])
```

This produces the same saturation curve with substantially reduced aliasing, verified against harmonic distortion measurements taken during development (see `MATLAB/Figures/`).

<figure>
  <img src = "Figures/adaa_comparison.png">
  <p align = "center"><em> Application of ADAA is shown to have measurable reduction in harmonics above the fundamental frequency </em> </p>
</figure>



## Development Process

This plugin was prototyped in MATLAB before being translated to a production JUCE/C++ plugin:

1. **Algorithm design & validation (MATLAB)** — the ladder filter, ADAA saturation, and GUI parameter mapping were built as a MATLAB `audioPlugin` class (`MATLAB/Functions_Subclasses/MoogLadderPlugin.m`), validated using exponential sine sweep measurements and Volterra/Hammerstein nonlinear system characterization to confirm the filter's nonlinear behavior matched expectations.
2. **Export** — the validated plugin was exported to a JUCE project via MATLAB's `generateAudioPlugin -juceproject`, which uses JUCE internally to generate a working C++ project from the MATLAB class.
3. **Refinement (JUCE/C++)** — the generated project was hand-refined: migrated from Projucer to a CMake build (via `FetchContent`), fixed a VST2/VST3 parameter automation conflict, corrected parameter default states, and updated GUI assets with the final Lumon-styled filmstrips and background.

See `MATLAB/` for the full prototype, including validation scripts and measurement figures.

## Platform Support

**Currently confirmed on Windows only** — built and CI-tested with MSVC via GitHub Actions (`windows-latest`). The underlying JUCE framework is cross-platform, but macOS/Linux builds have not been configured or tested for this project.

## Building from Source

### Prerequisites
- Visual Studio 2022 (Desktop development with C++ workload)
- CMake ≥ 3.22
- Git

### Steps
```powershell
git clone https://github.com/swijewardane/corporate-ladder-filter.git
cd corporate-ladder-filter
cmake -B out/build/x64-Release
cmake --build out/build/x64-Release --config Release
```

The build automatically copies the VST3 to `%APPDATA%\VST3\` — no manual install step needed. Rescan plugins in your DAW, or launch the Standalone build directly from `out/build/x64-Release/MoogLadderPlugin_artefacts/Release/Standalone/`.

*A packaged Release download for non-technical users is planned but not yet available — building from source is currently the only distribution method.*

## Project Structure

```
CorporateLadderFilter/
├── Source/          # JUCE/C++ plugin source
├── MATLAB/          # Original MATLAB prototype, validation scripts, figures
├── docs/            # Screenshots and supporting documentation
├── CMakeLists.txt
└── LICENSE
```

## Tech Stack

C++ · JUCE · CMake · MATLAB (Audio Toolbox) · GitHub Actions

## License

This project's source code is licensed under AGPLv3. It uses the [JUCE](https://github.com/juce-framework/JUCE) framework under its [AGPLv3 license terms](https://github.com/juce-framework/JUCE/blob/master/LICENSE.md).

## Acknowledgments

- [JUCE](https://juce.com/) — C++ audio application framework
- MathWorks Audio Toolbox — MATLAB plugin prototyping and `generateAudioPlugin` export
