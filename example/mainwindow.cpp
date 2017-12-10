#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <qt-input-tools/drag-controller.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    QtInputTools::DragController::attach(ui->spinBox, QtInputTools::DragController::LINEAR, 0.05);
    QtInputTools::DragController::attach(ui->courseAngleEdit, QtInputTools::DragController::CIRCULAR_SYM);
    QtInputTools::DragController::attach(ui->courseEdit, QtInputTools::DragController::CIRCULAR);
    QtInputTools::DragController::attach(ui->timeEdit, QtInputTools::DragController::CIRCULAR_INF);
}

MainWindow::~MainWindow() {
    delete ui;
}
