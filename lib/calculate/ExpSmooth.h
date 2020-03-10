/*
  ExpSmooth.h - Exponential Smoothing using only integers

  Copyright 2017 Costin STROIE <costinstroie@eridu.eu.org>

  https://en.wikipedia.org/wiki/Exponential_smoothing
  y0 = x
  y = (1-a)y + ax
*/

#ifndef EXP_SMOOTH_H
#define EXP_SMOOTH_H

#include "Arduino.h"

class ExpSmooth
{
public:
    ExpSmooth();
    int calc06(int x);      /* a = 6.25%  */
    int calc12(int x);      /* a = 12.5%  */
    int calc25(int x);      /* a = 25%    */
    int calc50(int x);      /* a = 50%    */
    int calc75(int x);      /* a = 75%    */
    int calc88(int x);      /* a = 87.5%  */
    int calc94(int x);      /* a = 93.75% */
    void setInitial(int x); /* Set initial value */
private:
    int y;
};

#endif /* EXP_SMOOTH_H */
