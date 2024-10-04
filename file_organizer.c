#include <gtk/gtk.h>
#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

void show_message(const char *message) {
    GtkWidget *dialog=gtk_message_dialog_new(NULL,
                                               GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_INFO,
                                               GTK_BUTTONS_OK,
                                               "%s", message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void organize_files(const char *folder_path) {
    DIR *dir=opendir(folder_path);
    struct dirent *entry;

    if (dir == NULL) {
        g_print("Error opening directory: %s\n", folder_path);
        return;
    }

    while ((entry=readdir(dir)) != NULL) {
        // Skip "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Get file extension
        char *dot=strrchr(entry->d_name, '.');
        if (dot) {
            char *ext=dot + 1;
            char type_folder[256];

            // Create folder for the file type
            snprintf(type_folder, sizeof(type_folder), "%s/%s", folder_path, ext);
            mkdir(type_folder, 0755); // Ensure the folder is created

            // Move file to the corresponding folder
            char old_path[512], new_path[512];
            snprintf(old_path, sizeof(old_path), "%s/%s", folder_path, entry->d_name);
            snprintf(new_path, sizeof(new_path), "%s/%s/%s", folder_path, ext, entry->d_name);
            rename(old_path, new_path);
        }
    }
    closedir(dir);
    show_message("Files organized successfully!");
}

void on_add_folder(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    dialog=gtk_file_chooser_dialog_new("Select Folder",
                                         NULL,
                                         GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                         "_Cancel", GTK_RESPONSE_CANCEL,
                                         "_Open", GTK_RESPONSE_ACCEPT,
                                         NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *folder_path=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        organize_files(folder_path);
        g_free(folder_path);
    }
    gtk_widget_destroy(dialog);
}

void on_about(GtkWidget *widget, gpointer data) {
    show_message("Author: Jay Mee\n@ J~Net 2024");
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "File Organizer");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 200);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

    // Set dark theme
    GtkCssProvider *provider=gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, "window { background-color: #2e2e2e; }", -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                               GTK_STYLE_PROVIDER(provider),
                                               GTK_STYLE_PROVIDER_PRIORITY_USER);

    GtkWidget *button_add=gtk_button_new_with_label("Add Folder");
    g_signal_connect(button_add, "clicked", G_CALLBACK(on_add_folder), NULL);

    GtkWidget *button_about=gtk_button_new_with_label("About");
    g_signal_connect(button_about, "clicked", G_CALLBACK(on_about), NULL);

    GtkWidget *box=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(box), button_add, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), button_about, TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(window), box);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

