#ifndef SOFTBODYBOX_H
#define SOFTBODYBOX_H

#include "drawable.h"
#include "particle.h"
#include "spring.h"
#include <vector>
#include <glm/glm.hpp>

class SoftBodyBox : public Drawable {
public:
    SoftBodyBox(OpenGLContext* mp_context, int w, int h, int d, float spacing, glm::vec3 origin);

    void initializeAndBufferGeometryData();
    void update(float deltaTime, Integration index);
    void updatePositionBuffer();
    void resetBox();
    void dropBox();
    Particle* findClosestParticle(const glm::vec3& rayOrigin, const glm::vec3& rayDirection);
    GLenum drawMode();
    int drawType;
    int width, height, depth;
    float spacing;

private:
    std::vector<Particle> particles;
    std::vector<Spring> springs;
    bool initialized;

    void generateParticles(glm::vec3 origin);
    void generateSprings();
    void drawParticles(std::vector<glm::vec3>& pos, std::vector<glm::vec3>& nor, std::vector<glm::vec3>& col, std::vector<GLuint>& idx);
    void drawSprings(std::vector<glm::vec3>& pos, std::vector<glm::vec3>& nor, std::vector<glm::vec3>& col, std::vector<GLuint>& idx);
    void drawExteriorFaces(std::vector<glm::vec3>& pos, std::vector<glm::vec3>& nor, std::vector<glm::vec3>& col, std::vector<GLuint>& idx);
    void addQuad(std::vector<glm::vec3>& pos, std::vector<glm::vec3>& nor, std::vector<glm::vec3>& col, std::vector<GLuint>& idx,
        int x, int y, int z, glm::vec3 normal);
    int getIndex(int x, int y, int z);
};

#endif // SOFTBODYBOX_H
