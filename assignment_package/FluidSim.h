#pragma once
#include "drawable.h"
#include "particle.h"

class FluidSim : public Drawable
{
public:
    FluidSim(OpenGLContext* context, int particleCount, glm::vec3 origin, float spacing);

    void initializeAndBufferGeometryData() override;
    void update(float deltaTime, Integration index);
    void updatePositionBuffer();
    GLenum drawMode() override;
    void reset();

private:
    std::vector<Particle> particles;
    float spacing;
    int particleCount;
    glm::vec3 origin;
    bool initialized;

    void applyForces();
    void integrate(float deltaTime, Integration index);
};

