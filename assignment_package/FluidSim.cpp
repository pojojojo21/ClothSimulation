#include "FluidSim.h"

// Constants for SPH
const float REST_DENSITY = 10.0f;
const float GAS_CONSTANT = 0.4f; // (e.g., 0.1 to 1.0)
const float smoothingRadius = 1.0f; // Smoothing radius
const float MASS = 0.1f;
const float VISCOSITY = 0.01f; // 0.01 to 0.1 dampens motion
const float EPSILON = 0.0001f;
const float BOUND_DAMPING = -0.5f;
const float TIME_STEP = 0.003f;

float poly6Kernel(float r, float h) {
    if (r >= 0 && r <= h) {
        float term = (h * h - r * r);
        return (315.0f / (64.0f * M_PI * pow(h, 9))) * pow(term, 3);
    }
    return 0.0f;
}

glm::vec3 spikyGradient(const glm::vec3& r, float h) {
    float r_len = glm::length(r);
    if (r_len > 0 && r_len <= h) {
        float coeff = -45.f / (M_PI * pow(h, 6));
        coeff *= pow(h - r_len, 2);
        return coeff * glm::normalize(r);
    }
    return glm::vec3(0.f);
}

float viscosityLaplacian(float r, float h) {
    if (r >= 0 && r <= h) {
        return 45.0f / (M_PI * pow(h, 6)) * (h - r);
    }
    return 0.0f;
}

FluidSim::FluidSim(OpenGLContext* context, int count, glm::vec3 origin, float spacing)
    : Drawable(context), particleCount(count*count*count), origin(origin), spacing(spacing), initialized(false)
{
    m_grid = std::make_unique<SpatialGrid>();

    initialize(origin, count, count, count, spacing);

}

void FluidSim::initialize(glm::vec3 origin, int numX, int numY, int numZ, float spacing) {
    particles.clear();

    for (int z = 0; z < numZ; ++z) {
        for (int y = 0; y < numY; ++y) {
            for (int x = 0; x < numX; ++x) {
                glm::vec3 pos = origin + glm::vec3(x, y, z) * spacing;

                Particle p(pos, MASS);
                p.velocity = glm::vec3(0.0f);
                p.acceleration = glm::vec3(0.0f);
                p.density = REST_DENSITY;
                p.pressure = 0.0f;

                particles.push_back(p);
            }
        }
    }

    domainMin = origin;
    domainMax = origin + glm::vec3(numX, numY, numZ) * spacing;

    m_grid->initialize(domainMin, domainMax, smoothingRadius);
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
    // 1. Update the spatial grid with current particle positions
    m_grid->clear();
    for (auto& p : particles) {
        m_grid->insertParticle(&p);
    }

    // 2. Compute density and pressure using neighbor search
    computeDensityPressure();

    // 3. Compute pressure, viscosity, and external forces
    computeForces();

    // 4. Integrate motion (Euler, Verlet, or RK2)
    integrate(deltaTime, index);

    // 5. Boundary condition handling (basic AABB bounce)
    for (auto& p : particles) {
        for (int i = 0; i < 3; ++i) {
            if (p.position[i] < domainMin[i]) {
                p.position[i] = domainMin[i];
                p.velocity[i] *= BOUND_DAMPING;
            }
            else if (p.position[i] > domainMax[i]) {
                p.position[i] = domainMax[i];
                p.velocity[i] *= BOUND_DAMPING;
            }
        }
    }

    // 6. Update GPU buffer
    updatePositionBuffer();
}

void FluidSim::integrate(float deltaTime, Integration index) {
    for (auto& p : particles) {
        p.update(deltaTime, index, true);
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

void FluidSim::computeDensityPressure() {

    for (auto& pi : particles) {
        pi.density = 0.0f;
        std::vector<Particle*> neighbors = m_grid->getNeighbors(pi.position, smoothingRadius);
        for (Particle* pj : neighbors) {
            if (&pi == pj) continue;
            float r = glm::length(pi.position - pj->position);
            pi.density += pj->mass * poly6Kernel(r, smoothingRadius);
        }

        pi.pressure = GAS_CONSTANT * (pi.density - REST_DENSITY);
    }
}

void FluidSim::computeForces() {
    for (auto& pi : particles) {
        glm::vec3 pressureForce(0.0f);
        glm::vec3 viscosityForce(0.0f);
        glm::vec3 gravity(0.0f, -9.8f, 0.0f);
        glm::vec3 gravityForce = gravity * pi.density;

        std::vector<Particle*> neighbors = m_grid->getNeighbors(pi.position, smoothingRadius);
        for (Particle* pj : neighbors) {
            if (&pi == pj) continue;

            glm::vec3 rij = pi.position - pj->position;
            float r = glm::length(rij);
            if (r < smoothingRadius && r > EPSILON) {
                // Pressure force
                pressureForce += -pi.mass * (pi.pressure + pj->pressure) / (2.0f * pj->density)
                    * spikyGradient(rij, smoothingRadius);
                
                // Viscosity force
                viscosityForce += VISCOSITY * pi.mass *
                    (pj->velocity - pi.velocity) / pj->density *
                    viscosityLaplacian(r, smoothingRadius);
            }
        }
        pi.acceleration = (pressureForce + viscosityForce + gravityForce) / pi.density;
    }
}