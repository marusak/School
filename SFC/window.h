#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QColorDialog>
#include <QMessageBox>
#include <QMainWindow>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QPalette>

#include "canvas.h"

namespace Ui {
class Window;
}

class Window : public QMainWindow
{
    Q_OBJECT

public:
    explicit Window(QWidget *parent = 0);
    ~Window();

private:
    Ui::Window *ui;
    Canvas* canvas;
    QPushButton* bg_color;
    QPushButton* eq_color;
    QPushButton* first_line_color;
    QPushButton* second_line_color;
    QPushButton* stop_start_b;
    QLineEdit* b_value;
    QLineEdit* p_value;
    QLineEdit* r_value;
    QLineEdit* start_x;
    QLineEdit* start_y;
    QLineEdit* start_z;
    QLineEdit* step_size;
    QComboBox* method;

    bool run = true;

public slots:
    void toggle_axis(int state);
    void resize_axis(int new_size);
    void drawing_speed(int new_speed);
    void background_color();
    void toggle_equilibrium(int state);
    void equilibrium_color();
    void first_color();
    void second_color();
    void new_start();
    void stop_start();
};

#endif // MAINWINDOW_H
