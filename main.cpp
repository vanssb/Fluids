#include "mainwindow.h"
#include <QApplication>
#include <QSurfaceFormat>
#include <scene.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setSamples(8);
    QSurfaceFormat::setDefaultFormat(format);
    //w.setWindowFlags(Qt::WindowStaysOnTopHint);
    w.setFocusPolicy(Qt::StrongFocus);
    w.show();
    return a.exec();
}
