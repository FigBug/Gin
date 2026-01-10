#include "PluginProcessor.h"
#include "PluginEditor.h"

// Unit test includes
using namespace gin;
    
#include "../../../modules/gin/geometry/gin_catenary.test.h"
#include "../../../modules/gin/geometry/gin_geometry.test.h"
#include "../../../modules/gin/utilities/gin_diff.test.h"
#include "../../../modules/gin/utilities/gin_easing.test.h"
#include "../../../modules/gin/utilities/gin_equationparser.test.h"
#include "../../../modules/gin/utilities/gin_filesystemwatcher.test.h"
#include "../../../modules/gin/utilities/gin_fileutilities.test.h"
#include "../../../modules/gin/utilities/gin_integrator.test.h"
#include "../../../modules/gin/utilities/gin_lagrange.test.h"
#include "../../../modules/gin/utilities/gin_leastsquaresregression.test.h"
#include "../../../modules/gin/utilities/gin_linearregression.test.h"
#include "../../../modules/gin/utilities/gin_lockfreequeue.test.h"
#include "../../../modules/gin/utilities/gin_lrucache.test.h"
#include "../../../modules/gin/utilities/gin_messagepack.test.h"
#include "../../../modules/gin/utilities/gin_plist.test.h"
#include "../../../modules/gin/utilities/gin_realtimeasyncupdater.test.h"
#include "../../../modules/gin/utilities/gin_riffparser.test.h"
#include "../../../modules/gin/utilities/gin_sharedmemory.test.h"
#include "../../../modules/gin/utilities/gin_spline.test.h"
#include "../../../modules/gin/utilities/gin_systemsemaphore.test.h"
#include "../../../modules/gin/utilities/gin_texthistory.test.h"
#include "../../../modules/gin/utilities/gin_threading.test.h"
#include "../../../modules/gin/utilities/gin_util.test.h"
#include "../../../modules/gin/utilities/gin_valuetreeobject.test.h"
#include "../../../modules/gin/utilities/gin_valuetreeutilities.test.h"
#include "../../../modules/gin_dsp/dsp/gin_adsr.test.h"
#include "../../../modules/gin_dsp/dsp/gin_analogadsr.test.h"
#include "../../../modules/gin_dsp/dsp/gin_audiofifo.test.h"
#include "../../../modules/gin_dsp/dsp/gin_audiometadata.test.h"
#include "../../../modules/gin_dsp/dsp/gin_audiofunctions.test.h"
#include "../../../modules/gin_dsp/dsp/gin_audiomidififo.test.h"
#include "../../../modules/gin_dsp/dsp/gin_audioutil.test.h"
#include "../../../modules/gin_dsp/dsp/gin_delayline.test.h"
#include "../../../modules/gin_dsp/dsp/gin_distortion.test.h"
#include "../../../modules/gin_dsp/dsp/gin_dynamics.test.h"
#include "../../../modules/gin_dsp/dsp/gin_easedvaluesmoother.test.h"
#include "../../../modules/gin_dsp/dsp/gin_fastmath.test.h"
#include "../../../modules/gin_dsp/dsp/gin_leveltracker.test.h"
#include "../../../modules/gin_dsp/dsp/gin_lfo.test.h"
#include "../../../modules/gin_dsp/dsp/gin_midififo.test.h"
#include "../../../modules/gin_dsp/dsp/gin_midifileplayer.test.h"
#include "../../../modules/gin_dsp/dsp/gin_modulation.test.h"
#include "../../../modules/gin_dsp/dsp/gin_noise.test.h"
#include "../../../modules/gin_dsp/dsp/gin_oscillators.test.h"
#include "../../../modules/gin_dsp/dsp/gin_sampleplayer.test.h"
#include "../../../modules/gin_dsp/dsp/gin_scratchbuffer.test.h"
#include "../../../modules/gin_dsp/dsp/gin_valuesmoother.test.h"
#include "../../../modules/gin_graphics/formats/gin_bmpimageformat.test.h"
#include "../../../modules/gin_graphics/images/gin_imageeffects.test.h"
#include "../../../modules/gin_graphics/images/gin_imageeffects_blending.test.h"
#include "../../../modules/gin_gui/utilities/gin_componentmap.test.h"
#include "../../../modules/gin_gui/utilities/gin_layoutsupport.test.h"
#include "../../../modules/gin_gui/utilities/gin_varianthelpers.test.h"
#include "../../../modules/gin_plugin/plugin/gin_noteduration.test.h"
#include "../../../modules/gin_plugin/plugin/gin_parameter.test.h"
#include "../../../modules/gin_plugin/plugin/gin_modmatrix.test.h"
#include "../../../modules/gin_plugin/plugin/gin_program.test.h"
#include "../../../modules/gin_plugin/plugin/gin_smoothedparameter.test.h"
#include "../../../modules/gin_dsp/dsp/gin_filter.test.h"
#include "../../../modules/gin_dsp/dsp/gin_eq.test.h"
#include "../../../modules/gin_dsp/dsp/gin_graphiceq.test.h"
#include "../../../modules/gin_dsp/dsp/gin_synthesiser.test.h"
#include "../../../modules/gin_dsp/dsp/gin_wavetable.test.h"
#include "../../../modules/gin_3d/3d/gin_vec3.test.h"
#include "../../../modules/gin_3d/3d/gin_mat4.test.h"
#include "../../../modules/gin_3d/3d/gin_camera3d.test.h"
#include "../../../modules/gin_3d/3d/gin_scene3d.test.h"
#include "../../../modules/gin_3d/3d/gin_renderer3d.test.h"

UnitTestsAudioProcessor::UnitTestsAudioProcessor()
    : gin::Processor (BusesProperties()
                      .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{
}

UnitTestsAudioProcessor::~UnitTestsAudioProcessor()
{
}

void UnitTestsAudioProcessor::prepareToPlay (double, int)
{
}

void UnitTestsAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                            juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();
}

bool UnitTestsAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* UnitTestsAudioProcessor::createEditor()
{
    return new UnitTestsAudioProcessorEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new UnitTestsAudioProcessor();
}
