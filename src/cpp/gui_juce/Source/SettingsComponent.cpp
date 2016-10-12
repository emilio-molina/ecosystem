//
//  SettingsComponent.cpp
//  gui_juce
//
//  Created by Juan Luis Salas García on 12/9/16.
//
//

//#include <stdio.h>
#include "SettingsComponent.h"

ValueTree createTree (const String& desc)
{
    ValueTree t ("Item");
    t.setProperty ("name", desc, nullptr);
    return t;
}

static ValueTree createRandomTree (int& counter, int depth)
{
    ValueTree t = createTree ("Item " + String (counter++));
    
    if (depth < 3)
        for (int i = 1 + Random::getSystemRandom().nextInt (7); --i >= 0;)
            t.addChild (createRandomTree (counter, depth + 1), -1, nullptr);
    
    return t;
}

static void addChildrenFromMap (ValueTree &vt, map<string, float> source_map) {
    for (auto item : source_map) {
        ValueTree childrenTree = createTree (item.first + ": " + String (item.second));
        vt.addChild(childrenTree, -1, nullptr);
        cout << item.first + ": " + String (item.second);
    }
}

static void addChildrenFromMap (ValueTree &vt, map<string, int> source_map) {
    for (auto item : source_map) {
        ValueTree childrenTree = createTree (item.first + ": " + String (item.second));
        vt.addChild(childrenTree, -1, nullptr);
        cout << item.first + ": " + String (item.second);
    }
}

static void addChildrenFromMap (ValueTree &vt, map<int, int> source_map) {
    for (auto item : source_map) {
        ValueTree childrenTree = createTree (String (item.first) + ": " + String (item.second));
        vt.addChild(childrenTree, -1, nullptr);
        cout << item.first << ": " << String (item.second) << "\n";
    }
}

static ValueTree createRootValueTree()
{
    ValueTree vt = createTree ("Settings");
    return vt;
}

static ValueTree createRootValueTree(json* settings_json_ptr)
    {
    ValueTree vt = createTree ("Settings");
    json root = *settings_json_ptr;

    for (json::iterator it = root.begin(); it != root.end(); ++it) {
        std::cout << it.key() << " : " << it.value() << "\n\n\n\n";
        
    }
    
    return vt;
}
/*
static ValueTree createRootValueTree(json* settings_json_ptr)
{
    PLANT = "P";
    HERBIVORE = "H";
    CARNIVORE = "C";

    ENERGY_COST = {
        {"to have the capability of moving", 0.5f},
        {"to move", 5.0f},
        {"to have the capability of hunting", 1.0f},
        {"to hunt", 10.0f},
        {"to have the capability of procreating", 0.0f},
        {"to procreate", 10.0f},
    };
    
    MINIMUM_ENERGY_REQUIRED_TO = {
        {"move", 100.0f},
        {"hunt", 100.0f},
        {"procreate", 100.0f},
    };
    
    PHOTOSYNTHESIS_CAPACITY = {
        {PLANT, 10.0f},
        {HERBIVORE, 0.0f},
        {CARNIVORE, 0.0f}
    };
    
    // Definition of gens grouped by species
    MAX_LIFESPAN = {
        {PLANT, 30},
        {HERBIVORE, 50},
        {CARNIVORE, 100}
    };
    
    PROCREATION_PROBABILITY = {
        {PLANT, 0.7f},
        {HERBIVORE, 0.2f},
        {CARNIVORE, 0.05f}
    };
    
    INITIAL_NUM_OF_ORGANISMS = {
        {PLANT, 30},
        {HERBIVORE, 30},
        {CARNIVORE, 30}
    };
    
    BIOTOPE_SETTINGS = {
        {"size_x", 200},
        {"size_y", 200}
    };

    ValueTree energy_costs_t = createTree("Energy costs");
    ValueTree minimum_energy_required_to_t = createTree("Minimum energy required to");
    ValueTree photosynthesis_capacity_t = createTree("Photosynthesis capacity");
    ValueTree max_lifespan_t = createTree("Max lifespan");
    ValueTree procreation_probability_t = createTree("Procreation probability");
    ValueTree initial_num_of_organisms_t = createTree("Initial number of organisms");
    ValueTree biotope_settings_t = createTree("Biotope settings");
    ValueTree organisms_t = createTree ("Organisms settings");
    ValueTree constraints_t = createTree ("Constraints");
    ValueTree costs_t = createTree ("Costs");
    ValueTree vt = createTree ("Settings");
    
    addChildrenFromMap (energy_costs_t, ENERGY_COST);
    addChildrenFromMap (minimum_energy_required_to_t, MINIMUM_ENERGY_REQUIRED_TO);
    addChildrenFromMap (photosynthesis_capacity_t, PHOTOSYNTHESIS_CAPACITY);
    addChildrenFromMap (max_lifespan_t, MAX_LIFESPAN);
    addChildrenFromMap (procreation_probability_t, PROCREATION_PROBABILITY);
    addChildrenFromMap (initial_num_of_organisms_t, INITIAL_NUM_OF_ORGANISMS);
    addChildrenFromMap (biotope_settings_t, BIOTOPE_SETTINGS);
 
    costs_t.addChild(       energy_costs_t,                 -1, nullptr);
    constraints_t.addChild( minimum_energy_required_to_t,   -1, nullptr);
    organisms_t.addChild(   photosynthesis_capacity_t,      -1, nullptr);
    organisms_t.addChild(   max_lifespan_t,                 -1, nullptr);
    organisms_t.addChild(   procreation_probability_t,      -1, nullptr);
    organisms_t.addChild(   initial_num_of_organisms_t,     -1, nullptr);
    
    /*   ¿Cómo se hace esto? Así da error:
    biotope_settings_t.setOpen( false );
    organisms_t.setOpen( false );
    constraints_t.setOpen( false );
    costs_t.setOpen( false );
    /
    
    vt.addChild (biotope_settings_t, -1, nullptr);
    vt.addChild (organisms_t, -1, nullptr);
    vt.addChild (constraints_t, -1, nullptr);
    vt.addChild (costs_t, -1, nullptr);
        
    return vt;
}
*/


class ValueTreeItem  : public TreeViewItem,
private ValueTree::Listener
{
public:
    ValueTreeItem (const ValueTree& v, SettingsComponent* parent_ec)
    : tree (v), parent_ec (parent_ec)
    {
        tree.addListener (this);
    }
    
    
    String getUniqueName() const override
    {
        return tree["name"].toString();
    }
    
    bool mightContainSubItems() override
    {
        return tree.getNumChildren() > 0;
    }
    
    void paintItem (Graphics& g, int width, int height) override
    {
        g.setColour (Colours::black);
        g.setFont (15.0f);
        if (isSelected()) {
            g.setFont (20.0f);
        }
        
        g.drawText (tree["name"].toString(),
                    4, 0, width - 4, height,
                    Justification::centredLeft, true);
    }
    
    void itemOpennessChanged (bool isNowOpen) override
    {
        if (isNowOpen && getNumSubItems() == 0)
            refreshSubItems();
        else
            clearSubItems();
    }
    
private:
    ValueTree tree;
    SettingsComponent* parent_ec;
    
    void refreshSubItems()
    {
        clearSubItems();
        
        for (int i = 0; i < tree.getNumChildren(); ++i)
            addSubItem (new ValueTreeItem (tree.getChild (i), parent_ec));
    }
    
    void valueTreePropertyChanged (ValueTree&, const Identifier&) override
    {
        repaintItem();
    }
    void itemClicked (const MouseEvent & 	e) override {
        parent_ec->changeSelectedItem(this);
    }
    void valueTreeChildAdded (ValueTree& parentTree, ValueTree&) override         { treeChildrenChanged (parentTree); }
    void valueTreeChildRemoved (ValueTree& parentTree, ValueTree&, int) override  { treeChildrenChanged (parentTree); }
    void valueTreeChildOrderChanged (ValueTree& parentTree, int, int) override    { treeChildrenChanged (parentTree); }
    void valueTreeParentChanged (ValueTree&) override {}
    
    void treeChildrenChanged (const ValueTree& parentTree)
    {
        if (parentTree == tree)
        {
            refreshSubItems();
            treeHasChanged();
            setOpen (true);
        }
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ValueTreeItem)
};



/** @brief SettingsComponent constructor
 *
 * @param[in] parent_component Parent component of this one
 */
SettingsComponent::SettingsComponent(MainContentComponent* _parent_component) {
    parent_component = _parent_component;
    //settings_json = parent_component->experiment_interface->getSettings_json_ptr();
    setOpaque (true);
    addAndMakeVisible(_tv);
    _tv.setDefaultOpenness (true);
    _tv.setMultiSelectEnabled (true);
    _tv.setRootItem (rootItem = new ValueTreeItem (createRootValueTree(), this));
    _tv.setColour (TreeView::backgroundColourId, Colours::white);
}

/** @brief Override callback to paint component
 */
void SettingsComponent::paint (Graphics& g)
{
    g.fillAll(Colour::fromFloatRGBA(0.7f, 0.777f, 0.517f, 1.0f));
}

/** @brief Override callback for resizing
 */

void SettingsComponent::resized() {
    Rectangle<int> r (getLocalBounds().reduced (100));
    _tv.setBounds (r);
}

/** @brief Refreshing Experimente size
 */
// void SettingsComponent::refreshExperimentSize() { };

/** @brief Destructor:
 */
SettingsComponent::~SettingsComponent() {};

/** @brief Change selected item to selectedItem
 */
void SettingsComponent::changeSelectedItem(ValueTreeItem* selectedItem) {
    this->selectedItem = selectedItem;
    std::cout << selectedItem->getUniqueName() << std::endl;
    focusGained(focusChangedDirectly);
}

void SettingsComponent::focusGained (FocusChangeType cause) {
    json* settings_json_ptr = parent_component->experiment_interface->getSettings_json_ptr();
    _tv.setRootItem (rootItem = new ValueTreeItem (createRootValueTree(settings_json_ptr), this));
    cout << "Focus gained";
}

