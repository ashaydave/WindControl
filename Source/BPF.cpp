/*
  ==============================================================================

    BPF.cpp
    Created: 16 Dec 2023 6:09:25pm
    Author:  ashay

  ==============================================================================
*/

#include "BPF.h"

WindControl::BPF::BPF() : windSpeedCB{}
{
    windCutoff = 0;
    windQ = 0;
    windAmplitude = 0;
    gain = 0;
    windSpeed = 0;
    windPan = 0;
    whistlePan1 = 0;
    whistlePan2 = 0;
    whistleAmplitude = 0;
    howlPan1 = 0;
    howlPan2 = 0;
    howlAmplitude = 0;
    whistleWindSpeed1 = 0;
    whistleWindSpeed2 = 0;
    howlWindSpeed1 = 0;
    howlWindSpeed2 = 0;


}

WindControl::BPF::~BPF()
{
}

void WindControl::BPF::prepareToPlay(const juce::dsp::ProcessSpec& spec)
{
    // Noise Oscillator
    // noiseOscillator.prepare(spec);

    // Wind

    windFilter.prepare(spec);
    windFilter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
    windFilter.setCutoffFrequency(windCutoff->get());
    windFilter.setResonance(windQ->get());
    windFilter.reset();

    // Whistle

    whistleFilter1.prepare(spec);
    whistleFilter1.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
    whistleFilter1.setCutoffFrequency(1000.0f);
    whistleFilter1.setResonance(60.0f);
    whistleFilter1.reset();

    whistleFilter2.prepare(spec);
    whistleFilter2.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
    whistleFilter2.setCutoffFrequency(1000.0f);
    whistleFilter2.setResonance(60.0f);
    whistleFilter2.reset();

    // Howl

    howlFilter1.prepare(spec);
    howlFilter1.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
    howlFilter1.setCutoffFrequency(400.0f);
    howlFilter1.setResonance(40.0f);
    howlFilter1.reset();

    howlFilter2.prepare(spec);
    howlFilter2.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
    howlFilter2.setCutoffFrequency(200.0f);
    howlFilter2.setResonance(40.0f);
    howlFilter2.reset();

    howlOscillator1.initialise([](float x) { return std::sin(x); }, 128); // Wavetable
    howlOscillator2.initialise([](float x) { return std::sin(x); }, 128);

    howlLPF1.prepareToPlay(0.5f, spec.maximumBlockSize, spec.sampleRate);
    howlLPF2.prepareToPlay(0.4f, spec.maximumBlockSize, spec.sampleRate);
}

void WindControl::BPF::windProcess(juce::AudioBuffer<float>& buffer)
{
    int numSamples = buffer.getNumSamples();
    float frameAmplitude = windAmplitude->get();

    float pan[2];
    cosPan(pan, windPan->get());

    for (int sample = 0; sample < numSamples; ++sample)
    {
		float output = windFilter.processSample(0, random.nextFloat() * 2.0f - 1.0f) * frameAmplitude;
		buffer.addSample(0, sample, output * pan[0]);
		buffer.addSample(1, sample, output * pan[1]);
	}

    windFilter.snapToZero();


   /* int numSamples = buffer.getNumSamples();
    int numChannels = buffer.getNumChannels();
    float frameAmplitude = windAmplitude->get();

    for (int channel = 0; channel < numChannels; ++channel)
    {
        for (int sample = 0; sample < numSamples; ++sample)
        {
			float output = windFilter.processSample(channel, noiseOscillator.processSample(0.0f));
            buffer.addSample(channel, sample, output * frameAmplitude);
		}
	}*/
}

void WindControl::BPF::whistleProcess(juce::AudioBuffer<float>& buffer)
{
    int numSamples = buffer.getNumSamples();
	float frameAmplitude = whistleAmplitude->get();

	float pan1[2];
	cosPan(pan1, whistlePan1->get());
	float pan2[2];
	cosPan(pan2, whistlePan2->get());
    float AmpMod1 = juce::square(juce::jmax(0.0f, whistleWindSpeed1 * 0.02f - 0.1f));
    float AmpMod2 = juce::square(juce::jmax(0.0f, whistleWindSpeed2 * 0.02f - 0.1f));

    for (int sample = 0; sample < numSamples; ++sample)
    {
        float noiseOutput = random.nextFloat() * 2.0f - 1.0f;
		float output1 = whistleFilter1.processSample(0, noiseOutput) * frameAmplitude * AmpMod1;
		float output2 = whistleFilter2.processSample(0, noiseOutput) * frameAmplitude * AmpMod2;
		buffer.addSample(0, sample, output1 * pan1[0] + output2 * pan2[0]);
		buffer.addSample(1, sample, output1 * pan1[1] + output2 * pan2[1]);
	}

	whistleFilter1.snapToZero();
	whistleFilter2.snapToZero();
}

void WindControl::BPF::howlProcess(juce::AudioBuffer<float>& buffer)
{
    int numSamples = buffer.getNumSamples();
	float frameAmplitude = howlAmplitude->get();

	float pan1[2];
	cosPan(pan1, 0.5f); //howlPan1->get()
	float pan2[2];
	cosPan(pan2, 0.5f); //howlPan2->get()
    float AmpMod1 = howlLPF1.process(juce::dsp::FastMathApproximations::cos(
        ((juce::jlimit(0.35f, 0.6f, howlWindSpeed1 * 0.02f) - 0.35f) * 2.0f - 0.25f) * juce::MathConstants<float>::twoPi));
    float AmpMod2 = howlLPF2.process(juce::dsp::FastMathApproximations::cos(
		((juce::jlimit(0.35f, 0.6f, howlWindSpeed2 * 0.02f) - 0.35f) * 2.0f - 0.25f) * juce::MathConstants<float>::twoPi));

    howlOscillator1.setFrequency(AmpMod1 * 400.0f + 30.0f);
    howlOscillator2.setFrequency(AmpMod2 * 200.0f + 20.0f);

    for (int sample = 0; sample < numSamples; ++sample)
    {
		float noiseOutput = random.nextFloat() * 2.0f - 1.0f;
		float output1 = howlFilter1.processSample(0, noiseOutput) * frameAmplitude * AmpMod1 * howlOscillator1.processSample(0.0f);
		float output2 = howlFilter2.processSample(0, noiseOutput) * frameAmplitude * AmpMod2 * howlOscillator2.processSample(0.0f);
		buffer.addSample(0, sample, output1 * pan1[0] + output2 * pan2[0]);
		buffer.addSample(1, sample, output1 * pan1[1] + output2 * pan2[1]);
	}

	howlFilter1.snapToZero();
	howlFilter2.snapToZero();
}

void WindControl::BPF::cosPan(float* output, float pan)
{
    output[0] = juce::dsp::FastMathApproximations::cos((pan * 0.25f - 0.5f) * juce::MathConstants<float>::twoPi);
    output[1] = juce::dsp::FastMathApproximations::cos((pan * 0.25f - 0.25f) * juce::MathConstants<float>::twoPi);
}

void WindControl::BPF::updateParameters()
{
    // Update Circular Buffer

    float currentWindSpeed = windSpeed->get();
    windSpeedCB[windSpeedCBWriteIndex] = currentWindSpeed;
    ++windSpeedCBWriteIndex;
    windSpeedCBWriteIndex = (windSpeedCBWriteIndex < windSpeedCBSize) ? windSpeedCBWriteIndex : 0;

    // Update Panning

    float whsPan1 = whistlePan1->get();
    float whsPan2 = whistlePan2->get();
    float hwlPan1 = 0.5f; //howlPan1->get()
    float hwlPan2 = 0.5f; //howlPan2->get()

    // Update Wind Filter

    float currentIntensity = windCutoff->get();
    float currentQ = windQ->get();
	windFilter.setCutoffFrequency(currentWindSpeed * currentIntensity);
	windFilter.setResonance(currentQ);

    // Update Whistle Filter

    whistleReadIndex1 = windSpeedCBWriteIndex - (int)(whsPan1 * maxPanFrames);
    whistleReadIndex1 = (whistleReadIndex1 < 0) ? whistleReadIndex1 + windSpeedCBSize : whistleReadIndex1;
    whistleReadIndex2 = windSpeedCBWriteIndex - (int)(whsPan2 * maxPanFrames);
    whistleReadIndex2 = (whistleReadIndex2 < 0) ? whistleReadIndex2 + windSpeedCBSize : whistleReadIndex2;
    whistleWindSpeed1 = windSpeedCB[whistleReadIndex1]; 
    whistleWindSpeed2 = windSpeedCB[whistleReadIndex2];
    whistleFilter1.setCutoffFrequency(whistleWindSpeed1 * 8.0f + 600.0f); // Can play with the numbers here or expose to Unity
    whistleFilter2.setCutoffFrequency(whistleWindSpeed2 * 20.0f + 1000.0f);

    // Update Howl Filter

    howlReadIndex1 = windSpeedCBWriteIndex - (int)(hwlPan1 * maxPanFrames);
    howlReadIndex1 = (howlReadIndex1 < 0) ? howlReadIndex1 + windSpeedCBSize : howlReadIndex1;
    howlReadIndex2 = windSpeedCBWriteIndex - (int)(hwlPan2 * maxPanFrames);
    howlReadIndex2 = (howlReadIndex2 < 0) ? howlReadIndex2 + windSpeedCBSize : howlReadIndex2;
    howlWindSpeed1 = windSpeedCB[howlReadIndex1];
    howlWindSpeed2 = windSpeedCB[howlReadIndex2];
}

void WindControl::BPF::setWindParameters(juce::AudioParameterFloat* windCutoff, juce::AudioParameterFloat* windQ, juce::AudioParameterFloat* windAmplitude,
    juce::AudioParameterFloat* windPan, juce::AudioParameterFloat* windSpeed)
{
    this->windCutoff = windCutoff;
    this->windQ = windQ;
    this->windAmplitude = windAmplitude;
    this->windPan = windPan;
    this->windSpeed = windSpeed;
}

void WindControl::BPF::setWhistleParameters(juce::AudioParameterFloat* whistlePan1, juce::AudioParameterFloat* whistlePan2, juce::AudioParameterFloat* whistleAmplitude)
{
    this->whistlePan1 = whistlePan1;
	this->whistlePan2 = whistlePan2;
	this->whistleAmplitude = whistleAmplitude;
}

void WindControl::BPF::setHowlParameters(juce::AudioParameterFloat* howlPan1, juce::AudioParameterFloat* howlPan2, juce::AudioParameterFloat* howlAmplitude)
{
    this->howlPan1 = howlPan1;
	this->howlPan2 = howlPan2;
	this->howlAmplitude = howlAmplitude;
}
