set_property (DIRECTORY APPEND PROPERTY LABELS Demo)

add_subdirectory (Resources)

set (icon_path "${CMAKE_CURRENT_LIST_DIR}/Resources/icon.png")

juce_add_gui_app (Demo
				  PRODUCT_NAME Demo
				  VERSION 1.0.0
				  COMPANY_NAME "Rabien Software"
				  COMPANY_WEBSITE "https://rabien.com/"
				  BUNDLE_ID com.rabien.gin.demo
				  ICON_BIG "${icon_path}"
				  ICON_SMALL "${icon_path}")

if (APPLE)
	target_compile_options(Demo PRIVATE 
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
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h)

target_sources (Demo PRIVATE ${source_files})
source_group (TREE ${CMAKE_CURRENT_SOURCE_DIR}/Source PREFIX Source FILES ${source_files})

target_link_libraries (Demo PRIVATE
						DemoAssets

						gin
						gin_graphics
						gin_gui
						gin_webp
						gin_dsp
						gin_metadata
						gin_network
						gin_simd

						juce::juce_cryptography
						juce::juce_data_structures
						juce::juce_events
						juce::juce_graphics
						juce::juce_gui_basics
						juce::juce_gui_extra
						juce::juce_core
						juce::juce_audio_basics

						$<$<PLATFORM_ID:Linux>:curl>

    					juce::juce_recommended_config_flags
					)

if (APPLE)
	set_target_properties(Demo PROPERTIES XCODE_ATTRIBUTE_CLANG_LINK_OBJC_RUNTIME "NO")
endif()

if (WIN32)
	target_compile_options(Demo PRIVATE -wd4996 -wd4458)
endif()

juce_generate_juce_header (Demo)

get_property (debug_configs GLOBAL PROPERTY DEBUG_CONFIGURATIONS)

if (NOT debug_configs)
    set (debug_configs Debug)
endif ()

set (config_is_debug "$<IN_LIST:$<CONFIG>,${debug_configs}>")
set (config_is_release "$<NOT:${config_is_debug}>")

target_compile_definitions (Demo PRIVATE 
								JUCE_DISPLAY_SPLASH_SCREEN=0
								JUCE_MODAL_LOOPS_PERMITTED=0
								JUCE_WEB_BROWSER=0
								JUCE_SILENCE_XCODE_15_LINKER_WARNING=1
								JUCE_COREGRAPHICS_DRAW_ASYNC=1
							)

if (APPLE)
	set_target_properties(Demo PROPERTIES
			XCODE_ATTRIBUTE_DEPLOYMENT_POSTPROCESSING[variant=Release] YES
			XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] "YES"
			)
endif()
