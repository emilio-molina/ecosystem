/*
  ==============================================================================

    MapComponent.h
    Created: 16 May 2016 6:46:10pm
    Author:  Emilio Molina

  ==============================================================================
*/

#ifndef MapComponent_H_INCLUDED
#define MapComponent_H_INCLUDED

#include "MainComponent.h"
#include "ExperimentComponent.h"
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


void jsonToVertices(string jsonPath, Array<Vertex> &vertices, Array<int> &indices);
void ecosystemToVertices(Ecosystem* ecosystem, Array<Vertex> &vertices, Array<int> &indices);


/** @brief Component able to render ecosystem using OpenGL
 *
 * It is a simplified version of OpenGL example provided with JUCE
 */
class MapComponent   : public OpenGLAppComponent, public KeyListener, public SliderListener, public ButtonListener, private Timer
{
public:
    MainContentComponent* parent_component;
    //==============================================================================
    MapComponent(MainContentComponent* parent_component);
    ~MapComponent();
    void initialise() override;
    void shutdown() override;
    void render() override;
    void paint (Graphics& g) override;
    void resized() override;
    void createShaders();
    bool keyPressed(const KeyPress &key, Component *originatingComponent) override;
    void buttonClicked (Button*) override;
    void sliderValueChanged(Slider* s) override; // needed to make it compile
    void sliderDragEnded(Slider* s) override;
    void mouseDown (const MouseEvent& e) override;
    void auxRender1();
    void auxRender2();
    void auxRender3();
    void setMaxTime(int max_time);
private:
    int time;
    bool _running;
    bool _historyView;
    bool _autoForward;
    int _timeHistory;
    Array<Vertex> vertices;
    Array<int> indices;
    GLuint vertexBuffer, indexBuffer;
    
    const char* vertexShader;
    const char* fragmentShader;
    
    ScopedPointer<OpenGLShaderProgram> shader;
    ScopedPointer<OpenGLShaderProgram::Attribute> position, normal, sourceColour, textureCoordIn;
    
    String newVertexShader, newFragmentShader;

    ToggleButton _autoForwardToggle;
    ToggleButton _historyToggle;
    ToggleButton _runToggle;
    TextButton _loadButton;
    Label _ecosystemInfoLabel;
    Slider _timeSlider;
    int _max_time;
    void _toggleAutoForward();
    void timerCallback() override;
    void _increaseTimeHistory(int n);
};



#endif  // MapComponent_H_INCLUDED
