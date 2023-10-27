set_property (DIRECTORY APPEND PROPERTY LABELS Effect)

juce_add_plugin (Effect
				 PRODUCT_NAME Effect
				 VERSION 1.0.0
				 COMPANY_NAME "Rabien Software"
				 COMPANY_WEBSITE "https://rabien.com/"
				 FORMATS "VST3" "AU" "Standalone" "LV2"
				 BUNDLE_ID com.rabien.gin.effect
				 PLUGIN_NAME "Effect"
				 DESCRIPTION "Effect"
				 PLUGIN_MANUFACTURER_CODE "Soca"
				 PLUGIN_CODE "Effc"
				 AAX_IDENTIFIER "com.rabien.effect"
				 AU_EXPORT_PREFIX "EffectAU"
				 AU_MAIN_TYPE "kAudioUnitType_Effect"
				 VST2_CATEGORY "kPlugCategEffect"
				 VST3_CATEGORIES Fx
				 LV2URI "https://rabien.com/effect/"
			   )

if (APPLE)
	target_compile_options(Effect PRIVATE 
			-Wall -Wstrict-aliasing -Wunused-parameter -Wconditional-uninitialized -Woverloaded-virtual -Wreorder -Wconstant-conversion -Wbool-conversion -Wextra-semi 
			-Wunreachable-code -Winconsistent-missing-destructor-override -Wshift-sign-overflow -Wnullable-to-nonnull-conversion -Wuninitialized -Wno-missing-field-initializers 
			-Wno-ignored-qualifiers -Wno-missing-braces -Wno-char-subscripts -Wno-unused-private-field -fno-aligned-allocation -Wunused-private-field -Wunreachable-code 
			-Wenum-compare -Wshadow -Wfloat-conversion -Wshadow-uncaptured-local -Wshadow-field -Wsign-compare -Wdeprecated-this-capture -Wimplicit-float-conversion 
			-ffast-math -fno-finite-math-only -Wfloat-equal
			"$<$<CONFIG:RELEASE>:-Werror>"
		)
endif ()

file (GLOB_RECURSE source_files CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h)

target_sources (Effect PRIVATE ${source_files})
source_group (TREE ${CMAKE_CURRENT_SOURCE_DIR}/Source PREFIX Source FILES ${source_files})

target_link_libraries (Effect PRIVATE

						gin
						gin_graphics
						gin_gui
						gin_dsp
						gin_plugin
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
	set_target_properties(Effect PROPERTIES XCODE_ATTRIBUTE_CLANG_LINK_OBJC_RUNTIME "NO")
endif()

if (WIN32)
	target_compile_options(Effect PRIVATE -wd4996 -wd4458)
endif()

juce_generate_juce_header (Effect)

get_property (debug_configs GLOBAL PROPERTY DEBUG_CONFIGURATIONS)

if (NOT debug_configs)
    set (debug_configs Debug)
endif ()

set (config_is_debug "$<IN_LIST:$<CONFIG>,${debug_configs}>")
set (config_is_release "$<NOT:${config_is_debug}>")

target_compile_definitions (Effect PRIVATE 
								JUCE_DISPLAY_SPLASH_SCREEN=0
								JUCE_MODAL_LOOPS_PERMITTED=0
								JUCE_COREGRAPHICS_DRAW_ASYNC=1
								JUCE_VST3_CAN_REPLACE_VST2=0
								JUCE_WEB_BROWSER=0
								DONT_SET_USING_JUCE_NAMESPACE=1
								JUCE_SILENCE_XCODE_15_LINKER_WARNING=1
							)

if (APPLE)
	set_target_properties(Effect PROPERTIES
			XCODE_ATTRIBUTE_DEPLOYMENT_POSTPROCESSING[variant=Release] YES
			XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] "YES"
			)
endif()
