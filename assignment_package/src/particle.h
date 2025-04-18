#pragma once
#include <la.h>
#include <iostream>

enum class Integration {
    EULER,
    IMPLICITEULER,
    VERLET
};

class Particle
{
public:
    glm::vec3 position;
    glm::vec3 previousPosition; // Required for Verlet-style or fluid updates
    glm::vec3 initialPosition;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    float mass;
    bool isFixed;

    // Fluid properties
    float density;
    float pressure;

    Particle(glm::vec3 pos, float m);

    void applyForce(const glm::vec3& force);
    void update(float deltaTime, Integration type, bool sim);
    void eulerUpdate(float deltaTime);          // Basic Euler integration
    void implicitEulerUpdate(float deltaTime);  //Implicit Euler integration
    void verletUpdate(float deltaTime);         // Verlet integration
    void floorCollision(float floorHeight);     // Floor Collision
    void boxCollision();                        // Box Collision
    void reset();
};

