/*
  ==============================================================================

    MapComponent.cpp
    Created: 16 May 2016 6:46:10pm
    Author:  Emilio Molina

  ==============================================================================
*/

#include "MapComponent.h"

bool MapComponent::keyPressed(const KeyPress &key, Component *originatingComponent) {
    if (key == KeyPress::rightKey)
        time += 1;
    if (key == KeyPress::leftKey)
        time -= 1;
    if (key == KeyPress::spaceKey)
        _playing = !_playing;
    return true;
}

//==============================================================================
MapComponent::MapComponent(Ecosystem* ecosystem, MainContentComponent* parent_component)
{
    this->parent_component = parent_component;
    this->time = -1;
    this->_playing = false;
    position = nullptr;
    normal = nullptr;
    textureCoordIn = nullptr;
    sourceColour = nullptr;
    this->ecosystem = ecosystem;
    addKeyListener(this);
    setWantsKeyboardFocus(true);
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

/** @brief OpenGL render function continuously called (25 fps)
 */
void MapComponent::render()
{
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
    
    // ************************** TRIANGLES DEFINITION
    // Here you can draw whatever you want
    int vertex_counter = 0;
    
    // if ecosystem->time has changed, and mutex is not locked
    if ((ecosystem != nullptr) && (time != ecosystem->time) && (parent_component->mtx.tryEnter())) {
        vertices.clear();
        indices.clear();
        Vertex v1;
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
                    {x, y, 1.0f},
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
            time = ecosystem->time;
        }
        parent_component->mtx.exit();
    }
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
    
    glPointSize(4.0);
    glDrawElements (GL_POINTS, indices.size(), GL_UNSIGNED_INT, 0);  // Draw points!
    //glDrawElements (GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);  // Draw triangles!
    
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

/** @brief Paint function continuously called to fraw non-OpenGL graphics
 */
void MapComponent::paint (Graphics& g)
{
    // You can add your component specific drawing code here!
    // This will draw over the top of the openGL background.
    
    g.setColour(Colours::white);
    g.setFont (20);
    g.drawText ("Ecosystem map", 25, 20, 300, 30, Justification::left);
}

void MapComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
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