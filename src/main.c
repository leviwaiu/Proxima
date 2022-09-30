#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <curl/curl.h>
#include "cairo_function.h"
#include "cta_api.h"

void draw_information(cairo_t* cr, gpointer user_data){
    double type_height = 60.0;
    double type_width = 100;

    struct bus_requested *req = (struct bus_requested *)user_data;

    if(req != NULL) {

        int size = req->pred_length;
        for(int i = 0; i < size; i++) {
            char* destination = req->destinations[i];
            char* route = req->numbers[i];

            char minleft_string[3];
            snprintf(minleft_string, 3, "%d", req->minutes[i]);

            cairo_set_source_rgb(cr, 0.0, 0.404, 0.557);
            rounded_rectangle(cr, 10, 10 + (type_height + 5) * i, type_width, type_height);

            cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
            cairo_select_font_face(cr, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
            cairo_set_font_size(cr, 50.0);
            center_text(cr, route, 60, 40 + (type_height + 5) * i, 0);
            cairo_set_font_size(cr, 28.0);
            center_text(cr, destination, 120 , 40 + (type_height + 5) * i, 1);
            cairo_set_font_size(cr, 30.0);
            center_text(cr, minleft_string, 435, 40 + (type_height + 5) * i, 2);
            cairo_set_font_size(cr, 14.0);
            center_text(cr,"mins", 475, 40 + (type_height + 5) * i, 2);
        }
    }

//    cairo_move_to(cr, 480, 0);
//    cairo_line_to(cr, 480, 320);
}

void draw_function(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data){

    GPtrArray* user_pointer = (GPtrArray*)user_data;
    cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
    cairo_paint(cr);
    cairo_set_line_width (cr, 2.0);

    gpointer bus_info = g_ptr_array_index(user_pointer, 1);

    draw_information(cr, bus_info);

    gpointer string_ptr = g_ptr_array_index(user_pointer, 2);
    char* time_string =(char*)string_ptr;

    g_ptr_array_remove(user_data, string_ptr);

    cairo_set_font_size(cr, 30);
    center_text(cr, time_string, 240, 305, 0);

    cairo_stroke(cr);
}


int update(gpointer user_data){
    GPtrArray* user_data_pointers = (GPtrArray*)user_data;
    GtkWidget *drawing = (GtkWidget*)g_ptr_array_index(user_data_pointers, 0);


    time_t timestamp = time(0);
    struct tm *local_struct = localtime(&timestamp);

    char local_string[10];
    snprintf(local_string, sizeof(local_string), "%02d:%02d:%02d", local_struct->tm_hour, local_struct->tm_min, local_struct->tm_sec);
    char *timestamp_string = malloc(sizeof(char) * 10);
    strcpy(timestamp_string,local_string);


    g_ptr_array_add(user_data_pointers, (gpointer) timestamp_string);

    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing), draw_function, (gpointer)user_data_pointers, NULL);

    return 1;
}

void activate (GtkApplication *app, gpointer user_data) {
    GtkWidget *window;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Window");
    gtk_window_set_default_size(GTK_WINDOW(window), 480, 320);

    GtkWidget *drawing = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing, 480, 320);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing), draw_function, NULL, NULL);
    gtk_window_set_child(GTK_WINDOW(window), drawing);

    struct bus_requested *req = malloc(sizeof(struct bus_requested));

    int* station_list = malloc(2 * sizeof(int));
    station_list[0] = 6342;
    station_list[1] = 303;
    req->stn_no = station_list;

    req->numbers = malloc(4 * sizeof(char*));
    req->destinations = malloc(4 * sizeof(char*));
    req->minutes = malloc(4 * sizeof(int));

    GPtrArray* user_data_pointers = g_ptr_array_new();
    g_ptr_array_add(user_data_pointers, (gpointer) drawing);
    g_ptr_array_add(user_data_pointers, (gpointer)req);

    //debug
    g_timeout_add(100, (GSourceFunc) update, (gpointer)user_data_pointers);

    check_api_pointer(req);

    g_timeout_add_seconds(30, (GSourceFunc)check_api_pointer, req);

    gtk_window_fullscreen(GTK_WINDOW(window));

    gtk_window_present(GTK_WINDOW(window));
}


int main(int argc, char **argv) {

    GtkApplication *app;
    int status;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    curl_global_cleanup();
    return status;
}
