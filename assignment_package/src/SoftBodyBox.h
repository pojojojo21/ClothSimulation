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
    void update(float deltaTime);
    void updatePositionBuffer();
    void resetBox(glm::vec3 origin);

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
    void drawTetrahedrons(std::vector<glm::vec3>& pos, std::vector<glm::vec3>& nor, std::vector<glm::vec3>& col, std::vector<GLuint>& idx);
};

#endif // SOFTBODYBOX_H
