/** @file MainComponent.cpp
 * @brief Main of GUI (based on JUCE library)
 *
 *
 */

#include "MainComponent.h"
#include "MapComponent.h"
#include "ExperimentComponent.h"
#include "SettingsComponent.h"
namespace bf=boost::filesystem;
using json = nlohmann::json;


/** @brief MainContentComponent constructor
 *
 */
MainContentComponent::MainContentComponent()
{
    setSize (800, 600);
    _experiment_component = new ExperimentComponent(this);
    _map_component = new MapComponent(this);
    _settings_component = new SettingsComponent(this);
    // Create tabs and add components to each tab
    _tabbedComponent = new TabbedComponent(TabbedButtonBar::TabsAtTop);
    _tabbedComponent->addTab("Experiment", Colour::fromFloatRGBA(0.8f, 0.677f, 0.617f, 1.0f), _experiment_component, true);
    //_tabbedComponent->addTab("View", Colour::fromFloatRGBA(0.0f, 0.077f, 0.217f, 1.0f), _map_component, true);
    //_tabbedComponent->addTab("Settings", Colour::fromFloatRGBA(0.7f, 0.777f, 0.517f, 1.0f), _settings_component, true);
    addAndMakeVisible(_tabbedComponent);
    startTimer(100);  // call timer callback every 100ms
    running = false;
    experiment_interface = nullptr;
    experiment_has_changed = false;
    _backupCounter = 0;
}

/** @brief MainContentComponent destructor
 *
 */
MainContentComponent::~MainContentComponent()
{
    delete experiment_interface;
}

/** @brief Override method to paint JUCE component
 *
 */
void MainContentComponent::paint (Graphics& g)
{
    g.fillAll (Colour::greyLevel(0.1f));
    _tabbedComponent->setBounds(0, 0, getWidth(), getHeight());  // TODO: Check, is this right?
    
}

/** @brief Override method to resize JUCE component
 *
 */
void MainContentComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}

/** @brief This function is called periodically to evolve ecosystem
 *
 */
void MainContentComponent::timerCallback() {  // evolve ecosystem
    if (this->running) {
        if (_backupCounter == 0)
            experiment_interface->saveEcosystem();
        _backupCounter = (_backupCounter + 1) % 10;
        experiment_interface->evolve();
        experiment_has_changed = true;
        _map_component->setMaxTime(experiment_interface->getTimesHavingCompleteBackups().back());
        _map_component->setRunningTime(experiment_interface->getRunningTime());
    }
}

/** @brief Load an ecosystem interface
 *
 */
void MainContentComponent::loadEcosystemInterface(ExperimentInterface* ei) {
    experiment_interface = ei;
    //_tabbedComponent->addTab("View", Colour::fromFloatRGBA(0.0f, 0.077f, 0.217f, 1.0f), _map_component, true);
    //_tabbedComponent->addTab("Settings", Colour::fromFloatRGBA(0.7f, 0.777f, 0.517f, 1.0f), _settings_component, true);
    StringArray tab_names = _tabbedComponent->getTabNames();
    if (!tab_names.contains("View"))
    {
        _tabbedComponent->addTab("View", Colour::fromFloatRGBA(0.0f, 0.077f, 0.217f, 1.0f), _map_component, true);
    }
    if (!tab_names.contains("Settings"))
    {
        _tabbedComponent->addTab("Settings", Colour::fromFloatRGBA(0.7f, 0.777f, 0.517f, 1.0f), _settings_component, true);
    }
    _map_component->setMaxTime(experiment_interface->getTimesHavingCompleteBackups().back());
    _map_component->setRunningTime(experiment_interface->getRunningTime());
    experiment_has_changed = true;
}


