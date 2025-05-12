#ifndef MYGL_H
#define MYGL_H

#include <openglcontext.h>
#include <utils.h>
#include <shaderprogram.h>
#include <scene/squareplane.h>
#include <cloth.h>
#include <SoftBodyBox.h>
#include <FluidSim.h>
#include "camera.h"

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QTimer>

class Cloth;

class MyGL
    : public OpenGLContext
{
    Q_OBJECT
private:
    QTimer timer;
    float currTime;

    SquarePlane m_geomSquare;// The instance of a unit cylinder we can use to render any cylinder
    std::unique_ptr<Cloth> m_cloth;// The instance of the cloth
    std::unique_ptr<SoftBodyBox> m_box;// The instance of the cloth
    std::unique_ptr<FluidSim> m_fluidSim;

    ShaderProgram m_progLambert;// A shader program that uses lambertian reflection
    ShaderProgram m_progFlat;// A shader program that uses "flat" reflection (no shadowing at all)

    GLuint vao; // A handle for our vertex array object. This will store the VBOs created in our geometry classes.
                // Don't worry too much about this. Just know it is necessary in order to render geometry.

    Camera m_camera;
    // A variable used to track the mouse's previous position when
    // clicking and dragging on the GL viewport. Used to move the camera
    // in the scene.
    glm::vec2 m_mousePosPrev;

    int objType;
    Integration integrationType;
    Particle* selectedParticle;
    float bounceLevel;

public:
    explicit MyGL(QWidget *parent = nullptr);
    ~MyGL();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void reset();
    void dropCorner();
    void drop();
    void setDrawType(int index);
    void setObjType(int index);
    void setIntegrationType(int index);
    void setBounce(float bounce);
    void setGas(float gas);
    void setVis(float vis);
    void setSmoothingRadius(float sR);
    void changeCloth(bool changeW, int width, bool changeH, int height, bool changeS, float spacing);
    void changeBox(bool changeW, int width, bool changeH, int height, bool changeD, int depth, bool changeS, float spacing);
    void changeFluidSim(bool changeW, int width, bool changeH, int height, bool changeD, int depth, bool changeS, float spacing);

protected:
    void keyPressEvent(QKeyEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent* e);
    void wheelEvent(QWheelEvent *e);
    void screenToWorldRay(int mouseX, int mouseY, glm::vec3& rayOrigin, glm::vec3& rayDirection);

public slots:
    void tick();
};


#endif // MYGL_H
