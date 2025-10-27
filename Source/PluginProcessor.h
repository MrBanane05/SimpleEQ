/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


class SimpleEQAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================

    // Constructeur : initialise le processeur audio
    SimpleEQAudioProcessor();

    // Destructeur : nettoie les ressources utilisées
    ~SimpleEQAudioProcessor() override;

    // Prépare le processeur avant la lecture audio (initialisation, allocation)
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;

    // Libère les ressources lorsque l'audio s'arrête
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    // Vérifie si la configuration des bus audio est supportée
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    // Fonction principale de traitement audio (modifie le buffer audio à chaque bloc)
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================

    // Crée l'éditeur graphique du plugin
    juce::AudioProcessorEditor* createEditor() override;

    // Indique si le plugin possède une interface graphique
    bool hasEditor() const override;

    //==============================================================================

    // Retourne le nom du plugin
    const juce::String getName() const override;

    // Indique si le plugin accepte des messages MIDI
    bool acceptsMidi() const override;

    // Indique si le plugin produit des messages MIDI
    bool producesMidi() const override;

    // Indique si le plugin est un effet MIDI
    bool isMidiEffect() const override;

    // Retourne la durée de la "queue" audio (utile pour les effets comme la réverb)
    double getTailLengthSeconds() const override;

    //==============================================================================

    // Gestion des programmes/presets
    int getNumPrograms() override;                          // Nombre de programmes disponibles
    int getCurrentProgram() override;                       // Programme actuellement sélectionné
    void setCurrentProgram (int index) override;            // Change le programme actif
    const juce::String getProgramName (int index) override; // Nom d'un programme
    void changeProgramName (int index, const juce::String& newName) override; // Change le nom d'un programme

    //==============================================================================

    // Sauvegarde l'état du plugin (paramètres, etc.)
    void getStateInformation (juce::MemoryBlock& destData) override;

    // Restaure l'état du plugin à partir de données sauvegardées
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Crée la structure des paramètres du plugin
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Gestion centralisée des paramètres du plugin
    juce::AudioProcessorValueTreeState apvts {*this, nullptr, "parameters", createParameterLayout()};

private:
    
	// Type alias pour simplifier la déclaration des filtres (coefficients)
	using Filter = juce::dsp::IIR::Filter<float>;

	// Chaîne de traitement pour les filtres coupe-bas et coupe-haut (filtres à 4 coefficients)
	using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;

	// Chaîne de traitement complète pour chaque canal (gauche et droit)
	using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;

	// Instances des chaînes de traitement pour les canaux gauche et droit
	MonoChain leftChain, rightChain;

    // Empêche la copie et détecte les fuites de mémoire
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleEQAudioProcessor)
};
