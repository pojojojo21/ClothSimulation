#pragma once
#include <la.h>
#include "particle.h"
#include "spring.h"
#include "drawable.h"
#include <qopenglfunctions.h>

class Particle;
class Spring;

class Cloth : public Drawable
{
public:
    std::vector<Particle> particles;
    std::vector<Spring> springs;
    int width, height;
    float spacing;
    int drawType;
    bool initialized;

    Cloth(OpenGLContext* mp_context, int w, int h, float spacing, glm::vec3 origin);

    void update(float deltaTime, Integration index);
    void initializeAndBufferGeometryData() override;
    void drawParticles(std::vector<glm::vec3>& pos, std::vector<glm::vec3>& nor, std::vector<glm::vec3>& col, std::vector<GLuint>& idx);
    void drawSprings(std::vector<glm::vec3>& pos, std::vector<glm::vec3>& nor, std::vector<glm::vec3>& col, std::vector<GLuint>& idx);
    void drawTriangles(std::vector<glm::vec3>& pos, std::vector<glm::vec3>& nor, std::vector<glm::vec3>& col, std::vector<GLuint>& idx);
    void updatePositionBuffer();
    void resetCloth();
    void dropCorner();
    void dropCloth();
    Particle* findClosestParticle(const glm::vec3& rayOrigin, const glm::vec3& rayDirection);
    GLenum drawMode() override;
};

