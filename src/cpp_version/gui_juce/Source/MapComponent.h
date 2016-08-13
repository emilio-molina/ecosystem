/** @file MapComponent.h
 * @brief Header of MapComponent
 *
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
    float position[3];  // position of the vertex
    float normal[3];    // normal vector of the vertex
    float colour[4];    // color (rgb + alpha)
    float texCoord[2];  // coordinate of texture
};

// Documentation in MapComponent.cpp
void ecosystemToVertices(Ecosystem* ecosystem, Array<Vertex> &vertices, Array<int> &indices);


/** @brief Component able to render ecosystem using OpenGL
 *
 */
class MapComponent   : public OpenGLAppComponent, public KeyListener, public SliderListener, public ButtonListener, private Timer
{
public:
    /** @brief Pointer to parent MainContentComponent
     */
    MainContentComponent* parent_component;

    // Public methods (documentation in MapComponent.cpp)
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
    void sliderValueChanged(Slider* s) override;
    void mouseDown (const MouseEvent& e) override;
    void auxRender1();
    void auxRender2();
    void auxRender3();
    void setMaxTime(int max_time);
    void setRunningTime(int time);
private:
    /** @brief True when ecosystem is running
     */
    bool _running;
    
    /** @brief True when history-view mode is chosen
     */
    bool _historyView;
    
    /** @brief True when auto forward (only valid in history view mode)
     */
    bool _autoForward;
    
    /** @brief Current time in history-view mode
     */
    int _timeHistory;
    
    /** @brief Max time available in history-view mode
     */
    int _max_time;

    // GUI elements
    ToggleButton _autoForwardToggle;
    ToggleButton _historyToggle;
    ToggleButton _runToggle;
    TextButton _loadButton;
    Label _ecosystemInfoLabel;
    Slider _timeSlider;
    
    // OpenGL variables:
    Array<Vertex> vertices;
    Array<int> indices;
    GLuint vertexBuffer, indexBuffer;
    const char* vertexShader;
    const char* fragmentShader;
    ScopedPointer<OpenGLShaderProgram> shader;
    ScopedPointer<OpenGLShaderProgram::Attribute> position, normal, sourceColour, textureCoordIn;
    String newVertexShader, newFragmentShader;
    
    // Private methods (documentation in MapComponent.cpp)
    void _toggleAutoForward();
    void timerCallback() override;
    void _increaseTimeHistory(int n);
};



#endif  // MapComponent_H_INCLUDED
