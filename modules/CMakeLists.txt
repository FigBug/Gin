cmake_minimum_required(VERSION 3.15)

juce_add_module(gin)
juce_add_module(gin_3d)
juce_add_module(gin_controllers)
juce_add_module(gin_dsp)
juce_add_module(gin_graphics)
juce_add_module(gin_gui)
juce_add_module(gin_location)
juce_add_module(gin_metadata)
juce_add_module(gin_network)
juce_add_module(gin_plugin)
juce_add_module(gin_svg)
juce_add_module(gin_webp)

# gin_webp bundles its own copy of libwebp, which collides with the copy JUCE ships
# when JUCE_USE_WEBP is enabled (the default in newer JUCE). Turn JUCE's off for anything that links gin_webp.
target_compile_definitions(gin_webp INTERFACE JUCE_USE_WEBP=0)

juce_add_module(gin_simd)
