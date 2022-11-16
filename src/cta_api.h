//
// Created by leviwaiu on 30/08/22.
//
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <gtk/gtk.h>

#ifndef PROXIMA_CTA_API_H
#define PROXIMA_CTA_API_H

struct memory {
    char* response;
    size_t size;
};

struct route_requested{
        char** line;
        int* stn_id;
        char** destinations;
        int* minutes;
        int pred_length;
};


int check_api_pointer(gpointer user_data);
void check_bus_api(CURL *curl, struct route_requested* reply);
time_t find_time_t(char* time_string);
void check_train_api(CURL *curl, struct route_requested* reply);

#endif //PROXIMA_CTA_API_H
