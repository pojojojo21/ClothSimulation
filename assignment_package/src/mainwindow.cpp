#include "mainwindow.h"
#include <ui_mainwindow.h>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mygl->setFocus();

    // Connect button click signals to functions
    connect(ui->resetClothButton, &QPushButton::clicked, this, &MainWindow::on_resetClothButton_clicked);
    connect(ui->dropClothButton, &QPushButton::clicked, this, &MainWindow::on_dropClothButton_clicked);
    connect(ui->drawTypeSelection, &QComboBox::currentIndexChanged, this, &MainWindow::on_drawTypeSelection_changed);
    connect(ui->objTypeSelection, &QComboBox::currentIndexChanged, this, &MainWindow::on_objTypeSelection_changed);
    connect(ui->Spin_W, &QSpinBox::valueChanged, this, &MainWindow::on_width_changed);
    connect(ui->Spin_H, &QSpinBox::valueChanged, this, &MainWindow::on_height_changed);
    connect(ui->Spin_SS, &QDoubleSpinBox::valueChanged, this, &MainWindow::on_spacing_changed);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::exit();
}

void MainWindow::on_actionReset_triggered()
{
    ui->mygl->resetCloth();
}

void MainWindow::on_resetClothButton_clicked()
{
    ui->mygl->resetCloth();
}

void MainWindow::on_dropCornerButton_clicked()
{
    ui->mygl->dropCorner();
}

void MainWindow::on_dropClothButton_clicked()
{
    ui->mygl->dropCloth();
}

void MainWindow::on_drawTypeSelection_changed(int index)
{
    ui->mygl->setDrawType(index);
}

void MainWindow::on_objTypeSelection_changed(int index)
{
    ui->mygl->setObjType(index);
}

void MainWindow::on_width_changed(int width)
{
    ui->mygl->changeCloth(true, width, 0, 0, 0, 0);
    ui->mygl->changeBox(true, width, 0, 0, 0, 0, 0, 0);
}

void MainWindow::on_height_changed(int height)
{
    ui->mygl->changeCloth(0, 0, true, height, 0, 0);
    ui->mygl->changeBox(0, 0, true, height, 0, 0, 0, 0);
}

void MainWindow::on_depth_changed(int depth)
{
    ui->mygl->changeBox(0, 0, 0, 0, true, depth, 0, 0);
}

void MainWindow::on_spacing_changed(float spacing)
{
    ui->mygl->changeCloth(0, 0, 0 , 0, true, spacing);
    ui->mygl->changeBox(0, 0, 0 , 0, 0, 0, true, spacing);
}