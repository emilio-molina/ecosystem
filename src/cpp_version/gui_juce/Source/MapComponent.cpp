/*
  ==============================================================================

    MapComponent.cpp
    Created: 16 May 2016 6:46:10pm
    Author:  Emilio Molina

  ==============================================================================
*/

#include "MapComponent.h"


void ecosystemToVertices(Ecosystem* ecosystem, Array<Vertex> &vertices, Array<int> &indices) {
    vertices.clear();
    indices.clear();
    Vertex v1;
    int vertex_counter = 0;
    for (auto o:ecosystem->biotope) {
        int x_size = ecosystem->biotope_size_x;
        int y_size = ecosystem->biotope_size_y;
        
        tuple<int, int> position = o.first;
        string ORGANISM_TYPE = o.second->species;
        float x = 2 * (float)get<0>(position) / (float)x_size - 1.0f;
        float y = 2 * (float)get<1>(position) / (float)y_size - 1.0f;
        if (ORGANISM_TYPE == "P") {
            Vertex v2 =
            {
                {x, y, 1.0f},  // x, y, z coordinates
                { 0.5f, 0.5f, 0.5f},
                { 0.0f, 1.0f, 0.0f, 1.0f },  // green
                { 0.5f, 0.5f,}
            };
            v1 = v2;
        }
        if (ORGANISM_TYPE == "H") {
            Vertex v2 =
            {
                {x, y, 1.0f},
                { 0.5f, 0.5f, 0.5f},
                { 0.5f, 0.5f, 0.5f, 1.0f },  // grey
                { 0.5f, 0.5f,}
            };
            v1 = v2;
        }
        if (ORGANISM_TYPE == "C") {
            Vertex v2 =
            {
                {x, y, 1.0f},
                { 0.5f, 0.5f, 0.5f},
                { 1.0f, 0.0f, 0.0f, 1.0f },  // red
                { 0.5f, 0.5f,}
            };
            v1 = v2;
        }
        indices.add(vertex_counter);
        vertices.add(v1);
        vertex_counter += 1;
    }
}


void jsonToVertices(string jsonPath, Array<Vertex> &vertices, Array<int> &indices) {
    ifstream f_data_json;
    f_data_json.open(jsonPath);
    json data_json;
    f_data_json >> data_json;
    f_data_json.close();
    Ecosystem* ecosystem = new Ecosystem(data_json);
    ecosystemToVertices(ecosystem, vertices, indices);
}
    
    
bool MapComponent::keyPressed(const KeyPress &key, Component *originatingComponent) {
    if (key == KeyPress::rightKey)
        time += 1;
    if (key == KeyPress::leftKey)
        time -= 1;
    if (key == KeyPress::spaceKey)
        _running = !_running;
    return true;
}

//==============================================================================
MapComponent::MapComponent(MainContentComponent* parent_component)
{
    this->parent_component = parent_component;
    this->time = -1;
    this->_running = false;
    position = nullptr;
    normal = nullptr;
    textureCoordIn = nullptr;
    sourceColour = nullptr;
    addKeyListener(this);
    setWantsKeyboardFocus(true);
    addAndMakeVisible(_timeSlider);
    addAndMakeVisible(_historyToggle);
    addAndMakeVisible(_runToggle);
    addAndMakeVisible(_autoForwardToggle);
    addAndMakeVisible(_loadButton);
    _timeSlider.setRange(0, 10000, 1);
    _timeSlider.setVelocityBasedMode(true);
    _timeSlider.addListener(this);
    _timeSlider.setEnabled(false);

    _historyToggle.setButtonText("View history");
    _historyToggle.setColour(ToggleButton::textColourId, Colours::white);
    _historyToggle.addListener(this);
    
    _runToggle.setButtonText("Run");
    _runToggle.setColour(ToggleButton::textColourId, Colours::white);
    _runToggle.addListener(this);
    
    _autoForwardToggle.setButtonText("Auto-forward");
    _autoForwardToggle.setColour(ToggleButton::textColourId, Colours::white);
    _autoForwardToggle.setEnabled(false);
    
    _loadButton.setButtonText("Load");
    _loadButton.setEnabled(false);
    _historyView = false;
    _timeHistory = 0;
    
}

MapComponent::~MapComponent()
{
    shutdownOpenGL();
}

/** @brief OpenGL initialization function called only once
 */
void MapComponent::initialise()
{
    createShaders();
}

void MapComponent::shutdown()
{
    shader = nullptr;
}


/** @brief Needed code for render function
 *
 */
void MapComponent::auxRender1() {
    // Stuff to be done before defining your triangles
    jassert (OpenGLHelpers::isContextActive());
    const float desktopScale = (float) openGLContext.getRenderingScale();
    OpenGLHelpers::clear (Colour::greyLevel (0.05f));
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glViewport (0, 0, roundToInt (desktopScale * getWidth()), roundToInt (desktopScale * getHeight()));
    shader->use();
    openGLContext.extensions.glGenBuffers (1, &vertexBuffer);
    openGLContext.extensions.glBindBuffer (GL_ARRAY_BUFFER, vertexBuffer);
}


/** @brief Needed code for render function
 *
 */
void MapComponent::auxRender2() {
    // ************************************************
    
    // Now prepare this information to be drawn
    openGLContext.extensions.glBufferData (GL_ARRAY_BUFFER,
                                           static_cast<GLsizeiptr> (static_cast<size_t> (vertices.size()) * sizeof (Vertex)),
                                           vertices.getRawDataPointer(), GL_DYNAMIC_DRAW);
    
    openGLContext.extensions.glGenBuffers (1, &indexBuffer);
    openGLContext.extensions.glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    openGLContext.extensions.glBufferData (GL_ELEMENT_ARRAY_BUFFER,
                                           static_cast<GLsizeiptr> (static_cast<size_t> (indices.size()) * sizeof (juce::uint32)),
                                           indices.getRawDataPointer(), GL_DYNAMIC_DRAW);
    openGLContext.extensions.glBindBuffer (GL_ARRAY_BUFFER, vertexBuffer);
    openGLContext.extensions.glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    
    if (position != nullptr)
    {
        openGLContext.extensions.glVertexAttribPointer (position->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), 0);
        openGLContext.extensions.glEnableVertexAttribArray (position->attributeID);
    }
    
    if (normal != nullptr)
    {
        openGLContext.extensions.glVertexAttribPointer (normal->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 3));
        openGLContext.extensions.glEnableVertexAttribArray (normal->attributeID);
    }
    
    if (sourceColour != nullptr)
    {
        openGLContext.extensions.glVertexAttribPointer (sourceColour->attributeID, 4, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 6));
        openGLContext.extensions.glEnableVertexAttribArray (sourceColour->attributeID);
    }
    
    if (textureCoordIn != nullptr)
    {
        openGLContext.extensions.glVertexAttribPointer (textureCoordIn->attributeID, 2, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 10));
        openGLContext.extensions.glEnableVertexAttribArray (textureCoordIn->attributeID);
    }
}


/** @brief Needed code for render function
 *
 */
void MapComponent::auxRender3() {
    if (position != nullptr)       openGLContext.extensions.glDisableVertexAttribArray (position->attributeID);
    if (normal != nullptr)         openGLContext.extensions.glDisableVertexAttribArray (normal->attributeID);
    if (sourceColour != nullptr)   openGLContext.extensions.glDisableVertexAttribArray (sourceColour->attributeID);
    if (textureCoordIn != nullptr)  openGLContext.extensions.glDisableVertexAttribArray (textureCoordIn->attributeID);
    
    // Reset the element buffers so child Components draw correctly
    openGLContext.extensions.glBindBuffer (GL_ARRAY_BUFFER, 0);
    openGLContext.extensions.glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
    
    openGLContext.extensions.glDeleteBuffers (1, &vertexBuffer);
    openGLContext.extensions.glDeleteBuffers (1, &indexBuffer);
    repaint();
}


/** @brief OpenGL render function continuously called (25 fps)
 */
void MapComponent::render()
{
    auxRender1();
    ExperimentInterface* ei = parent_component->experiment_interface;
    if ((ei != nullptr) && parent_component->experiment_has_changed) {
        if (_historyView) {
            string experimentFolder = ei->getExperimentFolder();
            ExperimentInterface* einterface = new ExperimentInterface(experimentFolder, false);
            einterface->loadEcosystem(_timeHistory);
            Ecosystem* ecosystem = einterface->getEcosystemPointer();
            ecosystemToVertices(ecosystem, vertices, indices);
            delete einterface;
        } else {
            if (ei->tryLockEcosystem()) {
                Ecosystem* ecosystem = ei->getEcosystemPointer();
                ecosystemToVertices(ecosystem, vertices, indices);
                ei->unlockEcosystem();
            }
        }
        parent_component->experiment_has_changed = false;
    }
    auxRender2();
    glPointSize(4.0);
    glDrawElements (GL_POINTS, indices.size(), GL_UNSIGNED_INT, 0);  // Draw points
    //glDrawElements (GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);  // Draw triangles
    auxRender3();
}

/** @brief Paint function continuously called to fraw non-OpenGL graphics
 */
void MapComponent::paint (Graphics& g)
{
    // You can add your component specific drawing code here!
    // This will draw over the top of the openGL background.
    int percentage_x = getWidth();
    int percentage_y = getHeight();
    g.setColour(Colours::darkred);
    g.fillRect(0 * percentage_x, 0 * percentage_y,
               1.0 * percentage_x, 0.04 * percentage_y);
    //g.setFont (20);
    //g.drawText ("Ecosystem map", 25, 20, 300, 30, Justification::left);
}

void MapComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    int percentage_x = getWidth();
    int percentage_y = getHeight();
    _runToggle.setBounds (0 * percentage_x, 0 * percentage_y,       // x, y
                          0.1 * percentage_x, 0.03 * percentage_y);      // width, height
    _historyToggle.setBounds (0.50 * percentage_x, 0 * percentage_y,       // x, y
                              0.10 * percentage_x, 0.03 * percentage_y);      // width, height
    _timeSlider.setBounds (0.60 * percentage_x, 0 * percentage_y,       // x, y
                           0.25 * percentage_x, 0.03 * percentage_y);      // width, height
    _autoForwardToggle.setBounds (0.85 * percentage_x, 0 * percentage_y,       // x, y
                          0.10 * percentage_x, 0.03 * percentage_y);      // width, height
    _loadButton.setBounds (0.95 * percentage_x, 0 * percentage_y,       // x, y
                           0.05 * percentage_x, 0.03 * percentage_y);      // width, height
}

/** @brief Create OpenGL shaders
 *
 * Vertex shader: Graphic-card routine determining how vertices modify their
 *                coordinates, as a function of camera position, etc.
 *
 * Fragment shader: Graphic-card routine determining how pixels are colored
 *                  depending on their position, colors, texture, light, etc.
 */
void MapComponent::createShaders()
{
    // Here we define the shaders use to draw our triangle. They are very simple.
    
    vertexShader =
    "attribute vec4 position;\n"
    "attribute vec4 sourceColour;\n"
    "attribute vec2 textureCoordIn;\n"
    "\n"
    "varying vec4 destinationColour;\n"
    "varying vec2 textureCoordOut;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    destinationColour = sourceColour;\n"
    "    textureCoordOut = textureCoordIn;\n"
    "    gl_Position = position;\n"
    "}\n";
    
    fragmentShader =
#if JUCE_OPENGL_ES
    "varying lowp vec4 destinationColour;\n"
    "varying lowp vec2 textureCoordOut;\n"
#else
    "varying vec4 destinationColour;\n"
    "varying vec2 textureCoordOut;\n"
#endif
    "\n"
    "uniform sampler2D demoTexture;\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor = destinationColour; \n"
    "}\n";
    
    ScopedPointer<OpenGLShaderProgram> newShader (new OpenGLShaderProgram (openGLContext));
    String statusText;
    
    if (newShader->addVertexShader (OpenGLHelpers::translateVertexShaderToV3 (vertexShader))
        && newShader->addFragmentShader (OpenGLHelpers::translateFragmentShaderToV3 (fragmentShader))
        && newShader->link())
    {
        shader = newShader;
        shader->use();
        
        if (openGLContext.extensions.glGetAttribLocation (shader->getProgramID(), "position") < 0)
            position      = nullptr;
        else
            position      = new OpenGLShaderProgram::Attribute (*shader,    "position");
        
        if (openGLContext.extensions.glGetAttribLocation (shader->getProgramID(), "sourceColour") < 0)
            sourceColour      = nullptr;
        else
            sourceColour  = new OpenGLShaderProgram::Attribute (*shader,    "sourceColour");
        if (openGLContext.extensions.glGetAttribLocation (shader->getProgramID(), "normal") < 0)
            normal      = nullptr;
        else
            normal      = new OpenGLShaderProgram::Attribute (*shader,    "normal");
        if (openGLContext.extensions.glGetAttribLocation (shader->getProgramID(), "textureCoordIn") < 0)
            textureCoordIn      = nullptr;
        else
            textureCoordIn      = new OpenGLShaderProgram::Attribute (*shader,    "textureCoordIn");
        
        statusText = "GLSL: v" + String (OpenGLShaderProgram::getLanguageVersion(), 2);
    }
    else
    {
        statusText = newShader->getLastError();
    }
}

void MapComponent::sliderValueChanged(Slider* s) {
    if (s == &_timeSlider) {
        _timeHistory = _timeSlider.getValue();
    }
}

void MapComponent::sliderDragEnded(Slider* s) {
}

void MapComponent::mouseDown (const MouseEvent& e)
{
    cout << e.getPosition().getX() << "  " << e.getPosition().getY() << endl;
}

void MapComponent::buttonClicked (Button* b) {
    /* Run button
     * ==========
     *
     * Indicate the ecosystem must run
     *
     */
    if (b == &_runToggle) {
        if (_runToggle.getToggleState())
            this->parent_component->running = true;
        else
            this->parent_component->running = false;
    }
    
    if (b == &_historyToggle) {
        bool enable = _historyToggle.getToggleState();
        _timeSlider.setEnabled(enable);
        _autoForwardToggle.setEnabled(enable);
        _loadButton.setEnabled(enable);
        _historyView = enable;
        parent_component->experiment_has_changed = true;
    }
}