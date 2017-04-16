#ifndef SCENE_H
#define SCENE_H

#include <QOpenGLWidget>
#include <QOpenGLTexture>
#include <engine.h>
#include <QMap>
#include <QBasicTimer>
#include <QTime>
#include "camera.h"
#include <QMatrix4x4>
#include <QThread>

#include <QOpenGLFunctions>

class Scene : public QOpenGLWidget, protected QOpenGLFunctions{
    Q_OBJECT
private:
    QOpenGLShaderProgram program;
    QOpenGLTexture* texture;
    QMap<QString,int> attributes;
    QBasicTimer timer;
    QMatrix4x4 mMatrix;
    QMatrix4x4 pMatrix;

    QMap<QString,ObjectData> data;
    void drawObjects();
    void drawMap();
    QTime time;
    QThread thread;
public:
    explicit Scene(QWidget* parent);
    ~Scene();
    Engine engine;
    Camera camera;
protected:
    void paintGL();
    void initializeGL();
    void resizeGL(int w,int h);
    void timerEvent(QTimerEvent *);
signals:
    void renderTimeChanged(int msec);
};

#endif // SCENE_H
