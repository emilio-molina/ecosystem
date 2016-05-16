/*
  ==============================================================================

    MapComponent.h
    Created: 16 May 2016 6:46:10pm
    Author:  Emilio Molina

  ==============================================================================
*/

#ifndef MapComponent_H_INCLUDED
#define MapComponent_H_INCLUDED
#include "../JuceLibraryCode/JuceHeader.h"
#include "MainComponent.h"
#include "ecosystem.h"
#include <boost/filesystem.hpp>

namespace bf=boost::filesystem;
using json = nlohmann::json;

/** @brief Struct storing information about one OpenGL vertex
 */
struct Vertex
{
    float position[3];
    float normal[3];
    float colour[4];
    float texCoord[2];
};


/** @brief Component able to render ecosystem using OpenGL
 *
 * It is a simplified version of OpenGL example provided with JUCE
 */
class MapComponent   : public OpenGLAppComponent, public KeyListener
{
public:
    Ecosystem* ecosystem;
    MainContentComponent* parent_component;
    //==============================================================================
    MapComponent(Ecosystem& ecosystem, MainContentComponent& parent_component);
    ~MapComponent();
    void initialise() override;
    void shutdown() override;
    void render() override;
    void paint (Graphics& g) override;
    void resized() override;
    void createShaders();
    bool keyPressed(const KeyPress &key, Component *originatingComponent) override;
    
private:
    int time;
    bool _playing;
    Array<Vertex> vertices;
    Array<int> indices;
    GLuint vertexBuffer, indexBuffer;
    
    const char* vertexShader;
    const char* fragmentShader;
    
    ScopedPointer<OpenGLShaderProgram> shader;
    ScopedPointer<OpenGLShaderProgram::Attribute> position, normal, sourceColour, textureCoordIn;
    
    String newVertexShader, newFragmentShader;
};



#endif  // MapComponent_H_INCLUDED
