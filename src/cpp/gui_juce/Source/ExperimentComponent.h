/** @file ExperimentComponent.h
 * @brief Header of ExperimentComponent
 *
 */

#ifndef EXPERIMENTCOMPONENT_H_INCLUDED
#define EXPERIMENTCOMPONENT_H_INCLUDED

#include "MainComponent.h"
#include "ecosystem.h"
#include "ExperimentInterface.h"
#include <boost/filesystem.hpp>

namespace bf=boost::filesystem;
using json = nlohmann::json;

/** @brief Component for experiment tab
 */
class ExperimentComponent : public Component, public Button::Listener {
public:
    /** @brief Pointer to parent MainContentComponent
     */
    MainContentComponent* parent_component;
    
    // Public methods (documentation in ExperimentComponent.cpp)
    ExperimentComponent(MainContentComponent* parent_component);
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


#endif  // EXPERIMENTCOMPONENT_H_INCLUDED
