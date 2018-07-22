#define _GNU_SOURCE

#include <gmp.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "primes.h"

void error(int error_code, char* error_msg){
    printf("%s\n",error_msg);
    exit(error_code);
}

void find_sd(mpz_t p, mpz_t s, mpz_t d) {
    unsigned long i = 0;
    mpz_t rt; mpz_init(rt);
    while(++i){
        mpz_ui_pow_ui(rt, 2, i);
        mpz_mod(s, p, rt);
        if (mpz_cmp_ui(s, 0) == 0){
            mpz_set_ui(s,i);
            mpz_cdiv_q(d, p, rt);
            break;
        }
    }
}

/* Miller–Rabin primality test
 * Alg comes from https://en.wikipedia.org/wiki/Miller%E2%80%93Rabin_primality_test
 */
bool is_prime(mpz_t p, gmp_randstate_t state){
    // Less then 4 is prime
    if (mpz_cmp_ui(p, 3) <= 0)
        return true;

    // There is only one even number -2-
    if (mpz_even_p(p) != 0)
        return false;

    bool res = false;
    mpz_t p1; mpz_init(p1);
    mpz_t p3; mpz_init(p3);
    mpz_t r; mpz_init(r);
    mpz_t d; mpz_init(d);
    mpz_t a; mpz_init(a);
    mpz_t x; mpz_init(x);
    mpz_t r_count; mpz_init(r_count);
    mpz_sub_ui(p1, p, 1);
    mpz_sub_ui(p3, p, 3);
    find_sd(p1, r, d);

    for (int i=0; i<10; i++) {
        // Random number from 2 to p-2
        mpz_urandomm(a, state, p3);
        mpz_add_ui(a, a, 2);
        mpz_powm(x, a, d, p);
        if (mpz_cmp_ui(x, 1) == 0) continue;
        if (mpz_cmp(x, p1) == 0) continue;
        mpz_set_ui(r_count, 0);
        while( mpz_cmp(r_count, r) < 0 ) {
            mpz_powm_ui(x, x, 2, p);
            if (mpz_cmp(x, p1) == 0) break;
            mpz_add_ui(r_count, r_count, 1);
        }
        if (mpz_cmp(x, p1) == 0) continue;
        goto done;
    }
    res = true;

done:
    mpz_clear(p1);
    mpz_clear(p3);
    mpz_clear(r);
    mpz_clear(d);
    mpz_clear(a);
    mpz_clear(x);
    mpz_clear(r_count);

    return res;
}

void generate_prime(mpz_t p, long bits, gmp_randstate_t state){
    mpz_urandomb(p, state, bits);
    while(1) {
        if (is_prime(p, state))
            break;
        mpz_urandomb(p, state, bits);
    }
}

bool generate_e(mpz_t nphi, unsigned long *res) {
    unsigned long i = 2;
    while (mpz_cmp_ui(nphi, i) > 0) {
        mpz_t np;
        if (mpz_mod_ui(np, nphi, i) != 0)
            break;
        else
            i++;
    }
    if (mpz_cmp_ui(nphi, i) <= 0)
        return false;
    *res = i;
    return true;
}

void gcdext(mpz_t v1, mpz_t v2, mpz_t f1, mpz_t f2, mpz_t res) {
    if (mpz_cmp_ui(v1,0) == 0) {
        mpz_set_ui(f1,0);
        mpz_set_ui(f2,1);
        mpz_set(res,v2);
        return;
    }
    mpz_t tmp; mpz_init(tmp);
    mpz_t tmp1; mpz_init(tmp1);
    mpz_t tmp2; mpz_init(tmp2);
    mpz_t tmp3; mpz_init(tmp3);

    mpz_mod(tmp,v2,v1);
    mpz_set(tmp1,v1);
    gcdext(tmp, tmp1, tmp2, tmp3, res);

    mpz_set(f2, tmp2);
    mpz_fdiv_q(tmp, v2, v1);
    mpz_mul(tmp, tmp, tmp2);
    mpz_sub(f1, tmp3, tmp);

    mpz_clear(tmp);
    mpz_clear(tmp1);
    mpz_clear(tmp2);
    mpz_clear(tmp3);
}

bool generate_d(mpz_t d, unsigned long e, mpz_t nphi){
    bool res = true;
    mpz_t f1; mpz_init(f1);
    mpz_t f2; mpz_init(f2);
    mpz_t tmp; mpz_init(tmp);
    mpz_set(tmp, nphi);
    mpz_t empz; mpz_init(empz); mpz_set_ui(empz, e);
    gcdext(empz, tmp, f1, f2, d);
    if (mpz_cmp_ui(d, 1) != 0)
        res = false;
    else
    {
        mpz_mod(f1, f1, nphi);
        mpz_set(d, f1);
    }
    mpz_clear(f1);
    mpz_clear(f2);
    mpz_clear(tmp);
    mpz_clear(empz);
    return res;
}

unsigned long count_bit_length(mpz_t n, unsigned long bits) {
    while(--bits > 0) {
        if (mpz_tstbit(n, bits))
            break;
    }
    return bits + 1;
}

void code(char *mc, char* ec, char* nc, mpz_t result){
    mpz_t m; mpz_init(m);
    mpz_t e; mpz_init(e);
    mpz_t n; mpz_init(n);

    FILE *stream;

    stream = fmemopen(mc, strlen(mc), "r");
    if (! mpz_inp_str(m, stream, 0))
        error(5, "Incorrect input format");
    fclose (stream);
    stream = fmemopen(ec, strlen(ec), "r");
    if (! mpz_inp_str(e, stream, 0))
        error(5, "Incorrect input format");
    fclose (stream);
    stream = fmemopen(nc, strlen(nc), "r");
    if (! mpz_inp_str(n, stream, 0))
        error(5, "Incorrect input format");
    fclose (stream);

    mpz_powm(result,m, e, n);

    mpz_clear(m);
    mpz_clear(e);
    mpz_clear(n);
}

void gcd(mpz_t result, mpz_t x, mpz_t y){
    mpz_t tmp; mpz_init(tmp);
    mpz_t tmp_x; mpz_init(tmp_x); mpz_set(tmp_x,x);
    mpz_t tmp_y; mpz_init(tmp_y); mpz_set(tmp_y,y);
    while (mpz_cmp_ui(tmp_y,0) != 0){
        mpz_mod(tmp,tmp_x,tmp_y);
        mpz_set(tmp_x,tmp_y);
        mpz_set(tmp_y,tmp);
    }
    mpz_set(result,tmp_x);
    mpz_clear(tmp);mpz_clear(tmp_x);mpz_clear(tmp_y);
}

void brachen(char* nc, mpz_t result, gmp_randstate_t state){
    mpz_t r; mpz_init(r);
    mpz_t n; mpz_init(n);

    FILE *stream;
    stream = fmemopen(nc, strlen(nc), "r");
    if (! mpz_inp_str(n, stream, 0))
        error(5, "Incorrect input format");
    fclose(stream);

    // Check if is prime - 96 bit prime will not be broken under 120s
    if (is_prime(n, state)){
        mpz_set(result,n);
        mpz_clear(r);mpz_clear(n);
        return;
    }

    // trivial dividing
    for (unsigned long i = 0; i < 78498; i++) {
        if (!mpz_mod_ui(r, n, primes[i])){
            mpz_set_ui(result,primes[i]);
            mpz_clear(r);mpz_clear(n);
            return;
        }
    }

    // Pollard Rho Method
    mpz_t y; mpz_init(y); mpz_set_ui(y, 2);
    mpz_t x; mpz_init(x); mpz_set_ui(x, 2);
    mpz_t d; mpz_init(d); mpz_set_ui(d, 2);
    mpz_t tmp; mpz_init(tmp);
    mpz_t i; mpz_init(i);
    mpz_set_ui(result,1);
    while (mpz_cmp_ui(result,1) == 0){
        mpz_set_ui(i, 1);
        while (mpz_cmp(i,d) <= 0 && mpz_cmp_ui(result,1) <= 0){
            mpz_mul(x,x,x);
            mpz_add_ui(x,x,1);
            mpz_mod(x,x,n);
            mpz_sub(tmp,x,y);
            gcd(result,tmp,n);
            mpz_add_ui(i,i,1);
        }
        mpz_mul_ui(d,d,2);
        mpz_set(y,x);
    }
    mpz_clear(r);mpz_clear(n);
    mpz_clear(y);mpz_clear(x);mpz_clear(d);
}

int main(int argc, char** argv){
    gmp_randstate_t state;
    gmp_randinit_mt(state);
    gmp_randseed_ui(state,(unsigned long)time(NULL));

    if (argc == 3 && strcmp(argv[1], "-g") == 0){
        unsigned long bits = strtol(argv[2], NULL, 10);
        unsigned long e;
        mpz_t p; mpz_t q;
        mpz_t n; mpz_t nphi;
        mpz_t d; mpz_t pt; mpz_t qt;

        mpz_init(p); mpz_init(q);
        mpz_init(n); mpz_init(nphi);
        mpz_init(d); mpz_init(pt); mpz_init(qt);

        unsigned long hf = ceil(bits/2.0);
        generate_prime(p, hf, state);
        generate_prime(q, hf, state);
        mpz_mul(n, p, q);
        while (1){
            // regenerate the smaller one
            if (mpz_cmp(p,q) > 0)
                generate_prime(q, hf, state);
            else
                generate_prime(p, hf, state);
            mpz_mul(n, p, q);
            if (count_bit_length(n, bits) != bits)
                continue;
            mpz_sub_ui(pt, p, 1);
            mpz_sub_ui(qt, q, 1);
            mpz_mul(nphi, pt, qt);
            if (!generate_e(nphi, &e))
                continue;
            if(!generate_d(d, e, nphi))
                continue;
            break;
        }

        printf("0x");
        mpz_out_str(stdout,16,p);
        printf(" 0x");
        mpz_out_str(stdout,16,q);
        printf(" 0x");
        mpz_out_str(stdout,16,n);
        printf(" 0x%lx 0x", e);
        mpz_out_str(stdout,16,d);
        printf("\n");

        mpz_clear(p); mpz_clear(q);
        mpz_clear(n); mpz_clear(nphi);
        mpz_clear(d); mpz_clear(pt); mpz_clear(qt);
    }
    else if (argc == 5 && ((strcmp(argv[1], "-e") == 0) || (strcmp(argv[1], "-d") == 0))){
        mpz_t result;
        mpz_init(result);
        code(argv[4], argv[2], argv[3], result);
        printf("0x");
        mpz_out_str(stdout,16,result);
        printf("\n");
        mpz_clear(result);
    }
    else if (argc == 3 && strcmp(argv[1], "-b") == 0){
        mpz_t result;
        mpz_init(result);
        brachen(argv[2], result, state);
        printf("0x");
        mpz_out_str(stdout,16,result);
        printf("\n");
        mpz_clear(result);
    }
    else
        error(1, "Invalid usage");
    gmp_randclear(state);
    return 0;
}
