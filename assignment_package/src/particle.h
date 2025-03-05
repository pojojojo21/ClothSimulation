#pragma once
#include <la.h>
#include <iostream>

class Particle
{
public:
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    float mass;
    bool isFixed;

    Particle(glm::vec3 pos, float m);

    void applyForce(const glm::vec3& force);
    void update(float deltaTime);
};

