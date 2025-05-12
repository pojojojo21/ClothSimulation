#pragma once
#include "drawable.h"
#include "particle.h"
#include "SpatialGrid.h"
#include <fstream>
#include "utils.h"

struct GPU_Particle {
    glm::vec4 position;     // use vec4 to ensure 16-byte alignment
    glm::vec4 velocity;
    glm::vec4 acceleration;
    float density;
    float pressure;
    float mass;
    float padding[1]; // padding to keep 16-byte alignment
};

class FluidSim : public Drawable
{
public:
    FluidSim(OpenGLContext* context, int particleCount, glm::vec3 origin, float spacing);
    FluidSim(OpenGLContext* context, int numX, int numY, int numZ, glm::vec3 origin, float spacing, 
        float gasConstant, float viscosity, float smoothingRadius);
    void initialize(glm::vec3 origin, int numX, int numY, int numZ, float spacing);
    void initializeAndBufferGeometryData() override;
    void update(float deltaTime, Integration index);
    void updatePositionBuffer();
    GLenum drawMode() override;
    void reset();

    float spacing;
    int width, height, depth;
    float gasConstant;
    float viscosity;
    float smoothingRadius;

private:
    std::vector<Particle> particles;
    std::unique_ptr<SpatialGrid> m_grid;
    int particleCount;
    glm::vec3 origin;
    glm::vec3 domainMin;
    glm::vec3 domainMax;
    bool initialized;

    // SSBO handles
    GLuint particleSSBO;
    GLuint gridSSBO;
    GLuint cellRangeSSBO;
    GLuint sphDensityShader;
    GLuint sphForcesShader;


    void integrate(float deltaTime, Integration index);
    void computeDensityPressure();
    void computeForces();
    void createComputeBuffers();
    void uploadParticleDataToGPU();
    void dispatchComputeShader();
    void downloadParticleDataFromGPU();
    GLuint loadComputeShaderFromFile(const std::string& path);
    void setSPHUniforms(GLuint shader);
};

