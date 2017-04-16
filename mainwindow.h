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
    void countChanged(int count);
    void timeChanged(int msec );
    void keyPressEvent(QKeyEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void renderTimeChanged(int msec);
};

#endif // MAINWINDOW_H
