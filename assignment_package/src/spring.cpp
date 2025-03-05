#include "spring.h"

Spring::Spring(int a_idx, int b_idx, Particle* a, Particle* b, float restLen, float k, float d)
	: p1_idx(a_idx), p2_idx(b_idx), p1(a), p2(b), restLength(restLen), stiffness(k), damping(d)
{}

void Spring::applyForces() {
    glm::vec3 delta = p2->position - p1->position;
    float currentLength = glm::length(delta);
    
    // Avoid division by zero
    if (currentLength == 0.0f) return;

    glm::vec3 direction = glm::normalize(delta);

    // Hooke's Law: F = -k * (length - restLength)
    glm::vec3 springForce = -stiffness * (currentLength - restLength) * direction;

    // Damping force: F = -d * relative velocity in spring direction
    glm::vec3 relativeVelocity = p2->velocity - p1->velocity;
    glm::vec3 dampingForce = -damping * glm::dot(relativeVelocity, direction) * direction;

    p1->applyForce(-springForce);
    p2->applyForce(springForce);
    p1->applyForce(-dampingForce);
    p2->applyForce(dampingForce);
}

void Spring::applyConstraint() {
    glm::vec3 delta = p2->position - p1->position;
    float currentLength = glm::length(delta);

    // Avoid division by zero
    if (currentLength == 0.0f) return;

    glm::vec3 direction = delta / currentLength; // Normalized direction
    float correction = (currentLength - restLength) * 0.5f;

    // Adjust particle positions (if not fixed)
    if (!p1->isFixed) p1->position += correction * direction;
    if (!p2->isFixed) p2->position -= correction * direction;
}