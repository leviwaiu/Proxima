//
// Created by leviwaiu on 01/10/22.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <curl/curl.h>

#include "cairo_function.h"
#include "cta_api.h"
#include "application.h"

void draw_information(cairo_t* cr, gpointer user_data){
    double type_height = 60.0;
    double type_width = 100;

    struct display_list *req = (struct display_list *)user_data;

    if(req != NULL) {

        int size = req->pred_length;
        for(int i = 0; i < size; i++) {
            char* destination = req->destinations[i];
            char* route = req->rect_display[i];

            struct color rect_col = req->rect_bgcol[i];

            char minleft_string[3];
            snprintf(minleft_string, 3, "%d", req->minutes[i]);

            cairo_set_source_rgb(cr, rect_col.red, rect_col.green, rect_col.blue);
            rounded_rectangle(cr, 10, 10 + (type_height + 5) * i, type_width, type_height);

            cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
            cairo_select_font_face(cr, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
            if(req->type[i] == TYPE_BUS) {
                cairo_set_font_size(cr, 50.0);
                center_text(cr, route, 60, 40 + (type_height + 5) * i, 0);
            }
            if(req->type[i] == TYPE_TRAIN) {
                center_image(cr, "res/train_white.png", 60, 41 + (type_height + 5) * i, 0.4, 0.4);
            }
            cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
            cairo_set_font_size(cr, 28.0);
            center_text(cr, destination, 120 , 40 + (type_height + 5) * i, 1);
            cairo_set_font_size(cr, 30.0);
            center_text(cr, minleft_string, 435, 40 + (type_height + 5) * i, 2);
            cairo_set_font_size(cr, 14.0);
            center_text(cr,"mins", 475, 40 + (type_height + 5) * i, 2);

        }
    }

    cairo_move_to(cr, 480, 0);
    cairo_line_to(cr, 480, 320);
}

void draw_function(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data){

    GPtrArray* user_pointer = (GPtrArray*)user_data;
    cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
    cairo_paint(cr);
    cairo_set_line_width (cr, 2.0);

    gpointer display_list = g_ptr_array_index(user_pointer, 3);
    draw_information(cr, display_list);

    gpointer string_ptr = g_ptr_array_index(user_pointer, 4);
    char* time_string =(char*)string_ptr;

    g_ptr_array_remove(user_data, string_ptr);

    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_select_font_face(cr, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 30);
    center_text(cr, time_string, 240, 305, 0);

    cairo_stroke(cr);
}

int update(gpointer user_data){
    GPtrArray* user_data_pointers = (GPtrArray*)user_data;
    GtkWidget* drawing = (GtkWidget*)g_ptr_array_index(user_data_pointers, 0);

    time_t timestamp = time(0);
    struct tm* local_struct = localtime(&timestamp);

    char local_string[10];
    snprintf(local_string, sizeof(local_string), "%02d:%02d:%02d", local_struct->tm_hour, local_struct->tm_min, local_struct->tm_sec);
    char* timestamp_string = malloc(sizeof(char) * 10);
    strcpy(timestamp_string,local_string);


    g_ptr_array_add(user_data_pointers, (gpointer) timestamp_string);

    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing), draw_function, (gpointer)user_data_pointers, NULL);

    return 1;
}


void display_list_copy(struct route_requested* route_list, int route_index, struct display_list *display_list, int display_index){

    size_t dest_len = strlen(route_list->destinations[route_index]) + 1;
    memcpy(display_list->destinations[display_index], route_list->destinations[route_index], dest_len);

    size_t line_len = strlen(route_list->line[route_index]) + 1;
    memcpy(display_list->rect_display[display_index], route_list->line[route_index], line_len);

    display_list->minutes[display_index] = route_list->minutes[route_index];

}

void set_color(struct color* col, double red, double green, double blue){
   col->red = red;
   col->green = green;
   col->blue = blue;
}

void arrange_list(gpointer bus_info, gpointer train_info, gpointer display){
    struct route_requested *bus_req = (struct route_requested *)bus_info;
    struct route_requested *train_req = (struct route_requested *)train_info;
    struct display_list *display_list = (struct display_list *) display;

    int bus_index = 0;
    int train_index = 0;
    int display_length = 0;

    while(display_length < 4) {

        if(bus_index >= bus_req->pred_length && train_index >= train_req->pred_length){
            break;
        }

        if(bus_index >= bus_req->pred_length) {
            display_list_copy(train_req, train_index, display_list, display_length);
            display_list->type[display_length] = TYPE_TRAIN;
            set_color(&display_list->rect_bgcol[display_length], 0.0, 0.613,0.859);
            train_index++;
        }
        else if(train_index >= train_req->pred_length) {
            display_list_copy(bus_req, bus_index, display_list, display_length);
            display_list->type[display_length] = TYPE_BUS;
            set_color(&display_list->rect_bgcol[display_length], 0.0, 0.404,0.557);
            bus_index++;
        }
        else {
            int current_bus_minute = bus_req->minutes[bus_index];
            int current_train_minute = train_req->minutes[train_index];

            if (current_bus_minute < current_train_minute) {
                display_list_copy(bus_req, bus_index, display_list, display_length);
                display_list->type[display_length] = TYPE_BUS;
                set_color(&display_list->rect_bgcol[display_length], 0.0, 0.404,0.557);
                bus_index++;
            } else {
                display_list_copy(train_req, train_index, display_list, display_length);
                display_list->type[display_length] = TYPE_TRAIN;
                set_color(&display_list->rect_bgcol[display_length], 0.0, 0.613,0.859);
                train_index++;
            }
        }

        display_length++;
    }

    display_list->pred_length = display_length;

}


int list_update(gpointer user_data){
    GPtrArray *user_data_pointers = (GPtrArray*)user_data;
    check_api_pointer(user_data_pointers);

    gpointer bus_info = g_ptr_array_index(user_data_pointers, 1);
    gpointer train_info = g_ptr_array_index(user_data_pointers, 2);
    gpointer display_list = g_ptr_array_index(user_data_pointers, 3);

    arrange_list(bus_info, train_info, display_list);

    return 1;
}

void activate (GtkApplication *app, gpointer user_data) {
    GtkWidget *window;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Window");
    gtk_window_set_default_size(GTK_WINDOW(window), 480, 320);

    GtkWidget *drawing = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing, 480, 320);
    //gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing), draw_function, NULL, NULL);
    gtk_window_set_child(GTK_WINDOW(window), drawing);

    struct route_requested *req_bus = malloc(sizeof(struct route_requested));

    int* bus_station_list = malloc(2 * sizeof(int));
    bus_station_list[0] = 6342;
    bus_station_list[1] = 303;
    req_bus->stn_id = bus_station_list;

    req_bus->line = malloc(MAX_DISPLAY_LENGTH * sizeof(char*));
    req_bus->destinations = malloc(MAX_DISPLAY_LENGTH * sizeof(char*));
    req_bus->minutes = malloc(MAX_DISPLAY_LENGTH * sizeof(int));

    for(int i = 0; i < MAX_DISPLAY_LENGTH; i++){
        req_bus->line[i] = malloc(MAX_CHARACTER_LENGTH * sizeof(char));
        req_bus->destinations[i] = malloc(MAX_CHARACTER_LENGTH * sizeof(char));
    }

    struct route_requested *req_train = malloc(sizeof(struct route_requested));

    int* train_station_list = malloc(1 * sizeof(int));
    train_station_list[0] = 40470;

    req_train->stn_id = train_station_list;
    req_train->destinations = malloc(MAX_DISPLAY_LENGTH * sizeof(char*));
    req_train->line = malloc(MAX_DISPLAY_LENGTH * sizeof(char*));
    req_train->minutes = malloc(MAX_DISPLAY_LENGTH * sizeof(int));

    for(int i = 0; i < MAX_DISPLAY_LENGTH; i++){
        req_train->line[i] = malloc(MAX_CHARACTER_LENGTH * sizeof(char));
        req_train->destinations[i] = malloc(MAX_CHARACTER_LENGTH * sizeof(char));
    }

    struct display_list *display_list = malloc(sizeof(struct display_list));
    display_list->minutes = malloc(MAX_DISPLAY_LENGTH * sizeof(int));
    display_list->type = malloc(MAX_DISPLAY_LENGTH * sizeof(int));
    display_list->destinations = malloc(MAX_DISPLAY_LENGTH * sizeof(char*));
    display_list->rect_display = malloc(MAX_DISPLAY_LENGTH * sizeof(char*));
    display_list->rect_bgcol = malloc(MAX_DISPLAY_LENGTH * sizeof(struct color));

    for(int i = 0; i < MAX_DISPLAY_LENGTH; i++){
        display_list->destinations[i] = malloc(MAX_CHARACTER_LENGTH * sizeof(char));
        display_list->rect_display[i] = malloc(MAX_CHARACTER_LENGTH * sizeof(char));
    }

    GPtrArray* user_data_pointers = g_ptr_array_new();
    g_ptr_array_add(user_data_pointers, (gpointer) drawing);
    g_ptr_array_add(user_data_pointers, (gpointer)req_bus);
    g_ptr_array_add(user_data_pointers, (gpointer)req_train);
    g_ptr_array_add(user_data_pointers, (gpointer)display_list);

    //debug
    list_update((gpointer)user_data_pointers);

    g_timeout_add(100, (GSourceFunc) update, (gpointer)user_data_pointers);

    g_timeout_add_seconds(30, (GSourceFunc) list_update, (gpointer) user_data_pointers);

    //gtk_window_fullscreen(GTK_WINDOW(window));

    gtk_window_present(GTK_WINDOW(window));
}