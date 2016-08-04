/*
  ==============================================================================

    ExperimentComponent.cpp
    Created: 16 May 2016 6:46:19pm
    Author:  Emilio Molina

  ==============================================================================
*/

#include "ExperimentComponent.h"


/** @brief ExperimentComponent constructor
 *
 * @param[in] parent_component Parent component of this one
 */
ExperimentComponent::ExperimentComponent(MainContentComponent* parent_component) {
    this->parent_component = parent_component;
    setOpaque (true);
    addAndMakeVisible (_folderLabel);
    addAndMakeVisible (_folderButton);
    addAndMakeVisible(_runButton);
    addAndMakeVisible(_maxTimeLabel);
    addAndMakeVisible(_lastBackupLabel);
    addAndMakeVisible(_pauseButton);
    _folderButton.setButtonText("Choose experiment folder");
    _folderButton.addListener(this);
    _runButton.setButtonText("Run");
    _runButton.addListener(this);
    _runButton.setEnabled(false);
    _maxTimeLabel.setText("max time:", dontSendNotification);
    _lastBackupLabel.setText("3", dontSendNotification);
    _pauseButton.setButtonText("Pause");
    _pauseButton.addListener(this);
    _pauseButton.setEnabled(false);
}



void ExperimentComponent::paint (Graphics& g)
{
    g.fillAll(Colour::fromFloatRGBA(0.8f, 0.677f, 0.617f, 1.0f));
}
    
    
void ExperimentComponent::resized()
{
    int percentage_x = getWidth() / 100;
    int percentage_y = getHeight() / 100;
    _folderButton.setBounds(1 * percentage_x, 2 * percentage_y,       // x, y
                            20 * percentage_x, 5 * percentage_y);     // width, height
    
    _folderLabel.setBounds (1 * percentage_x, 6 * percentage_y,       // x, y
                            90 * percentage_x, 5 * percentage_y);     // width, height
    
    _runButton.setBounds(1 * percentage_x, 7 * percentage_y,          // x, y
                         20 * percentage_x, 5 * percentage_y);        // width, height
    
    _maxTimeLabel.setBounds (1 * percentage_x, 12 * percentage_y,     // x, y
                             90 * percentage_x, 5 * percentage_y);    // width, height
    
    _lastBackupLabel.setBounds (1 * percentage_x, 17 * percentage_y,  // x, y
                                90 * percentage_x, 5 * percentage_y); // width, height
    
    _pauseButton.setBounds (1 * percentage_x, 23 * percentage_y,      // x, y
                            20 * percentage_x, 5 * percentage_y);     // width, height
}


void ExperimentComponent::buttonClicked(Button* b)
{
    /* Folder button
     * =============
     *
     * 1. Open directory dialog
     * 2. Set experiment folder
     *    - If exists: choose if load it or delete existing experiment
     */
    if (b == &_folderButton) {
        FileChooser fc ("Choose an experiment directory",
                        File::getCurrentWorkingDirectory(),
                        "*.",
                        true);
        if (fc.browseForDirectory())
        {
            if (parent_component->experiment_interface != nullptr)
                delete parent_component->experiment_interface;
            parent_component->experiment_has_changed = true;
            _runButton.setEnabled(true);
            _pauseButton.setEnabled(true);
            File chosenDirectory = fc.getResult();
            string full_directory_path = chosenDirectory.getFullPathName().toStdString();
            if (experimentAlreadyExists(full_directory_path)) {
                bool chosen_ok;
                chosen_ok = AlertWindow::showOkCancelBox (AlertWindow::QuestionIcon,
                                                          "Experiment already exists",
                                                          "Press OK to load existing experiment, or Cancel to delete it and start a new experiment.",
                                                          String(),
                                                          String(),
                                                          0);
                if (chosen_ok) {
                    parent_component->experiment_interface =
                    new ExperimentInterface(full_directory_path, false);
                } else {
                    parent_component->experiment_interface =
                    new ExperimentInterface(full_directory_path, true);
                }
            } else {
                parent_component->experiment_interface =
                new ExperimentInterface(full_directory_path, true);
            }
            //refreshExperimentSize();
        }
    }
    
    /* Run button
     * ==========
     *
     * Indicate the ecosystem must run
     *
     */
    if (b == &_runButton) {
        this->parent_component->running = true;
    }
    
    /* Run button
     * ==========
     *
     * Indicate the ecosystem must stop
     *
     */
    if (b == &_pauseButton) {
        this->parent_component->running = false;
    }
}