#ifndef _LORENZ_
#define _LORENZ_


#include <stdlib.h>


class Lorenz
{
public:
    Lorenz(float x, float y, float z,
           float b, float p, float r,
           float step_size, int method_i);
    ~Lorenz();

    long get_arrays(float **x, float **y, float **z);
    void double_arrays_and_recalculate();
    void set_parameters(float p_new, float r_new, float b_new);
    void get_parameters(float *p_ret, float *r_ret, float *b_ret);
    float get_step_size();

private:
    float step_size;
    float *lorenz_x;
    float *lorenz_y;
    float *lorenz_z;

    float prandt;
    float rayleig;
    float b;

    long known = 100;

    void lorenz_euler(int from);
    void lorenz_rk2(int from);
    void lorenz_rk4(int from);
    void create_arrays(void);
    void lorenz(void);
    double fx(double x, double y);
    double fy(double x, double y, double z);
    double fz(double x, double y, double z);

    void (Lorenz::*method)(int);

    void (Lorenz::*methods[3])(int) = {&Lorenz::lorenz_euler,
                                       &Lorenz::lorenz_rk2,
                                       &Lorenz::lorenz_rk4};
};

#endif
