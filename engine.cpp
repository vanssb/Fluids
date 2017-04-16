#include "engine.h"
#include <math.h>
#include <QTime>
#include <omp.h>

Engine::Engine(){
    connect(&timer, SIGNAL(timeout()), this, SLOT(move()) );
    timer.start(1000/60);
    cellSize = 7;
}

void Engine::initialize(){
    //Generate limiting borders
    //bottom plane
    Plane plane;
    plane.plane = QVector3D(0.0f, -160.0f, 0.0f);
    plane.normal = QVector3D(0.0f, 1.0f, 0.0f).normalized();
    planes.push_back(plane);
    //top plane
    plane.plane = QVector3D(0.0f,160.0f,0.0f);
    plane.normal = QVector3D(0.0f,-1.0f,0.0f).normalized();
    planes.push_back(plane);
    //left plane
    plane.plane = QVector3D(-160.0f, 0.0f, 0.0f);
    plane.normal = QVector3D(1.0f,0.0f, 0.0f).normalized();
    planes.push_back(plane);
    //right plane
    plane.plane = QVector3D(160.0f, 0.0f, 0.0f);
    plane.normal = QVector3D(-1.0f,0.0f,0.0f).normalized();
    planes.push_back(plane);
    //near plane
    plane.plane = QVector3D( 0.0f, 0.0f, -160.0f);
    plane.normal = QVector3D( 0.0f,0.0f, 1.0f).normalized();
    planes.push_back(plane);
    //far plane
    plane.plane = QVector3D( 0.0f, 0.0f, 160.0f);
    plane.normal = QVector3D( 0.0f, 0.0f, -1.0f).normalized();
    planes.push_back(plane);
    //near inclined plane
    plane.plane = QVector3D( 0.0f, -80.0f, -160.0f);
    plane.normal = QVector3D( 0.0f,1.0f, 1.0f).normalized();
    planes.push_back(plane);
    //far inclined plane
    plane.plane = QVector3D( 0.0f, -80.0f, 160.0f);
    plane.normal = QVector3D( 0.0f, 1.0f, -1.0f).normalized();
    planes.push_back(plane);
    //right inclined plane
    plane.plane = QVector3D(160.0f, -80.0f, 0.0f);
    plane.normal = QVector3D(-1.0f,1.0f,0.0f).normalized();
    planes.push_back(plane);
    //left inclined plane
    plane.plane = QVector3D(-160.0f, -80.0f, 0.0f);
    plane.normal = QVector3D(1.0f,1.0f, 0.0f).normalized();
    planes.push_back(plane);


}

void Engine::pushParticle(){
    Particle* p = new Particle(6.0f,QVector3D(5.0f,70.0f,0.1f),QVector3D(1.0f,0.0f,0.1f));
    particles.push_back(p);
    p = new Particle(6.0f,QVector3D(5.0f,65.0f,0.1f),QVector3D(1.0f,0.0f,0.0f));
    particles.push_back(p);
    p = new Particle(6.0f,QVector3D(5.0f,60.0f,0.0f),QVector3D(1.0f,0.0f,0.1f));
    particles.push_back(p);
    p = new Particle(6.0f,QVector3D(5.0f,55.0f,0.0f),QVector3D(1.0f,0.0f,0.0f));
    particles.push_back(p);
    p = new Particle(6.0f,QVector3D(5.0f,50.0f,0.0f),QVector3D(1.0f,0.0f,0.2f));
    particles.push_back(p);
    emit countChanged(particles.size());
}

Engine::~Engine(){
    for (unsigned int i = 0; i < particles.size(); i++){
        delete particles[i];
    }
    particles.clear();
}

void Engine::move(){
    time.start();
    #pragma omp parallel for
    for (unsigned int i = 0; i < particles.size(); i++){
        particles[i]->applyAcceleration();
        particles[i]->move();
    }
    solve();
    emit timeChanged(time.elapsed());
}

//Solving all collisions
void Engine::solve(){
    //Solving collisions with each other
    if (particles.size() > 1){
        //First variant: axis sort
        std::qsort( &particles[0], particles.size(),sizeof(Particle*), sortFunc );
        #pragma omp parallel for
        for(unsigned int i = 0; i < particles.size() - 1; i++){
            for(unsigned int j = i + 1; ((j < particles.size()) && (particles[j]->getPosition().x() - particles[j]->getRadius() < particles[i]->getPosition().x() + particles[i]->getRadius())); j++){
                collide(particles[i],particles[j]);
            }
        }

        //Second variant: spatial hasing
        /*
        generateCollideMap();

        for(unsigned int i = 0; i < particles.size(); i++){
            std::vector<Particle*> neighbors = getNeighbors(particles[i]);
            #pragma omp parallel for
            for(unsigned int j = 0; j < neighbors.size(); j++){
                collide(particles[i],neighbors[j]);
            }
        }
        */
    }
    //Solving collisions with limiting borders
    collideBorderMap();
}

//Solving collision of two balls
void Engine::collide(Particle* p1, Particle* p2){
    QVector3D pos1 = p1->getPosition();
    QVector3D pos2 = p2->getPosition();
    if( (pos1 - pos2).length() < p1->getRadius() + p2->getRadius()){
    // Distance between balls' centers
    // less then sum of their radiuses -> they interseted
    // It is more convenient to push the balls along a straight line passing through their centers
        QVector3D penetrationDirection = (pos2 - pos1).normalized();
        // Penetration depth = sum of balls' radiuses - lenght between their centers
        float penetrationDepth = p1->getRadius() +
                             p2->getRadius() - (pos2 - pos1).length();
        //Push the balls in opposite directions
        p1->push( -penetrationDirection * penetrationDepth * 0.5f);
        p2->push( penetrationDirection * penetrationDepth * 0.5f);
        float bounce = -0.1f; //Bounce factor, [0..1]
        //relative velocity of the balls
        QVector3D relativeVelocity = p1->getDelta() - p2->getDelta();
        //Objects exchange a pulse along the normal of contact
        float exchangeVelocity = (1.0f + bounce) * QVector3D::dotProduct(relativeVelocity, penetrationDirection);
        if(exchangeVelocity > 0){
            p1->setDelta( p1->getDelta() + penetrationDirection * exchangeVelocity * 0.5f );
            p2->setDelta( p2->getDelta() - penetrationDirection * exchangeVelocity * 0.5f );
        }
    }
}

//Solving collisions with limiting borders
void Engine::collideBorderMap(){
    for (unsigned int k = 0; k < planes.size(); k++){
        #pragma omp parallel for
        for(unsigned int i = 0; i < particles.size(); i++){
            //Calculate distance to plane
            float distance = fabs( particles[i]->getPosition().distanceToPlane( planes[k].plane, planes[k].normal ) );
            if ( distance <= particles[i]->getRadius() ){
                //Calculate length of penetration vector ( scalar composition delta on plane's normal )
                QVector3D newDelta( 0.0f, 0.0f, 0.0f );
                QVector3D delta = particles[i]->getDelta();
                newDelta = ( fabs( QVector3D::dotProduct(delta, planes[k].normal) ) ) * planes[k].normal;
                particles[i]->push( (1.0f + particles[i]->getRadius() * Particle::dt ) * newDelta );
            }
        }
    }
}

int Engine::sortFunc(const void *n1, const void *n2){
    Particle *p1 = *((Particle **)n1);
    Particle *p2 = *((Particle **)n2);
    float d1 = p1->getPosition().x() - p1->getRadius();
    float d2 = p2->getPosition().x() - p2->getRadius();
    return (d1 > d2) ? 1 : -1;
}

std::vector<Particle*> Engine::getObjectsData(){
    return particles;
}

std::vector<Plane> Engine::getPlanes(){
    return planes;
}

int Engine::getHash( QVector3D pos ){
    return ((int)( floor( pos.x() / cellSize ) * 73856093 ) ^
            (int)( floor( pos.y() / cellSize ) * 19349663 ) ^
            (int)( floor( pos.z() / cellSize ) * 83492791 ));
}

void Engine::generateCollideMap(){
    collideMap.clear();
    //#pragma omp parallel for
    for (unsigned int i = 0; i < particles.size(); i++){
        QVector3D p = particles[i]->getPosition();
        int hash = getHash(p);
        collideMap[hash].push_back(particles[i]);
    }
}

std::vector<Particle*> Engine::getNeighbors(Particle* p){
    QVector3D pos = p->getPosition();
    QVector3D nPos = pos + QVector3D(0,0,0);
    std::vector<Particle*> result;
    std::vector<QVector3D> neighbors;
    nPos = pos + QVector3D(cellSize,0,0);
    neighbors.push_back(nPos);
    nPos = pos + QVector3D(-cellSize,0,0);
    neighbors.push_back(nPos);
    nPos = pos + QVector3D(0,cellSize,0);
    neighbors.push_back(nPos);
    nPos = pos + QVector3D(0,-cellSize,0);
    neighbors.push_back(nPos);
    nPos = pos + QVector3D(0,0,cellSize);
    neighbors.push_back(nPos);
    nPos = pos + QVector3D(0,0,-cellSize);
    neighbors.push_back(nPos);
    //
    nPos = pos + QVector3D(cellSize,cellSize,0);
    neighbors.push_back(nPos);
    nPos = pos + QVector3D(-cellSize,cellSize,0);
    neighbors.push_back(nPos);
    nPos = pos + QVector3D(cellSize,-cellSize,0);
    neighbors.push_back(nPos);
    nPos = pos + QVector3D(-cellSize,-cellSize,0);
    neighbors.push_back(nPos);
    //
    nPos = pos + QVector3D(cellSize,cellSize,cellSize);
    neighbors.push_back(nPos);
    nPos = pos + QVector3D(-cellSize,cellSize,cellSize);
    neighbors.push_back(nPos);
    nPos = pos + QVector3D(cellSize,-cellSize,cellSize);
    neighbors.push_back(nPos);
    nPos = pos + QVector3D(-cellSize,-cellSize,cellSize);
    neighbors.push_back(nPos);
    nPos = pos + QVector3D(cellSize,0,cellSize);
    neighbors.push_back(nPos);
    nPos = pos + QVector3D(-cellSize,0,cellSize);
    neighbors.push_back(nPos);
    nPos = pos + QVector3D(0,cellSize,cellSize);
    neighbors.push_back(nPos);
    nPos = pos + QVector3D(0,-cellSize,cellSize);
    neighbors.push_back(nPos);
    //
    nPos = pos + QVector3D(cellSize,cellSize,-cellSize);
    neighbors.push_back(nPos);
    nPos = pos + QVector3D(-cellSize,cellSize,-cellSize);
    neighbors.push_back(nPos);
    nPos = pos + QVector3D(cellSize,-cellSize,-cellSize);
    neighbors.push_back(nPos);
    nPos = pos + QVector3D(-cellSize,-cellSize,-cellSize);
    neighbors.push_back(nPos);
    nPos = pos + QVector3D(cellSize,0,-cellSize);
    neighbors.push_back(nPos);
    nPos = pos + QVector3D(-cellSize,0,-cellSize);
    neighbors.push_back(nPos);
    nPos = pos + QVector3D(0,cellSize,-cellSize);
    neighbors.push_back(nPos);
    nPos = pos + QVector3D(0,-cellSize,-cellSize);
    neighbors.push_back(nPos);
    //
    for (unsigned int i = 0; i < neighbors.size(); i++){
        int hashCode = getHash(neighbors[i]);
        std::vector<Particle*> v = collideMap[hashCode];
        for (unsigned int j = 0; j < v.size(); j++) result.push_back(v[j]);
    }
    return result;
}
