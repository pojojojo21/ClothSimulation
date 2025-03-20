#include "particle.h"
#include <iostream>

Particle::Particle(glm::vec3 pos, float m)
    : position(pos), previousPosition(pos), mass(m), velocity(0.0f), acceleration(0.0f), isFixed(false)
{}

void Particle::applyForce(const glm::vec3& force) {
    if (!isFixed) {
        acceleration += force / mass;
    }
}

void Particle::update(float deltaTime) {
    if (!isFixed) {
        glm::vec3 temp = position; // Store current position
        glm::vec3 displacement = position - previousPosition;
        glm::vec3 accelerationTerm = acceleration * (deltaTime * deltaTime);

        // Verlet Integration Formula
        position = position + displacement + accelerationTerm;

        previousPosition = temp; // Update previous position
    }


    // Apply floor collision detection
    float floorHeight = -20.0f;  // Adjust this based on your scene
    if (position.y < floorHeight) {
        position.y = floorHeight;  // Keep particle above the floor
        previousPosition.y = floorHeight; // Prevent penetration
        velocity.y = 0.0f;         // Stop vertical movement
    }

    acceleration = glm::vec3(0.0f);
}