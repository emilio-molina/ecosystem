#include "MainComponent.h"
#include "MapComponent.h"
#include "ExperimentComponent.h"
namespace bf=boost::filesystem;
using json = nlohmann::json;


//==============================================================================
MainContentComponent::MainContentComponent()
{
    setSize (800, 600);
    _experiment_component = new ExperimentComponent(this);
    _map_component = new MapComponent(this);
    // Create tabs and add components to each tab
    _tabbedComponent = new TabbedComponent(TabbedButtonBar::TabsAtTop);
    _tabbedComponent->addTab("Experiment", Colour::fromFloatRGBA(0.8f, 0.677f, 0.617f, 1.0f), _experiment_component, true);
    _tabbedComponent->addTab("View", Colour::fromFloatRGBA(0.0f, 0.077f, 0.217f, 1.0f), _map_component, true);
    _tabbedComponent->addTab("Settings", Colour::fromFloatRGBA(0.8f, 0.677f, 0.617f, 1.0f), new Component(), true);
    addAndMakeVisible(_tabbedComponent);
    startTimer(100);  // call timer callback every 100ms
    running = false;
    experiment_interface = nullptr;
    experiment_has_changed = false;
}

MainContentComponent::~MainContentComponent()
{
    delete experiment_interface;
}

void MainContentComponent::paint (Graphics& g)
{
    g.fillAll (Colour::greyLevel(0.1f));
    _tabbedComponent->setBounds(0, 0, getWidth(), getHeight());
    
}

void MainContentComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}

void MainContentComponent::timerCallback() {
    if (this->running) {
        experiment_interface->saveEcosystem();
        experiment_interface->evolve();
        experiment_has_changed = true;
        _map_component->setMaxTime(experiment_interface->getTimesHavingCompleteBackups());
    }
}

void MainContentComponent::loadEcosystemInterface(ExperimentInterface* ei) {
    experiment_interface = ei;
    _map_component->setMaxTime(experiment_interface->getTimesHavingCompleteBackups());
    experiment_has_changed = true;
}


