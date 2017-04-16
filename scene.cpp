#include "scene.h"
#include <QOpenGLShader>
#include "loader.h"
#include <QCoreApplication>
#include <QImage>


Scene::Scene(QWidget *parent) : QOpenGLWidget(parent){
    engine.initialize();
    thread.start();
    //engine.moveToThread(&thread);
    camera.setPosition(QVector3D(0.0f,200.0f, -500.0f));
}

Scene::~Scene(){
    makeCurrent();
    // Describe the removal of buffers

    doneCurrent();
    thread.terminate();
}

void Scene::timerEvent(QTimerEvent *){
    update();
}

void Scene::initializeGL(){
    initializeOpenGLFunctions();

    //Preparing shader program and attributes for objects
    QOpenGLShader vShader(QOpenGLShader::Vertex);
    vShader.compileSourceFile(":/Shaders/vShader.glsl");
    QOpenGLShader fShader(QOpenGLShader::Fragment);
    fShader.compileSourceFile(":/Shaders/fShader.glsl");
    program.addShader(&vShader);
    program.addShader(&fShader);
    program.link();
    //Linking attributes
    attributes["vertex"] = program.attributeLocation("vertex");
    attributes["uv"] = program.attributeLocation("uv");
    attributes["MVP"] = program.uniformLocation("MVP");
    attributes["uvUniform"] = program.uniformLocation("uvUniform");
    //Loading model for borders
    //Loader::loadModel(QString(QCoreApplication::applicationDirPath()+"/Models/Cube.obj"), mapVBO,mapVAO,mapUVs);
    // Preparing VBOs and loading data
    std::vector<VertexData> vertex;
    std::vector<GLushort> index;
    // Object
    // Loading model
    Loader::loadModel(QString(QCoreApplication::applicationDirPath()+"/Models/Sphere.obj"), vertex,index);
    data["object"].vertex = vertex;
    data["object"].index = index;
    // Creating buffers
    QOpenGLBuffer* vertexBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    QOpenGLBuffer* indexBuffer = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    data["object"].vertexBuffer = vertexBuffer;
    data["object"].indexBuffer = indexBuffer;
    vertexBuffer->create();
    indexBuffer->create();
    // Memory allocating for vertices
    vertexBuffer->bind();
    vertexBuffer->allocate( vertex.data(), vertex.size() * sizeof(VertexData));
    // Memory allocating for indexes
    indexBuffer->bind();
    indexBuffer->allocate(index.data(), index.size() * sizeof(GLushort) );
    // Map
    // Loading model
    Loader::loadModel(QString(QCoreApplication::applicationDirPath()+"/Models/Map.obj"), vertex,index);
    data["map"].vertex = vertex;
    data["map"].index = index;
    // Creating buffers
    vertexBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    indexBuffer = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    data["map"].vertexBuffer = vertexBuffer;
    data["map"].indexBuffer = indexBuffer;
    vertexBuffer->create();
    indexBuffer->create();
    // Memory allocating for vertices
    vertexBuffer->bind();
    vertexBuffer->allocate( vertex.data(), vertex.size() * sizeof(VertexData));
    // Memory allocating for indexes
    indexBuffer->bind();
    indexBuffer->allocate(index.data(), index.size() * sizeof(GLushort) );
    //Loading common texture
    QImage image(QCoreApplication::applicationDirPath()+"/Textures/Common.jpg" );
    texture = new QOpenGLTexture( image.mirrored() );
    //Opengl own settings
    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);
    // Enable back face culling
    glEnable(GL_CULL_FACE);
    // Setting clear color
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    timer.start(12,this);
}

void Scene::paintGL(){
    time.start();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    program.bind();
    texture->bind();

    drawObjects();
    drawMap();

    texture->release();
    program.release();

    emit renderTimeChanged(time.elapsed());
}



void Scene::resizeGL(int w, int h){
    //Матрица проекции, использует: угол обзора, соотношение сторон и ближнюю и дальнюю стенки проекции
        float aspect = float(w) / float(h ? h : 1);
        const float zNear = 0.1f, zFar = 1550.0f, fov = 60.0;
        pMatrix.setToIdentity();
        pMatrix.perspective(fov, aspect, zNear, zFar);
}

void Scene::drawObjects(){
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    std::vector<Particle*> objects = engine.getObjectsData();

    data["object"].vertexBuffer->bind();
    data["object"].indexBuffer->bind();

    int indexSize = data["object"].index.size();

    for (unsigned int i = 0; i < objects.size(); i++){
        mMatrix.setToIdentity();
        mMatrix.translate(objects[i]->getPosition());
        mMatrix.scale(objects[i]->getRadius());
        program.setUniformValue( attributes["MVP"], pMatrix * camera.getMatrix() * mMatrix );
        program.setUniformValue( attributes["uvUniform"], 0 );

        quintptr offset = 0;

        program.enableAttributeArray( attributes["vertex"] );
        program.setAttributeBuffer( attributes["vertex"], GL_FLOAT, offset, 3, sizeof(VertexData) );

        offset += sizeof(QVector3D);

        program.enableAttributeArray( attributes["uv"]);
        program.setAttributeBuffer( attributes["uv"], GL_FLOAT, offset, 2, sizeof(VertexData) );

        glDrawElements(GL_TRIANGLES, indexSize , GL_UNSIGNED_SHORT, 0);

    }
    data["object"].vertexBuffer->release();
    data["object"].indexBuffer->release();
}

void Scene::drawMap(){
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDisable(GL_CULL_FACE);
    data["map"].vertexBuffer->bind();
    data["map"].indexBuffer->bind();

    mMatrix.setToIdentity();
    mMatrix.scale(160.0f);

    program.setUniformValue( attributes["MVP"], pMatrix * camera.getMatrix() * mMatrix );
    program.setUniformValue( attributes["uvUniform"], 0 );

    quintptr offset = 0;

    program.enableAttributeArray( attributes["vertex"] );
    program.setAttributeBuffer( attributes["vertex"], GL_FLOAT, offset, 3, sizeof(VertexData) );

    offset += sizeof(QVector3D);

    program.enableAttributeArray( attributes["uv"]);
    program.setAttributeBuffer( attributes["uv"], GL_FLOAT, offset, 2, sizeof(VertexData) );

    glDrawElements(GL_TRIANGLES, data["map"].index.size() , GL_UNSIGNED_SHORT, 0);

    data["map"].vertexBuffer->release();
    data["map"].indexBuffer->release();
}
