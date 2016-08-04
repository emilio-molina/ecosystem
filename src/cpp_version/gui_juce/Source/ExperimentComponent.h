/*
  ==============================================================================

    ExperimentComponent.h
    Created: 16 May 2016 6:46:19pm
    Author:  Emilio Molina

  ==============================================================================
*/

#ifndef EXPERIMENTCOMPONENT_H_INCLUDED
#define EXPERIMENTCOMPONENT_H_INCLUDED

#include "MainComponent.h"
#include "ecosystem.h"
#include "ExperimentInterface.h"
#include <boost/filesystem.hpp>

namespace bf=boost::filesystem;
using json = nlohmann::json;


template <typename T>
std::string to_string_with_precision(const T a_value, const int n);

/** @brief Component for experiment tab
 *
 */
class ExperimentComponent : public Component, public ButtonListener, public SliderListener {
public:
    MainContentComponent* parent_component;
    ExperimentComponent(MainContentComponent* parent_component);
    
    void paint (Graphics& g) override;
    void resized() override;
    void refreshExperimentSize();
    void buttonClicked(Button* b) override;
    void sliderValueChanged(Slider* s) override; // needed to make it compile
    void sliderDragEnded(Slider* s) override;
    
private:
    /** @brief Size of experiment directory
     */
    double _directory_size;
    
    /** @brief Chosen directory for experiment
     */
    String _experimentFolder;
    
    /** @brief Label object showing directory folder and size
     */
    Label _folderLabel;
    
    /** @brief Label object showing max time value for experiment
     */
    Label _maxTimeLabel;
    
    /** @brief Label object showing last backup of complete experiment
     */
    Label _lastBackupLabel;
    
    /** @brief Button object to choose a folder
     */
    TextButton _folderButton;
    
    /** @brief Button object to run experiment
     */
    TextButton _runButton;
    
    /** @brief Button object to pause experiment
     */
    TextButton _pauseButton;
    
    /** @brief Slider object to select a time slice
     */
    Slider _timeSlider;
};


#endif  // EXPERIMENTCOMPONENT_H_INCLUDED
