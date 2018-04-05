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

namespace bf=boost::filesystem;
using json = nlohmann::json;

class ValueTreeItem;

/** @brief Component for experiment tab
 *
 * @ingroup GUI
 */
class SettingsComponent : public Component {
public:
    /** @brief Pointer to parent MainContentComponent
     */
    MainContentComponent* parent_component;
    
    // Public methods (documentation in SettingsComponent.cpp)
    SettingsComponent(MainContentComponent* parent_component);
    virtual ~SettingsComponent();
    void updateTree();
    void paint (Graphics& g) override;
    void resized() override;
    void refreshExperimentSize();
    void changeSelectedItem(ValueTreeItem* selectedItem);
    ValueTreeItem* selectedItem;
    
private:
    /** @brief Tree of settings
     */
    TreeView _tv;
    ScopedPointer<ValueTreeItem> rootItem;
};

#endif /* SettingsComponent_h */
