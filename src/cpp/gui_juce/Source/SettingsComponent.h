//
//  SettingsComponent.h
//  gui_juce
//
//  Created by Juan Luis Salas García on 12/9/16.
//
//

#ifndef SettingsComponent_h
#define SettingsComponent_h

#include "MainComponent.h"
#include "ecosystem.h"
#include "ExperimentInterface.h"
#include <boost/filesystem.hpp>

namespace bf=boost::filesystem;
using json = nlohmann::json;

/** @brief Component for experiment tab
 */
class SettingsComponent : public Component, public ButtonListener {
public:
    /** @brief Pointer to parent MainContentComponent
     */
    MainContentComponent* parent_component;
    
    // Public methods (documentation in SettingsComponent.cpp)
    SettingsComponent(MainContentComponent* parent_component);
    void paint (Graphics& g) override;
    void resized() override;
    void refreshExperimentSize();
    void buttonClicked(Button* b) override;
    
private:
    /** @brief Size of experiment directory
     */
    double _directory_size;
    
    /** @brief Chosen directory for experiment
     */
    String _experimentFolder;
    
    /** @brief Button object to choose a folder
     */
    TextButton _folderButton;
};

#endif /* SettingsComponent_h */
