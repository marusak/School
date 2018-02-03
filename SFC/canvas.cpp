#include "canvas.h"
#include <GL/gl.h>
#include <GL/glut.h>

#include <math.h>

#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#include <iostream>

void Canvas::init(float x, float y, float z,
                    float b, float p, float r,
                    float step_size, int method_i){
    if (!l_camera)
        l_camera = new Camera();
    if (lorenz){
        float speed = drawing_speed * lorenz->get_step_size();
        drawing_speed = speed / step_size;
        delete(lorenz);
    }
    lorenz = new Lorenz(x, y, z, b, p, r, step_size, method_i);
    current = 0.0;
}

void Canvas::grid(void){
    glPushMatrix();

    for(int i=-50; i < 50; i++){
        glBegin(GL_LINES);
        glVertex3f(i, 0, -50);
        glVertex3f(i, 0, 50);
        glEnd();
    }

    for(int i=-50; i < 50; i++){
        glBegin(GL_LINES);
        glVertex3f(-50, 0, i);
        glVertex3f(50, 0, i);
        glEnd();
    }

    glPopMatrix();
}

void Canvas::equilibrium(void){
    glColor3f(eq_color_r, eq_color_g, eq_color_b);

    glPushMatrix();
    glTranslatef(0.0, 0.0, 0.0);
    glutWireSphere (1.0, 20, 50);
    glPopMatrix();

    float p;
    float r;
    float b;
    lorenz->get_parameters(&p, &r, &b);

    glPushMatrix();
    glTranslatef(-sqrt(-b+b*r), -sqrt(-b+b*r), -1+r);
    glutWireSphere (1.0, 20, 50);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(sqrt(-b+b*r), sqrt(-b+b*r), -1+r);
    glutWireSphere (1.0, 20, 50);
    glPopMatrix();
}

void Canvas::draw_lorenz(void){
    float *lorenz_x, *lorenz_y, *lorenz_z;
    if (lorenz->get_arrays(&lorenz_x, &lorenz_y, &lorenz_z) < (int) (current + drawing_speed)){
        lorenz->double_arrays_and_recalculate();
        lorenz->get_arrays(&lorenz_x, &lorenz_y, &lorenz_z);
    }

    glPushMatrix();
    float tr = cr;
    float tg = cg;
    float tb = cb;

    float cd_r = d_step_r / current;
    float cd_g = d_step_g / current;
    float cd_b = d_step_b / current;

    for(int i=0; i < (int)current; i++) {
        glBegin(GL_LINES);
        glColor3f(tr, tg, tb);
        glVertex3f(lorenz_x[i], lorenz_y[i], lorenz_z[i]);
        glVertex3f(lorenz_x[i+1], lorenz_y[i+1], lorenz_z[i+1]);
        glEnd();
        tr += cd_r;
        tg += cd_g;
        tb += cd_b;
    }
    glPopMatrix();
    if (run){
        current += drawing_speed;
    }
}

void Canvas::print_char(char ch){
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, ch);
}

void Canvas::axis(double len){
    glPushMatrix();
    glBegin(GL_LINES);

    //Blue X ax
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(len, 0.0, 0.0);

    //Green Y ax
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, len, 0.0);

    //Red Z ax
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, len);

    glEnd();

    //Label axis
    glRasterPos3d(0, 0, len + 1);
    print_char('Z');

    glColor3f(0.0, 0.0, 1.0);
    glRasterPos3d(len + 1, 0, 0);
    print_char('X');

    glColor3f(0.0, 1.0, 0.0);
    glRasterPos3d(0, len + 1, 0);
    print_char('Y');

    glPopMatrix();
}

void Canvas::camera(){
    float x, y, z, look_x, look_y, look_z;
    l_camera->take_look();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    l_camera->get_positions(x, y, z, look_x, look_y, look_z);
    gluLookAt(x, y, z, x + look_x, y + look_y, z + look_z, 0.0, 1.0, 0.0);
}

Canvas::Canvas(QWidget *parent): QGLWidget(parent)
{
    connect(&timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    timer.start(16);
}

Canvas::~Canvas(){
    delete(l_camera);
    delete(lorenz);
}

void Canvas::initializeGL(){
    glClearColor(0.2, 0.2, 0.2, 1.0);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
}


void Canvas::paintGL(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera();
    draw_lorenz();
    if (with_axis){
        axis(axis_size);
    }
    if (with_equilibrium){
        equilibrium();
    }
    //grid();
}

void Canvas::resizeGL(int w, int h){
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float)w/h, 0.5, 5000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    camera();
}

void Canvas::keyPressEvent(QKeyEvent *event){
    switch (event->key()){
        case Qt::Key_W:
            l_camera->move_z(move_speed);
            break;
        case Qt::Key_S:
            l_camera->move_z(-move_speed);
            break;
        case Qt::Key_A:
            l_camera->move_x(move_speed);
            break;
        case Qt::Key_D:
            l_camera->move_x(-move_speed);
            break;
    }
}

void Canvas::mouseMoveEvent(QMouseEvent *event){
    int x = event->x();
    int y = event->y();
    int diffx=x-lastx;
    int diffy=y-lasty;
    lastx=x;
    lasty=y;
    l_camera->yaw(move_speed * diffx);
    l_camera->pitch(move_speed * -diffy);
}

void Canvas::mousePressEvent(QMouseEvent *event){
    lastx = event->x();
    lasty = event->y();
}

void Canvas::wheelEvent(QWheelEvent *event){
    l_camera->move_y(event->pixelDelta().y());
}

void Canvas::show_axis(int state){
    with_axis = state;
}

void Canvas::set_axis_size(int new_size){
    axis_size = new_size;
}

void Canvas::set_drawing_speed(int new_speed){
    drawing_speed = (new_speed/1000.0)/lorenz->get_step_size();
}

void Canvas::set_bg_color(double r, double g, double b, double a){
    glClearColor(r, g, b, a);
}

void Canvas::toggle_equilibrium(int state){
    with_equilibrium = state;
}

void Canvas::set_eq_color(double r, double g, double b){
    eq_color_r = r;
    eq_color_g = g;
    eq_color_b = b;
}

void Canvas::set_first_color(double r, double g, double b){
    cr = r;
    cg = g;
    cb = b;
    d_step_r = (finish_cr - cr);
    d_step_g = (finish_cg - cg);
    d_step_b = (finish_cb - cb);
}

void Canvas::set_second_color(double r, double g, double b){
    finish_cr = r;
    finish_cg = g;
    finish_cb = b;
    d_step_r = (finish_cr - cr);
    d_step_g = (finish_cg - cg);
    d_step_b = (finish_cb - cb);
}

void Canvas::set_run_pause(bool new_run){
    run = new_run;
}
