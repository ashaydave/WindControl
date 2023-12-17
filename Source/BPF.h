#ifndef ASHAY_WINDCONTROLPROCESSOR_H
#define ASHAY_WINDCONTROLPROCESSOR_H

/*
  ==============================================================================

    BPF.h
    Created: 16 Dec 2023 6:09:25pm
    Author:  ashay

  ==============================================================================
*/

#include <JuceHeader.h>
#include "LPF.h"

namespace WindControl
{
    class BPF
    {
	public:
		BPF();
		~BPF();
		void prepareToPlay(const juce::dsp::ProcessSpec& spec);
		void windProcess(juce::AudioBuffer<float>& buffer);
		void whistleProcess(juce::AudioBuffer<float>& buffer);
		void howlProcess(juce::AudioBuffer<float>& buffer);
		void cosPan(float* output, float pan);
		void updateParameters();
		void setWindParameters(juce::AudioParameterFloat* windCutoff, juce::AudioParameterFloat* windQ, juce::AudioParameterFloat* windAmplitude, juce::AudioParameterFloat* windPan, juce::AudioParameterFloat* windSpeed);
		void setWhistleParameters(juce::AudioParameterFloat* whistlePan1, juce::AudioParameterFloat* whistlePan2, juce::AudioParameterFloat* whistleAmplitude);
		void setHowlParameters(juce::AudioParameterFloat* howlPan1, juce::AudioParameterFloat* howlPan2, juce::AudioParameterFloat* howlAmplitude);

	private:
		float sampleRate;

		// Parameters

		juce::AudioParameterFloat* gain;

		juce::AudioParameterFloat* windSpeed;

		juce::AudioParameterFloat* windCutoff;
		juce::AudioParameterFloat* windQ;
		juce::AudioParameterFloat* windAmplitude;
		juce::AudioParameterFloat* windPan;

		juce::AudioParameterFloat* whistlePan1;
		juce::AudioParameterFloat* whistlePan2;
		juce::AudioParameterFloat* whistleAmplitude;
		float whistleWindSpeed1, whistleWindSpeed2;

		juce::AudioParameterFloat* howlPan1;
		juce::AudioParameterFloat* howlPan2;
		juce::AudioParameterFloat* howlAmplitude;
		float howlWindSpeed1, howlWindSpeed2;

		// Read Indexes and Static Constants

		static const int windSpeedCBSize = 500;
		static const int numOutputChannels = 2;
		static const int maxPanFrames = 20;
		float windSpeedCB[windSpeedCBSize];
		int windSpeedCBWriteIndex { 0 };

		int whistleReadIndex1 = windSpeedCBSize - 6; // delay of 6 frames
		int whistleReadIndex2 = windSpeedCBSize - 16; // delay of 16 frames

		int howlReadIndex1 = windSpeedCBSize - 6; // delay of 6 frames
		int howlReadIndex2 = windSpeedCBSize - 51; // delay of 51 frames

		// Oscillators and Filters

		juce::dsp::StateVariableTPTFilter<float> windFilter;

		juce::dsp::StateVariableTPTFilter<float> whistleFilter1;
		juce::dsp::StateVariableTPTFilter<float> whistleFilter2;

		juce::dsp::StateVariableTPTFilter<float> howlFilter1;
		juce::dsp::StateVariableTPTFilter<float> howlFilter2;

		juce::dsp::Oscillator<float> howlOscillator1;
		juce::dsp::Oscillator<float> howlOscillator2;
		WindControl::LPF howlLPF1;
		WindControl::LPF howlLPF2;

		juce::Random random;

		//juce::dsp::Oscillator<float> noiseOscillator
		//{ [](float x)
		//	{
		//		juce::Random random;
		//		return random.nextFloat() * 2.0f - 1.0f;
		//	}
		//};
	};
}
#endif
