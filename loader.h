#ifndef LOADER_H
#define LOADER_H

#include <vector>
#include <QString>
#include <QVector3D>
#include <types.h>
#include <qopengl.h>

class Loader
{
public:
    Loader();
    static void loadModel(QString fileName,
                          std::vector<VertexData> &vertex,
                          std::vector<GLushort> &vertexIndex);
};

#endif // LOADER_H
