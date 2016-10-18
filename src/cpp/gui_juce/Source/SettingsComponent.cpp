//
//  SettingsComponent.cpp
//  gui_juce
//
//  Created by Juan Luis Salas García on 12/9/16.
//
//

#include <typeinfo>
//#include <stdio.h>
#include "SettingsComponent.h"

ValueTree createTree (const String& desc)
{
    ValueTree t ("Item");
    t.setProperty ("name", desc, nullptr);
    return t;
}
/*
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
*/
static ValueTree createTreeFromJSON(String root_name, json* info_json)
{
    ValueTree vt = createTree (root_name);
    for (json::iterator it = info_json->begin(); it != info_json->end(); ++it) {
        if (it.value().size() > 1) {
            stringstream item_name;
            item_name << "big " << &it << ": " << it.value();
            //String child_tree_name = String ("it.key()");
            String child_tree_name = item_name.str();
            ValueTree child_tree = createTreeFromJSON( child_tree_name, &it.value() );
            vt.addChild(child_tree, -1, nullptr);
        }
        else {
            stringstream item_name;
            //item_name << "it.key()" << ": " << it.value();
            item_name << &it << ": " << it.value();
            ValueTree child_tree = createTree (item_name.str());
            vt.addChild(child_tree, -1, nullptr);
        }
        
    }
    return vt;
}


class ValueTreeItem  : public TreeViewItem,
private ValueTree::Listener
{
public:
    ValueTreeItem (const ValueTree& v, SettingsComponent* parent_component)
    : tree (v), parent_component (parent_component)
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
    SettingsComponent* parent_component;
    
    void refreshSubItems()
    {
        clearSubItems();
        
        for (int i = 0; i < tree.getNumChildren(); ++i)
            addSubItem (new ValueTreeItem (tree.getChild (i), parent_component));
    }
    
    void valueTreePropertyChanged (ValueTree&, const Identifier&) override
    {
        repaintItem();
    }
    void itemClicked (const MouseEvent & 	e) override {
        parent_component->changeSelectedItem(this);
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
    if (parent_component == nullptr)
    setOpaque (true);
    addAndMakeVisible(_tv);
    _tv.setDefaultOpenness (true);
    _tv.setMultiSelectEnabled (true);
    _tv.setRootItem (rootItem = new ValueTreeItem (createTree("Settings"), this));
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
}

void SettingsComponent::updateTree() {
    json* settings_json_ptr = parent_component->experiment_interface->getSettings_json_ptr();
    _tv.setRootItem (rootItem = new ValueTreeItem (createTreeFromJSON("Settings", settings_json_ptr), this));
    cout << "Focus gained";
}

