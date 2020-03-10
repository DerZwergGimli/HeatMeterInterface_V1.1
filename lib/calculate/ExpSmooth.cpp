/*
  ExpSmooth.cpp - Exponential Smoothing using only integers

  Copyright 2017 Costin STROIE <costinstroie@eridu.eu.org>

  https://en.wikipedia.org/wiki/Exponential_smoothing
  y0 = x
  y = (1-a)y + ax
*/

#include "Arduino.h"
#include "ExpSmooth.h"

ExpSmooth::ExpSmooth()
{
}

/* a = 6.25% */
int ExpSmooth::calc06(int x)
{
    y = ((y << 4) - y + x + 8) >> 4;
    return y;
}

/* a = 12.5% */
int ExpSmooth::calc12(int x)
{
    y = ((y << 3) - y + x + 4) >> 3;
    return y;
}

/* a = 25% */
int ExpSmooth::calc25(int x)
{
    y = (((y << 2) - y + x) + 2) >> 2;
    return y;
}

/* a = 50% */
int ExpSmooth::calc50(int x)
{
    y = (y + x + 1) >> 1;
    return y;
}

/* a = 75% */
int ExpSmooth::calc75(int x)
{
    y = ((y + (x << 2) - x) + 2) >> 2;
    return y;
}

/* a = 87.5% */
int ExpSmooth::calc88(int x)
{
    y = (y + (x << 3) - x + 4) >> 3;
    return y;
}

/* a = 93.75% */
int ExpSmooth::calc94(int x)
{
    y = (y + (x << 4) - x + 8) >> 4;
    return y;
}

/* Set initial value */
void ExpSmooth::setInitial(int x)
{
    y = x;
}
