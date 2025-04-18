#pragma once
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include "particle.h"

class SpatialGrid {
public:
    SpatialGrid();

    void initialize(const glm::vec3& minCorner, const glm::vec3& maxCorner, float cellSize);
    void clear();
    void insertParticle(Particle* particle);
    std::vector<Particle*> getNeighbors(const glm::vec3& position, float smoothingRadius) const;

private:
    struct GridHash {
        size_t operator()(const glm::ivec3& v) const {
            return std::hash<int>()(v.x) ^ std::hash<int>()(v.y * 73856093) ^ std::hash<int>()(v.z * 19349663);
        }
    };

    glm::ivec3 getCellIndex(const glm::vec3& pos) const;

    std::unordered_map<glm::ivec3, std::vector<Particle*>, GridHash> grid;
    glm::vec3 minCorner;
    float cellSize;
};