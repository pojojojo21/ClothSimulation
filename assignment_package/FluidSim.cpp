#include "FluidSim.h"

FluidSim::FluidSim(OpenGLContext* context, int count, glm::vec3 origin, float spacing)
    : Drawable(context), particleCount(count), origin(origin), spacing(spacing), initialized(false)
{
    for (int i = 0; i < particleCount; ++i) {
        glm::vec3 offset = glm::vec3((i % 10) * spacing, (i / 10) * spacing, 0); // simple grid init
        particles.emplace_back(origin + offset, 1.0f);
    }
}

void FluidSim::initializeAndBufferGeometryData() {
    std::vector<glm::vec3> pos, nor, col;
    std::vector<GLuint> idx;

    for (int i = 0; i < particles.size(); ++i) {
        pos.push_back(particles[i].position);
        nor.push_back(glm::vec3(0, 1, 0)); // default normal
        col.push_back(glm::vec3(0.2f, 0.5f, 1.0f)); // fluid blue
        idx.push_back(i);
    }

    indexBufferLength = idx.size();
    generateBuffer(INDEX); bindBuffer(INDEX); bufferData(INDEX, idx);
    generateBuffer(POSITION); bindBuffer(POSITION); glContext->glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec3), pos.data(), GL_DYNAMIC_DRAW);
    generateBuffer(NORMAL); bindBuffer(NORMAL); glContext->glBufferData(GL_ARRAY_BUFFER, nor.size() * sizeof(glm::vec3), nor.data(), GL_DYNAMIC_DRAW);
    generateBuffer(COLOR); bindBuffer(COLOR); bufferData(COLOR, col);

    initialized = true;
}

void FluidSim::update(float deltaTime, Integration index) {
    applyForces();
    integrate(deltaTime, index);
}

void FluidSim::applyForces() {
    for (auto& p : particles) {
        p.applyForce(glm::vec3(0, -9.8f, 0)); // gravity
        // TODO: Add pressure and viscosity later
    }
}

void FluidSim::integrate(float deltaTime, Integration index) {
    for (auto& p : particles) {
        p.update(deltaTime, index);
    }
}

void FluidSim::updatePositionBuffer() {
    if (!initialized)
    {
        return;
    }

    std::vector<glm::vec3> pos, nor;
    for (auto& p : particles) {
        pos.push_back(p.position);
        nor.push_back(glm::vec3(0, 1, 0));
    }

    bindBuffer(POSITION);
    glContext->glBufferSubData(GL_ARRAY_BUFFER, 0, pos.size() * sizeof(glm::vec3), pos.data());
    bindBuffer(NORMAL);
    glContext->glBufferSubData(GL_ARRAY_BUFFER, 0, nor.size() * sizeof(glm::vec3), nor.data());
}

GLenum FluidSim::drawMode() {
    return GL_POINTS;
}

void FluidSim::reset() {
    for (auto& p : particles) {
        p.reset();
    }

    // If you're also buffering geometry data like Cloth/SoftBodyBox:
    updatePositionBuffer();  // optional, depends on your pipeline
}