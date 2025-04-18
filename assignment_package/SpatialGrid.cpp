#include "SpatialGrid.h"

SpatialGrid::SpatialGrid() : cellSize(1.0f) {}

void SpatialGrid::initialize(const glm::vec3& minCorner, const glm::vec3& maxCorner, float cellSize) {
    this->minCorner = minCorner;
    this->cellSize = cellSize;
    clear();
}

void SpatialGrid::clear() {
    grid.clear();
}

glm::ivec3 SpatialGrid::getCellIndex(const glm::vec3& pos) const {
    return glm::ivec3(glm::floor((pos - minCorner) / cellSize));
}

void SpatialGrid::insertParticle(Particle* particle) {
    glm::ivec3 index = getCellIndex(particle->position);
    grid[index].push_back(particle);
}

std::vector<Particle*> SpatialGrid::getNeighbors(const glm::vec3& position, float smoothingRadius) const {
    std::vector<Particle*> neighbors;
    glm::ivec3 base = getCellIndex(position);

    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dz = -1; dz <= 1; ++dz) {
                glm::ivec3 neighborCell = base + glm::ivec3(dx, dy, dz);
                auto it = grid.find(neighborCell);
                if (it != grid.end()) {
                    for (Particle* p : it->second) {
                        if (glm::length(p->position - position) <= smoothingRadius) {
                            neighbors.push_back(p);
                        }
                    }
                }
            }
        }
    }

    return neighbors;
}
