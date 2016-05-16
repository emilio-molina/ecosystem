#include "MainComponent.h"
namespace bf=boost::filesystem;
using json = nlohmann::json;


CriticalSection mtx;

/** @brief Convert float / double to string with n digits of precision
*
* @param[in] a_value Float or double input value
* @param[in] n Number of decimal digits
* @returns String containing number with the desired number of decimals
*/
template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(n) << a_value;
    return out.str();
}


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
class mapComponent   : public OpenGLAppComponent, public KeyListener
{
public:
    
    bool keyPressed(const KeyPress &key, Component *originatingComponent) override {
        if (key == KeyPress::rightKey)
            time += 1;
        if (key == KeyPress::leftKey)
            time -= 1;
        if (key == KeyPress::spaceKey)
            _playing = !_playing;
        return true;
    }
    
    Ecosystem* ecosystem;
    //==============================================================================
    mapComponent(Ecosystem& ecosystem)
    {
        this->time = -1;
        this->_playing = false;
        position = nullptr;
        normal = nullptr;
        textureCoordIn = nullptr;
        sourceColour = nullptr;
        this->ecosystem = &ecosystem;
        addKeyListener(this);
        setWantsKeyboardFocus(true);
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
        repaint();
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


/** @brief Class to easily interact with experiment folder
*/
class ExperimentFolderInterface {
    
    /** @brief Initializer
    *
    * @param[in] experiment_folder String with experiment_folder path
    */
    ExperimentFolderInterface(string experiment_folder);


    /** @brief Get a list of time slices containing a complete backup of ecosystem
    *
    * @returns List of time slices allowing complete backup
    */
    vector<int> getTimesHavingCompleteBackups();
    

    /** @brief Get path of JSON containing ecosystem data
    *
    * @param[in] time_slice Time slice for which ecosystem data will be get
    *
    * @returns Path of JSON file
    */
    string getEcosystemJSONPath(int time_slice);
    
    /** @brief Get a list of time slices containing a given organism parameter
    *
    * @param[in] parameter Parameter to read from JSON files
    *
    * @returns Vector of time values
    */
    vector<int> getTimesHavingOrganismParameter(string parameter);
    
    
    /** @brief Get a list of time slices containing a given ecosystem parameter
    *
    * @param[in] parameter Parameter to read from JSON files
    *
    * @returns Vector of time values
    */
    vector<int> getTimesHavingEcosystemParameter(string parameter);
    

    /** @brief Get a list of available ecosystem parameters
     *
     * @returns Vector of parameters available
     */
    vector<string> getAvailableEcosystemParameters();

    
    /** @brief Get a list of available organism parameters
    *
    * @returns Vector of parameters available
    */
    vector<string> getAvailableOrganismParameters();
    
    /** @brief Get ecosystem data for a given time slice
    *
    * @param[in] time_slice Target time slice
    */
    json getEcosystemData(int time_slice);
    
    /** @brief Get ecosystem data for a given time slice
    *
    * @param[in] organism_id Organism ID
    * @param[in] time_slice Target time slice
    */
    json getOrganismData(int organism_id, int time_slice);
    
    
    /** @brief Get a list of avaiable living organisms ids
     *
     * @param[in] time_slice Target time slice
     */
    vector<int> getLivingOrganismsIds(int time_slice);
    
    /** @brief Get a list of avaiable dead organisms ids
     *
     * @param[in] time_slice Target time slice
     */
    vector<int> getDeadOrganismsIds(int time_slice);
    
    /* @brief Get experiment size in MBs in format e.g. "321.16MB"
     *
     * @returns String with experiment size
     */
    string getExperimentSize();
};

/** @brief Component for experiment tab
*
*/
class ExperimentComponent : public Component, public ButtonListener {
public:
    
    /** @brief ExperimentComponent constructor
     *
     * When a new instance is created, a browser to choose a directory appears.
     * Once a folder is selected, then it parse the directory contents into an Ecosystem object.
     */
    ExperimentComponent() {
        setOpaque (true);
        addAndMakeVisible (_folderLabel);
        addAndMakeVisible (_folderButton);
        addAndMakeVisible(_runButton);
        addAndMakeVisible(_maxTimeLabel);
        addAndMakeVisible(_lastBackupLabel);
        addAndMakeVisible(_pauseButton);
        addAndMakeVisible(_timeSlider);
        _folderButton.setButtonText("Choose experiment folder");
        _folderButton.addListener(this);
        _runButton.setButtonText("Run");
        _runButton.addListener(this);
        _maxTimeLabel.setText("max time:", dontSendNotification);
        _lastBackupLabel.setText("3", dontSendNotification);
        _pauseButton.setButtonText("Pause");
        _pauseButton.addListener(this);
        _timeSlider.setRange(0, 10000, 1);
        _timeSlider.setVelocityBasedMode(true);
    }
    
    void paint (Graphics& g) override
    {
        g.fillAll(Colour::fromFloatRGBA(0.8f, 0.677f, 0.617f, 1.0f));
    }
    
    
    void resized() override
    {
        int percentage_x = getWidth() / 100;
        int percentage_y = getHeight() / 100;
        _folderButton.setBounds(1 * percentage_x, 2 * percentage_y,       // x, y
                                20 * percentage_x, 5 * percentage_y);     // width, height

        _folderLabel.setBounds (1 * percentage_x, 6 * percentage_y,       // x, y
                                90 * percentage_x, 5 * percentage_y);     // width, height

        _runButton.setBounds(1 * percentage_x, 7 * percentage_y,       // x, y
                             20 * percentage_x, 5 * percentage_y);     // width, height
        
        _maxTimeLabel.setBounds (1 * percentage_x, 12 * percentage_y,       // x, y
                                 90 * percentage_x, 5 * percentage_y);     // width, height
        
        _lastBackupLabel.setBounds (1 * percentage_x, 17 * percentage_y,       // x, y
                                    90 * percentage_x, 5 * percentage_y);     // width, height
        
        _pauseButton.setBounds (1 * percentage_x, 23 * percentage_y,       // x, y
                                20 * percentage_x, 5 * percentage_y);     // width, height
        
        _timeSlider.setBounds (1 * percentage_x, 30 * percentage_y,       // x, y
                               90 * percentage_x, 5 * percentage_y);     // width, height
    }
    
    void refreshExperimentSize()
    {
        fs::path p = fs::path(_experimentFolder.toStdString());
        double size=0.0;
        for(bf::recursive_directory_iterator it(p);
            it!=bf::recursive_directory_iterator();
            ++it)
        {
            if(!is_directory(*it))
                size+=bf::file_size(*it);
        }
        double _directory_size = size / 1000000;
        string str_directory_size = to_string_with_precision(_directory_size, 2);
        _folderLabel.setText ("Experiment folder: " + _experimentFolder + "  (" + str_directory_size + "MB)",
                              dontSendNotification);
    }
    
    void buttonClicked(Button* b) override {
        if (b == &_folderButton) {
            FileChooser fc ("Choose an experiment directory",
                            File::getCurrentWorkingDirectory(),
                            "*.",
                            true);
            if (fc.browseForDirectory())
            {
                File chosenDirectory = fc.getResult();
                _experimentFolder = chosenDirectory.getFullPathName();
                refreshExperimentSize();

            }
        }
    }
    
private:
    /** @brief Size of experiment directory
    */
    double _directory_size;
    
    /** @brief Chosen directory for experiment
     */
    String _experimentFolder;
    
    /** @brief Label object showing directory folder and size
     */
    Label _folderLabel;

    /** @brief Label object showing max time value for experiment
     */
    Label _maxTimeLabel;

    /** @brief Label object showing last backup of complete experiment
     */
    Label _lastBackupLabel;
    
    /** @brief Button object to choose a folder
    */
    TextButton _folderButton;
    
    /** @brief Button object to run experiment
     */
    TextButton _runButton;

    /** @brief Button object to pause experiment
     */
    TextButton _pauseButton;

    /** @brief Slider object to select a time slice
     */
    Slider _timeSlider;
};

//==============================================================================
MainContentComponent::MainContentComponent()
{
    setSize (800, 600);
    // Create tabs and add components to each tab
    _tabbedComponent = new TabbedComponent(TabbedButtonBar::TabsAtTop);
    _tabbedComponent->addTab("Experiment", Colour::fromFloatRGBA(0.8f, 0.677f, 0.617f, 1.0f), new ExperimentComponent(), true);
    _tabbedComponent->addTab("View", Colour::fromFloatRGBA(0.0f, 0.077f, 0.217f, 1.0f), new mapComponent(ecosystem), true);
    _tabbedComponent->addTab("Settings", Colour::fromFloatRGBA(0.8f, 0.677f, 0.617f, 1.0f), new Component(), true);
    addAndMakeVisible(_tabbedComponent);
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
    mtx.enter();
    ecosystem.evolve();
    mtx.exit();
    auto num_organisms = ecosystem.biotope.size();
    auto num_free_locs = ecosystem.biotope_free_locs.size();
    cout << "Time: " << ecosystem.time << endl;
    cout << "    num organism: " << num_organisms << endl;
    cout << "    num free locs: " << num_free_locs << endl;
    cout << "    sum previous numbers: " << num_organisms + num_free_locs << endl;
}
