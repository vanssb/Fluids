#include "camera.h"

Camera::Camera()
{
    distance = 1.0f;
}

Camera::~Camera(){

}

void Camera::setPosition( QVector3D position ){
    this->position = position;
    hRot = M_PI / 2.0f;
    vRot = M_PI / 2.0f;
    direction.setZ( position.x() + distance * sin(vRot) * cos(hRot) );
    direction.setY( position.y() + distance * cos(vRot) );
    direction.setZ( position.z() + distance * sin(vRot) * sin(hRot) );
}

void Camera::moveForward(){
    position = direction;
}

void Camera::moveBack(){
    position.setX( position.x() - distance * sin(vRot) * cos(hRot) );
    position.setY( position.y() - distance * cos(vRot) );
    position.setZ( position.z() - distance * sin(vRot) * sin(hRot) );
}

void Camera::rotateUp() // повернуть сцену вверх
{
    if (vRot > M_PI / 10) vRot -= 0.02f;
}

void Camera::rotateDown() // повернуть сцену вниз
{
    if (vRot < M_PI * 9 / 10) vRot += 0.02f;
}

void Camera::rotateLeft() // повернуть сцену влево
{

    if (hRot > 0)
        hRot -= 0.02f;
    else
        hRot = 2 * M_PI - hRot;

}

void Camera::rotateRight() // повернуть сцену вправо
{
    if (hRot < 2 * M_PI)
        hRot += 0.02f;
    else
        hRot = hRot - 2 * M_PI;
}

QMatrix4x4 Camera::getMatrix(){
    vMatrix.setToIdentity();
    direction.setX( position.x() + distance * sin(vRot) * cos(hRot) );
    direction.setY( position.y() + distance * cos(vRot) );
    direction.setZ( position.z() + distance * sin(vRot) * sin(hRot) );
    vMatrix.lookAt( position, direction, QVector3D(0.0f, 1.0f, 0.0f) );
    return vMatrix;
}

QVector3D Camera::getPosition(){
    return position;
}
