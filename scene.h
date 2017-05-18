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
#include <QOpenGLFunctions_4_5_Core>

class Scene : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core{
    Q_OBJECT
private:
    QOpenGLShaderProgram program;
    QOpenGLTexture* texture;
    QMap<QString,int> attributes;
    QBasicTimer timer;
    QMatrix4x4 mMatrix;
    QMatrix4x4 pMatrix;
    void setLightSettings();
    QMap<QString,ObjectData> data;
    void drawObjects();
    void drawMap();
    QTime time;
    QThread thread;
    std::vector<QMatrix4x4> mMatrixes;
    QOpenGLBuffer* mMatrixesBuffer;
    //QOpenGLVertexArrayObject* vao;
//Light settings
    QVector3D color;
    float ambient;
    float diffuse;
    float specular;
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
public slots:
    void setAmbient(int ambient);
    void setDiffuse(int diffuse);
    void setSpecular(int specular);
    void setColorR(int red);
    void setColorG(int green);
    void setColorB(int blue);
};

#endif // SCENE_H
