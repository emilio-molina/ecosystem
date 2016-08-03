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


class ExperimentInterface;

//==============================================================================
/*
    Main component of the application
*/
class MainContentComponent   : public Component,
                               private Timer
{
public:
    /** @brief Used to lock ecosystem in some atomic operations
        @TODO: Check if should be deleted
     */
    CriticalSection mtx;

    /** @brief Instance of ExperimentInterface. Where the Ecosystem is running.
     */
    ExperimentInterface* experiment_interface;
    
    /** @brief True if ecosystem is running
     */
    bool playing;
    MainContentComponent();
    ~MainContentComponent();

    void paint (Graphics&);
    void resized();

private:
    /** @brief Function where Ecosystem evolves. It is called every 100ms by default
     */
    void timerCallback();
    //==============================================================================
    ScopedPointer<TabbedComponent> _tabbedComponent;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


#endif  // MAINCOMPONENT_H_INCLUDED
