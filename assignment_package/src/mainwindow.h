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

private slots:
    void on_actionQuit_triggered();
    void on_actionReset_triggered();
    void on_resetClothButton_clicked();
    void on_dropCornerButton_clicked();
    void on_dropClothButton_clicked();
    void on_drawTypeSelection_changed(int index);
    void on_objTypeSelection_changed(int index);
    void on_width_changed(int width);
    void on_height_changed(int height);
    void on_depth_changed(int depth);
    void on_spacing_changed(float spacing);
    void on_itegration_changed(int index);
    void on_bounce_changed(float bounce);
    void on_gas_changed(float gas);
    void on_vis_changed(float vis);
    void on_SR_changed(float sR);

private:
    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H
