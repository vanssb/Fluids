#ifndef CAMERA_H
#define CAMERA_H

#include <QMatrix4x4>
#include <math.h>

class Camera : public QObject{
    Q_OBJECT
private:
    QVector3D position;
    QVector3D direction;
    float hRot;
    float vRot;
    float distance;
    QMatrix4x4 vMatrix;
    void calculateDirection();
public:
    explicit Camera(QObject *parent = 0);
    ~Camera();
    void setPosition( QVector3D position);
    void moveForward();
    void moveBack();
    void rotateUp();
    void rotateDown();
    void rotateRight();
    void rotateLeft();
    QMatrix4x4 getMatrix();
    QVector3D getPosition();
    float getHorizontalRotation(){
        return hRot * 180.0f / M_PI;
    }
    float getVerticalRotation(){
        return vRot * 180.0f / M_PI;
    }
signals:
    void anglesChanged();
    void positionChanged();
};

#endif // CAMERA_H
