#include "softbodybox.h"

SoftBodyBox::SoftBodyBox(OpenGLContext* mp_context, int w, int h, int d, float spacing, glm::vec3 origin)
    : Drawable(mp_context), width(w), height(h), depth(d), spacing(spacing), drawType(0), initialized(false) {
    generateParticles(origin);
    generateSprings();
}

void SoftBodyBox::generateParticles(glm::vec3 origin) {
    particles.clear();
    for (int z = 0; z < depth; ++z) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                particles.emplace_back(origin + glm::vec3(x * spacing, y * spacing, z * spacing), 1.0f);
            }
        }
    }

    // Fix corners to prevent free floating
    particles[0].isFixed = true;
    particles[width - 1].isFixed = true;
    particles[width * height - 1].isFixed = true;
    particles[width * height * depth - 1].isFixed = true;
}

void SoftBodyBox::generateSprings() {
    springs.clear();

    for (int z = 0; z < depth; ++z) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int index = z * width * height + y * width + x;

                // Structural Springs (X, Y, Z directions)
                if (x < width - 1)
                    springs.emplace_back(index, index + 1, &particles[index], &particles[index + 1], spacing, 1000.0f, 0.1f);
                if (y < height - 1)
                    springs.emplace_back(index, index + width, &particles[index], &particles[index + width], spacing, 1000.0f, 0.1f);
                if (z < depth - 1)
                    springs.emplace_back(index, index + (width * height), &particles[index], &particles[index + (width * height)], spacing, 1000.0f, 0.1f);

                // Shear Springs (Diagonal X, Y, Z)
                if (x < width - 1 && y < height - 1)
                    springs.emplace_back(index, index + width + 1, &particles[index], &particles[index + width + 1], spacing * 1.414f, 500.0f, 0.3f);
                if (x > 0 && y < height - 1)
                    springs.emplace_back(index, index + width - 1, &particles[index], &particles[index + width - 1], spacing * 1.414f, 500.0f, 0.3f);
                if (y < height - 1 && z < depth - 1)
                    springs.emplace_back(index, index + (width * height) + width, &particles[index], &particles[index + (width * height) + width], spacing * 1.414f, 500.0f, 0.3f);
                if (x < width - 1 && z < depth - 1)
                    springs.emplace_back(index, index + (width * height) + 1, &particles[index], &particles[index + (width * height) + 1], spacing * 1.414f, 500.0f, 0.3f);

                // Bend Springs (Longer range stability)
                if (x < width - 2)
                    springs.emplace_back(index, index + 2, &particles[index], &particles[index + 2], spacing * 2.0f, 200.0f, 0.5f);
                if (y < height - 2)
                    springs.emplace_back(index, index + (width * 2), &particles[index], &particles[index + (width * 2)], spacing * 2.0f, 200.0f, 0.5f);
                if (z < depth - 2)
                    springs.emplace_back(index, index + (width * height * 2), &particles[index], &particles[index + (width * height * 2)], spacing * 2.0f, 200.0f, 0.5f);
            }
        }
    }
}

void SoftBodyBox::initializeAndBufferGeometryData() {
    std::vector<glm::vec3> pos, nor, col;
    std::vector<GLuint> idx;

    switch (drawType) {
    case 0: // particles
        drawParticles(pos, nor, col, idx);
        break;
    case 1:
        drawSprings(pos, nor, col, idx);
        break;
    case 2:
        drawExteriorFaces(pos, nor, col, idx);
        break;
    }

    indexBufferLength = idx.size();

    generateBuffer(INDEX);
    bindBuffer(INDEX);
    bufferData(INDEX, idx);

    generateBuffer(POSITION);
    bindBuffer(POSITION);
    glContext->glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec3), pos.data(), GL_DYNAMIC_DRAW);

    generateBuffer(NORMAL);
    bindBuffer(NORMAL);
    glContext->glBufferData(GL_ARRAY_BUFFER, nor.size() * sizeof(glm::vec3), nor.data(), GL_DYNAMIC_DRAW);

    generateBuffer(COLOR);
    bindBuffer(COLOR);
    bufferData(COLOR, col);

    initialized = true;
}

void SoftBodyBox::update(float deltaTime) {
    for (auto& spring : springs) {
        spring.applyForces();
    }
    for (auto& particle : particles) {
        particle.applyForce(glm::vec3(0.f, -9.8f, 0.f)); // Gravity
        particle.update(deltaTime);
    }

    // Apply spring constraints multiple times per frame
    int iterations = 4; // More iterations = more stable cloth
    for (int i = 0; i < iterations; i++) {
        for (auto& spring : springs) {
            spring.applyConstraint(); // Position-based constraint
        }
    }
}

void SoftBodyBox::updatePositionBuffer() {
    if (!initialized)
    {
        return;
    }

    std::vector<glm::vec3> pos, nor, col;
    std::vector<GLuint> idx;
    switch (drawType) {
    case 0: // particles
        for (const auto& particle : particles) {
            pos.push_back(particle.position);               // Particle position
            nor.push_back(glm::vec3(0, 0, 1));              // Default normal
        }
        break;
    case 1: // springs
        for (const auto& particle : particles) {
            pos.push_back(particle.position);               // Particle position
            nor.push_back(glm::vec3(0, 0, 1));              // Default normal
        }
        break;
    case 2:

        // Generate triangulated positions, normals, and colors
        drawExteriorFaces(pos, nor, col, idx);
        break;
    }

    // Update position buffer
    bindBuffer(POSITION);
    glContext->glBufferSubData(GL_ARRAY_BUFFER, 0, pos.size() * sizeof(glm::vec3), pos.data());

    // Update normal buffer
    bindBuffer(NORMAL);
    glContext->glBufferSubData(GL_ARRAY_BUFFER, 0, nor.size() * sizeof(glm::vec3), nor.data());
}

void SoftBodyBox::resetBox(glm::vec3 origin) {// Reset particle positions and velocities
    int index = 0;
    for (int z = 0; z < depth; ++z) {  // Z controls depth
        for (int y = 0; y < height; ++y) {  // Y controls height
            for (int x = 0; x < width; ++x) {  // X controls width
                particles[index].position = origin + glm::vec3(x * spacing, y * spacing, z * spacing);
                particles[index].previousPosition = origin + glm::vec3(x * spacing, y * spacing, z * spacing);
                particles[index].velocity = glm::vec3(0.0f); // Reset velocity to zero
                particles[index].acceleration = glm::vec3(0.0f); // Reset acceleration to zero
                ++index;
            }
        }
    }

    particles[0].isFixed = true;
    particles[width - 1].isFixed = true;
    particles[width * height - 1].isFixed = true;
    particles[width * height * depth - 1].isFixed = true;

    // Update the position buffer for rendering
    updatePositionBuffer();
}

void SoftBodyBox::dropBox()
{
    particles[0].isFixed = false;
    particles[width - 1].isFixed = false;
    particles[width * height - 1].isFixed = false;
    particles[width * height * depth - 1].isFixed = false;
}

GLenum SoftBodyBox::drawMode() {
    switch (drawType) {
    case 0: // particles
        return GL_POINTS;
    case 1:
        return GL_LINES;
    case 2:
        return GL_TRIANGLES;
    }
}

void SoftBodyBox::drawParticles(std::vector<glm::vec3>& pos,
    std::vector<glm::vec3>& nor,
    std::vector<glm::vec3>& col,
    std::vector<GLuint>& idx)
{
    // Generate positions, normals, and colors for each particle
    for (int i = 0; i < particles.size(); ++i) {
        pos.push_back(particles[i].position); // Particle position
        nor.push_back(glm::vec3(0, 1, 0));    // Default normal (upward)
        col.push_back(glm::vec3(1, 1, 1));    // Default color (white)

        idx.push_back(i); // Each particle is a point
    }
}

void SoftBodyBox::drawSprings(std::vector<glm::vec3>& pos,
    std::vector<glm::vec3>& nor,
    std::vector<glm::vec3>& col,
    std::vector<GLuint>& idx)
{
    // Generate positions, normals, and colors for each particle
    for (const auto& particle : particles) {
        pos.push_back(particle.position);
        nor.push_back(glm::vec3(0, 1, 0)); // Default normal
        col.push_back(glm::vec3(1, 1, 1)); // Default white color
    }

    // Generate indices for drawing lines (springs)
    for (const auto& spring : springs) {
        int idx1 = spring.p1_idx;
        int idx2 = spring.p2_idx;

        idx.push_back(idx1);
        idx.push_back(idx2);
    }
}

void SoftBodyBox::drawExteriorFaces(std::vector<glm::vec3>& pos,
                                    std::vector<glm::vec3>& nor,
                                    std::vector<glm::vec3>& col,
                                    std::vector<GLuint>& idx) {
    pos.clear();
    nor.clear();
    col.clear();
    idx.clear();

    // Color for shading
    glm::vec3 color(1.0f, 1.0f, 1.0f);

    // Iterate through the 6 exterior faces of the box
    for (int z = 0; z < depth; ++z) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int index = z * width * height + y * width + x;

                // Generate faces for each of the 6 sides
                if (z == 0) { // XY plane -Z
                    if (x < width - 1 && y < height - 1) { // Boundary check
                        addQuad(pos, nor, col, idx, x, y, z, glm::vec3(0, 0, -1));
                    }
                }
                if (z == depth - 1) { // XY plane +Z
                    if (x < width - 1 && y < height - 1) { // Boundary check
                        addQuad(pos, nor, col, idx, x, y, z, glm::vec3(0, 0, 1));
                    }
                }
                if (y == 0) { // XZ plane -Y
                    if (x < width - 1 && z < depth - 1) { // Boundary check
                        addQuad(pos, nor, col, idx, x, y, z, glm::vec3(0, -1, 0));
                    }
                }
                if (y == height - 1) { // XZ plane +Y
                    if (x < width - 1 && z < depth - 1) { // Boundary check
                        addQuad(pos, nor, col, idx, x, y, z, glm::vec3(0, 1, 0));
                    }
                }
                if (x == 0) { // YZ plane -X
                    if (y < height - 1 && z < depth - 1) { // Boundary check
                        addQuad(pos, nor, col, idx, x, y, z, glm::vec3(-1, 0, 0));
                    }
                }
                if (x == width - 1) { // YZ plane +X
                    if (y < height - 1 && z < depth - 1) { // Boundary check
                        addQuad(pos, nor, col, idx, x, y, z, glm::vec3(1, 0, 0));
                    }
                }
            }
        }
    }
}

void SoftBodyBox::addQuad(std::vector<glm::vec3>& pos,
    std::vector<glm::vec3>& nor,
    std::vector<glm::vec3>& col,
    std::vector<GLuint>& idx,
    int x, int y, int z,
    glm::vec3 normal) {
    glm::vec3 color(1.0f, 1.0f, 1.0f); // Default white color

    int v000, v100, v010, v110;

    if (normal.z != 0)
    {
        v000 = getIndex(x, y, z);
        v100 = getIndex(x+1, y, z);
        v010 = getIndex(x, y+1, z);
        v110 = getIndex(x+1, y+1, z);
    }
    if (normal.y != 0)
    {
        v000 = getIndex(x, y, z);
        v100 = getIndex(x + 1, y, z);
        v010 = getIndex(x, y, z + 1);
        v110 = getIndex(x + 1, y, z + 1);
    }
    if (normal.x != 0)
    {
        v000 = getIndex(x, y, z);
        v100 = getIndex(x, y + 1, z);
        v010 = getIndex(x, y, z + 1);
        v110 = getIndex(x, y + 1, z + 1);
    }

    glm::vec3 p1 = particles[v000].position;
    glm::vec3 p2 = particles[v010].position;
    glm::vec3 p3 = particles[v100].position;
    glm::vec3 p4 = particles[v110].position;

    // Compute normals (cross product of triangle edges)
    glm::vec3 normal1, normal2;

    //normal1 = glm::normalize(glm::cross(p2 - p1, p3 - p1));
    //normal2 = glm::normalize(glm::cross(p2 - p3, p4 - p3));

    if (normal.x < 0 || normal.y > 0 || normal.z < 0)
    {
        normal1 = glm::normalize(glm::cross(p2 - p1, p3 - p1));
        normal2 = glm::normalize(glm::cross(p2 - p3, p4 - p3));
    }
    else {
        normal1 = glm::normalize(glm::cross(p3 - p1, p2 - p1));
        normal2 = glm::normalize(glm::cross(p4 - p3, p2 - p3));
    }
    

    // First triangle (p1, p2, p3)
    idx.push_back(pos.size()); pos.push_back(p1); nor.push_back(normal1); col.push_back(color);
    idx.push_back(pos.size()); pos.push_back(p2); nor.push_back(normal1); col.push_back(color);
    idx.push_back(pos.size()); pos.push_back(p3); nor.push_back(normal1); col.push_back(color);

    // Second triangle (p2, p4, p3)
    idx.push_back(pos.size()); pos.push_back(p2); nor.push_back(normal2); col.push_back(color);
    idx.push_back(pos.size()); pos.push_back(p4); nor.push_back(normal2); col.push_back(color);
    idx.push_back(pos.size()); pos.push_back(p3); nor.push_back(normal2); col.push_back(color);
}

int SoftBodyBox::getIndex(int x, int y, int z)
{
    return z * width * height + y * width + x;
}

Particle* SoftBodyBox::findClosestParticle(const glm::vec3& rayOrigin, const glm::vec3& rayDirection) {
    Particle* closestParticle = nullptr;
    float minDistance = std::numeric_limits<float>::max();
    float maxSelectionDistance = 0.5f; // Threshold for valid selection

    for (auto& particle : particles) {
        glm::vec3 toParticle = particle.position - rayOrigin;
        float t = glm::dot(toParticle, rayDirection);

        // Ensure the projected point is in front of the ray origin
        if (t < 0) continue;

        glm::vec3 closestPoint = rayOrigin + t * rayDirection;
        float distance = glm::length(closestPoint - particle.position);

        if (distance < maxSelectionDistance && distance < minDistance) {
            minDistance = distance;
            closestParticle = &particle;
        }
    }

    // If the closest particle is still too far, return nullptr
    if (minDistance >= maxSelectionDistance) {
        return nullptr;
    }

    return closestParticle;
}