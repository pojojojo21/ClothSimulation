#include "FluidSim.h"

// Constants for SPH
const float REST_DENSITY = 10.0f;
const float MASS = 0.1f;
const float EPSILON = 0.0001f;
const float BOUND_DAMPING = -0.5f;
const float TIME_STEP = 0.003f;
const float MAX_PARTICLES = 65536;
const float MAX_CELLS = 65536;

struct CellRange {
    int start;
    int end;
};

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
    : Drawable(context), particleCount(count*count*count), origin(origin), spacing(spacing),
    initialized(false), gasConstant(0.4), viscosity(0.01), smoothingRadius(1.0), width(count), 
    height(count), depth(count)
{
    m_grid = std::make_unique<SpatialGrid>();

    initialize(origin, count, count, count, spacing);

}

FluidSim::FluidSim(OpenGLContext* context, int numX, int numY, int numZ, glm::vec3 origin, float spacing,
    float gasConstant, float viscosity, float smoothingRadius)
    : Drawable(context), particleCount(numX* numY* numZ), origin(origin), spacing(spacing),
    initialized(false), gasConstant(gasConstant), viscosity(viscosity), smoothingRadius(smoothingRadius),
    width(numX), height(numY), depth(numZ)
{
    m_grid = std::make_unique<SpatialGrid>();

    initialize(origin, numX, numY, numZ, spacing);

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

    //QString projectPath = getCurrentPath();
    //projectPath.append("/assignment_package/glsl/");
    //QString SPHPath = projectPath + "SPH.comp";
    //QString SPHforcesPath = projectPath + "SPH_Forces.comp";
    //sphDensityShader = loadComputeShaderFromFile(SPHPath.toStdString().c_str());
    //sphForcesShader = loadComputeShaderFromFile(SPHforcesPath.toStdString().c_str());
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

    //uploadParticleDataToGPU();      // Optional: only needed if CPU modifies data
    //dispatchComputeShader();        // Runs physics on GPU
    //downloadParticleDataFromGPU();  // Reads results back for rendering (optional)

    // 6. Updates OpenGL position buffer for drawing
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

        pi.pressure = gasConstant * (pi.density - REST_DENSITY);
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
                viscosityForce += viscosity * pi.mass *
                    (pj->velocity - pi.velocity) / pj->density *
                    viscosityLaplacian(r, smoothingRadius);
            }
        }
        pi.acceleration = (pressureForce + viscosityForce + gravityForce) / pi.density;
    }
}

void FluidSim::createComputeBuffers() {
    glContext->glGenBuffers(1, &particleSSBO);
    glContext->glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleSSBO);
    glContext->glBufferData(GL_SHADER_STORAGE_BUFFER, particles.size() * sizeof(GPU_Particle), nullptr, GL_DYNAMIC_DRAW);

    glContext->glGenBuffers(1, &gridSSBO);
    glContext->glBindBuffer(GL_SHADER_STORAGE_BUFFER, gridSSBO);
    glContext->glBufferData(GL_SHADER_STORAGE_BUFFER, MAX_PARTICLES * sizeof(int), nullptr, GL_DYNAMIC_DRAW);

    glContext->glGenBuffers(1, &cellRangeSSBO);
    glContext->glBindBuffer(GL_SHADER_STORAGE_BUFFER, cellRangeSSBO);
    glContext->glBufferData(GL_SHADER_STORAGE_BUFFER, MAX_CELLS * sizeof(CellRange), nullptr, GL_DYNAMIC_DRAW);
}

void FluidSim::uploadParticleDataToGPU() {
    std::vector<GPU_Particle> gpuParticles;
    for (auto& p : particles) {
        GPU_Particle gp;
        gp.position = glm::vec4(p.position, 0.f);
        gp.velocity = glm::vec4(p.velocity, 0.f);
        gp.acceleration = glm::vec4(p.acceleration, 0.f);
        gp.density = p.density;
        gp.pressure = p.pressure;
        gp.mass = p.mass;
        gpuParticles.push_back(gp);
    }

    glContext->glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleSSBO);
    glContext->glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, gpuParticles.size() * sizeof(GPU_Particle), gpuParticles.data());
}

void FluidSim::dispatchComputeShader() {

    glContext->glUseProgram(sphDensityShader);
    setSPHUniforms(sphDensityShader);

    // Bind SSBOs
    glContext->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleSSBO);
    glContext->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, gridSSBO);
    glContext->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, cellRangeSSBO);

    GLuint numGroups = (GLuint)ceil(particles.size() / 128.0f);
    glContext->glDispatchCompute(numGroups, 1, 1);
    glContext->glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    glContext->glUseProgram(sphForcesShader);
    setSPHUniforms(sphForcesShader);
    glContext->glDispatchCompute(numGroups, 1, 1);
    glContext->glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void FluidSim::downloadParticleDataFromGPU() {
    glContext->glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleSSBO);
    GPU_Particle* gpuPtr = (GPU_Particle*)glContext->glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);

    for (int i = 0; i < particles.size(); ++i) {
        particles[i].position = glm::vec3(gpuPtr[i].position);
        particles[i].velocity = glm::vec3(gpuPtr[i].velocity);
        particles[i].acceleration = glm::vec3(gpuPtr[i].acceleration);
        particles[i].density = gpuPtr[i].density;
        particles[i].pressure = gpuPtr[i].pressure;
    }

    glContext->glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}

GLuint FluidSim::loadComputeShaderFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open compute shader file: " << path << std::endl;
        return 0;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string shaderCode = buffer.str();
    const char* shaderSource = shaderCode.c_str();

    GLuint shader = glContext->glCreateShader(GL_COMPUTE_SHADER);
    glContext->glShaderSource(shader, 1, &shaderSource, nullptr);
    glContext->glCompileShader(shader);

    // Check for compilation errors
    GLint success;
    glContext->glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glContext->glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Compute Shader compilation failed:\n" << log << std::endl;
        return 0;
    }

    GLuint program = glContext->glCreateProgram();
    glContext->glAttachShader(program, shader);
    glContext->glLinkProgram(program);
    glContext->glDeleteShader(shader);  // no longer needed after linking

    // Check for linking errors
    glContext->glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glContext->glGetProgramInfoLog(program, 512, nullptr, log);
        std::cerr << "Shader Program linking failed:\n" << log << std::endl;
        return 0;
    }

    return program;
}

void FluidSim::setSPHUniforms(GLuint shader) {
    glContext->glUseProgram(shader);
    glContext->glUniform1f(glContext->glGetUniformLocation(shader, "smoothingRadius"), smoothingRadius);
    glContext->glUniform1f(glContext->glGetUniformLocation(shader, "mass"), MASS);
    glContext->glUniform1f(glContext->glGetUniformLocation(shader, "restDensity"), REST_DENSITY);
    glContext->glUniform1f(glContext->glGetUniformLocation(shader, "gasConstant"), gasConstant);
    glContext->glUniform1f(glContext->glGetUniformLocation(shader, "viscosity"), viscosity);
    glContext->glUniform1f(glContext->glGetUniformLocation(shader, "dt"), TIME_STEP);
}