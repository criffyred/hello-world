#include "ModulationSystem.h"

ModulationSystem::ModulationSystem()
{
    lfo1.frequency = 0.5f; // 0.5 Hz
    lfo2.frequency = 2.0f; // 2 Hz
}

void ModulationSystem::prepare(double sampleRate)
{
    randomUpdateInterval = static_cast<int>(sampleRate * 0.1); // Update random every 100ms
    sampleCounter = 0;
}

void ModulationSystem::update(int numSamples, double sampleRate)
{
    // Update LFOs
    lfo1.update(sampleRate, numSamples);
    lfo2.update(sampleRate, numSamples);

    lfo1Value = lfo1.value;
    lfo2Value = lfo2.value;

    // Update random modulators periodically
    sampleCounter += numSamples;
    if (sampleCounter >= randomUpdateInterval)
    {
        random1Value = random.nextFloat();
        random2Value = random.nextFloat();
        sampleCounter = 0;
    }
}
