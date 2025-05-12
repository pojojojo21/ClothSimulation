#include "cloth.h"

Cloth::Cloth(OpenGLContext* mp_context, int w, int h, float spacing, glm::vec3 origin) : Drawable(mp_context), width(w), height(h), spacing(spacing), drawType(0), initialized(false) {
    // Create particles
    for (int z = 0; z < height; ++z) {  // Z controls height
        for (int x = 0; x < width; ++x) {  // X controls width
            particles.emplace_back(origin + glm::vec3(x * spacing, 0, -z * spacing), 1.0f);
        }
    }
    
    particles[0].isFixed = true;
    particles[w-1].isFixed = true;

    particles[w*h - (w*0.5)].isFixed = true;

    // Create springs
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = y * width + x;

            // Structural springs
            if (x < width - 1) springs.emplace_back(index, index + 1, &particles[index], &particles[index + 1], spacing, 500.0f, 2.0f);
            if (y < height - 1) springs.emplace_back(index, index + width, &particles[index], &particles[index + width], spacing, 500.0f, 2.0f);

            // Shear springs (diagonal X shape)
            if (x < width - 1 && y < height - 1) { // Bottom-right neighbor
                springs.emplace_back(index, index + width + 1, &particles[index], &particles[index + width + 1], spacing * 1.414f, 300.0f, 1.5f);
            }
            if (x > 0 && y < height - 1) { // Bottom-left neighbor
                springs.emplace_back(index, index + width - 1, &particles[index], &particles[index + width - 1], spacing * 1.414f, 300.0f, 1.5f);
            }

            // Bend springs (connect across 2 grid cells)
            if (x < width - 2) {
                springs.emplace_back(index, index + 2, &particles[index], &particles[index + 2], spacing * 2.0f, 100.0f, 1.0f);
            }
            if (y < height - 2) {
                springs.emplace_back(index, index + 2 * width, &particles[index], &particles[index + 2 * width], spacing * 2.0f, 100.0f, 1.0f);
            }
        }
    }
}

void Cloth::initializeAndBufferGeometryData()
{
    std::vector<glm::vec3> pos;
    std::vector<glm::vec3> nor;
    std::vector<glm::vec3> col;
    std::vector<GLuint> idx;

    switch (drawType) {
        case 0: // particles
            drawParticles(pos, nor, col, idx);
            break;
        case 1:
            drawSprings(pos, nor, col, idx);
            break;
        case 2:
            drawTriangles(pos, nor, col, idx);
            break;
    }

    indexBufferLength = idx.size();
    
    // Buffer index data
    generateBuffer(INDEX);
    bindBuffer(INDEX);
    bufferData(INDEX, idx);

    // Buffer position data
    generateBuffer(POSITION);
    bindBuffer(POSITION);
    glContext->glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec3), pos.data(), GL_DYNAMIC_DRAW); // Use GL_DYNAMIC_DRAW

    // Buffer normal data
    generateBuffer(NORMAL);
    bindBuffer(NORMAL);
    glContext->glBufferData(GL_ARRAY_BUFFER, nor.size() * sizeof(glm::vec3), nor.data(), GL_DYNAMIC_DRAW); // Use GL_DYNAMIC_DRAW

    // Buffer color data
    generateBuffer(COLOR);
    bindBuffer(COLOR);
    bufferData(COLOR, col);

    initialized = true;
}

void Cloth::drawParticles(std::vector<glm::vec3>& pos, std::vector<glm::vec3>& nor, std::vector<glm::vec3>& col, std::vector<GLuint>& idx)
{
    // Generate positions, normals, and colors for each particle
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = y * width + x;
            pos.push_back(particles[index].position);    // Particle position
            nor.push_back(glm::vec3(0, 0, 1));           // Default normal
            col.push_back(glm::vec3(1, 1, 1));           // Default color (white)
        }
    }

    // Generate indices for drawing lines (springs)
    for (int i = 0; i < particles.size(); i++) {
        idx.push_back(i);
    }
}

void Cloth::drawSprings(std::vector<glm::vec3> &pos, std::vector<glm::vec3> &nor, std::vector<glm::vec3> &col, std::vector<GLuint> &idx)
{
    // Generate positions, normals, and colors for each particle
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = y * width + x;
            pos.push_back(particles[index].position);    // Particle position
            nor.push_back(glm::vec3(0, 0, 1));           // Default normal
            col.push_back(glm::vec3(1, 1, 1));           // Default color (white)
        }
    }

    // Generate indices for drawing lines (springs)
    for (const auto& spring : springs) {
        // Index of p1 in the particles array
        int idx1 = spring.p1_idx;

        // Index of p2 in the particles array
        int idx2 = spring.p2_idx;

        idx.push_back(idx1);
        idx.push_back(idx2);
    }
}

void Cloth::drawTriangles(std::vector<glm::vec3>& pos,
    std::vector<glm::vec3>& nor,
    std::vector<glm::vec3>& col,
    std::vector<GLuint>& idx)
{
    // Clear old data
    pos.clear();
    nor.clear();
    col.clear();
    idx.clear();

    // Generate triangulated positions, normals, and colors
    for (int z = 0; z < height - 1; ++z) {
        for (int x = 0; x < width - 1; ++x) {
            // Get particle positions
            glm::vec3 topLeft = particles[z * width + x].position;
            glm::vec3 topRight = particles[z * width + (x + 1)].position;
            glm::vec3 bottomLeft = particles[(z + 1) * width + x].position;
            glm::vec3 bottomRight = particles[(z + 1) * width + (x + 1)].position;

            // Compute normals (cross product of triangle edges)
            glm::vec3 normal1 = glm::normalize(glm::cross(topRight - topLeft, bottomLeft - topLeft));
            glm::vec3 normal2 = glm::normalize(glm::cross(topRight - bottomLeft, bottomRight - bottomLeft));

            // Assign colors (can be adjusted)
            glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);  // Default white color

            // First triangle (top-left, bottom-left, top-right)
            idx.push_back(pos.size()); pos.push_back(topLeft); nor.push_back(normal1); col.push_back(color);
            idx.push_back(pos.size()); pos.push_back(bottomLeft); nor.push_back(normal1); col.push_back(color);
            idx.push_back(pos.size()); pos.push_back(topRight); nor.push_back(normal1); col.push_back(color);

            // Second triangle (bottom-left, bottom-right, top-right)
            idx.push_back(pos.size()); pos.push_back(bottomLeft); nor.push_back(normal2); col.push_back(color);
            idx.push_back(pos.size()); pos.push_back(bottomRight); nor.push_back(normal2); col.push_back(color);
            idx.push_back(pos.size()); pos.push_back(topRight); nor.push_back(normal2); col.push_back(color);
        }
    }
}

GLenum Cloth::drawMode() {
    switch (drawType) {
    case 0: // particles
        return GL_POINTS;
    case 1:
        return GL_LINES;
    case 2:
        return GL_TRIANGLES;
    }
}

void Cloth::update(float deltaTime, Integration index, float bounce) {
    for (auto& spring : springs) {
        spring.applyForces();
    }
    for (auto& particle : particles) {
        particle.applyForce(glm::vec3(0.f, -9.8f, 0.f));
        particle.update(deltaTime, index, false, bounce);
    }

    // Apply spring constraints multiple times per frame
    int iterations = 4; // More iterations = more stable cloth
    for (int i = 0; i < iterations; i++) {
        for (auto& spring : springs) {
            spring.applyConstraint(); // Position-based constraint
        }
    }
}

void Cloth::updatePositionBuffer() {
    if (!initialized)
    {
        return;
    }
    std::vector<glm::vec3> pos; std::vector<glm::vec3> nor;
    switch (drawType) {
    case 0: // particles
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int index = y * width + x;
                pos.push_back(particles[index].position);    // Particle position
                nor.push_back(glm::vec3(0, 0, 1));           // Default normal
            }
        }
        break;
    case 1:
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int index = y * width + x;
                pos.push_back(particles[index].position);    // Particle position
                nor.push_back(glm::vec3(0, 0, 1));           // Default normal
            }
        }
        break;
    case 2:
        // Generate triangulated positions, normals, and colors
        for (int z = 0; z < height - 1; ++z) {
            for (int x = 0; x < width - 1; ++x) {
                // Get particle positions
                glm::vec3 topLeft = particles[z * width + x].position;
                glm::vec3 topRight = particles[z * width + (x + 1)].position;
                glm::vec3 bottomLeft = particles[(z + 1) * width + x].position;
                glm::vec3 bottomRight = particles[(z + 1) * width + (x + 1)].position;

                // Compute normals (cross product of triangle edges)
                glm::vec3 normal1 = glm::normalize(glm::cross(topRight - topLeft, bottomLeft - topLeft));
                glm::vec3 normal2 = glm::normalize(glm::cross(topRight - bottomLeft, bottomRight - bottomLeft));

                // First triangle (top-left, bottom-left, top-right)
                pos.push_back(topLeft); nor.push_back(normal1);
                pos.push_back(bottomLeft); nor.push_back(normal1);
                pos.push_back(topRight); nor.push_back(normal1);

                // Second triangle (bottom-left, bottom-right, top-right)
                pos.push_back(bottomLeft); nor.push_back(normal2);
                pos.push_back(bottomRight); nor.push_back(normal2);
                pos.push_back(topRight); nor.push_back(normal2);
            }
        }
        break;
    }
    
    // Update position buffer
    bindBuffer(POSITION);
    glContext->glBufferSubData(GL_ARRAY_BUFFER, 0, pos.size() * sizeof(glm::vec3), pos.data());

    // Update normal buffer
    bindBuffer(NORMAL);
    glContext->glBufferSubData(GL_ARRAY_BUFFER, 0, nor.size() * sizeof(glm::vec3), nor.data());
}

void Cloth::resetCloth() {
    // Reset particle positions and velocities
    for (auto& p : particles) {
        p.reset();
    }

    // Ensure fixed particles remain fixed
    particles[0].isFixed = true;
    particles[width - 1].isFixed = true;

    particles[width * height - (width * 0.5)].isFixed = true;

    // Update the position buffer for rendering
    updatePositionBuffer();
}

void Cloth::dropCorner()
{
    particles[width * height - (width * 0.5)].isFixed = false;
}

void Cloth::dropCloth()
{
    particles[0].isFixed = false;
    particles[width - 1].isFixed = false;
    particles[width * height - (width * 0.5)].isFixed = false;
}

Particle* Cloth::findClosestParticle(const glm::vec3& rayOrigin, const glm::vec3& rayDirection) {
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