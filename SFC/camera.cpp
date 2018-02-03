#include "camera.h"

#include <math.h>

Camera::Camera(){
    init();
}

Camera::~Camera(){
}

void Camera::init(){
    yaw_d = 0.0;
    pitch_d = 0.0;
    x = -100.0;
    y = 0.0;
    z = 0.0;

    take_look();
}

void Camera::move_x(float step){
    x += step * look_x_p;
    z += step * look_z_p;

    take_look();
}

void Camera::move_y(float step){
    y += step;

    take_look();
}

void Camera::move_z(float step){
    x += step * cos(yaw_d) * cos(pitch_d);
    y += step * sin(pitch_d);
    z += step * sin(yaw_d) * cos(pitch_d);

    take_look();
}

void Camera::yaw(float angle){
    yaw_d += angle * 0.017;

    take_look();
}

void Camera::pitch(float angle){
    pitch_d += angle * 0.017;

    //get number between <-limit, limit>
    if (pitch_d > PITCH_LIMIT)
        pitch_d = PITCH_LIMIT;
    else if (pitch_d < -PITCH_LIMIT)
        pitch_d = -PITCH_LIMIT;

    take_look();
}

void Camera::take_look(){
    look_x = cos(yaw_d) * cos(pitch_d);
    look_y = sin(pitch_d);
    look_z = sin(yaw_d) * cos(pitch_d);

    look_x_p = cos(yaw_d - M_PI_2);
    look_z_p = sin(yaw_d - M_PI_2);
}

void Camera::get_positions(float &r_x, float &r_y, float &r_z,
                           float &r_look_x, float &r_look_y, float &r_look_z){
    r_x = x;
    r_y = y;
    r_z = z;
    r_look_x = look_x;
    r_look_y = look_y;
    r_look_z = look_z;
}
