#ifndef PHYSICS_HPP
#define PHYSICS_HPP
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Particle
{
    public:


    // Constructor with default values for position, velocity, acceleration, mass, etc.
    Particle(const glm::vec3& initPosition = glm::vec3(0.0f), 
             const glm::vec3& initVelocity = glm::vec3(0.0f), 
             const glm::vec3& initAcceleration = glm::vec3(0.0f), 
             float initMass = 1.0f, 
             float initDamping = 0.99f)
        : position(initPosition), 
          velocity(initVelocity), 
          acceleration(initAcceleration), 
          forceAccum(glm::vec3(0.0f)), // Initialize accumulated forces to zero
          mass(initMass), 
          inverseMass(1.0f / initMass),  // Inverse mass (1 / mass)
          damping(initDamping) {}


/**
    * Integrates the particle forward in time by the given amount.
    * This function uses a Newton-Euler integration method, which is a
    * linear approximation of the correct integral. For this reason it
    * may be inaccurate in some cases.
*/

    void integrate(float deltatime);



    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    glm::vec3 forceAccum;  // Force accumulation vector
    float mass;
    float inverseMass;  // Inverse mass for efficiency
    float damping;      // Damping factor (e.g., drag)
    

};












#endif