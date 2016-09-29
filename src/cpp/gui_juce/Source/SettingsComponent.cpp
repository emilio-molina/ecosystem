//
//  SettingsComponent.cpp
//  gui_juce
//
//  Created by Juan Luis Salas García on 12/9/16.
//
//

#include <stdio.h>
#include "SettingsComponent.h"

static ValueTree createRandomTree (int& counter, int depth)
{
    ValueTree t = createTree ("Item " + String (counter++));
    
    if (depth < 3)
        for (int i = 1 + Random::getSystemRandom().nextInt (7); --i >= 0;)
            t.addChild (createRandomTree (counter, depth + 1), -1, nullptr);
    
    return t;
}


ValueTree createTree (const String& desc)
{
    ValueTree t ("Item");
    t.setProperty ("name", desc, nullptr);
    return t;
}

static ValueTree createRootValueTree()
{
    /* 
    //From demo:
    ValueTree vt = createTree ("This demo displays a ValueTree as a treeview.");
    vt.addChild (createTree ("You can drag around the nodes to rearrange them"), -1, nullptr);
    vt.addChild (createTree ("..and press 'delete' to delete them"), -1, nullptr);
    vt.addChild (createTree ("Then, you can use the undo/redo buttons to undo these changes"), -1, nullptr);
    */

    ValueTree vt = createTree ("Settings");
    vt.addChild (createTree ("Biotope settings"), -1, nullptr);
    vt.addChild (createTree ("Organisms settings"), -1, nullptr);
    vt.addChild (createTree ("Constraints"), -1, nullptr);
    vt.addChild (createTree ("Cost"), -1, nullptr);
    
    int n = 1;
    vt.addChild (createRandomTree (n, 0), -1, nullptr);
    
    return vt;
}

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
SettingsComponent::SettingsComponent(MainContentComponent* parent_component) {
    this->parent_component = parent_component;
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
}

