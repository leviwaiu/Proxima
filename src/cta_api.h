//
// Created by leviwaiu on 30/08/22.
//
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <gtk/gtk.h>

#ifndef PROXIMA_CTA_API_H
#define PROXIMA_CTA_API_H

struct bus_requested{
        char** numbers;
        int* stn_no;
        char** destinations;
        int* minutes;
        int pred_length;
};

struct train_requested{
        char** line;
        int map_id;
        char** destinations;
        int* minutes;
        int pred_length;
};


int check_api_pointer(gpointer user_data);
void check_bus_api(CURL *curl, struct bus_requested* reply);

#endif //PROXIMA_CTA_API_H
