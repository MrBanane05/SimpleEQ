/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleEQAudioProcessor::SimpleEQAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

SimpleEQAudioProcessor::~SimpleEQAudioProcessor()
{
}

//==============================================================================
const juce::String SimpleEQAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SimpleEQAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SimpleEQAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SimpleEQAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SimpleEQAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SimpleEQAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SimpleEQAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SimpleEQAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SimpleEQAudioProcessor::getProgramName (int index)
{
    return {};
}

void SimpleEQAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SimpleEQAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void SimpleEQAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SimpleEQAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void SimpleEQAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
}

//==============================================================================
bool SimpleEQAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SimpleEQAudioProcessor::createEditor()
{
    //return new SimpleEQAudioProcessorEditor (*this);

	return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void SimpleEQAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SimpleEQAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

// Fonction qui retourne l'ensemble des param�tres utilis�s par le plugin.

juce::AudioProcessorValueTreeState::ParameterLayout
SimpleEQAudioProcessor::createParameterLayout()
{
    // Cr�ation d�un objet qui va contenir tous les param�tres du plugin
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // ========== PARAM�TRE 1 : LowCut Frequency ==========
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "LowCut Freq",                    // ID interne utilis� pour identifier le param�tre
        "LowCut Freq",                    // Nom visible dans l'interface ou automation
        juce::NormalisableRange<float>(
            20.f,                         // Valeur minimale (20 Hz)
            20000.f,                      // Valeur maximale (20 000 Hz)
            1.f,                          // Pas (incr�ment par clic ou molette)
            1.f                           // Skew factor (1 = lin�aire ; pas de ressenti "log")
        ),
        20.f                              // Valeur par d�faut
    ));

    // ========== PARAM�TRE 2 : HighCut Frequency ==========
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "HighCut Freq",
        "HighCut Freq",
        juce::NormalisableRange<float>(20.f,
            20000.f,
            1.f,
            1.f
        ), // Idem que LowCut
        20000.f                           // Valeur par d�faut (maximum)
    ));

    // ========== PARAM�TRE 3 : Peak Frequency ==========
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Peak Freq",
        "Peak Freq",
        juce::NormalisableRange<float>(20.f,
            20000.f,
            1.f,
            1.f
        ), // Fr�quence centrale du filtre "bell"
        750.f                              // Valeur par d�faut : 750 Hz
    ));

    // ========== PARAM�TRE 4 : Peak Gain ==========
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Peak gain",
        "Peak gain",
        juce::NormalisableRange<float>(
            -24.f,                        // Gain minimum (att�nuation)
            24.f,                         // Gain maximum (amplification)
            0.5f,                         // Pas de 0.5 dB
            1.f                           // Skew = lin�aire (ce qu'on veut pour des dB)
        ),
        0.0f                              // Valeur par d�faut : 0 dB (aucune modification)
    ));

    // ========== PARAM�TRE 5 : Peak Quality (facteur Q) ==========
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Peak Quality",
        "Peak Quality",
        juce::NormalisableRange<float>(
            0.1f,                         // Q minimum (tr�s large bande)
            10.f,                         // Q maximum (bande tr�s �troite)
            0.05f,                        // Pas d'ajustement fin
            1.f                           // Skew lin�aire (convient bien ici)
        ),
        1.0f                              // Valeur par d�faut : Q = 1.0 (standard)
    ));

    juce::StringArray StringArray;
    for (int i = 0; i < 4; i++) {
        juce::String str;
		str << (12 + i*12);               // 12, 24, 36, 48
        str << " dB/oct";                 // Ajoute l'unit�
        
        // Ajoute la cha�ne de caract�res � la liste
		// Ajoute la cha�ne format�e � l'array
		StringArray.add(str);
    }

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "LowCut Slope",                 // ID interne du param�tre
        "LowCut Slope",                 // Nom affich� dans l'interface
        StringArray,                    // Liste des options (StringArray)
        0                               // Valeur par d�faut : index 0 = "12 dB/oct"
    ));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "HighCut Slope",
        "HighCut Slope",
        StringArray,
        0
    ));



    // Retourne la structure avec tous les param�tres ajout�s
    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleEQAudioProcessor();
}
