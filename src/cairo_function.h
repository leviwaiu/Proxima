//
// Created by leviwaiu on 28/08/22.
//
#include <gtk/gtk.h>

#ifndef PROXIMA_CAIRO_FUNCTION_H
#define PROXIMA_CAIRO_FUNCTION_H

void rounded_rectangle(cairo_t* cr, double x, double y, double width, double height);

void center_text(cairo_t* cr, char* text, double x, double y, int align);

void center_image(cairo_t* cr, char* image_filepath, double x, double y, double scale_x, double scale_y);

#endif //PROXIMA_CAIRO_FUNCTION_H
