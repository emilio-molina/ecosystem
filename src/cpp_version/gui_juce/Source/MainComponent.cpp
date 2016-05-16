#include "MainComponent.h"
#include "MapComponent.h"
#include "ExperimentComponent.h"
namespace bf=boost::filesystem;
using json = nlohmann::json;


//==============================================================================
MainContentComponent::MainContentComponent()
{
    setSize (800, 600);
    // Create tabs and add components to each tab
    _tabbedComponent = new TabbedComponent(TabbedButtonBar::TabsAtTop);
    _tabbedComponent->addTab("Experiment", Colour::fromFloatRGBA(0.8f, 0.677f, 0.617f, 1.0f), new ExperimentComponent(), true);
    _tabbedComponent->addTab("View", Colour::fromFloatRGBA(0.0f, 0.077f, 0.217f, 1.0f), new MapComponent(ecosystem, *this), true);
    _tabbedComponent->addTab("Settings", Colour::fromFloatRGBA(0.8f, 0.677f, 0.617f, 1.0f), new Component(), true);
    addAndMakeVisible(_tabbedComponent);
    startTimer(100);  // call timer callback every 100ms
}

MainContentComponent::~MainContentComponent()
{
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
    ecosystem.evolve();
    auto num_organisms = ecosystem.biotope.size();
    auto num_free_locs = ecosystem.biotope_free_locs.size();
    cout << "Time: " << ecosystem.time << endl;
    cout << "    num organism: " << num_organisms << endl;
    cout << "    num free locs: " << num_free_locs << endl;
    cout << "    sum previous numbers: " << num_organisms + num_free_locs << endl;
}
