set_property (DIRECTORY APPEND PROPERTY LABELS UnitTests)

add_subdirectory (Resources)

juce_add_plugin (UnitTests
				  	  PRODUCT_NAME UnitTests
				  	  VERSION 1.0.0
				  	  COMPANY_NAME "Rabien Software"
				  	  COMPANY_WEBSITE "https://rabien.com/"
				  	  BUNDLE_ID com.rabien.gin.unittests
				  	  PLUGIN_MANUFACTURER_CODE Rabi
				  	  PLUGIN_CODE GnUT
				  	  FORMATS Standalone
				  	  NEEDS_MIDI_INPUT FALSE
				  	  NEEDS_MIDI_OUTPUT FALSE
				  	  IS_SYNTH FALSE
				  	  EDITOR_WANTS_KEYBOARD_FOCUS FALSE
				  	  COPY_PLUGIN_AFTER_BUILD FALSE)

if (APPLE)
	target_compile_options(UnitTests PRIVATE
			-Wall -Wstrict-aliasing -Wunused-parameter -Wconditional-uninitialized -Woverloaded-virtual -Wreorder -Wconstant-conversion -Wbool-conversion -Wextra-semi
			-Wunreachable-code -Winconsistent-missing-destructor-override -Wshift-sign-overflow -Wnullable-to-nonnull-conversion -Wuninitialized -Wno-missing-field-initializers
			-Wno-ignored-qualifiers -Wno-missing-braces -Wno-char-subscripts -Wno-unused-private-field -fno-aligned-allocation -Wunused-private-field -Wunreachable-code
			-Wenum-compare -Wshadow -Wfloat-conversion -Wshadow-uncaptured-local -Wshadow-field -Wsign-compare -Wdeprecated-this-capture -Wimplicit-float-conversion
			-ffast-math -fno-finite-math-only -Wfloat-equal -Wpedantic -Wmissing-field-initializers -Wdeprecated -Wcast-align
			"$<$<CONFIG:RELEASE>:-Werror>"
		)
endif ()

file (GLOB_RECURSE source_files CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h)

target_sources (UnitTests PRIVATE ${source_files})
source_group (TREE ${CMAKE_CURRENT_SOURCE_DIR}/Source PREFIX Source FILES ${source_files})

target_link_libraries (UnitTests PRIVATE
						UnitTestAssets

						gin
						gin_3d
						gin_dsp
						gin_graphics
						gin_gui
						gin_metadata
						gin_network
						gin_plugin
						gin_simd
						gin_webp

						juce::juce_audio_processors
						juce::juce_audio_utils
						juce::juce_cryptography
						juce::juce_data_structures
						juce::juce_events
						juce::juce_core
						juce::juce_audio_basics
						juce::juce_gui_basics
						juce::juce_gui_extra

						$<$<PLATFORM_ID:Linux>:curl>

    					juce::juce_recommended_config_flags
					)

if (APPLE)
	set_target_properties(UnitTests PROPERTIES XCODE_ATTRIBUTE_CLANG_LINK_OBJC_RUNTIME "NO")
endif()

if (WIN32)
	target_compile_options(UnitTests PRIVATE -wd4996 -wd4458)
endif()

juce_generate_juce_header (UnitTests)

get_property (debug_configs GLOBAL PROPERTY DEBUG_CONFIGURATIONS)

if (NOT debug_configs)
    set (debug_configs Debug)
endif ()

set (config_is_debug "$<IN_LIST:$<CONFIG>,${debug_configs}>")
set (config_is_release "$<NOT:${config_is_debug}>")

target_compile_definitions (UnitTests PRIVATE
								JUCE_WEB_BROWSER=0
								JUCE_COREGRAPHICS_DRAW_ASYNC=1
								GIN_UNIT_TESTS=1
								DONT_SET_USING_JUCE_NAMESPACE=1
								JucePlugin_Name="UnitTests"
								JucePlugin_Build_Standalone=1
							)

if (APPLE)
	set_target_properties(UnitTests PROPERTIES
			XCODE_ATTRIBUTE_DEPLOYMENT_POSTPROCESSING[variant=Release] YES
			XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] "YES"
			)
endif()
