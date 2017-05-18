#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    int oldPosX, oldPosY, newPosX, newPosY;
public slots:
//Moving slots
    void keyPressEvent(QKeyEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
//Slots for basic information updating
    void countChanged(int count);
    void timeChanged(int msec );
    void renderTimeChanged(int msec);
    void basicInformationShow();
    void lightSettingsShow();
    void cameraAnglesChanged();
    void cameraPositionChanged();
};

#endif // MAINWINDOW_H
