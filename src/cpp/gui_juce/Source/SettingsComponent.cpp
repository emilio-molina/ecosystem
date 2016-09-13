//
//  SettingsComponent.cpp
//  gui_juce
//
//  Created by Juan Luis Salas García on 12/9/16.
//
//

#include <stdio.h>
#include "SettingsComponent.h"


/** @brief SettingsComponent constructor
 *
 * @param[in] parent_component Parent component of this one
 */
SettingsComponent::SettingsComponent(MainContentComponent* parent_component) {
    this->parent_component = parent_component;
    setOpaque (true);
}

/** @brief Override callback to paint component
 */
void SettingsComponent::paint (Graphics& g)
{
    g.fillAll(Colour::fromFloatRGBA(0.7f, 0.777f, 0.517f, 1.0f));
}

/** @brief Override callback for resizing
 */

void SettingsComponent::resized() {}

/** @brief Callback when a button is clicked
 */
void SettingsComponent::buttonClicked(Button* b) {}