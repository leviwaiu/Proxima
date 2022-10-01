//
// Created by leviwaiu on 30/08/22.
//

#include "cta_api.h"
#include "key.h"

#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <stdlib.h>
#include <gtk/gtk.h>


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

    CURL* curl = curl_easy_init();
    struct bus_requested* reply = (struct bus_requested*) user_data;

    if(curl) {
        check_bus_api(curl, reply);
    }

    return 1;
}

void check_bus_api(CURL *curl, struct bus_requested* reply){
    CURLcode res;
    cJSON* result_json;
    cJSON* bustime_response;
    cJSON* prd;

    struct memory content = {0};

    char api_string[128];

    char stn_str[32];
    snprintf(stn_str, 32, "%d,%d", reply->stn_no[0], reply->stn_no[1]);
    snprintf(api_string, 128,
             "https://ctabustracker.com/bustime/api/v2/getpredictions?key=%s&stpid=%s&format=json",
             BUS_API_KEY, stn_str);

    curl_easy_setopt(curl, CURLOPT_URL, api_string);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&content);
    res = curl_easy_perform(curl);

    int prd_size = 0;
    if(res == CURLE_OK){
        result_json = cJSON_Parse(content.response);
        bustime_response = cJSON_GetObjectItem(result_json, "bustime-response");
        prd = cJSON_GetObjectItem(bustime_response, "prd");

        prd_size = cJSON_GetArraySize(prd);
        if(prd_size > 4){
            prd_size = 4;
        }
        reply->pred_length = prd_size;
        for(int i = 0; i < prd_size; i++){
            cJSON* item = cJSON_GetArrayItem(prd, i);

            cJSON* dest_json = cJSON_GetObjectItem(item, "des");
            cJSON* min_json = cJSON_GetObjectItem(item, "prdctdn");
            cJSON* route_json = cJSON_GetObjectItem(item, "rt");
            char* min_string = min_json->valuestring;
            long min = strtol(min_string, NULL, 10);
            reply->minutes[i] = (int)min;
            reply->numbers[i] = route_json->valuestring;
            reply->destinations[i] = dest_json->valuestring;
        }
    }
}

void check_train_api(CURL *curl, struct train_requested *req){
    CURLcode res;
    struct memory content = {0};

    char api_string[128];
    snprintf(api_string, 128,
             "https://lapi.transitchicago.com/api/1.0/ttarrivals.aspx?key=%s&mapid=%d&max=4&outputType=JSON",
             TRAIN_API_KEY, req->map_id);

    curl_easy_setopt(curl, CURLOPT_URL, api_string);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&content);
    res = curl_easy_perform(curl);

    int eta_size = 0;
    if(res == CURLE_OK){
        cJSON *result_json = cJSON_Parse(content.response);
        cJSON *train_response = cJSON_GetObjectItem(result_json, "ctatt");
        cJSON *etas = cJSON_GetObjectItem(train_response, "etas");

        eta_size = cJSON_GetArraySize(etas);

        for(int i = 0; i < eta_size; i++){
            cJSON* item = cJSON_GetArrayItem(etas, i);

            cJSON* dest_json = cJSON_GetObjectItem(item, "destNm");
            cJSON* line_json = cJSON_GetObjectItem(item, "rt");
        }
    }

}