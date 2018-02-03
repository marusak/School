#ifndef _CAMERA_
#define _CAMERA_

// 90 degrees in radians
#define PITCH_LIMIT 1.57

class Camera
{
public:
    Camera();
    ~Camera();

    void init();
    void move_x(float step); // >0 right, <0 left
    void move_y(float step); // >0 up, <0 down
    void move_z(float step); // >0 front, <0 back
    void yaw(float angle); // >0 right, <0 left
    void pitch(float angle); // >0 up, <0 down

    void take_look();
    void get_positions(float &r_x, float &r_y, float &r_z,
                       float &r_look_x, float &r_look_y, float &r_look_z);

private:
    float x, y, z; // Position of camera
    float look_x, look_y, look_z; // Vector through which we look
    float look_x_p, look_z_p; // Perpendicular on ^
    float yaw_d, pitch_d;
};

#endif
