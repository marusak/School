#include "window.h"
#include "ui_window.h"

Window::Window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Window)
{
    ui->setupUi(this);
    canvas = this->findChild<Canvas*>("canvas");
    bg_color = this->findChild<QPushButton*>("background_color");
    eq_color = this->findChild<QPushButton*>("equilibrium_color");
    first_line_color = this->findChild<QPushButton*>("first_color");
    second_line_color = this->findChild<QPushButton*>("second_color");
    stop_start_b = this->findChild<QPushButton*>("stop_start_b");
    p_value = this->findChild<QLineEdit*>("p_value");
    r_value = this->findChild<QLineEdit*>("r_value");
    b_value = this->findChild<QLineEdit*>("b_value");
    start_x = this->findChild<QLineEdit*>("start_x");
    start_y = this->findChild<QLineEdit*>("start_y");
    start_z = this->findChild<QLineEdit*>("start_z");
    step_size = this->findChild<QLineEdit*>("step_size");
    method = this->findChild<QComboBox*>("method");
    bg_color->setPalette(QPalette(QColor(51, 51, 51, 255)));
    eq_color->setPalette(QPalette(QColor(96, 204, 247, 200)));
    first_line_color->setPalette(QPalette(QColor(226, 217, 43, 250)));
    second_line_color->setPalette(QPalette(QColor(173, 22, 22, 248)));
    canvas->init(0.01, 0.0, 0.0, 8.0/3.0, 10.0, 28.0, 0.001, 0);
}

Window::~Window()
{
    delete ui;
}

void Window::toggle_axis(int state){
    canvas->show_axis(state);
}

void Window::resize_axis(int new_size){
    canvas->set_axis_size(new_size);
}

void Window::drawing_speed(int new_speed){
    canvas->set_drawing_speed(new_speed);
}

void Window::background_color(){
    QColor color = QColorDialog::getColor();
    if (color.isValid()){
        bg_color->setPalette(QPalette(color));
        canvas->set_bg_color(color.redF(),
                             color.greenF(),
                             color.blueF(),
                             color.alphaF());
    }
}

void Window::toggle_equilibrium(int state){
    canvas->toggle_equilibrium(state);
}

void Window::equilibrium_color(){
    QColor color = QColorDialog::getColor();
    if (color.isValid()){
        eq_color->setPalette(QPalette(color));
        canvas->set_eq_color(color.redF(),
                             color.greenF(),
                             color.blueF());
    }
}

void Window::first_color(){
    QColor color = QColorDialog::getColor();
    if (color.isValid()){
        first_line_color->setPalette(QPalette(color));
        canvas->set_first_color(color.redF(),
                             color.greenF(),
                             color.blueF());
    }
}

void Window::second_color(){
    QColor color = QColorDialog::getColor();
    if (color.isValid()){
        second_line_color->setPalette(QPalette(color));
        canvas->set_second_color(color.redF(),
                             color.greenF(),
                             color.blueF());
    }
}

void Window::new_start(){
    bool ok;
    float new_p = p_value->text().toFloat(&ok);
    if (!ok){
        QMessageBox::critical(this, "Wrong number", "Value of 'Prandtl' must be floating point number", QMessageBox::Ok);
        p_value->setFocus();
        return;
    }
    float new_b = b_value->text().toFloat(&ok);
    if (!ok){
        QMessageBox::critical(this, "Wrong number", "Value of 'b' must be floating point number", QMessageBox::Ok);
        b_value->setFocus();
        return;
    }
    float new_r = r_value->text().toFloat(&ok);
    if (!ok){
        QMessageBox::critical(this, "Wrong number", "Value of 'Rayleign' must be floating point number", QMessageBox::Ok);
        r_value->setFocus();
        return;
    }
    float new_x = start_x->text().toFloat(&ok);
    if (!ok){
        QMessageBox::critical(this, "Wrong number", "Value of 'Start x' must be floating point number", QMessageBox::Ok);
        start_x->setFocus();
        return;
    }
    float new_y = start_y->text().toFloat(&ok);
    if (!ok){
        QMessageBox::critical(this, "Wrong number", "Value of 'Start y' must be floating point number", QMessageBox::Ok);
        start_y->setFocus();
        return;
    }
    float new_z = start_z->text().toFloat(&ok);
    if (!ok){
        QMessageBox::critical(this, "Wrong number", "Value of 'Start z' must be floating point number", QMessageBox::Ok);
        start_z->setFocus();
        return;
    }
    float new_step_size = step_size->text().toFloat(&ok);
    if (!ok){
        QMessageBox::critical(this, "Wrong number", "Value of 'Smoothness' must be floating point number", QMessageBox::Ok);
        step_size->setFocus();
        return;
    }
    int method_i = method->currentIndex();
    canvas->init(new_x, new_y, new_z, new_b, new_p, new_r, new_step_size, method_i);
}

void Window::stop_start(){
    run = !run;
    if (run)
        stop_start_b->setIcon(QIcon("/usr/share/icons/Adwaita/16x16/actions/media-playback-start.png"));
    else
        stop_start_b->setIcon(QIcon("/usr/share/icons/Adwaita/16x16/actions/media-playback-pause.png"));
    canvas->set_run_pause(run);
}
