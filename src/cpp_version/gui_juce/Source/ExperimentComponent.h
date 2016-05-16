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
#include <boost/filesystem.hpp>

namespace bf=boost::filesystem;
using json = nlohmann::json;


template <typename T>
std::string to_string_with_precision(const T a_value, const int n);

/** @brief Class to easily interact with experiment folder
 */
class ExperimentReader {
    
    /** @brief Initializer
     *
     * @param[in] experiment_folder String with experiment_folder path
     */
    ExperimentReader(string experiment_folder);
    
    
    /** @brief Get a list of time slices containing a complete backup of ecosystem
     *
     * @returns List of time slices allowing complete backup
     */
    vector<int> getTimesHavingCompleteBackups();
    
    
    /** @brief Get path of JSON containing ecosystem data
     *
     * @param[in] time_slice Time slice for which ecosystem data will be get
     *
     * @returns Path of JSON file
     */
    string getEcosystemJSONPath(int time_slice);
    
    /** @brief Get a list of time slices containing a given organism parameter
     *
     * @param[in] parameter Parameter to read from JSON files
     *
     * @returns Vector of time values
     */
    vector<int> getTimesHavingOrganismParameter(string parameter);
    
    
    /** @brief Get a list of time slices containing a given ecosystem parameter
     *
     * @param[in] parameter Parameter to read from JSON files
     *
     * @returns Vector of time values
     */
    vector<int> getTimesHavingEcosystemParameter(string parameter);
    
    
    /** @brief Get a list of available ecosystem parameters
     *
     * @returns Vector of parameters available
     */
    vector<string> getAvailableEcosystemParameters();
    
    
    /** @brief Get a list of available organism parameters
     *
     * @returns Vector of parameters available
     */
    vector<string> getAvailableOrganismParameters();
    
    /** @brief Get ecosystem data for a given time slice
     *
     * @param[in] time_slice Target time slice
     */
    json getEcosystemData(int time_slice);
    
    /** @brief Get ecosystem data for a given time slice
     *
     * @param[in] organism_id Organism ID
     * @param[in] time_slice Target time slice
     */
    json getOrganismData(int organism_id, int time_slice);
    
    
    /** @brief Get a list of avaiable living organisms ids
     *
     * @param[in] time_slice Target time slice
     */
    vector<int> getLivingOrganismsIds(int time_slice);
    
    /** @brief Get a list of avaiable dead organisms ids
     *
     * @param[in] time_slice Target time slice
     */
    vector<int> getDeadOrganismsIds(int time_slice);
    
    /* @brief Get experiment size in MBs in format e.g. "321.16MB"
     *
     * @returns String with experiment size
     */
    string getExperimentSize();
};

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
