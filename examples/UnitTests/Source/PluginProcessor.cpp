#include "PluginProcessor.h"
#include "PluginEditor.h"

UnitTestsAudioProcessor::UnitTestsAudioProcessor()
    : AudioProcessor (BusesProperties()
                      .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{
}

UnitTestsAudioProcessor::~UnitTestsAudioProcessor()
{
}

const juce::String UnitTestsAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool UnitTestsAudioProcessor::acceptsMidi() const
{
    return false;
}

bool UnitTestsAudioProcessor::producesMidi() const
{
    return false;
}

bool UnitTestsAudioProcessor::isMidiEffect() const
{
    return false;
}

double UnitTestsAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int UnitTestsAudioProcessor::getNumPrograms()
{
    return 1;
}

int UnitTestsAudioProcessor::getCurrentProgram()
{
    return 0;
}

void UnitTestsAudioProcessor::setCurrentProgram (int)
{
}

const juce::String UnitTestsAudioProcessor::getProgramName (int)
{
    return {};
}

void UnitTestsAudioProcessor::changeProgramName (int, const juce::String&)
{
}

void UnitTestsAudioProcessor::prepareToPlay (double, int)
{
}

void UnitTestsAudioProcessor::releaseResources()
{
}

bool UnitTestsAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
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

void UnitTestsAudioProcessor::getStateInformation (juce::MemoryBlock&)
{
}

void UnitTestsAudioProcessor::setStateInformation (const void*, int)
{
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new UnitTestsAudioProcessor();
}
