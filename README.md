# Gin

![Build Windows](https://github.com/FigBug/Gin/workflows/Build%20Windows/badge.svg "Build Windows")
![Build macOS](https://github.com/FigBug/Gin/workflows/Build%20macOS/badge.svg "Build macOS")
![Build Linux](https://github.com/FigBug/Gin/workflows/Build%20Linux/badge.svg "Build Linux")
[![codecov](https://codecov.io/gh/FigBug/Gin/branch/master/graph/badge.svg)](https://codecov.io/gh/FigBug/Gin)

A comprehensive collection of extra modules for [JUCE](https://juce.com/), providing professional-grade utilities, UI components, DSP processors, and more for audio plugin and application development.

## Table of Contents

- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Modules](#modules)
- [Features](#features)
- [Examples](#examples)
- [Help & Documentation](#help--documentation)
- [Credits](#credits)

## Prerequisites

- **JUCE 7.0+** (included as submodule)
- **CMake 3.15+**
- **C++20** compatible compiler
- **Supported Platforms:** Windows, macOS (10.13+), Linux

## Installation

### Adding Gin to Your CMake Project

1. Add Gin as a submodule:
```bash
git submodule add https://github.com/FigBug/Gin.git modules/gin
```

2. In your `CMakeLists.txt`:
```cmake
add_subdirectory(modules/gin/modules)

target_link_libraries(YourTarget PRIVATE
    gin
    gin_plugin      # For audio plugins
    gin_dsp         # For DSP components
    gin_gui         # For UI components
    # ... other gin modules as needed
)
```

3. Gin is automatically in the JUCE Header:
```cpp
#include <JuceHeader.h>
```

## Modules

Gin is organized into focused modules that can be included independently:

| Module | Description |
|--------|-------------|
| **gin** | Core utilities, file system watching, download manager, math utilities |
| **gin_3d** | Basic wireframe 3D rendering engine |
| **gin_dsp** | Audio DSP components: oscillators, filters, effects, modulation |
| **gin_graphics** | Image processing, effects, metadata, BMP format support |
| **gin_gui** | UI components, property editors, layout system, OpenStreetMap integration |
| **gin_location** | GPS location services (macOS CoreLocation) |
| **gin_metadata** | Image metadata reading from JPEG, GIF, and PNG files |
| **gin_network** | WebSocket, secure sockets |
| **gin_plugin** | Audio plugin utilities and common functionality |
| **gin_simd** | SIMD-optimized operations |
| **gin_standaloneplugin** | Standalone audio plugin application wrapper |
| **gin_webp** | WebP image format support |

## Features

### Core Utilities (gin)
- **DownloadManager** - Asynchronous downloads with zero main thread delay
- **FileSystemWatcher** - Real-time notifications when files change
- **SharedMemory** - Inter-process memory sharing
- **Ellipse** - Geometric calculations for ellipses
- **LeastSquaresRegression** - Curve fitting to data points
- **LinearRegression** - Line fitting to data points
- **Spline** - Smooth cubic spline interpolation
- **Integrator** - Numerical integration

### Graphics & Images (gin_graphics)
- **ImageEffects** - Sepia, vignette, soften, sharpen, invert, contrast, hue, saturation, stack blur
- **ImageMetadata** - Read/write JPEG and PNG metadata
- **BMPImageFormat** - Windows Bitmap support

### DSP & Audio (gin_dsp)
- **Oscillators** - Wavetable oscillator with multiple waveforms
- **Effects** - Compressor, reverb, filters
- **Envelopes** - ADSR envelope generator
- **PerlinNoise** - Natural-sounding noise generation
- **Modulation** - LFO and modulation matrix

### GUI Components (gin_gui)
- **Property Components** - File chooser, color picker
- **Layout System** - JSON-based flexible layouts
- **Synth UI** - Ready-to-use synthesizer interface components
- **Map Components** - OpenStreetMap integration and tile rendering

### Network (gin_network)
- **WebSocket** - Full-featured WebSocket client
- **SecureStreamingSocket** - SSL/TLS socket support

### Plugin Utilities (gin_plugin)
- **Processor** - Extended audio processor with parameter management
- **Parameter** - Smart parameters with modulation support
- **ModMatrix** - Flexible modulation matrix system
- **Plugin Components** - Knobs, ADSR, LFO, and MSEG UI components
- **Patch Browser** - Preset management and browsing

### Image Metadata (gin_metadata)
- **EXIF** - Read EXIF metadata from images
- **IPTC** - Read IPTC metadata from images
- **XMP** - Read XMP metadata from images

### SIMD Helpers (gin_simd)
- **MIPP Integration** - Cross-platform SIMD abstraction (SSE/NEON)
- **Optimized Math** - SIMD-accelerated math operations

### WebP Support (gin_webp)
- **WebPImageFormat** - Read and write WebP images

### 3D Wireframe Engine (gin_3d)
- **Vec3/Mat4** - 3D vector and matrix math
- **Camera3D** - 3D camera with projection
- **Scene3D** - Scene graph for 3D objects
- **Renderer3D** - Wireframe rendering to 2D

### Standalone Plugin (gin_standaloneplugin)
- **StandaloneApp** - Application wrapper for running plugins standalone
- **StandaloneFilterWindow** - Window hosting the plugin UI
- **SidebarComponent** - Audio analysis display

## Examples

The repository includes several complete example projects:

- **Demo** - Comprehensive demonstration of all Gin features
- **Synth** - Complete synthesizer plugin example
- **Effect** - Audio effect plugin example
- **UnitTests** - Test suite for Gin components

### Building the Examples

```bash
cd Gin
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

The built applications will be in `build/examples/`.

### Quick Start Example

```cpp
#include <gin/gin.h>

// Use the download manager
gin::DownloadManager dm;
dm.downloadAsync(juce::URL("https://example.com/file.zip"),
                 [](juce::InputStream* stream) {
    // Process downloaded data
});

// Watch a directory for changes
gin::FileSystemWatcher watcher;
watcher.addFolder(juce::File::getSpecialLocation(
    juce::File::userDocumentsDirectory));
watcher.addListener(this);

// Use a spline for smooth interpolation
gin::Spline spline;
spline.addPoint({0.0f, 0.0f});
spline.addPoint({1.0f, 1.0f});
float interpolated = spline.get(0.5f);
```

## Help & Documentation

- **[Discord Community](https://discord.gg/zvpyqsBjc3)** - Get help and discuss Gin
- **[Developer Blog](https://rabiensoftware.com/index.php/category/gin/)** - Tutorials and articles
- **[Doxygen API Documentation](https://figbug.github.io/Gin/annotated.html)** - Complete API reference

## Credits

Gin incorporates and builds upon several excellent open-source libraries and algorithms:

- **[JUCE](https://juce.com/)** by Roli - The foundational framework
- **[Stack Blur](http://incubator.quasimondo.com/processing/fast_blur_deluxe.php)** by Mario Klingemann - Fast blur algorithm
- **[muParser](http://beltoforion.de/article.php?a=muparser)** by Ingo Berg - Mathematical expression parser
- **[Spline](https://shiftedbits.org/2011/01/30/cubic-spline-interpolation/)** by Devin Lane - Cubic spline interpolation
- **[AVIR](https://github.com/avaneev/avir)** by Aleksey Vaneev - Image resizing
- **[easywsclient](https://github.com/dhbaird/easywsclient)** - WebSocket client implementation
- **[mbed TLS](https://tls.mbed.org/)** - SSL/TLS library
- **[Least Squares Regression](https://www.codeproject.com/Articles/63170/Least-Squares-Regression-for-Quadratic-Curve-Fitti)** - Curve fitting algorithms

### Special Thanks

**reFX Audio Software Inc.** for supporting the development of Gin.
