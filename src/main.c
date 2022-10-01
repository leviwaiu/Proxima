#include <string.h>
#include <gtk/gtk.h>
#include <curl/curl.h>

#include "cairo_function.h"
#include "cta_api.h"
#include "application.h"


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
