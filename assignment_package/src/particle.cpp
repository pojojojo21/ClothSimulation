#include "particle.h"
#include <iostream>

Particle::Particle(glm::vec3 pos, float m)
    : position(pos), mass(m), velocity(0.0f), acceleration(0.0f), isFixed(false)
{}

void Particle::applyForce(const glm::vec3& force) {
    if (!isFixed) {
        acceleration += force / mass;
    }
}

void Particle::update(float deltaTime) {
    if (!isFixed) {
        velocity += acceleration * deltaTime;
        position += velocity * deltaTime;
    }

    // Apply floor collision detection
    float floorHeight = -20.0f;  // Adjust this based on your scene
    if (position.y < floorHeight) {
        position.y = floorHeight;  // Keep particle above the floor
        velocity.y = 0.0f;         // Stop vertical movement
    }

    acceleration = glm::vec3(0.0f);
}