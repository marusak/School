#include "lorenz.h"

void Lorenz::set_parameters(float p_new, float r_new, float b_new){
    prandt = p_new;
    rayleig = r_new;
    b = b_new;
}

void Lorenz::get_parameters(float *p_ret, float *r_ret, float *b_ret){
    *p_ret = prandt;
    *r_ret = rayleig;
    *b_ret = b;
}

Lorenz::Lorenz(float x, float y, float z,
               float b_new, float p_new, float r_new,
               float step_size_new, int method_i){
    create_arrays();
    lorenz_x[0] = x;
    lorenz_y[0] = y;
    lorenz_z[0] = z;
    b = b_new;
    prandt = p_new;
    rayleig = r_new;
    step_size = step_size_new;
    method = methods[method_i];
    (this->*method)(1);
}

Lorenz::~Lorenz(){
    free(lorenz_x);
    free(lorenz_y);
    free(lorenz_z);
}

long Lorenz::get_arrays(float **x, float **y, float **z){
    *x = lorenz_x;
    *y = lorenz_y;
    *z = lorenz_z;
    return known;
}

double Lorenz::fx(double x, double y){
    return (y - x) * prandt;
}

double Lorenz::fy(double x, double y, double z){
    return x * (rayleig - z) - y;
}

double Lorenz::fz(double x, double y, double z){
    return x * y - b * z;
}

void Lorenz::lorenz_euler(int from){
    for (int i = from; i < known; i += 1){
        lorenz_x[i] = lorenz_x[i-1] +  fx(lorenz_x[i-1], lorenz_y[i-1]) * step_size;
        lorenz_y[i] = lorenz_y[i-1] +  fy(lorenz_x[i-1], lorenz_y[i-1], lorenz_z[i-1]) * step_size;
        lorenz_z[i] = lorenz_z[i-1] +  fz(lorenz_x[i-1], lorenz_y[i-1], lorenz_z[i-1]) * step_size;
    }
}

void Lorenz::lorenz_rk2(int from){
    for (int i = from; i < known; i += 1){
        double k1x = fx(lorenz_x[i-1], lorenz_y[i-1]);
        double k1y = fy(lorenz_x[i-1], lorenz_y[i-1], lorenz_y[i-1]);
        double k1z = fz(lorenz_x[i-1], lorenz_y[i-1], lorenz_y[i-1]);
        double x12k1 = lorenz_x[i-1] + k1x * step_size/2;
        double y12k1 = lorenz_y[i-1] + k1y * step_size/2;
        double z12k1 = lorenz_z[i-1] + k1z * step_size/2;
        double k2x = fx(x12k1, y12k1);
        double k2y = fy(x12k1, y12k1, z12k1);
        double k2z = fz(x12k1, y12k1, z12k1);
        lorenz_x[i] = lorenz_x[i-1] + k2x * step_size;
        lorenz_y[i] = lorenz_y[i-1] + k2y * step_size;
        lorenz_z[i] = lorenz_z[i-1] + k2z * step_size;
    }
}

void Lorenz::lorenz_rk4(int from){
    for (int i = from; i < known; i+= 1){
        double k1x = fx(lorenz_x[i-1], lorenz_y[i-1]);
        double k1y = fy(lorenz_x[i-1], lorenz_y[i-1], lorenz_z[i-1]);
        double k1z = fz(lorenz_x[i-1], lorenz_y[i-1], lorenz_z[i-1]);
        double x12k1 = lorenz_x[i-1] + 0.5 * step_size * k1x;
        double y12k1 = lorenz_y[i-1] + 0.5 * step_size * k1y;
        double z12k1 = lorenz_z[i-1] + 0.5 * step_size * k1z;
        double k2x = fx(x12k1, y12k1);
        double k2y = fy(x12k1, y12k1, z12k1);
        double k2z = fz(x12k1, y12k1, z12k1);
        double x12k2 = lorenz_x[i-1] + 0.5 * step_size * k2x;
        double y12k2 = lorenz_y[i-1] + 0.5 * step_size * k2y;
        double z12k2 = lorenz_z[i-1] + 0.5 * step_size * k2z;
        double k3x = fx(x12k2, y12k2);
        double k3y = fy(x12k2, y12k2, z12k2);
        double k3z = fz(x12k2, y12k2, z12k2);
        double x12k3 = lorenz_x[i-1] + 0.5 * step_size * k3x;
        double y12k3 = lorenz_y[i-1] + 0.5 * step_size * k3y;
        double z12k3 = lorenz_z[i-1] + 0.5 * step_size * k3z;
        double k4x = fx(x12k3, y12k3);
        double k4y = fy(x12k3, y12k3, z12k3);
        double k4z = fz(x12k3, y12k3, z12k3);
        lorenz_x[i] = lorenz_x[i-1] + (step_size / 6.0) * (k1x + 2.0 * k2x + 2.0 * k3x + k4x);
        lorenz_y[i] = lorenz_y[i-1] + (step_size / 6.0) * (k1y + 2.0 * k2y + 2.0 * k3y + k4y);
        lorenz_z[i] = lorenz_z[i-1] + (step_size / 6.0) * (k1z + 2.0 * k2z + 2.0 * k3z + k4z);
    }
}

void Lorenz::double_arrays_and_recalculate(){
    long old_known = known;
    known *= 2;
    lorenz_x = (float*) realloc(lorenz_x, sizeof(float) * known);
    lorenz_y = (float*) realloc(lorenz_y, sizeof(float) * known);
    lorenz_z = (float*) realloc(lorenz_z, sizeof(float) * known);
    (this->*method)(old_known);
}

void Lorenz::create_arrays(void){
    lorenz_x = (float*) malloc(sizeof(float) * known);
    lorenz_y = (float*) malloc(sizeof(float) * known);
    lorenz_z = (float*) malloc(sizeof(float) * known);
}

float Lorenz::get_step_size(){
    return step_size;
}
