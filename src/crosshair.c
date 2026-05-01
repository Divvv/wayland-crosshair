#include <gtk/gtk.h>
#include <gtk-layer-shell.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define RADIUS 3.0

static pid_t wlsunset_pid = -1;
static const char *image_path = NULL;

static void kill_wlsunset(void) {
    if (wlsunset_pid > 0)
        kill(wlsunset_pid, SIGTERM);
}

static void spawn_wlsunset(const char *gamma) {
    wlsunset_pid = fork();
    if (wlsunset_pid == 0) {
        execlp("wlsunset", "wlsunset", "-T", "6501", "-t", "6500", "-g", gamma, NULL);
        _exit(1);
    }
    atexit(kill_wlsunset);
}

static gboolean on_draw(GtkWidget *w, cairo_t *cr, gpointer _) {
    (void)_;
    GtkAllocation a;
    gtk_widget_get_allocation(w, &a);
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_source_rgba(cr, 0, 0, 0, 0);
    cairo_paint(cr);
    if (image_path) {
        cairo_surface_t *img = cairo_image_surface_create_from_png(image_path);
        double iw = cairo_image_surface_get_width(img);
        double ih = cairo_image_surface_get_height(img);
        cairo_set_source_surface(cr, img, (a.width - iw) / 2.0, (a.height - ih) / 2.0);
        cairo_paint(cr);
        cairo_surface_destroy(img);
    } else {
        cairo_set_source_rgb(cr, 0, 1, 0);
        cairo_arc(cr, a.width / 2.0, a.height / 2.0, RADIUS, 0, 2 * G_PI);
        cairo_fill(cr);
    }
    return FALSE;
}

static void activate(GtkApplication *app, gpointer _) {
    (void)_;
    GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_application_add_window(GTK_APPLICATION(app), GTK_WINDOW(win));
    gtk_window_set_decorated(GTK_WINDOW(win), FALSE);

    GdkScreen *screen = gtk_window_get_screen(GTK_WINDOW(win));
    GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
    if (visual) gtk_widget_set_visual(win, visual);
    gtk_widget_set_app_paintable(win, TRUE);

    gtk_layer_init_for_window(GTK_WINDOW(win));
    gtk_layer_set_layer(GTK_WINDOW(win), GTK_LAYER_SHELL_LAYER_OVERLAY);
    gtk_layer_set_keyboard_mode(GTK_WINDOW(win), GTK_LAYER_SHELL_KEYBOARD_MODE_NONE);
    gtk_layer_set_anchor(GTK_WINDOW(win), GTK_LAYER_SHELL_EDGE_LEFT,   TRUE);
    gtk_layer_set_anchor(GTK_WINDOW(win), GTK_LAYER_SHELL_EDGE_RIGHT,  TRUE);
    gtk_layer_set_anchor(GTK_WINDOW(win), GTK_LAYER_SHELL_EDGE_TOP,    TRUE);
    gtk_layer_set_anchor(GTK_WINDOW(win), GTK_LAYER_SHELL_EDGE_BOTTOM, TRUE);
    /* -1 = ignore exclusive zones from bars/docks, use full screen dimensions */
    gtk_layer_set_exclusive_zone(GTK_WINDOW(win), -1);

    GtkWidget *da = gtk_drawing_area_new();
    g_signal_connect(da, "draw", G_CALLBACK(on_draw), NULL);
    gtk_container_add(GTK_CONTAINER(win), da);

    gtk_widget_show_all(win);

    cairo_region_t *empty = cairo_region_create();
    gdk_window_input_shape_combine_region(gtk_widget_get_window(win), empty, 0, 0);
    cairo_region_destroy(empty);
}

int main(int argc, char **argv) {
    const char *gamma = "1.2";
    for (int i = 1; i < argc; i++) {
        if ((strcmp(argv[i], "--image") == 0 || strcmp(argv[i], "-i") == 0) && i + 1 < argc) {
            image_path = argv[++i];
        } else if ((strcmp(argv[i], "--gamma") == 0 || strcmp(argv[i], "-g") == 0) && i + 1 < argc) {
            gamma = argv[++i];
        } else {
            gamma = argv[i];
        }
    }
    spawn_wlsunset(gamma);

    GtkApplication *app = gtk_application_new("se.n1k0.crosshair", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), 0, NULL);
    g_object_unref(app);
    return status;
}
