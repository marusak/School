#ifndef CANVAS_H
#define CANVAS_H

#include <QGLWidget>
#include <QTimer>

#include "lorenz.h"
#include "camera.h"

class Canvas : public QGLWidget
{
    Q_OBJECT
public:
    explicit Canvas(QWidget *parent = 0);
    ~Canvas();

    void init(float x, float y, float z,
              float b, float p, float r,
              float step_size, int method_i);
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    void keyPressEvent(QKeyEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    void show_axis(int state);
    void set_axis_size(int new_state);
    void set_drawing_speed(int new_speed);
    void set_bg_color(double r, double g, double b, double a);
    void toggle_equilibrium(int state);
    void set_eq_color(double r, double g, double b);
    void set_first_color(double r, double g, double b);
    void set_second_color(double r, double g, double b);
    void set_run_pause(bool run);

private:
    QTimer timer;
    float cr = 0.850;
    float cg = 0.850;
    float cb = 0.169;

    float finish_cr = 0.678;
    float finish_cg = 0.086;
    float finish_cb = 0.086;

    float d_step_r = (finish_cr - cr);
    float d_step_g = (finish_cg - cg);
    float d_step_b = (finish_cb - cb);

    float current = 0.0;
    float drawing_speed = 0.01/0.001;

    int with_axis = 1;
    int with_equilibrium = 0;

    float eq_color_r = 0.376;
    float eq_color_g = 0.8;
    float eq_color_b = 0.968;

    Camera *l_camera = NULL;
    Lorenz *lorenz = NULL;

    float move_speed = 1; //How sensitive controlling of camera is
    float lastx = 0.0, lasty = 0.0; //Last position of cursor

    void camera();
    void grid (void);
    void draw_lorenz(void);
    void print_char(char ch);
    void axis(double len);
    void equilibrium(void);

    int axis_size = 20;

    bool run = true;
};

#endif
