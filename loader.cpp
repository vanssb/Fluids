#include "loader.h"
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QStringList>
#include <vector>

Loader::Loader()
{

}

void Loader::loadModel(QString fileName,
                       std::vector<VertexData> &vertex,
                       std::vector<GLushort> &vertexIndex){
    QFile f( fileName );
    QString str;
    if (!f.open(QIODevice::ReadOnly)){
        return;
    }
    vertex.clear();
    vertexIndex.clear();
    QTextStream stream(&f);
    std::vector<QVector2D> uv;
    std::vector<QVector3D> normalArray;
    std::vector<int> normalSequence;

    while( !stream.atEnd() ){
        str = stream.readLine();
        if ( str[0] == 'v' && str[1] == ' ' ){
            QList<QString> cords = str.split(" ",QString::SkipEmptyParts);
            VertexData v;
            v = { QVector3D(cords[1].toFloat(),cords[2].toFloat(),cords[3].toFloat()),QVector2D() };
            vertex.push_back(v);
        }
        if ( str[0] == 'v' && str[1] == 't' ){
            QList<QString> cords = str.split(" ",QString::SkipEmptyParts);
            uv.push_back( QVector2D( cords[1].toFloat(), cords[2].toFloat() ) );
        }
        if (str[0] == 'f'){
            QList<QString> numbers = str.split( QRegExp( "(\\ |\\/)" ),QString::SkipEmptyParts );

            int indexVertex = numbers[1].toShort() - 1;
            int indexUV = numbers[2].toShort() - 1;
            vertexIndex.push_back(indexVertex);
            vertex[indexVertex].uv = uv[indexUV];

            indexVertex = numbers[3].toShort() - 1;
            indexUV = numbers[4].toShort() - 1;
            vertexIndex.push_back(indexVertex);
            vertex[indexVertex].uv = uv[indexUV];

            indexVertex = numbers[5].toShort() - 1;
            indexUV = numbers[6].toShort() - 1;
            vertexIndex.push_back(indexVertex);
            vertex[indexVertex].uv = uv[indexUV];
        }
    }
}
