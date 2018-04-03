/** @file MapComponent.cpp
 * @brief Implementation of MapComponent
 *
 */

#include "MapComponent.h"

Colour organismToColour(Organism* o) {
    string ORGANISM_TYPE = o->species;
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 1.0f;
    if (ORGANISM_TYPE == "P") {
        // green
        r = 0.0f;
        g = 1.0f;
        b = 0.0f;
    } else if (ORGANISM_TYPE == "H1") {
        // grey
        r = 0.5f;
        g = 0.5f;
        b = 0.5f;
    } else if (ORGANISM_TYPE == "H2") {
        // blue
        r = 0.2f;
        g = 0.2f;
        b = 1.0f;
    } else if (ORGANISM_TYPE == "C1") {
        // red
        r = 1.0f;
        g = 0.0f;
        b = 0.0f;
    } else if (ORGANISM_TYPE == "C2") {
        // orange
        r = 1.0f;
        g = 0.5f;
        b = 0.0f;
    } else if (ORGANISM_TYPE == "C3") {
        // light blue
        r = 0.0f;
        g = 0.5f;
        b = 1.0f;
    }
    float energy_ratio = (float)o->energy_reserve / o->initial_energy_reserve;
    float age_ratio = 1.0f - (float)o->age / o->death_age;
    r *= 0.75 * energy_ratio * age_ratio;
    g *= 0.75 * energy_ratio * age_ratio;
    b *= 0.75 * energy_ratio * age_ratio;
    return Colour::fromFloatRGBA(r, g, b, a);
}

/** @brief Read ecosystem object and generate a OpenGL formatted list of vertices
 *
 * @param[in] ecosystem Pointer to input ecosystem object
 * @param[out] vertices List of OpenGL formatted vertices
 * @param[out] indices List of vertices indices
 */
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
        Colour color_o = organismToColour(o.second);
        Vertex v2 =
        {
            {x, y, 1.0f},  // x, y, z coordinates
            { 0.5f, 0.5f, 0.5f},
            { color_o.getFloatRed(),
                color_o.getFloatGreen(),
                color_o.getFloatBlue(),
                color_o.getFloatAlpha()},
            { 0.5f, 0.5f,}
        };
        v1 = v2;
        indices.add(vertex_counter);
        vertices.add(v1);
        vertex_counter += 1;
    }
}


/** @brief MapComponent constructor
 *
 * @param[in] parent_component Pointer to parent MainContentComponent
 */
MapComponent::MapComponent(MainContentComponent* parent_component)
{
    this->parent_component = parent_component;
    _running = false;
    position = nullptr;
    normal = nullptr;
    textureCoordIn = nullptr;
    sourceColour = nullptr;
    _historyView = false;
    _timeHistory = 0;
    _autoForward = false;
    
    addKeyListener(this);
    setWantsKeyboardFocus(true);
    addAndMakeVisible(_timeSlider);
    addAndMakeVisible(_historyToggle);
    addAndMakeVisible(_runToggle);
    addAndMakeVisible(_autoForwardToggle);
    addAndMakeVisible(_loadButton);
    addAndMakeVisible(_ecosystemInfoLabel);
    _ecosystemInfoLabel.setText("", juce::NotificationType::dontSendNotification);
    _ecosystemInfoLabel.setColour(Label::textColourId, Colours::white);
    _timeSlider.setRange(0, 1, 1);
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
    _autoForwardToggle.addListener(this);
    
    _loadButton.setButtonText("Load");
    _loadButton.setEnabled(false);
    _loadButton.addListener(this);
}

Matrix3D<float> MapComponent::getProjectionMatrix() const
{
    float w = 1.0f;
    
    float h = w * getLocalBounds().toFloat().getAspectRatio (false);
    return Matrix3D<float>::fromFrustum (-w, w, -h, h, 1.0f, 0.0f);
}

Matrix3D<float> MapComponent::getViewMatrix() const
{
    //Matrix3D<float> viewMatrix (Vector3D<float> (0.0f, -0.25f, -2.5f));
    Matrix3D<float> viewMatrix (Vector3D<float> (0.0f, 0.0f, -3.0f));
    Matrix3D<float> rotationMatrix
    //= viewMatrix.rotation (Vector3D<float> (-0.5f, 0.0f, 0.0f));
    = viewMatrix.rotation (Vector3D<float> (-0.0f, 0.0f, 0.0f));
    return rotationMatrix * viewMatrix;
}


/** @brief Override callback to control ecosystem with keyboard
 */
bool MapComponent::keyPressed(const KeyPress &key, Component *originatingComponent) {
    ExperimentInterface* ei = parent_component->experiment_interface;
    if (key == KeyPress::rightKey) {
        _increaseTimeHistory(ei->getBackupPeriod());
    }
    
    if (key == KeyPress::leftKey) {
        _increaseTimeHistory(-1 * ei->getBackupPeriod());
        
    }
    
    if (key == KeyPress::spaceKey) {
        _toggleAutoForward();
    }
    return true;
}

/** @brief MapComponent destructor
 */
MapComponent::~MapComponent()
{
    shutdownOpenGL();
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
            // Load time slot _timeHistory into einterface to render it
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

/** @brief Paint function continuously called to draw non-OpenGL graphics
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
}

/** @brief Resize function for MapComponent
 */
void MapComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    int percentage_x = getWidth();
    int percentage_y = getHeight();
    _runToggle.setBounds (0 * percentage_x, 0 * percentage_y,
                          0.1 * percentage_x, 0.03 * percentage_y);
    _ecosystemInfoLabel.setBounds (0.1 * percentage_x, 0 * percentage_y,
                          0.15 * percentage_x, 0.03 * percentage_y);
    
    _historyToggle.setBounds (0.50 * percentage_x, 0 * percentage_y,
                              0.10 * percentage_x, 0.03 * percentage_y);
    _timeSlider.setBounds (0.60 * percentage_x, 0 * percentage_y,
                           0.25 * percentage_x, 0.03 * percentage_y);
    _autoForwardToggle.setBounds (0.85 * percentage_x, 0 * percentage_y,
                          0.10 * percentage_x, 0.03 * percentage_y);
    _loadButton.setBounds (0.95 * percentage_x, 0 * percentage_y,
                           0.05 * percentage_x, 0.03 * percentage_y);
}

/** @brief Callback called when slider value is changed
 */
void MapComponent::sliderValueChanged(Slider* s) {
    if (s == &_timeSlider) {
        _timeHistory = _timeSlider.getValue();
        parent_component->experiment_has_changed = true;
    }
}

/** @brief Callback when mouse is clicked
 *
 * NOTE: Just for debug by the moment
 */
void MapComponent::mouseDown (const MouseEvent& e)
{
    cout << e.getPosition().getX() << "  " << e.getPosition().getY() << endl;
}

/** @brief Callback when a button is clicked
 */
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
        if (_autoForward)
            _toggleAutoForward();
        _timeSlider.setEnabled(enable);
        _autoForwardToggle.setEnabled(enable);
        _loadButton.setEnabled(enable);
        _historyView = enable;
        parent_component->experiment_has_changed = true;
    }
    
    if (b == &_autoForwardToggle) {
        _toggleAutoForward();
    }

    if (b == &_loadButton) {
        ExperimentInterface* ei = parent_component->experiment_interface;
        ei->loadEcosystem(_timeHistory);
        parent_component->experiment_has_changed = true;
        setRunningTime(ei->getRunningTime());
    }
}

/** @brief Timer callback for history-view mode with auto-forward
 */
void MapComponent::timerCallback() {
    ExperimentInterface* ei = parent_component->experiment_interface;
    _increaseTimeHistory(ei->getBackupPeriod());
}

/** @brief Toggle auto-forward function
 */
void MapComponent::_toggleAutoForward() {
    _autoForward = !_autoForward;
    _autoForwardToggle.setToggleState(_autoForward, NotificationType::dontSendNotification);
    if (_autoForward)
        startTimer(100);
    else
        stopTimer();
}

/** @brief Set max time for history-view mode
 *
 * @param[in] max_time Value of maximum time
 */
void MapComponent::setMaxTime(int max_time) {
    ExperimentInterface* ei = parent_component->experiment_interface;
    _timeSlider.setRange(0,
                         max(1, max_time),
                         ei->getBackupPeriod());
    _max_time = max_time;
}


/** @brief Set running time for running-view mode
 *
 * @param[in] time Value of running time
 */
void MapComponent::setRunningTime(int time) {
    _ecosystemInfoLabel.setText(to_string(time), juce::NotificationType::dontSendNotification);
}

/** @brief Increase time history
 *
 * @param[in] n Step of increase
 */
void MapComponent::_increaseTimeHistory(int n) {
    _timeHistory += n;
    _timeHistory = min(max(_timeHistory, 0), _max_time);
    _timeSlider.setValue(_timeHistory);
    parent_component->experiment_has_changed = true;
}


//=== OpenGL auxiliar functions

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
    "attribute vec2 texureCoordIn;\n"
    "\n"
    "uniform mat4 projectionMatrix;\n"
    "uniform mat4 viewMatrix;\n"
    "\n"
    "varying vec4 destinationColour;\n"
    "varying vec2 textureCoordOut;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    destinationColour = sourceColour;\n"
    "    textureCoordOut = texureCoordIn;\n"
    "    gl_Position = projectionMatrix * viewMatrix * position;\n"
    //"    gl_Position = position;\n"
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
        
        uniforms   = new Uniforms (openGLContext, *shader);
        
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

/** @brief OpenGL initialization function called only once
 */
void MapComponent::initialise()
{
    createShaders();
}

/** @brief Shutdown OpenGL
 */
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
    if (uniforms->projectionMatrix != nullptr)
        uniforms->projectionMatrix->setMatrix4 (getProjectionMatrix().mat, 1, false);
    
    if (uniforms->viewMatrix != nullptr)
        uniforms->viewMatrix->setMatrix4 (getViewMatrix().mat, 1, false);
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
