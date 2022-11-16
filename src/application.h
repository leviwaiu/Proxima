//
// Created by leviwaiu on 01/10/22.
//
#include <gtk/gtk.h>

#ifndef PROXIMA_APPLICATION_H
#define PROXIMA_APPLICATION_H

static const int MAX_DISPLAY_LENGTH = 4;
static const int MAX_CHARACTER_LENGTH = 128;

static const int TYPE_TRAIN = 100;
static const int TYPE_BUS = 200;

struct display_list{
    int pred_length;
    struct color* rect_bgcol;
    int* type;
    char** rect_display;
    char** destinations;
    int* minutes;
};

struct color{
    double red;
    double green;
    double blue;
};

void draw_information(cairo_t* cr, gpointer user_data);

void draw_function(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data);

int update(gpointer user_data);

void activate (GtkApplication *app, gpointer user_data);

#endif //PROXIMA_APPLICATION_H
