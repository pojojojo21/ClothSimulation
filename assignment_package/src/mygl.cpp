#include "mygl.h"
#include <la.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>


MyGL::MyGL(QWidget* parent)
    : OpenGLContext(parent),
    timer(), currTime(0.),
    m_geomSquare(this),
    m_cloth(nullptr),
    m_box(nullptr),
    m_fluidSim(nullptr),
    m_progLambert(this), m_progFlat(this),
    vao(),
    m_camera(width(), height()),
    m_mousePosPrev(),
    objType(0),
    integrationType(Integration::VERLET),
    selectedParticle(nullptr),
    bounceLevel(1.0)
{
    setFocusPolicy(Qt::StrongFocus);

    m_cloth = std::make_unique<Cloth>(this, 10, 10, 1.2f, glm::vec3(0, 0, 0));
    m_box = std::make_unique<SoftBodyBox>(this, 10, 10, 10, 1.2f, glm::vec3(0, 0, 0));
    m_fluidSim = std::make_unique<FluidSim>(this, 10, glm::vec3(0, 0, 0), 0.7f);

    connect(&timer, SIGNAL(timeout()), this, SLOT(tick()));
    // Tell the timer to redraw 60 times per second
    timer.start(16);
}

MyGL::~MyGL()
{
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.5, 0.5, 0.5, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the instances of Cylinder and Sphere.
    m_geomSquare.initializeAndBufferGeometryData();

    //Create the cloth instance
    m_cloth->initializeAndBufferGeometryData();

    //Create the box instance
    m_box->initializeAndBufferGeometryData();

    //Create the fluidSim instance
    m_fluidSim->initializeAndBufferGeometryData();

    // Create and set up the diffuse shader
    m_progLambert.createAndCompileShaderProgram("lambert.vert.glsl", "lambert.frag.glsl");
    // Create and set up the flat lighting shader
    m_progFlat.createAndCompileShaderProgram("flat.vert.glsl", "flat.frag.glsl");


    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);
}

void MyGL::resizeGL(int w, int h)
{
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    m_camera.recomputeAspectRatio(w, h);

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)
    glm::mat4 viewproj = m_camera.getViewProj();
    m_progLambert.setUnifMat4("u_ViewProj", viewproj);
    m_progFlat.setUnifMat4("u_ViewProj", viewproj);

    printGLErrorLog();
}

//This function is called by Qt any time your GL window is supposed to update
//For example, when the function update() is called, paintGL is called implicitly.
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 viewproj = m_camera.getViewProj();
    m_progLambert.setUnifMat4("u_ViewProj", viewproj);
    m_progFlat.setUnifMat4("u_ViewProj", viewproj);
    m_progLambert.setUnifVec3("u_CamPos", m_camera.eye);
    m_progFlat.setUnifMat4("u_Model", glm::mat4(1.f));
    glm::mat4 model = glm::mat4(1.f);
    m_progLambert.setUnifMat4("u_Model", model);
    m_progLambert.setUnifMat4("u_ModelInvTr", glm::inverse(glm::transpose(model)));

    //Create a model matrix. This one rotates the square by PI/4 radians then translates it by <-2,0,0>.
    //Note that we have to transpose the model matrix before passing it to the shader
    //This is because OpenGL expects column-major matrices, but you've
    //implemented row-major matrices.
    //glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-2,0,0)) * glm::rotate(glm::mat4(), 0.25f * 3.14159f, glm::vec3(0,1,0));
    ////Send the geometry's transformation matrix to the shader
    //m_progLambert.setUnifMat4("u_Model", model);
    //m_progLambert.setUnifMat4("u_ModelInvTr", glm::inverse(glm::transpose(model)));
    ////Draw the example sphere using our lambert shader
    //m_progLambert.draw(m_geomSquare);

    ////Now do the same to render the cylinder
    ////We've rotated it -45 degrees on the Z axis, then translated it to the point <2,2,0>
    //model = glm::translate(glm::mat4(1.0f), glm::vec3(2,2,0)) * glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0,0,1));
    //m_progLambert.setUnifMat4("u_Model", model);
    //m_progLambert.setUnifMat4("u_ModelInvTr", glm::inverse(glm::transpose(model)));
    //m_progLambert.draw(m_geomSquare);

    
    switch (objType)
    {
    case 0: // Update cloth simulation
        m_cloth->update(1.0f / 60.0f, integrationType, bounceLevel); // Assuming 60 FPS for deltaTime
        m_cloth->updatePositionBuffer(); // Update position buffer with the latest particle positions

        // Draw the cloth
        switch (m_cloth->drawType)
        {
        case 0:
            m_progFlat.draw(*m_cloth);
            break;
        case 1:
            m_progFlat.draw(*m_cloth);
            break;
        case 2:
            m_progLambert.draw(*m_cloth);
            break;
        }

        break;
    case 1: // Update box simulation
        m_box->update(1.0f / 60.0f, integrationType, bounceLevel); // Assuming 60 FPS for deltaTime
        m_box->updatePositionBuffer(); // Update position buffer with the latest particle positions

        // Draw the cloth
        switch (m_box->drawType)
        {
        case 0:
            m_progFlat.draw(*m_box);
            break;
        case 1:
            m_progFlat.draw(*m_box);
            break;
        case 2:
            m_progLambert.draw(*m_box);
            break;
        }
        //m_progFlat.draw(*m_box);

        break;
    case 2: // Update fluid simulation
        m_fluidSim->update(1.0f / 60.0f, integrationType);
        m_progFlat.draw(*m_fluidSim);
        break;
    }
}

void MyGL::keyPressEvent(QKeyEvent *e) {
    ;
}

void MyGL::mousePressEvent(QMouseEvent *e)
{
    if(e->buttons() & Qt::RightButton)
    {
        m_mousePosPrev = glm::vec2(e->pos().x(), e->pos().y());
    }
    else if (e->buttons() & Qt::LeftButton)
    {
        m_mousePosPrev = glm::vec2(e->pos().x(), e->pos().y());

        // Convert screen coordinates to world ray
        glm::vec3 rayOrigin, rayDirection;
        screenToWorldRay(e->pos().x(), e->pos().y(), rayOrigin, rayDirection);

        // Find closest particle
        if (objType == 0) {
            selectedParticle = m_cloth->findClosestParticle(rayOrigin, rayDirection);
        }
        else {
            selectedParticle = m_box->findClosestParticle(rayOrigin, rayDirection);
        }
        
        if (selectedParticle) {
            selectedParticle->isFixed = true; // Lock particle in place
        }
    }
}

void MyGL::mouseMoveEvent(QMouseEvent *e) {
    glm::vec2 pos(e->pos().x(), e->pos().y());
    if(e->buttons() & Qt::LeftButton)
    {
        // if particle selected move particle
        if (selectedParticle) {
            glm::vec3 rayOrigin, rayDirection;
            screenToWorldRay(e->pos().x(), e->pos().y(), rayOrigin, rayDirection);

            // Move the particle along the ray direction
            selectedParticle->position = rayOrigin + rayDirection * 1.0f;
            return;
        }

        // Rotation
        glm::vec2 diff = 0.2f * (pos - m_mousePosPrev);
        m_mousePosPrev = pos;
        m_camera.RotateAboutGlobalUp(-diff.x);
        m_camera.RotateAboutLocalRight(-diff.y);
    }
    else if(e->buttons() & Qt::RightButton)
    {
        // Panning
        glm::vec2 diff = 0.05f * (pos - m_mousePosPrev);
        m_mousePosPrev = pos;
        m_camera.PanAlongRight(-diff.x);
        m_camera.PanAlongUp(diff.y);
    }
}

void MyGL::mouseReleaseEvent(QMouseEvent* e) {
    if (selectedParticle) {
        selectedParticle->isFixed = false;  // Release particle
        selectedParticle = nullptr;
    }
}


void MyGL::wheelEvent(QWheelEvent *e) {
    m_camera.Zoom(e->angleDelta().y() * 0.001f);
}

void MyGL::reset()
{
    m_cloth->resetCloth();
    m_box->resetBox();
    m_fluidSim->reset();
}

void MyGL::dropCorner()
{
    m_cloth->dropCorner();
}

void MyGL::drop()
{
    m_cloth->dropCloth();
    m_box->dropBox();
}

void MyGL::tick() {
    ++currTime;
    update();
}

void MyGL::setDrawType(int index)
{
    this->m_cloth->drawType = index;
    this->m_cloth->initializeAndBufferGeometryData();

    this->m_box->drawType = index;
    this->m_box->initializeAndBufferGeometryData();
}

void MyGL::setObjType(int index)
{
    this->objType = index;
}

void MyGL::setIntegrationType(int index)
{
    switch (index)
    {
    case 0:
        this->integrationType = Integration::VERLET;
        break;
    case 1:
        this->integrationType = Integration::IMPLICITEULER;
        break;
    case 2:
        this->integrationType = Integration::EULER;
        break;
    }
}

void MyGL::setBounce(float bounce)
{
    bounceLevel = bounce;
}

void MyGL::setGas(float gas)
{
    m_fluidSim->gasConstant = gas;
}

void MyGL::setVis(float vis)
{
    m_fluidSim->viscosity = vis;
}

void MyGL::setSmoothingRadius(float sR)
{
    m_fluidSim->smoothingRadius = sR;
}

void MyGL::changeCloth(bool changeW, int width, bool changeH, int height, bool changeS, float spacing)
{
    int w = m_cloth->width;
    int h = m_cloth->height;
    float s = m_cloth->spacing;
    int dT = m_cloth->drawType;

    if (changeW)
    {
        w = width;
    }
    if (changeH)
    {
        h = height;
    }
    if (changeS)
    {
        s = spacing;
    }
    m_cloth = std::make_unique<Cloth>(this, w, h, s, glm::vec3(0, 0, 0));
    m_cloth->drawType = dT;
    this->m_cloth->initializeAndBufferGeometryData();
}

void MyGL::changeBox(bool changeW, int width, bool changeH, int height, bool changeD, int depth, bool changeS, float spacing)
{
    int w = m_box->width;
    int h = m_box->height;
    int d = m_box->depth;
    float s = m_box->spacing;
    int dT = m_box->drawType;

    if (changeW)
    {
        w = width;
    }
    if (changeH)
    {
        h = height;
    }
    if (changeD)
    {
        d = depth;
    }
    if (changeS)
    {
        s = spacing;
    }
    m_box = std::make_unique<SoftBodyBox>(this, w, h, d, s, glm::vec3(0, 0, 0));
    m_box->drawType = dT;
    this->m_box->initializeAndBufferGeometryData();
}

void MyGL::changeFluidSim(bool changeW, int width, bool changeH, int height, bool changeD, int depth, bool changeS, float spacing)
{
    int w = m_fluidSim->width;
    int h = m_fluidSim->height;
    int d = m_fluidSim->depth;
    float s = m_fluidSim->spacing;
    int gas = m_fluidSim->gasConstant;
    int vis = m_fluidSim->viscosity;
    int sR = m_fluidSim->smoothingRadius;

    if (changeW)
    {
        w = width;
    }
    if (changeH)
    {
        h = height;
    }
    if (changeD)
    {
        d = depth;
    }
    if (changeS)
    {
        s = spacing;
    }

    m_fluidSim = std::make_unique<FluidSim>(this, w, h, d, glm::vec3(0, 0, 0), s, 
        gas, vis, sR);

    this->m_fluidSim->initializeAndBufferGeometryData();
}

void MyGL::screenToWorldRay(int mouseX, int mouseY, glm::vec3& rayOrigin, glm::vec3& rayDirection) {
    glm::mat4 invVP = glm::inverse(m_camera.getViewProj());

    float x = (2.0f * mouseX) / width() - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / height();  // Invert Y for OpenGL
    glm::vec4 nearPoint(x, y, -1.0f, 1.0f);  // Near plane
    glm::vec4 farPoint(x, y, 1.0f, 1.0f);   // Far plane

    glm::vec4 worldNear = invVP * nearPoint;
    glm::vec4 worldFar = invVP * farPoint;

    worldNear /= worldNear.w;
    worldFar /= worldFar.w;

    rayOrigin = glm::vec3(worldNear);
    rayDirection = glm::normalize(glm::vec3(worldFar - worldNear));
}