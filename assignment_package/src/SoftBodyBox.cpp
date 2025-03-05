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
        //drawTriangles(pos, nor, col, idx);
        //break;
        return;
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
}

void SoftBodyBox::updatePositionBuffer() {
    if (!initialized)
    {
        return;
    }

    std::vector<glm::vec3> pos; std::vector<glm::vec3> nor;
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
        //// Generate triangulated positions, normals, and colors
        //for (int z = 0; z < height - 1; ++z) {
        //    for (int x = 0; x < width - 1; ++x) {
        //        // Get particle positions
        //        glm::vec3 topLeft = particles[z * width + x].position;
        //        glm::vec3 topRight = particles[z * width + (x + 1)].position;
        //        glm::vec3 bottomLeft = particles[(z + 1) * width + x].position;
        //        glm::vec3 bottomRight = particles[(z + 1) * width + (x + 1)].position;

        //        // Compute normals (cross product of triangle edges)
        //        glm::vec3 normal1 = glm::normalize(glm::cross(topRight - topLeft, bottomLeft - topLeft));
        //        glm::vec3 normal2 = glm::normalize(glm::cross(topRight - bottomLeft, bottomRight - bottomLeft));

        //        // First triangle (top-left, bottom-left, top-right)
        //        pos.push_back(topLeft); nor.push_back(normal1);
        //        pos.push_back(bottomLeft); nor.push_back(normal1);
        //        pos.push_back(topRight); nor.push_back(normal1);

        //        // Second triangle (bottom-left, bottom-right, top-right)
        //        pos.push_back(bottomLeft); nor.push_back(normal2);
        //        pos.push_back(bottomRight); nor.push_back(normal2);
        //        pos.push_back(topRight); nor.push_back(normal2);
        //    }
        //}
        //break;
        return;
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
                particles[index].position = origin + glm::vec3(x * spacing, y * spacing, -z * spacing);
                particles[index].velocity = glm::vec3(0.0f); // Reset velocity to zero
                particles[index].acceleration = glm::vec3(0.0f); // Reset acceleration to zero
                ++index;
            }
        }
    }

    // Update the position buffer for rendering
    updatePositionBuffer();
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

void SoftBodyBox::drawTetrahedrons(std::vector<glm::vec3>& pos,
    std::vector<glm::vec3>& nor,
    std::vector<glm::vec3>& col,
    std::vector<GLuint>& idx)
{
    pos.clear();
    nor.clear();
    col.clear();
    idx.clear();

    // Iterate through volume to create tetrahedral mesh
    for (int z = 0; z < depth - 1; ++z) {
        for (int y = 0; y < height - 1; ++y) {
            for (int x = 0; x < width - 1; ++x) {
                // Get indices of corner particles
                int v000 = z * width * height + y * width + x;
                int v100 = v000 + 1;
                int v010 = v000 + width;
                int v110 = v010 + 1;
                int v001 = v000 + (width * height);
                int v101 = v100 + (width * height);
                int v011 = v010 + (width * height);
                int v111 = v110 + (width * height);

                // Define tetrahedral faces (split cube into tetrahedrons)
                glm::vec3 verts[8] = {
                    particles[v000].position, particles[v100].position,
                    particles[v010].position, particles[v110].position,
                    particles[v001].position, particles[v101].position,
                    particles[v011].position, particles[v111].position
                };

                // Define normals (cross products)
                glm::vec3 normal1 = glm::normalize(glm::cross(verts[1] - verts[0], verts[2] - verts[0]));
                glm::vec3 normal2 = glm::normalize(glm::cross(verts[1] - verts[2], verts[3] - verts[2]));
                glm::vec3 normal3 = glm::normalize(glm::cross(verts[0] - verts[4], verts[5] - verts[4]));
                glm::vec3 normal4 = glm::normalize(glm::cross(verts[0] - verts[5], verts[1] - verts[5]));
                glm::vec3 normal5 = glm::normalize(glm::cross(verts[2] - verts[6], verts[7] - verts[6]));
                glm::vec3 normal6 = glm::normalize(glm::cross(verts[2] - verts[7], verts[3] - verts[7]));

                glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

                // First tetrahedron (0,1,2,3)
                idx.push_back(pos.size()); pos.push_back(verts[0]); nor.push_back(normal1); col.push_back(color);
                idx.push_back(pos.size()); pos.push_back(verts[1]); nor.push_back(normal1); col.push_back(color);
                idx.push_back(pos.size()); pos.push_back(verts[2]); nor.push_back(normal1); col.push_back(color);
                idx.push_back(pos.size()); pos.push_back(verts[3]); nor.push_back(normal2); col.push_back(color);

                // Second tetrahedron (0,4,5,1)
                idx.push_back(pos.size()); pos.push_back(verts[0]); nor.push_back(normal3); col.push_back(color);
                idx.push_back(pos.size()); pos.push_back(verts[4]); nor.push_back(normal3); col.push_back(color);
                idx.push_back(pos.size()); pos.push_back(verts[5]); nor.push_back(normal3); col.push_back(color);
                idx.push_back(pos.size()); pos.push_back(verts[1]); nor.push_back(normal4); col.push_back(color);

                // Third tetrahedron (2,6,7,3)
                idx.push_back(pos.size()); pos.push_back(verts[2]); nor.push_back(normal5); col.push_back(color);
                idx.push_back(pos.size()); pos.push_back(verts[6]); nor.push_back(normal5); col.push_back(color);
                idx.push_back(pos.size()); pos.push_back(verts[7]); nor.push_back(normal5); col.push_back(color);
                idx.push_back(pos.size()); pos.push_back(verts[3]); nor.push_back(normal6); col.push_back(color);
            }
        }
    }
}
