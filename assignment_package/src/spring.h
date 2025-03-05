#pragma once
#include "particle.h"

class Particle;

class Spring
{
public:
    int p1_idx;
    int p2_idx;
    Particle* p1;
    Particle* p2;
    float restLength;
    float stiffness;
    float damping;

    Spring(int a_idx, int b_idx, Particle* a, Particle* b, float restLen, float k, float d);

    void applyForces();
    void applyConstraint();
};

