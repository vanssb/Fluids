#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect( &ui->scene->engine, SIGNAL(countChanged(int)), this, SLOT(countChanged(int)) );
    connect( &ui->scene->engine, SIGNAL(timeChanged(int)), this, SLOT(timeChanged(int)) );
    connect( ui->scene, SIGNAL(renderTimeChanged(int)), this, SLOT(renderTimeChanged(int)) );
    connect( ui->basicInfoCheck, SIGNAL(clicked(bool)), this, SLOT(basicInformationShow()) );
    connect( ui->lightSettingsCheck, SIGNAL(clicked(bool)), this, SLOT(lightSettingsShow()) );
    connect ( &ui->scene->camera, SIGNAL(positionChanged()), this, SLOT(cameraPositionChanged()) );
    connect ( &ui->scene->camera, SIGNAL(anglesChanged()), this, SLOT(cameraAnglesChanged()) );
    connect ( ui->ambientSlider, SIGNAL(valueChanged(int)), ui->scene, SLOT(setAmbient(int)) );
    connect ( ui->diffuseSlider, SIGNAL(valueChanged(int)), ui->scene, SLOT(setDiffuse(int)) );
    connect ( ui->specularSlider, SIGNAL(valueChanged(int)), ui->scene, SLOT(setSpecular(int)) );
    connect ( ui->redComponentSlider, SIGNAL(valueChanged(int)), ui->scene, SLOT(setColorR(int)) );
    connect ( ui->greenComponentSlider, SIGNAL(valueChanged(int)), ui->scene, SLOT(setColorG(int)) );
    connect ( ui->blueComponentSlider, SIGNAL(valueChanged(int)), ui->scene, SLOT(setColorB(int)) );

    this->setMouseTracking( true );
    ui->centralWidget->setMouseTracking( true );
    oldPosX = pos().x() + ui->scene->pos().x() + ui->scene->width() / 2.0f;
    oldPosY = pos().y() + ui->scene->pos().y() + ui->scene->height() / 2.0f;
    QCursor::setPos( oldPosX, oldPosY );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event){
    switch ( event->key() ) {
    case Qt::Key_N:
        ui->scene->engine.pushParticle();
    break;
    case Qt::Key_Escape:
        this->close();
    break;
    case Qt::Key_Up:
        ui->scene->camera.rotateUp();      // повернуть сцену вверх
    break;
    case Qt::Key_Down:
        ui->scene->camera.rotateDown();    // повернуть сцену вниз
    break;
    case Qt::Key_A:
        ui->scene->camera.rotateLeft();     // повернуть сцену влево
    break;
    case Qt::Key_D:
        ui->scene->camera.rotateRight();   // повернуть сцену вправо

    break;
    case Qt::Key_W:
        ui->scene->camera.moveForward();  //перемещение камеры вперед

    break;
    case Qt::Key_S:
        ui->scene->camera.moveBack();     //перемещение камеры назад
    break;
    }
}

void MainWindow::countChanged(int count){
    ui->objectsCount->display(count);
}

void MainWindow::timeChanged(int msec){
    ui->physTime->display(msec);
}

void MainWindow::renderTimeChanged(int msec){
    ui->renderTime->display(msec);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event){
    newPosX = event->globalPos().x();
    newPosY = event->globalPos().y();
    if ( newPosX  == oldPosX && newPosY == oldPosY ) return;
//Смещение старой позиции если курсор заблокирова
        oldPosX = pos().x() + ui->scene->pos().x() + ui->scene->width()/2.0f;
        oldPosY = pos().y() + ui->scene->pos().y() + ui->scene->height()/2.0f;
//Блок проверки смещения
    if ( newPosX > oldPosX ){
        ui->scene->camera.rotateRight();
    }else if ( newPosX < oldPosX ){
        ui->scene->camera.rotateLeft();
    }
    if ( newPosY > oldPosY ){
        ui->scene->camera.rotateDown();
    }else if( newPosY < oldPosY ){
        ui->scene->camera.rotateUp();
    }
//Возврат курсора если он заблокирован
    QCursor::setPos( oldPosX, oldPosY );
    ui->hRot->display(ui->scene->camera.getHorizontalRotation());
    ui->vRot->display(ui->scene->camera.getVerticalRotation());
}

void MainWindow::basicInformationShow(){
    ui->basicInfoGroup->setVisible( ui->basicInfoCheck->isChecked() );
}

void MainWindow::cameraAnglesChanged(){
    ui->hRot->display(ui->scene->camera.getHorizontalRotation());
    ui->vRot->display(ui->scene->camera.getVerticalRotation());
}

void MainWindow::cameraPositionChanged(){
    ui->xPosition->display(ui->scene->camera.getPosition().x());
    ui->yPosition->display(ui->scene->camera.getPosition().y());
    ui->zPosition->display(ui->scene->camera.getPosition().z());
}

void MainWindow::lightSettingsShow(){
    ui->lightSettingsGroup->setVisible( ui->lightSettingsCheck->isChecked() );
}

