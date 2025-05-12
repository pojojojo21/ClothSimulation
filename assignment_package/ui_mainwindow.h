/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>
#include "mygl.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionReset;
    QAction *actionQuit;
    QAction *actionCamera_Controls;
    QWidget *centralWidget;
    MyGL *mygl;
    QPushButton *resetClothButton;
    QPushButton *dropCornerButton;
    QPushButton *dropClothButton;
    QComboBox *drawTypeSelection;
    QComboBox *objTypeSelection;
    QSpinBox *Spin_W;
    QLabel *Label_Width;
    QLabel *Label_Height;
    QSpinBox *Spin_H;
    QLabel *Label_SpringSize;
    QDoubleSpinBox *Spin_SS;
    QSpinBox *Spin_D;
    QLabel *Label_Depth;
    QComboBox *integrationTypeSelection;
    QLabel *Label_BL;
    QDoubleSpinBox *Spin_BL;
    QLabel *Label_GAS;
    QDoubleSpinBox *Spin_GAS;
    QLabel *Label_VIS;
    QDoubleSpinBox *Spin_VIS;
    QLabel *Label_SR;
    QDoubleSpinBox *Spin_SR;
    QMenuBar *menuBar;
    QMenu *menuFile;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1057, 492);
        actionReset = new QAction(MainWindow);
        actionReset->setObjectName("actionReset");
        actionQuit = new QAction(MainWindow);
        actionQuit->setObjectName("actionQuit");
        actionCamera_Controls = new QAction(MainWindow);
        actionCamera_Controls->setObjectName("actionCamera_Controls");
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName("centralWidget");
        mygl = new MyGL(centralWidget);
        mygl->setObjectName("mygl");
        mygl->setGeometry(QRect(11, 11, 618, 432));
        resetClothButton = new QPushButton(centralWidget);
        resetClothButton->setObjectName("resetClothButton");
        resetClothButton->setGeometry(QRect(680, 30, 100, 30));
        dropCornerButton = new QPushButton(centralWidget);
        dropCornerButton->setObjectName("dropCornerButton");
        dropCornerButton->setGeometry(QRect(800, 30, 100, 30));
        dropClothButton = new QPushButton(centralWidget);
        dropClothButton->setObjectName("dropClothButton");
        dropClothButton->setGeometry(QRect(930, 30, 100, 30));
        drawTypeSelection = new QComboBox(centralWidget);
        drawTypeSelection->addItem(QString());
        drawTypeSelection->addItem(QString());
        drawTypeSelection->addItem(QString());
        drawTypeSelection->setObjectName("drawTypeSelection");
        drawTypeSelection->setGeometry(QRect(680, 100, 72, 24));
        objTypeSelection = new QComboBox(centralWidget);
        objTypeSelection->addItem(QString());
        objTypeSelection->addItem(QString());
        objTypeSelection->addItem(QString());
        objTypeSelection->setObjectName("objTypeSelection");
        objTypeSelection->setGeometry(QRect(810, 100, 72, 24));
        Spin_W = new QSpinBox(centralWidget);
        Spin_W->setObjectName("Spin_W");
        Spin_W->setEnabled(true);
        Spin_W->setGeometry(QRect(730, 140, 42, 22));
        Spin_W->setMinimum(2);
        Spin_W->setMaximum(500);
        Spin_W->setValue(10);
        Label_Width = new QLabel(centralWidget);
        Label_Width->setObjectName("Label_Width");
        Label_Width->setGeometry(QRect(680, 140, 49, 16));
        Label_Height = new QLabel(centralWidget);
        Label_Height->setObjectName("Label_Height");
        Label_Height->setGeometry(QRect(850, 140, 49, 16));
        Spin_H = new QSpinBox(centralWidget);
        Spin_H->setObjectName("Spin_H");
        Spin_H->setEnabled(true);
        Spin_H->setGeometry(QRect(910, 140, 42, 22));
        Spin_H->setMinimum(2);
        Spin_H->setMaximum(500);
        Spin_H->setValue(10);
        Label_SpringSize = new QLabel(centralWidget);
        Label_SpringSize->setObjectName("Label_SpringSize");
        Label_SpringSize->setGeometry(QRect(770, 220, 61, 16));
        Spin_SS = new QDoubleSpinBox(centralWidget);
        Spin_SS->setObjectName("Spin_SS");
        Spin_SS->setGeometry(QRect(850, 220, 62, 22));
        Spin_SS->setMinimum(0.100000000000000);
        Spin_SS->setMaximum(5.000000000000000);
        Spin_SS->setSingleStep(0.100000000000000);
        Spin_SS->setValue(1.200000000000000);
        Spin_D = new QSpinBox(centralWidget);
        Spin_D->setObjectName("Spin_D");
        Spin_D->setEnabled(true);
        Spin_D->setGeometry(QRect(840, 180, 42, 22));
        Spin_D->setMinimum(2);
        Spin_D->setMaximum(500);
        Spin_D->setValue(10);
        Label_Depth = new QLabel(centralWidget);
        Label_Depth->setObjectName("Label_Depth");
        Label_Depth->setGeometry(QRect(780, 180, 49, 16));
        integrationTypeSelection = new QComboBox(centralWidget);
        integrationTypeSelection->addItem(QString());
        integrationTypeSelection->addItem(QString());
        integrationTypeSelection->addItem(QString());
        integrationTypeSelection->setObjectName("integrationTypeSelection");
        integrationTypeSelection->setGeometry(QRect(930, 100, 72, 24));
        Label_BL = new QLabel(centralWidget);
        Label_BL->setObjectName("Label_BL");
        Label_BL->setGeometry(QRect(667, 260, 81, 20));
        Spin_BL = new QDoubleSpinBox(centralWidget);
        Spin_BL->setObjectName("Spin_BL");
        Spin_BL->setGeometry(QRect(810, 260, 62, 22));
        Spin_BL->setMinimum(0.100000000000000);
        Spin_BL->setMaximum(2.000000000000000);
        Spin_BL->setSingleStep(0.100000000000000);
        Spin_BL->setValue(1.000000000000000);
        Label_GAS = new QLabel(centralWidget);
        Label_GAS->setObjectName("Label_GAS");
        Label_GAS->setGeometry(QRect(670, 300, 91, 20));
        Spin_GAS = new QDoubleSpinBox(centralWidget);
        Spin_GAS->setObjectName("Spin_GAS");
        Spin_GAS->setGeometry(QRect(810, 300, 62, 22));
        Spin_GAS->setMinimum(0.100000000000000);
        Spin_GAS->setMaximum(1.000000000000000);
        Spin_GAS->setSingleStep(0.100000000000000);
        Spin_GAS->setValue(0.400000000000000);
        Label_VIS = new QLabel(centralWidget);
        Label_VIS->setObjectName("Label_VIS");
        Label_VIS->setGeometry(QRect(670, 340, 91, 20));
        Spin_VIS = new QDoubleSpinBox(centralWidget);
        Spin_VIS->setObjectName("Spin_VIS");
        Spin_VIS->setGeometry(QRect(810, 340, 62, 22));
        Spin_VIS->setMinimum(0.010000000000000);
        Spin_VIS->setMaximum(0.100000000000000);
        Spin_VIS->setSingleStep(0.010000000000000);
        Spin_VIS->setValue(0.010000000000000);
        Label_SR = new QLabel(centralWidget);
        Label_SR->setObjectName("Label_SR");
        Label_SR->setGeometry(QRect(670, 380, 121, 20));
        Spin_SR = new QDoubleSpinBox(centralWidget);
        Spin_SR->setObjectName("Spin_SR");
        Spin_SR->setGeometry(QRect(810, 380, 62, 22));
        Spin_SR->setMinimum(0.100000000000000);
        Spin_SR->setMaximum(5.000000000000000);
        Spin_SR->setSingleStep(0.100000000000000);
        Spin_SR->setValue(1.000000000000000);
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName("menuBar");
        menuBar->setGeometry(QRect(0, 0, 1057, 24));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName("menuFile");
        MainWindow->setMenuBar(menuBar);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(actionQuit);
        menuFile->addAction(actionReset);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "ClothSimDemo", nullptr));
        actionReset->setText(QCoreApplication::translate("MainWindow", "Reset", nullptr));
#if QT_CONFIG(shortcut)
        actionReset->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+R", nullptr));
#endif // QT_CONFIG(shortcut)
        actionQuit->setText(QCoreApplication::translate("MainWindow", "Quit", nullptr));
#if QT_CONFIG(shortcut)
        actionQuit->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Q", nullptr));
#endif // QT_CONFIG(shortcut)
        actionCamera_Controls->setText(QCoreApplication::translate("MainWindow", "Camera Controls", nullptr));
        resetClothButton->setText(QCoreApplication::translate("MainWindow", "Reset", nullptr));
        dropCornerButton->setText(QCoreApplication::translate("MainWindow", "Drop Corner", nullptr));
        dropClothButton->setText(QCoreApplication::translate("MainWindow", "Drop", nullptr));
        drawTypeSelection->setItemText(0, QCoreApplication::translate("MainWindow", "Particle", nullptr));
        drawTypeSelection->setItemText(1, QCoreApplication::translate("MainWindow", "Spring", nullptr));
        drawTypeSelection->setItemText(2, QCoreApplication::translate("MainWindow", "Lambert", nullptr));

        objTypeSelection->setItemText(0, QCoreApplication::translate("MainWindow", "Cloth", nullptr));
        objTypeSelection->setItemText(1, QCoreApplication::translate("MainWindow", "Box", nullptr));
        objTypeSelection->setItemText(2, QCoreApplication::translate("MainWindow", "FluidSim", nullptr));

        Label_Width->setText(QCoreApplication::translate("MainWindow", "Width", nullptr));
        Label_Height->setText(QCoreApplication::translate("MainWindow", "Height", nullptr));
        Label_SpringSize->setText(QCoreApplication::translate("MainWindow", "Spring Size", nullptr));
        Label_Depth->setText(QCoreApplication::translate("MainWindow", "Depth", nullptr));
        integrationTypeSelection->setItemText(0, QCoreApplication::translate("MainWindow", "Verlet", nullptr));
        integrationTypeSelection->setItemText(1, QCoreApplication::translate("MainWindow", "Implicit Euler", nullptr));
        integrationTypeSelection->setItemText(2, QCoreApplication::translate("MainWindow", "Euler", nullptr));

        Label_BL->setText(QCoreApplication::translate("MainWindow", "Bounce Level", nullptr));
        Label_GAS->setText(QCoreApplication::translate("MainWindow", "Gas Constant", nullptr));
        Label_VIS->setText(QCoreApplication::translate("MainWindow", "Viscosity", nullptr));
        Label_SR->setText(QCoreApplication::translate("MainWindow", "Smoothing Radius", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
