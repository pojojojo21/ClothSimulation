#pragma once
#include "drawable.h"
#include "particle.h"
#include "SpatialGrid.h"

class FluidSim : public Drawable
{
public:
    FluidSim(OpenGLContext* context, int particleCount, glm::vec3 origin, float spacing);
    void initialize(glm::vec3 origin, int numX, int numY, int numZ, float spacing);
    void initializeAndBufferGeometryData() override;
    void update(float deltaTime, Integration index);
    void updatePositionBuffer();
    GLenum drawMode() override;
    void reset();

private:
    std::vector<Particle> particles;
    std::unique_ptr<SpatialGrid> m_grid;
    float spacing;
    int particleCount;
    glm::vec3 origin;
    glm::vec3 domainMin;
    glm::vec3 domainMax;
    bool initialized;

    void integrate(float deltaTime, Integration index);
    void computeDensityPressure();
    void computeForces();
};

