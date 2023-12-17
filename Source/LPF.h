#ifndef LPF_H
#define LPF_H

/*
  ==============================================================================

    LPF.h
    Created: 17 Dec 2023 2:46:48pm
    Author:  ashay

  ==============================================================================
*/

#include <JuceHeader.h>

namespace WindControl
{
    class LPF
    {
	public:
		LPF();
		~LPF();
        void prepareToPlay(float cutoff, int samplesPerBlock, double sampleRate);
        float process(float input);

    private:
        float coefficient;
        float previousOutput;
	};
}

#endif
