/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "ecosystem.h"
#include "ExperimentInterface.h"
#include <boost/filesystem.hpp>



class MapComponent;
class ExperimentComponent;
class ExperimentInterface;

//==============================================================================
/*
    Main component of the application
*/
class MainContentComponent   : public Component,
                               private Timer
{
public:
    /** @brief Instance of ExperimentInterface. Where the Ecosystem is running.
     */
    ExperimentInterface* experiment_interface;
    bool experiment_has_changed;
    
    /** @brief True if ecosystem is running
     */
    bool running;
    MainContentComponent();
    ~MainContentComponent();

    void paint (Graphics&);
    void resized();
    void loadEcosystemInterface(ExperimentInterface* ei);
private:
    /** @brief Function where Ecosystem evolves. It is called every 100ms by default
     */
    void timerCallback();
    int _backupCounter;
    MapComponent* _map_component;
    ExperimentComponent* _experiment_component;
    //==============================================================================
    ScopedPointer<TabbedComponent> _tabbedComponent;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


#endif  // MAINCOMPONENT_H_INCLUDED
