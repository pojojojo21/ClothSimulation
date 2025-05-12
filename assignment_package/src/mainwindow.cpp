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
    connect(ui->Spin_D, &QSpinBox::valueChanged, this, &MainWindow::on_depth_changed);
    connect(ui->Spin_SS, &QDoubleSpinBox::valueChanged, this, &MainWindow::on_spacing_changed);
    connect(ui->integrationTypeSelection, &QComboBox::currentIndexChanged, this, &MainWindow::on_itegration_changed);
    connect(ui->Spin_BL, &QDoubleSpinBox::valueChanged, this, &MainWindow::on_bounce_changed);
    connect(ui->Spin_GAS, &QDoubleSpinBox::valueChanged, this, &MainWindow::on_gas_changed);
    connect(ui->Spin_VIS, &QDoubleSpinBox::valueChanged, this, &MainWindow::on_vis_changed);
    connect(ui->Spin_SR, &QDoubleSpinBox::valueChanged, this, &MainWindow::on_SR_changed);
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
    ui->mygl->reset();
}

void MainWindow::on_resetClothButton_clicked()
{
    ui->mygl->reset();
}

void MainWindow::on_dropCornerButton_clicked()
{
    ui->mygl->dropCorner();
}

void MainWindow::on_dropClothButton_clicked()
{
    ui->mygl->drop();
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
    ui->mygl->changeFluidSim(true, width, 0, 0, 0, 0, 0, 0);
}

void MainWindow::on_height_changed(int height)
{
    ui->mygl->changeCloth(0, 0, true, height, 0, 0);
    ui->mygl->changeBox(0, 0, true, height, 0, 0, 0, 0);
    ui->mygl->changeFluidSim(0, 0, true, height, 0, 0, 0, 0);
}

void MainWindow::on_depth_changed(int depth)
{
    ui->mygl->changeBox(0, 0, 0, 0, true, depth, 0, 0);
    ui->mygl->changeFluidSim(0, 0, 0, 0, true, depth, 0, 0);
}

void MainWindow::on_spacing_changed(float spacing)
{
    ui->mygl->changeCloth(0, 0, 0 , 0, true, spacing);
    ui->mygl->changeBox(0, 0, 0 , 0, 0, 0, true, spacing);
}

void MainWindow::on_itegration_changed(int index)
{
    ui->mygl->setIntegrationType(index);
}

void MainWindow::on_bounce_changed(float bounce)
{
    ui->mygl->setBounce(bounce);
}

void MainWindow::on_gas_changed(float gas)
{
    ui->mygl->setGas(gas);
}

void MainWindow::on_vis_changed(float vis)
{
    ui->mygl->setVis(vis);
}

void MainWindow::on_SR_changed(float sR)
{
    ui->mygl->setSmoothingRadius(sR);
}