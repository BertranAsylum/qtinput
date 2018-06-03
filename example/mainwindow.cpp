#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <qtinput/drag-controller.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    qtinput::DragController::attach(ui->spinBox, 0.05);
    qtinput::DragController::attach(ui->courseAngleEdit, 270.0, -90.0);
    qtinput::DragController::attach(ui->courseEdit, -270.0, 90.0);
    qtinput::DragController::attach(ui->timeEdit, true, 1.0);
}

MainWindow::~MainWindow() {
    delete ui;
}
