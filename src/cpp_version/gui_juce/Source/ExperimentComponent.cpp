/*
  ==============================================================================

    ExperimentComponent.cpp
    Created: 16 May 2016 6:46:19pm
    Author:  Emilio Molina

  ==============================================================================
*/

#include "ExperimentComponent.h"



/** @brief Convert float / double to string with n digits of precision
 *
 * @param[in] a_value Float or double input value
 * @param[in] n Number of decimal digits
 * @returns String containing number with the desired number of decimals
 */
template <typename T>
std::string to_string_with_precision(const T a_value, const int n)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(n) << a_value;
    return out.str();
}

/** @brief ExperimentComponent constructor
 *
 * When a new instance is created, a browser to choose a directory appears.
 * Once a folder is selected, then it parse the directory contents into an Ecosystem object.
 */
ExperimentComponent::ExperimentComponent() {
    setOpaque (true);
    addAndMakeVisible (_folderLabel);
    addAndMakeVisible (_folderButton);
    addAndMakeVisible(_runButton);
    addAndMakeVisible(_maxTimeLabel);
    addAndMakeVisible(_lastBackupLabel);
    addAndMakeVisible(_pauseButton);
    addAndMakeVisible(_timeSlider);
    _folderButton.setButtonText("Choose experiment folder");
    _folderButton.addListener(this);
    _runButton.setButtonText("Run");
    _runButton.addListener(this);
    _maxTimeLabel.setText("max time:", dontSendNotification);
    _lastBackupLabel.setText("3", dontSendNotification);
    _pauseButton.setButtonText("Pause");
    _pauseButton.addListener(this);
    _timeSlider.setRange(0, 10000, 1);
    _timeSlider.setVelocityBasedMode(true);
    _timeSlider.addListener(this);
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
    
    _runButton.setBounds(1 * percentage_x, 7 * percentage_y,       // x, y
                         20 * percentage_x, 5 * percentage_y);     // width, height
    
    _maxTimeLabel.setBounds (1 * percentage_x, 12 * percentage_y,       // x, y
                             90 * percentage_x, 5 * percentage_y);     // width, height
    
    _lastBackupLabel.setBounds (1 * percentage_x, 17 * percentage_y,       // x, y
                                90 * percentage_x, 5 * percentage_y);     // width, height
    
    _pauseButton.setBounds (1 * percentage_x, 23 * percentage_y,       // x, y
                            20 * percentage_x, 5 * percentage_y);     // width, height
    
    _timeSlider.setBounds (1 * percentage_x, 30 * percentage_y,       // x, y
                           90 * percentage_x, 5 * percentage_y);     // width, height
}
    
void ExperimentComponent::refreshExperimentSize()
{
    fs::path p = fs::path(_experimentFolder.toStdString());
    double size=0.0;
    for(bf::recursive_directory_iterator it(p);
        it!=bf::recursive_directory_iterator();
        ++it)
    {
        if(!is_directory(*it))
            size+=bf::file_size(*it);
    }
    double _directory_size = size / 1000000;
    string str_directory_size = to_string_with_precision(_directory_size, 2);
    _folderLabel.setText ("Experiment folder: " + _experimentFolder + "  (" + str_directory_size + "MB)",
                          dontSendNotification);
}
    
void ExperimentComponent::buttonClicked(Button* b)
{
    if (b == &_folderButton) {
        FileChooser fc ("Choose an experiment directory",
                        File::getCurrentWorkingDirectory(),
                        "*.",
                        true);
        if (fc.browseForDirectory())
        {
            File chosenDirectory = fc.getResult();
            _experimentFolder = chosenDirectory.getFullPathName();
            refreshExperimentSize();
            
        }
    }
}
    
void ExperimentComponent::sliderValueChanged(Slider* s) {  // needed to make it compile
}
    
void ExperimentComponent::sliderDragEnded(Slider* s) {
    if (s == &_timeSlider) {
        cout << _timeSlider.getValue() << endl;
    }
}