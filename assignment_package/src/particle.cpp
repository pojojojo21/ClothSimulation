#include "particle.h"
#include <iostream>

Particle::Particle(glm::vec3 pos, float m)
    : position(pos), previousPosition(pos), initialPosition(pos), mass(m), velocity(0.0f), acceleration(0.0f), isFixed(false),
    density(0.f), pressure(0.f)
{}

void Particle::applyForce(const glm::vec3& force) {
    if (!isFixed) {
        acceleration += force / mass;
    }
}

void Particle::update(float deltaTime, Integration type) {
    switch (type)
    {
    case Integration::EULER:
        eulerUpdate(deltaTime);
        break;

    case Integration::IMPLICITEULER:
        implicitEulerUpdate(deltaTime);
        break;

    case Integration::VERLET:
        verletUpdate(deltaTime);
        break;
    }

    // Apply floor collision detection
    float floorHeight = -20.0f;  // Adjust this based on your scene
    floorCollision(floorHeight);

    acceleration = glm::vec3(0.0f);
}

void Particle::eulerUpdate(float deltaTime) // adds energy to the system
{
    if (!isFixed) {
        position += velocity * deltaTime;
        velocity += acceleration * deltaTime;

        previousPosition = position;
    }
}

void Particle::implicitEulerUpdate(float deltaTime) // maintains energy in the system
{
    if (!isFixed) {
        velocity += acceleration * deltaTime;
        position += velocity * deltaTime;

        previousPosition = position;
    }
}

void Particle::verletUpdate(float deltaTime) // better numerical stability
{
    if (!isFixed) {
        glm::vec3 temp = position; // Store current position

        // Verlet Integration Formula
        position += (position - previousPosition) + (acceleration * deltaTime * deltaTime);

        previousPosition = temp; // Update previous position
    }
}

void Particle::floorCollision(float floorHeight)
{
    // Apply basic floor collision detection
    if (position.y < floorHeight) {
        position.y = floorHeight; // Keep particle above the floor
        previousPosition.y = floorHeight; // Prevent penetration
        velocity.y *= -0.3f;
    }
}

void Particle::reset()
{
    position = initialPosition;
    previousPosition = initialPosition;
    velocity = glm::vec3(0.0f);
    acceleration = glm::vec3(0.0f);
    density = 0.0f;
    pressure = 0.0f;
    isFixed = false;
}