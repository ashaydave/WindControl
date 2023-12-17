/*
  ==============================================================================

    LPF.cpp
    Created: 17 Dec 2023 2:46:48pm
    Author:  ashay

  ==============================================================================
*/

#include "LPF.h"

WindControl::LPF::LPF()
{
}

WindControl::LPF::~LPF()
{
}

void WindControl::LPF::prepareToPlay(float cutoff, int samplesPerBlock, double sampleRate)
{
    float c0 = std::tan(juce::MathConstants<float>::pi * cutoff / (sampleRate / samplesPerBlock));
    coefficient = c0/ (1 + c0);
}

float WindControl::LPF::process(float input)
{
    previousOutput = (1.0f - coefficient) * previousOutput + coefficient * input;
    previousOutput = previousOutput < 0.00001f ? 0.0f : previousOutput; //denormal check, if previous output is small, set it to 0 and switch the filter off
    return previousOutput;
}
