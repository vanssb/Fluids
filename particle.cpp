#include "particle.h"

float Particle::dt = 0.04f;

Particle::Particle(float radius,QVector3D position,QVector3D delta){
    this->delta = delta;
    this->position = position;
    this->acceleration = QVector3D(0.0f,0.0f,0.0f);
    this->radius = radius;
    g = QVector3D(0.0f,-9.8f,0.0f);
}

void Particle::move(){
    delta += acceleration * dt * dt / 2.0f;
    delta *= 0.99f;
    position += delta;
    acceleration = QVector3D(0.0f,0.0f,0.0f);
    if (delta.length() > 1.0f) delta *= 1.0f / delta.length();
}

void Particle::push(QVector3D delta){
    position += delta;
    this->delta += delta;
}


