//
// Created by leviwaiu on 01/10/22.
//
#include <gtk/gtk.h>

#ifndef PROXIMA_APPLICATION_H
#define PROXIMA_APPLICATION_H

void draw_information(cairo_t* cr, gpointer user_data);

void draw_function(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data);

int update(gpointer user_data);

void activate (GtkApplication *app, gpointer user_data);

#endif //PROXIMA_APPLICATION_H
