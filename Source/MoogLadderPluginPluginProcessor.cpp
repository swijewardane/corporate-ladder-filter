#include <JuceHeader.h>
#include <cstring>

#include "MoogLadderPlugin.h"
#include "MoogLadderPlugin_types.h"

#if JUCE_VERSION >= 0x050400
using Parameter = AudioProcessorValueTreeState::Parameter;
#endif

struct onParamChangeListener : AudioProcessorValueTreeState::Listener
{
    onParamChangeListener(MoogLadderPluginStackData* sd) : SD(sd)
    {
    }

    void parameterChanged (const String& parameterID, float newValue) override
    {
        const int idx = getParameterIndex(parameterID);
        onParamChangeCImpl(SD, idx, static_cast<double>(newValue));
    }
    
private:
    int getParameterIndex (const String& parameterID)
    {
        (void)parameterID;
    
        if (parameterID == "Cutoff") {
            return 0;
        }
        if (parameterID == "Resonance") {
            return 1;
        }
        if (parameterID == "Gain") {
            return 2;
        }
        if (parameterID == "bypass") {
            return 3;
        }
        if (parameterID == "ADAA_bypass") {
            return 4;
        }
    
        return (-1);
    }    
    
    MoogLadderPluginStackData *SD;
};

//==============================================================================
class MoogLadderPluginAudioProcessor  : public AudioProcessor
{
    //==============================================================================
#if JUCE_VERSION >= 0x050400

public:
    MoogLadderPluginAudioProcessor()
        : paramListener(&mStackData),
          parameters(*this, nullptr, "MoogLadderPlugin", {
                std::make_unique<Parameter>("Cutoff", juce::String::fromUTF8(u8"Cutoff"), juce::String::fromUTF8(u8"Hz"),
                    NormalisableRange<float>(20.f,20000.f,[](float min, float max, float norm) {return min*powf(max/min,norm);}, [](float min, float max, float val) {return logf(val/min)/logf(max/min);}), 2000.f, [](float val) {return String(val, 3);}, nullptr),
                std::make_unique<Parameter>("Resonance", juce::String::fromUTF8(u8"Resonance"), juce::String::fromUTF8(u8"%"),
                    NormalisableRange<float>(0.f,1.f), 0.f, [](float val) {return String(val*100.f, 1);}, [](const String& text){return text.getFloatValue()/100.f;}),
                std::make_unique<Parameter>("Gain", juce::String::fromUTF8(u8"Gain"), juce::String::fromUTF8(u8"db"),
                    NormalisableRange<float>(-60.f,12.f), 0.f, [](float val) {return String(val, 3);}, nullptr),
                std::make_unique<Parameter>("bypass", juce::String::fromUTF8(u8"Plugin"), juce::String::fromUTF8(u8""),
                    NormalisableRange<float>(0.f,1.f,1.f), 1.f, [](float val) {return String(val, 3);}, nullptr),
                std::make_unique<Parameter>("ADAA_bypass", juce::String::fromUTF8(u8"ADAA"), juce::String::fromUTF8(u8""),
                    NormalisableRange<float>(0.f,1.f,1.f), 1.f, [](float val) {return String(val, 3);}, nullptr) })

    {
#ifndef DNNL_HPP
        memset(&mStackData, 0, sizeof(mStackData));
        memset(&mPersistentData, 0, sizeof(mPersistentData));
#endif
        
        mStackData.pd = &mPersistentData;
        
        MoogLadderPlugin_initialize(&mStackData);

        createPluginInstance(&mStackData, reinterpret_cast<unsigned long long>(this));

        parameters.addParameterListener("Cutoff", &paramListener);
        parameters.addParameterListener("Resonance", &paramListener);
        parameters.addParameterListener("Gain", &paramListener);
        parameters.addParameterListener("bypass", &paramListener);
        parameters.addParameterListener("ADAA_bypass", &paramListener);

    }
    //==============================================================================
#else // For JUCE prior to 5.4.0
public:
    MoogLadderPluginAudioProcessor()
    :   paramListener(&mStackData), parameters (*this, nullptr)
    {
#ifndef DNNL_HPP
        memset(&mStackData, 0, sizeof(mStackData));
        memset(&mPersistentData, 0, sizeof(mPersistentData));
#endif

        mStackData.pd = &mPersistentData;
        
        MoogLadderPlugin_initialize(&mStackData);

        createPluginInstance(&mStackData, reinterpret_cast<unsigned long long>(this));

        //
        // Parameter property Cutoff
        //
        parameters.createAndAddParameter ("Cutoff", juce::String::fromUTF8(u8"Cutoff"), juce::String::fromUTF8(u8"Hz"),
            NormalisableRange<float>(20.f, 20000.f, 
                [](float min, float max, float norm) {return min * powf(max/min, norm);},
                [](float min, float max, float val) {return logf(val/min)/logf(max/min);} ),
            2000.f,
            [](float val) {return String(val, 3);},
            nullptr);
        parameters.addParameterListener("Cutoff", &paramListener);

        //
        // Parameter property Resonance
        //
        parameters.createAndAddParameter ("Resonance", juce::String::fromUTF8(u8"Resonance"), juce::String::fromUTF8(u8"%"),
            NormalisableRange<float>(0.f, 1.f), 0.f,
            [](float val) {return String(val*100.f, 1);},
            [](const String& text) {return text.getFloatValue() / 100.f;});
        parameters.addParameterListener("Resonance", &paramListener);

        //
        // Parameter property Gain
        //
        parameters.createAndAddParameter ("Gain", juce::String::fromUTF8(u8"Gain"), juce::String::fromUTF8(u8"db"),
            NormalisableRange<float>(-60.f, 12.f), 0.f,
            [](float val) {return String(val, 3);},
            nullptr);
        parameters.addParameterListener("Gain", &paramListener);

        //
        // Parameter property bypass
        //
        parameters.createAndAddParameter ("bypass", juce::String::fromUTF8(u8"Plugin Bypass"), juce::String::fromUTF8(u8""),
            NormalisableRange<float>(0.f, 1.f, 1.f), 0.f,
            [](float val) {return String(val, 3);},
            nullptr);
        parameters.addParameterListener("bypass", &paramListener);

        //
        // Parameter property ADAA_bypass
        //
        parameters.createAndAddParameter ("ADAA_bypass", juce::String::fromUTF8(u8"ADAA Bypass"), juce::String::fromUTF8(u8""),
            NormalisableRange<float>(0.f, 1.f, 1.f), 1.f,
            [](float val) {return String(val, 3);},
            nullptr);
        parameters.addParameterListener("ADAA_bypass", &paramListener);

        parameters.state = ValueTree(Identifier("MoogLadderPlugin"));
    }
#endif

    //==============================================================================
    ~MoogLadderPluginAudioProcessor()
    {
        MoogLadderPlugin_terminate(&mStackData);
    }
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override
    {
        (void)samplesPerBlock;
        resetCImpl(&mStackData, sampleRate);
        setLatencySamples(getLatencyInSamplesCImpl(&mStackData));
    }

    void releaseResources() override                { }
    
    
    void processBlock (AudioBuffer<double>& buffer, MidiBuffer&) override
    {
        ScopedNoDenormals noDenormals;
        const int nSamples = buffer.getNumSamples();
        const int nChannels = buffer.getNumChannels();
        const double* const* pin = buffer.getArrayOfReadPointers();
        double* const* pout = buffer.getArrayOfWritePointers();
        std::vector<const double*> inputs(pin,pin+nChannels);
        std::vector<double*> outputs(pout,pout+nChannels);
        int i_;

        MoogLadderPluginStackData *SD = &mStackData;

        if (nChannels < 2) {
            const int numExtraChannels = 2 - nChannels;
            tempBuffer.setSize(numExtraChannels, nSamples);
            if (nChannels < 2) {
                tempBuffer.clear(0, nSamples);
                const double* const* p = tempBuffer.getArrayOfReadPointers();
                std::copy(p, p+numExtraChannels, std::back_inserter(inputs));
            }
            if (nChannels < 2) {
                double* const* p = tempBuffer.getArrayOfWritePointers();
                std::copy(p, p+numExtraChannels, std::back_inserter(outputs));
            }
        }

        int osz0_;
        int osz1_;
        if (nSamples <= MAX_SAMPLES_PER_FRAME) {
            /* Fast path for common frame sizes. */
            const int isz0_ = nSamples;
            const int isz1_ = nSamples;
            processEntryPoint(SD, (double)nSamples,
                    inputs[0], &isz0_,
                    inputs[1], &isz1_,
                    outputs[0], &osz0_,
                    outputs[1], &osz1_);
        } else {
            /* Fallback for unusually large frames. */
            int isz0_ = MAX_SAMPLES_PER_FRAME;
            int isz1_ = MAX_SAMPLES_PER_FRAME;
            int n = MAX_SAMPLES_PER_FRAME;
            for (i_ = 0; i_ < nSamples; i_ += MAX_SAMPLES_PER_FRAME) {
                if (i_ + MAX_SAMPLES_PER_FRAME > nSamples) {
                    n = nSamples - i_;
                    isz0_ = nSamples - i_;
                    isz1_ = nSamples - i_;
                }
                processEntryPoint(SD, (double)n,
                        inputs[0]+i_, &isz0_,
                        inputs[1]+i_, &isz1_,
                        outputs[0]+i_, &osz0_,
                        outputs[1]+i_, &osz1_);
            }
        }

    }
    
    void processBlock (AudioBuffer<float>& buffer,  MidiBuffer& midiMessages) override
    {
        AudioBuffer<double> doubleBuffer;
        doubleBuffer.makeCopyOf(buffer);
        processBlock(doubleBuffer, midiMessages);
        buffer.makeCopyOf(doubleBuffer);
    }
    
    //==============================================================================
    bool hasEditor() const override                 { return true; }
    AudioProcessorEditor* createEditor() override;
    
    //==============================================================================
    const String getName() const override           { return JucePlugin_Name; }

    bool acceptsMidi() const override               { return false; }
    bool producesMidi() const override              { return false; }
    bool isMidiEffect () const override             { return false; }
    double getTailLengthSeconds() const override    { return 0.0;   }

    //==============================================================================
    // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
    int getNumPrograms() override                       { return 1;  }
    int getCurrentProgram() override                    { return 0;  }
    void setCurrentProgram (int index) override         { (void) index; }
    const String getProgramName (int index) override    { (void) index; return {}; }
    void changeProgramName (int index, const String& newName) override  { (void) index; (void) newName; }
    
    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override
    {
        auto xml (parameters.state.createXml());
        copyXmlToBinary (*xml, destData);
    }
    
    void setStateInformation (const void* data, int sizeInBytes) override
    {
        auto xmlState (getXmlFromBinary (data, sizeInBytes));
        if (xmlState != nullptr)
            if (xmlState->hasTagName (parameters.state.getType()))
                parameters.state = ValueTree::fromXml (*xmlState);
    }
    
    bool supportsDoublePrecisionProcessing() const override  { return true; }
    
private:
    //==============================================================================
    static const int MAX_SAMPLES_PER_FRAME = 4096;

    MoogLadderPluginStackData mStackData;
    MoogLadderPluginPersistentData mPersistentData;
    onParamChangeListener paramListener;
    AudioBuffer<double> tempBuffer;
    
    //==============================================================================
    AudioProcessorValueTreeState parameters;
 
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MoogLadderPluginAudioProcessor)
};

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MoogLadderPluginAudioProcessor();
}

#include "MoogLadderPluginPluginEditor.h"

AudioProcessorEditor* MoogLadderPluginAudioProcessor::createEditor()
{
    return new MoogLadderPluginAudioProcessorEditor(*this, parameters);
}

