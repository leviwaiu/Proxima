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

void center_image(cairo_t* cr, char* image_filepath, double x, double y, double scale_x, double scale_y){
    cairo_surface_t *image = cairo_image_surface_create_from_png(image_filepath);
    int image_width = cairo_image_surface_get_width(image);
    int image_height = cairo_image_surface_get_height(image);

    cairo_scale(cr, scale_x, scale_y);
    cairo_set_source_surface(cr, image, x / scale_x - image_width/2.0, y / scale_y - image_height/2.0);
    cairo_paint(cr);
    cairo_surface_destroy(image);
    cairo_scale(cr, 1.0/scale_x, 1.0/scale_y);
}