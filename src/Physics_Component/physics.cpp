#include <Physics_Component/physics.hpp>
#include <assert.h>

void Particle::integrate(float deltatime)
{
    assert(deltatime > 0.0);
    position += (velocity * deltatime);

    glm::vec3 resultingAcc = acceleration;
    resultingAcc += (forceAccum * inverseMass);

    velocity += (resultingAcc * deltatime);

    velocity *= pow(damping, deltatime);
    forceAccum = glm::vec3(0.0f);

}






