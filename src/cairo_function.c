#include <gtk/gtk.h>
#include "cairo_function.h"
//
// Created by leviwaiu on 27/08/22.
//


void rounded_rectangle(cairo_t* cr, double x, double y, double width, double height){
    double aspect = 1.0;
    double corner_radius = height / 10.0;

    double radius = corner_radius / aspect;
    double degrees = M_PI / 180.0;

    cairo_new_sub_path(cr);
    cairo_arc(cr, x + width - radius, y + radius, radius, -90 * degrees, 0);
    cairo_arc(cr, x + width - radius, y + height - radius, radius, 0, 90 * degrees);
    cairo_arc(cr, x + radius, y + height - radius, radius, 90 * degrees, 180 * degrees);
    cairo_arc(cr, x + radius, y + radius, radius, 180 * degrees, 270 * degrees);

    cairo_close_path(cr);
    cairo_fill_preserve(cr);

    cairo_stroke(cr);
}

void center_text(cairo_t* cr, char* text, double x, double y, int align){
    cairo_text_extents_t extents;
    cairo_text_extents(cr, text, &extents);

    y = y - (extents.height / 2 + extents.y_bearing);
    if(align == 0) {
        x = x - (extents.width / 2 + extents.x_bearing);
    }
    else if(align == 2){
        x = x - (extents.width + extents.x_bearing);
    }

    cairo_move_to(cr, x, y);
    cairo_show_text(cr, text);
};
