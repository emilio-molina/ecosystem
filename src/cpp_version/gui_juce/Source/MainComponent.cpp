#include "MainComponent.h"


CriticalSection mtx;


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
class mapComponent   : public OpenGLAppComponent
{
public:
    Ecosystem* ecosystem;
    //==============================================================================
    mapComponent(Ecosystem& ecosystem)
    {
        this->time = -1; 
        position = nullptr;
        normal = nullptr;
        textureCoordIn = nullptr;
        sourceColour = nullptr;
        this->ecosystem = &ecosystem;
    }

    ~mapComponent()
    {
        shutdownOpenGL();
    }

    /** @brief OpenGL initialization function called only once
    */
    void initialise() override
    {
        createShaders();
    }

    void shutdown() override
    {
        shader = nullptr;
    }

    /** @brief OpenGL render function continuously called (25 fps)
    */
    void render() override
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
        if ((time != ecosystem->time) && (mtx.tryEnter())) {
            vertices.clear();
            indices.clear();
            Vertex v1;
            for (auto o:ecosystem->biotope) {
                int x_size = ecosystem->biotope_size_x;
                int y_size = ecosystem->biotope_size_y;
                tuple<int, int> position = o.first;
                species_t ORGANISM_TYPE = o.second->species;
                float x = 2 * (float)get<0>(position) / (float)x_size - 1.0f;
                float y = 2 * (float)get<1>(position) / (float)y_size - 1.0f;
                if (ORGANISM_TYPE == PLANT) {
                    Vertex v2 =
                    {
                        {x, y, 1.0f},
                        { 0.5f, 0.5f, 0.5f},
                        { 0.0f, 1.0f, 0.0f, 1.0f },  // green
                        { 0.5f, 0.5f,}
                    };
                    v1 = v2;
                }
                if (ORGANISM_TYPE == HERBIVORE) {
                    Vertex v2 =
                    {
                        {x, y, 1.0f},
                        { 0.5f, 0.5f, 0.5f},
                        { 0.5f, 0.5f, 0.5f, 1.0f },  // grey
                        { 0.5f, 0.5f,}
                    };
                    v1 = v2;
                }
                if (ORGANISM_TYPE == CARNIVORE) {
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
        ecosystem->rendered = true;
        mtx.exit();
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
        
    }

    /** @brief Paint function continuously called to fraw non-OpenGL graphics
    */
    void paint (Graphics& g) override
    {
        // You can add your component specific drawing code here!
        // This will draw over the top of the openGL background.
        
        g.setColour(Colours::white);
        g.setFont (20);
        g.drawText ("Ecosystem map", 25, 20, 300, 30, Justification::left);
    }
    
    void resized() override
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
    void createShaders()
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
    
    
private:
    int time;
    Array<Vertex> vertices;
    Array<int> indices;
    GLuint vertexBuffer, indexBuffer;
    
    const char* vertexShader;
    const char* fragmentShader;
    
    ScopedPointer<OpenGLShaderProgram> shader;
    ScopedPointer<OpenGLShaderProgram::Attribute> position, normal, sourceColour, textureCoordIn;
    
    String newVertexShader, newFragmentShader;
};


/** @brief Component of dummy sliders taken from JUCE Demo
*/
struct SlidersPage  : public Component
{
    SlidersPage()
    : hintLabel ("hint", "Try right-clicking on a slider for an options menu. \n\n"
                 "Also, holding down CTRL while dragging will turn on a slider's velocity-sensitive mode")
    {
        Slider* s = createSlider (false);
        s->setSliderStyle (Slider::LinearVertical);
        s->setTextBoxStyle (Slider::TextBoxBelow, false, 100, 20);
        s->setBounds (10, 25, 70, 200);
        s->setDoubleClickReturnValue (true, 50.0); // double-clicking this slider will set it to 50.0
        s->setTextValueSuffix (" units");
        
        s = createSlider (false);
        s->setSliderStyle (Slider::LinearVertical);
        s->setVelocityBasedMode (true);
        s->setSkewFactor (0.5);
        s->setTextBoxStyle (Slider::TextBoxAbove, true, 100, 20);
        s->setBounds (85, 25, 70, 200);
        s->setTextValueSuffix (" rels");
        
        s = createSlider (true);
        s->setSliderStyle (Slider::LinearHorizontal);
        s->setTextBoxStyle (Slider::TextBoxLeft, false, 80, 20);
        s->setBounds (180, 35, 150, 20);
        
        s = createSlider (false);
        s->setSliderStyle (Slider::LinearHorizontal);
        s->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
        s->setBounds (180, 65, 150, 20);
        s->setPopupDisplayEnabled (true, this);
        s->setTextValueSuffix (" nuns required to change a lightbulb");
        
        s = createSlider (false);
        s->setSliderStyle (Slider::IncDecButtons);
        s->setTextBoxStyle (Slider::TextBoxLeft, false, 50, 20);
        s->setBounds (180, 105, 100, 20);
        s->setIncDecButtonsMode (Slider::incDecButtonsDraggable_Vertical);
        
        s = createSlider (false);
        s->setSliderStyle (Slider::Rotary);
        s->setRotaryParameters (float_Pi * 1.2f, float_Pi * 2.8f, false);
        s->setTextBoxStyle (Slider::TextBoxRight, false, 70, 20);
        s->setBounds (190, 145, 120, 40);
        s->setTextValueSuffix (" mm");
        
        s = createSlider (false);
        s->setSliderStyle (Slider::LinearBar);
        s->setBounds (180, 195, 100, 30);
        s->setTextValueSuffix (" gallons");
        
        s = createSlider (false);
        s->setSliderStyle (Slider::TwoValueHorizontal);
        s->setBounds (360, 20, 160, 40);
        
        s = createSlider (false);
        s->setSliderStyle (Slider::TwoValueVertical);
        s->setBounds (360, 110, 40, 160);
        
        s = createSlider (false);
        s->setSliderStyle (Slider::ThreeValueHorizontal);
        s->setBounds (360, 70, 160, 40);
        
        s = createSlider (false);
        s->setSliderStyle (Slider::ThreeValueVertical);
        s->setBounds (440, 110, 40, 160);
        
        s = createSlider (false);
        s->setSliderStyle (Slider::LinearBarVertical);
        s->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
        s->setBounds (540, 35, 20, 230);
        s->setPopupDisplayEnabled (true, this);
        s->setTextValueSuffix (" mickles in a muckle");
        
        for (int i = 7; i <= 10; ++i)
        {
            sliders.getUnchecked(i)->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
            sliders.getUnchecked(i)->setPopupDisplayEnabled (true, this);
        }
        
        /* Here, we'll create a Value object, and tell a bunch of our sliders to use it as their
         value source. By telling them all to share the same Value, they'll stay in sync with
         each other.
         We could also optionally keep a copy of this Value elsewhere, and by changing it,
         cause all the sliders to automatically update.
         */
        Value sharedValue;
        sharedValue = Random::getSystemRandom().nextDouble() * 100;
        for (int i = 0; i < 7; ++i)
            sliders.getUnchecked(i)->getValueObject().referTo (sharedValue);
        
        // ..and now we'll do the same for all our min/max slider values..
        Value sharedValueMin, sharedValueMax;
        sharedValueMin = Random::getSystemRandom().nextDouble() * 40.0;
        sharedValueMax = Random::getSystemRandom().nextDouble() * 40.0 + 60.0;
        
        for (int i = 7; i <= 10; ++i)
        {
            sliders.getUnchecked(i)->getMaxValueObject().referTo (sharedValueMax);
            sliders.getUnchecked(i)->getMinValueObject().referTo (sharedValueMin);
        }
        
        hintLabel.setBounds (20, 245, 350, 150);
        addAndMakeVisible (hintLabel);
    }
    
private:
    OwnedArray<Slider> sliders;
    Label hintLabel;
    
    Slider* createSlider (bool isSnapping)
    {
        Slider* s = new Slider();
        sliders.add (s);
        addAndMakeVisible (s);
        s->setRange (0.0, 100.0, 0.1);
        s->setPopupMenuEnabled (true);
        s->setValue (Random::getSystemRandom().nextDouble() * 100.0, dontSendNotification);
        return s;
    }
};


//==============================================================================
MainContentComponent::MainContentComponent()
{
    setSize (800, 600);
    // Create tabs and add components to each tab
    _tabbedComponent = new TabbedComponent(TabbedButtonBar::TabsAtTop);
    _tabbedComponent->addTab("Map", Colour::fromFloatRGBA(0.0f, 0.077f, 0.217f, 1.0f), new mapComponent(ecosystem), true);
    _tabbedComponent->addTab("Controls", Colour::fromFloatRGBA(0.8f, 0.677f, 0.617f, 1.0f), new SlidersPage(), true);
    addAndMakeVisible(_tabbedComponent);
    Ecosystem ecosystem = Ecosystem();
    startTimer(100);  // call timer callback every 100ms
}

MainContentComponent::~MainContentComponent()
{
}

void MainContentComponent::paint (Graphics& g)
{
    g.fillAll (Colour::greyLevel(0.1f));
    _tabbedComponent->setBounds(0, 0, getWidth(), getHeight());
    
}

void MainContentComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}

void MainContentComponent::timerCallback() {
if (ecosystem.rendered) {
    mtx.enter();
    ecosystem.evolve();
    ecosystem.rendered = false;
    mtx.exit();
    auto num_organisms = ecosystem.biotope.size();
    auto num_free_locs = ecosystem.biotope_free_locs.size();
    cout << "Time: " << ecosystem.time << endl;
    cout << "    num organism: " << num_organisms << endl;
    cout << "    num free locs: " << num_free_locs << endl;
    cout << "    sum previous numbers: " << num_organisms + num_free_locs << endl;
    }
}
