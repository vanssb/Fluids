#ifndef TYPES_H
#define TYPES_H

#include <QVector3D>
#include <QVector2D>
#include <vector>
#include <qopengl.h>
#include <QOpenGLBuffer>

struct Plane{
    QVector3D plane;
    QVector3D normal;
};

struct VertexData
{
    QVector3D position;
    QVector2D uv;
    QVector3D normal;
};

struct ObjectData{
    std::vector<VertexData> vertex;
    std::vector<GLushort> index;
    QOpenGLBuffer* vertexBuffer;
    QOpenGLBuffer* indexBuffer;
};


#endif // TYPES_H

