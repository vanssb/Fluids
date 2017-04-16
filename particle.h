#ifndef PARTICLE_H
#define PARTICLE_H

#include <QVector3D>


class Particle{
private:
    float radius;
    QVector3D acceleration;
    QVector3D g;
    QVector3D delta;
    QVector3D oppositionDelta;
public:
    static float dt;
    QVector3D position;
    Particle(float radius,QVector3D position,QVector3D delta);
    void move();
    float getRadius() const{
        return radius;
    }
    QVector3D getPosition() const{
        return position;
    }
    QVector3D getDelta(){
        return delta;
    }
    void setDelta(QVector3D delta){
        this->delta = delta;
    }
    void push(QVector3D delta);

    void setOppositionDelta(QVector3D delta){
        this->oppositionDelta = delta;
    }
    void applyAcceleration(){
        acceleration += g;
    }
    bool operator>(const Particle *p2);
};


#endif // PARTICLE_H
