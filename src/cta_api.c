//
// Created by leviwaiu on 30/08/22.
//

#include "cta_api.h"
#include "key.h"

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <gtk/gtk.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>


struct memory {
    char* response;
    size_t size;
};

static size_t cb(void *data, size_t size, size_t nmemb, void *userdata) {
    size_t realsize = size * nmemb;
    struct memory *mem = (struct memory *)userdata;

    char *ptr = realloc(mem->response, mem->size + realsize + 1);
    if(ptr == NULL){
        return 0;
    }

    mem->response = ptr;
    memcpy(&(mem->response[mem->size]), data, realsize);
    mem->size += realsize;
    mem->response[mem->size] = 0;

    return realsize;
}

int check_api_pointer(gpointer user_data){

    GPtrArray* user_array = (GPtrArray*)user_data;

    CURL* curl = curl_easy_init();
    struct route_requested* bus_reply = (struct route_requested*) g_ptr_array_index(user_array, 1);
    struct route_requested* train_reply = (struct route_requested*) g_ptr_array_index(user_array, 2);


    if(curl) {
        check_bus_api(curl, bus_reply);

        check_train_api(curl, train_reply);
    }

    return 1;
}

void check_bus_api(CURL *curl, struct route_requested* reply){
    CURLcode res;
    cJSON* result_json;
    cJSON* bustime_response;
    cJSON* prd;

    struct memory content = {0};

    char api_string[128];

    char stn_str[32];
    snprintf(stn_str, 32, "%d,%d", reply->stn_id[0], reply->stn_id[1]);
    snprintf(api_string, 128,
             "https://ctabustracker.com/bustime/api/v2/getpredictions?key=%s&stpid=%s&format=json",
             BUS_API_KEY, stn_str);

    curl_easy_setopt(curl, CURLOPT_URL, api_string);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&content);
    do {
        res = curl_easy_perform(curl);
    }
    while(res != CURLE_OK);

    result_json = cJSON_Parse(content.response);
    bustime_response = cJSON_GetObjectItem(result_json, "bustime-response");
    prd = cJSON_GetObjectItem(bustime_response, "prd");
    int prd_size = cJSON_GetArraySize(prd);
    if (prd_size > 4) {
        prd_size = 4;
    }
    reply->pred_length = prd_size;

    for (int i = 0; i < prd_size; i++) {
        cJSON *item = cJSON_GetArrayItem(prd, i);

        cJSON *dest_json = cJSON_GetObjectItem(item, "des");
        cJSON *min_json = cJSON_GetObjectItem(item, "prdctdn");
        cJSON *route_json = cJSON_GetObjectItem(item, "rt");
        char *min_string = min_json->valuestring;
        long min = strtol(min_string, NULL, 10);
        reply->minutes[i] = (int) min;

        size_t number_len = strlen(route_json->valuestring) + 1;
        memcpy(reply->line[i], route_json->valuestring, number_len);

        size_t dest_len = strlen(dest_json->valuestring) + 1;
        memcpy(reply->destinations[i], dest_json->valuestring, dest_len);
    }
    cJSON_Delete(result_json);
}

time_t find_time_t(char* time_string){

    struct tm timestruct = {0};
    sscanf(time_string, "%4d-%2d-%2dT%2d:%2d:%2d", &timestruct.tm_year, &timestruct.tm_mon, &timestruct.tm_mday,
           &timestruct.tm_hour, &timestruct.tm_min, &timestruct.tm_sec);

    timestruct.tm_year -= 100;
    timestruct.tm_mon --;

    time_t out_time = mktime(&timestruct) + 1;

    return out_time;
}


void check_train_api(CURL *curl, struct route_requested *req){
    CURLcode res;
    struct memory content = {0};

    char api_string[128];


    snprintf(api_string, 128,
             "https://lapi.transitchicago.com/api/1.0/ttarrivals.aspx?key=%s&mapid=%d&max=4&outputType=json",
             TRAIN_API_KEY, req->stn_id[0]);

    curl_easy_setopt(curl, CURLOPT_URL, api_string);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&content);
    do {
        res = curl_easy_perform(curl);
    }
    while(res != CURLE_OK);

    cJSON *result_json = cJSON_Parse(content.response);
    cJSON *train_response = cJSON_GetObjectItem(result_json, "ctatt");
    cJSON *etas = cJSON_GetObjectItem(train_response, "eta");
    int eta_size = cJSON_GetArraySize(etas);
    if(eta_size > 4){
        eta_size = 4;
    }
    req->pred_length = eta_size;

    for (int i = 0; i < eta_size; i++) {
        cJSON *item = cJSON_GetArrayItem(etas, i);

        cJSON *dest_json = cJSON_GetObjectItem(item, "destNm");
        cJSON *line_json = cJSON_GetObjectItem(item, "rt");

        cJSON *pred_time_json = cJSON_GetObjectItem(item, "prdt");
        cJSON *arr_time_json = cJSON_GetObjectItem(item, "arrT");

        time_t abs_pred_time = find_time_t(pred_time_json->valuestring);
        time_t abs_arr_time = find_time_t(arr_time_json->valuestring);

        int minutes = (int) (abs_arr_time - abs_pred_time) / 60;

        size_t line_len = strlen(line_json->valuestring) + 1;
        memcpy(req->line[i], line_json->valuestring, line_len);

        size_t dest_len = strlen(dest_json->valuestring) + 1;
        memcpy(req->destinations[i], dest_json->valuestring, dest_len);

        req->minutes[i] = minutes;
    }

    cJSON_Delete(result_json);

}