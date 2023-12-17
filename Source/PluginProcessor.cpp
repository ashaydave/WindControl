/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"

//================================= =============================================
WindControlAudioProcessor::WindControlAudioProcessor()
     : AudioProcessor (BusesProperties()
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                       )
{
    addParameter(gain = new juce::AudioParameterFloat("gain", "Master Volume", 0.0f, 1.0f, 0.5f));

    addParameter(windCutoff = new juce::AudioParameterFloat("windCutoff", "Intensity", 1.0f, 50.0f, 30.0f));
    addParameter(windQ = new juce::AudioParameterFloat("windQ", "Resonance", 1.0f, 100.0f, 10.0f));
    addParameter(windAmplitude = new juce::AudioParameterFloat("windAmplitude", "Amplitude", 0.0001f, 1.5f, 0.75f));
    addParameter(windSpeed = new juce::AudioParameterFloat("windSpeed", "Wind Speed", 0.0f, 40.0f, 1.0f));
    addParameter(windPan = new juce::AudioParameterFloat("windPan", "Wind Pan", 0.0f, 1.0f, 0.5f));

    addParameter(whistleAmplitude = new juce::AudioParameterFloat("whistleAmplitude", "Whistle Gain", 0.0001f, 1.5f, 0.75f));
    addParameter(whistlePan1 = new juce::AudioParameterFloat("whistlePan1", "Whistle Pan 1", 0.0f, 1.0f, 0.5f));
    addParameter(whistlePan2 = new juce::AudioParameterFloat("whistlePan2", "Whistle Pan 2", 0.0f, 1.0f, 0.5f));

    addParameter(howlAmplitude = new juce::AudioParameterFloat("howlAmplitude", "Howl Gain", 0.0001f, 1.5f, 0.75f));
    /*addParameter(howlPan1 = new juce::AudioParameterFloat("howlPan1", "Howl Pan 1", 0.0f, 1.0f, 0.5f));
    addParameter(howlPan2 = new juce::AudioParameterFloat("howlPan2", "Howl Pan 2", 0.0f, 1.0f, 0.5f));*/

    bpf.setWindParameters(windCutoff, windQ, windAmplitude, windPan, windSpeed);
    bpf.setWhistleParameters(whistlePan1, whistlePan2, whistleAmplitude);
    bpf.setHowlParameters(howlPan1, howlPan2, howlAmplitude);
}

WindControlAudioProcessor::~WindControlAudioProcessor()
{
}

//==============================================================================
const juce::String WindControlAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool WindControlAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool WindControlAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool WindControlAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double WindControlAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int WindControlAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int WindControlAudioProcessor::getCurrentProgram()
{
    return 0;
}

void WindControlAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String WindControlAudioProcessor::getProgramName (int index)
{
    return {};
}

void WindControlAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void WindControlAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = 1;
    currentSpec = spec;
  
    bpf.prepareToPlay(spec);

}

void WindControlAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool WindControlAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void WindControlAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    //auto totalNumInputChannels  = getTotalNumInputChannels();
    //auto totalNumOutputChannels = getTotalNumOutputChannels();
    buffer.clear();

    bpf.updateParameters();
    bpf.windProcess(buffer);
    bpf.whistleProcess(buffer);
    bpf.howlProcess(buffer);

    buffer.applyGain(gain->get());
    
}

//==============================================================================
bool WindControlAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* WindControlAudioProcessor::createEditor()
{
   return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void WindControlAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void WindControlAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WindControlAudioProcessor();
}
