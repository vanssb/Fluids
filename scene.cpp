#include "scene.h"
#include <QOpenGLShader>
#include "loader.h"
#include <QCoreApplication>
#include <QImage>



Scene::Scene(QWidget *parent) : QOpenGLWidget(parent){
    engine.initialize();
    thread.start();
    //engine.moveToThread(&thread);

}

Scene::~Scene(){
    makeCurrent();
    // Describe the removal of buffers
    data["object"].vertex.clear();
    data["object"].index.clear();
    data["object"].vertexBuffer->destroy();
    delete data["object"].vertexBuffer;
    data["object"].indexBuffer->destroy();
    delete data["object"].indexBuffer;
    mMatrixesBuffer->destroy();
    delete mMatrixesBuffer;
    doneCurrent();
    thread.terminate();
}

void Scene::timerEvent(QTimerEvent *){
    update();
}

void Scene::initializeGL(){
    initializeOpenGLFunctions();
    camera.setPosition(QVector3D(0.0f,200.0f, -500.0f));
    //Preparing shader program and attributes for objects
    QOpenGLShader vShader(QOpenGLShader::Vertex);
    vShader.compileSourceFile(":/Shaders/vShader.glsl");
    QOpenGLShader fShader(QOpenGLShader::Fragment);
    fShader.compileSourceFile(":/Shaders/fShader.glsl");
    program.addShader(&vShader);
    program.addShader(&fShader);
    if ( !program.link() )
    {
        qWarning( "Error: unable to link a shader program." );
        return;
    }
    //Linking attributes
    attributes["fragmentPos"] = program.attributeLocation("vertexPos");
    attributes["fragmentUV"] = program.attributeLocation("vertexUV");
    attributes["fragmentNorm"] = program.attributeLocation("vertexNorm");

    //attributes["M"] = program.attributeLocation( "M" );
    attributes["M"] = program.uniformLocation( "M" );
    attributes["V"] = program.uniformLocation( "V" );
    attributes["P"] = program.uniformLocation( "P" );

    attributes["textureUniform"] = program.uniformLocation("textureUniform");

    attributes["cameraPosUniform"] = program.uniformLocation( "cameraPos" );

    attributes["pointUniform"] = program.uniformLocation( "pointLight.position" );
    attributes["pointColorUniform"] = program.uniformLocation( "pointLight.color" );
    attributes["pointAmbientUniform"] = program.uniformLocation( "pointLight.ambientIntensity" );
    attributes["pointDiffuseUniform"] = program.uniformLocation( "pointLight.diffuseIntensity" );
    attributes["pointSpecularUniform"] = program.uniformLocation( "pointLight.specularIntensity" );

    attributes["materialSpecularFactorUniform"] = program.uniformLocation( "materialSpecularFactor" );
    attributes["materialEmissionUniform"] = program.uniformLocation( "materialEmission" );

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
    /*
    vao = new QOpenGLVertexArrayObject(this);
    vao->create();
    vao->bind();
    */
    QOpenGLBuffer* vertexBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    QOpenGLBuffer* indexBuffer = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    //vertexBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    //indexBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
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
    //Creating buffer for matrixes
    /*
    mMatrixesBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    mMatrixesBuffer->setUsagePattern(QOpenGLBuffer::DynamicDraw);
    mMatrixesBuffer->create();
    mMatrixesBuffer->bind();
    mMatrixesBuffer->allocate(mMatrixes.data(), mMatrixes.size() * 16 * sizeof(float) );
    vao->release();
    */
    // Map
    // Loading model
    /*
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
    */
    //Loading common texture
    QImage image(QCoreApplication::applicationDirPath()+"/Textures/Common.jpg" );
    texture = new QOpenGLTexture( image.mirrored() );
    //Opengl own settings
    // Enable depth buffer
    this->glEnable(GL_DEPTH_TEST);

    // Setting clear color
    this->glClearColor(0.0f,0.0f,0.0f,0.0f);

    this->glPolygonMode(GL_FRONT, GL_POLYGON);
    // Enable back face culling
    this->glEnable(GL_CULL_FACE);
    this->glCullFace(GL_FRONT);
    //Light settings
    color = QVector3D(1.0f,1.0f,1.0f);
    ambient = 0.0f;
    diffuse = 0.0f;
    specular = 0.0f;
    timer.start(12,this);
}

void Scene::paintGL(){
    time.start();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if ( !program.bind() ){
        qWarning("Cant bind shader program");
        return;
    }
    texture->bind();

    drawObjects();
    //drawMap();

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
    std::vector<Particle*> objects = engine.getObjectsData();
    data["object"].indexBuffer->bind();
    data["object"].vertexBuffer->bind();

    int indexSize = data["object"].index.size();
    /*
     mMatrixes.clear();
    for (unsigned int i = 0; i < objects.size(); i++){
        mMatrix.setToIdentity();
        mMatrix.translate(objects[i]->getPosition());
        mMatrix.scale(objects[i]->getRadius());
        mMatrixes.push_back( mMatrix );
    }

    mMatrixesBuffer->bind();
    mMatrixesBuffer->allocate(mMatrixes.data()->constData(), mMatrixes.size() * 4 * sizeof(QVector4D));
    //mMatrixesBuffer->release();
    int sizeBuffer = mMatrixesBuffer->size();

    program.setUniformValue( attributes["P"], pMatrix );
    program.setUniformValue( attributes["V"], camera.getMatrix() );
    program.setUniformValue( attributes["textureUniform"], 0 );

    setLightSettings();

    quintptr offset = 0;

    program.enableAttributeArray( attributes["vertexPos"] );
    program.setAttributeBuffer( attributes["vertexPos"], GL_FLOAT, offset, 3, sizeof(VertexData) );

    offset += sizeof(QVector3D);

    program.enableAttributeArray( attributes["vertexUV"]);
    program.setAttributeBuffer( attributes["vertexUV"], GL_FLOAT, offset, 2, sizeof(VertexData) );

    offset += sizeof(QVector2D);

    program.enableAttributeArray( attributes["vertexNorm"] );
    program.setAttributeBuffer( attributes["vertexNorm"], GL_FLOAT, offset, 3, sizeof(VertexData) );


    this->glDrawElementsInstanced(GL_TRIANGLES,indexSize, GL_FLOAT, 0, objects.size() );
    vao->release();
    */
    for (unsigned int i = 0; i < objects.size(); i++){
        mMatrix.setToIdentity();
        mMatrix.translate(objects[i]->getPosition());
        mMatrix.scale(objects[i]->getRadius());

        program.setUniformValue( attributes["P"], pMatrix );
        program.setUniformValue( attributes["V"], camera.getMatrix() );
        program.setUniformValue( attributes["M"], mMatrix );
        program.setUniformValue( attributes["textureUniform"], 0 );

        setLightSettings();

        quintptr offset = 0;

        program.enableAttributeArray( attributes["vertexPos"] );
        program.setAttributeBuffer( attributes["vertexPos"], GL_FLOAT, offset, 3, sizeof(VertexData) );

        offset += sizeof(QVector3D);

        program.enableAttributeArray( attributes["vertexUV"]);
        program.setAttributeBuffer( attributes["vertexUV"], GL_FLOAT, offset, 2, sizeof(VertexData) );

        offset += sizeof(QVector2D);

        program.enableAttributeArray( attributes["vertexNorm"] );
        program.setAttributeBuffer( attributes["vertexNorm"], GL_FLOAT, offset, 3, sizeof(VertexData) );
        this->glDrawElements(GL_TRIANGLES, indexSize , GL_UNSIGNED_SHORT, 0);
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

    this->glDrawElements(GL_TRIANGLES, data["map"].index.size() , GL_UNSIGNED_SHORT, 0);

    data["map"].vertexBuffer->release();
    data["map"].indexBuffer->release();
}

void Scene::setLightSettings(){
    program.setUniformValue( attributes["cameraPos"], camera.getPosition() );

    program.setUniformValue( attributes["pointUniform"], QVector3D(0.0f,100.0f,150.0f) );
    program.setUniformValue( attributes["pointColorUniform"], color );
    program.setUniformValue( attributes["pointAmbientUniform"], ambient ); // Мощность фонового освещения
    program.setUniformValue( attributes["pointDiffuseUniform"], diffuse ); // Мощность рассеяного освещения
    program.setUniformValue( attributes["pointSpecularUniform"], specular ); // Мощность отраженного освещения

    program.setUniformValue( attributes["materialSpecularFactorUniform"], 0.1f );
    program.setUniformValue( attributes["materialEmissionUniform"], 0.1f, 0.1f, 0.1f );
}

void Scene::setAmbient(int ambient){
    this->ambient = ambient;
}

void Scene::setDiffuse(int diffuse){
    this->diffuse = diffuse;
}

void Scene::setSpecular(int specular){
    this->specular = specular;
}

void Scene::setColorR(int red){
    color.setX( red / 100.0f );
}

void Scene::setColorG(int green){
    color.setY( green / 100.0f );
}

void Scene::setColorB(int blue){
    color.setZ( blue / 100.0f );
}
