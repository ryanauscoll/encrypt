#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void xor_encrypt_decrypt(char *data, int data_len, char *key, int key_len) {
    for (int i = 0; i < data_len; i++) {
        data[i] ^= key[i % key_len];  // Simple XOR encryption/decryption
    }
}

// Struct to hold GUI data
typedef struct {
    GtkWidget *input_file_entry;
    GtkWidget *output_file_entry;
    GtkWidget *key_entry;
    GtkWidget *encrypt_button;
    GtkWidget *decrypt_button;
} GUIData;

// Function to handle encryption/decryption
void process_file(GtkWidget *widget, gpointer data) {
    GUIData *gui_data = (GUIData *)data;

    const char *input_file = gtk_entry_get_text(GTK_ENTRY(gui_data->input_file_entry));
    const char *output_file = gtk_entry_get_text(GTK_ENTRY(gui_data->output_file_entry));
    const char *key = gtk_entry_get_text(GTK_ENTRY(gui_data->key_entry));

    if (strlen(input_file) == 0 || strlen(output_file) == 0 || strlen(key) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "Please fill in all fields!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    // Open the input file
    FILE *file = fopen(input_file, "rb");
    if (!file) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "Could not open input file!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    // Read the file into memory
    char *buffer = (char *)malloc(file_size);
    fread(buffer, 1, file_size, file);
    fclose(file);

    // Determine if we are encrypting or decrypting
    if (widget == gui_data->encrypt_button) {
        xor_encrypt_decrypt(buffer, file_size, (char *)key, strlen(key));  // Encrypt
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, "File successfully encrypted!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    } else if (widget == gui_data->decrypt_button) {
        xor_encrypt_decrypt(buffer, file_size, (char *)key, strlen(key));  // Decrypt
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, "File successfully decrypted!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }

    // Write the processed data to the output file
    FILE *output = fopen(output_file, "wb");
    fwrite(buffer, 1, file_size, output);
    fclose(output);

    free(buffer);
}

// Function to open a file chooser dialog and set the file path in the text entry
void on_file_browse(GtkWidget *widget, gpointer entry) {
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new("Choose File", NULL,
        GTK_FILE_CHOOSER_ACTION_OPEN, "_Cancel", GTK_RESPONSE_CANCEL, "_Open", GTK_RESPONSE_ACCEPT, NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *file_path;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        file_path = gtk_file_chooser_get_filename(chooser);
        gtk_entry_set_text(GTK_ENTRY(entry), file_path);
        g_free(file_path);
    }
    gtk_widget_destroy(dialog);
}

// Function to build the GTK GUI
int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Create main window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Ryan's File Encrypter");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 200);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    // Create a header bar for modern look
    GtkWidget *header_bar = gtk_header_bar_new();
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(header_bar), TRUE);
    gtk_header_bar_set_title(GTK_HEADER_BAR(header_bar), "Ryan's File Encrypter");
    gtk_window_set_titlebar(GTK_WINDOW(window), header_bar);

    // Create a grid for layout
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Create and add labels and entries for file paths and key
    GtkWidget *input_file_label = gtk_label_new("Input File:");
    gtk_grid_attach(GTK_GRID(grid), input_file_label, 0, 0, 1, 1);
    GtkWidget *output_file_label = gtk_label_new("Output File:");
    gtk_grid_attach(GTK_GRID(grid), output_file_label, 0, 1, 1, 1);
    GtkWidget *key_label = gtk_label_new("Encryption Key:");
    gtk_grid_attach(GTK_GRID(grid), key_label, 0, 2, 1, 1);

    GtkWidget *input_file_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), input_file_entry, 1, 0, 2, 1);
    GtkWidget *output_file_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), output_file_entry, 1, 1, 2, 1);
    GtkWidget *key_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), key_entry, 1, 2, 2, 1);

    // Create "Browse" buttons
    GtkWidget *input_browse_button = gtk_button_new_with_label("Browse...");
    gtk_grid_attach(GTK_GRID(grid), input_browse_button, 3, 0, 1, 1);
    GtkWidget *output_browse_button = gtk_button_new_with_label("Browse...");
    gtk_grid_attach(GTK_GRID(grid), output_browse_button, 3, 1, 1, 1);

    // Create encrypt and decrypt buttons
    GtkWidget *encrypt_button = gtk_button_new_with_label("Encrypt");
    gtk_grid_attach(GTK_GRID(grid), encrypt_button, 0, 3, 1, 1);
    GtkWidget *decrypt_button = gtk_button_new_with_label("Decrypt");
    gtk_grid_attach(GTK_GRID(grid), decrypt_button, 1, 3, 1, 1);

    // Store widget references in struct
    GUIData gui_data;
    gui_data.input_file_entry = input_file_entry;
    gui_data.output_file_entry = output_file_entry;
    gui_data.key_entry = key_entry;
    gui_data.encrypt_button = encrypt_button;
    gui_data.decrypt_button = decrypt_button;

    // Connect the "Browse" buttons to the file chooser dialog
    g_signal_connect(input_browse_button, "clicked", G_CALLBACK(on_file_browse), input_file_entry);
    g_signal_connect(output_browse_button, "clicked", G_CALLBACK(on_file_browse), output_file_entry);

    // Connect the encrypt and decrypt buttons to process_file function
    g_signal_connect(encrypt_button, "clicked", G_CALLBACK(process_file), &gui_data);
    g_signal_connect(decrypt_button, "clicked", G_CALLBACK(process_file), &gui_data);

    // Connect the window close button
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}