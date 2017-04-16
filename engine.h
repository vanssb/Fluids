#ifndef ENGINE_H
#define ENGINE_H

#include "particle.h"
#include <QTimer>
#include <vector>
#include <QMap>
#include <QOpenGLShaderProgram>
#include <QTimer>
#include <QTime>
#include "types.h"

class Engine : public QObject{
    Q_OBJECT
private:
    std::vector<Particle*> particles;
    QTimer timer;
    std::vector<Plane> planes;
    void solve();
    int physTime;
    QTime time;
    void collide(Particle* p1, Particle* p2);
    void collideBorderMap();
    QMap<int,std::vector<Particle*>> collideMap;
    int getHash(QVector3D pos);
    void generateCollideMap();
    std::vector<Particle*> getNeighbors(Particle* p);
    int cellSize;
public:
    Engine();
    ~Engine();
    void initialize();
    std::vector<Particle*> getObjectsData();
    std::vector<Plane> getPlanes();
    static int sortFunc(const void *n1, const void *n2);
private slots:
    void move();
public slots:
    void pushParticle();
signals:
    void countChanged(int count);
    void timeChanged(int msec);
};



#endif // ENGINE_H
