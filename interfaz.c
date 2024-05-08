#include <gtk/gtk.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define SIZE 255
#define COLA_MENSAJES 1234
int mi_socket;
    int tam, numbytes;
    char buffer[SIZE];
    struct sockaddr_in mi_estructura;
    struct solicitud nueva_solicitud;


struct solicitud { // ESTRUCTURA DE LA SOLICITUD
    long tipo;
    char nombre[50];
    char destino[50];
    char horario[50];
    char boleto_tipo[50];
    int costo_boleto;
};


void on_button_clicked(GtkWidget *widget, gpointer data) {
    const gchar *nombre = gtk_entry_get_text(GTK_ENTRY(data));
    const gchar *destino = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(data + 1));
    const gchar *horario_txt;
    const gchar *boleto_tipo;
    gdouble horario = gtk_spin_button_get_value(GTK_SPIN_BUTTON(data + 3));

    switch (gtk_combo_box_get_active(GTK_COMBO_BOX(data + 2))) {
        case 0:
            horario_txt = "Matutino 7-8 AM";
            break;
        case 1:
            horario_txt = "Matutino 9-10 AM";
            break;
        case 2:
            horario_txt = "Matutino 11-12 AM";
            break;
        case 3:
            horario_txt = "Vespertino 1-3 PM";
            break;
        case 4:
            horario_txt = "Vespertino 3-5 PM";
            break;
        case 5:
            horario_txt = "Vespertino 5-7 PM";
            break;
        case 6:
            horario_txt = "Nocturno 8-9 PM";
            break;
        case 7:
            horario_txt = "Nocturno 10-11 PM";
            break;
        case 8:
            horario_txt = "Nocturno 11-12 PM";
            break;
        default:
            horario_txt = "Error";
            break;
    }

    switch (gtk_combo_box_get_active(GTK_COMBO_BOX(data + 4))) {
        case 0:
            boleto_tipo = "Ordinario $50";
            nueva_solicitud.costo_boleto=50;
            break;
        case 1:
            boleto_tipo = "1ra Directo $100";
            nueva_solicitud.costo_boleto=100;
            break;
        case 2:
            boleto_tipo = "Gl Premier $150";
            nueva_solicitud.costo_boleto=150;
            break;
        default:
            boleto_tipo = "Error";
            break;
    }

    // Crear nueva ventana para mostrar los datos seleccionados
    GtkWidget *window;
    GtkWidget *label_nombre;
    GtkWidget *label_destino;
    GtkWidget *label_horario;
    GtkWidget *label_boleto;
    GtkWidget *grid;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Datos Seleccionados");
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 150);

    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    label_nombre = gtk_label_new(NULL);
    gtk_label_set_text(GTK_LABEL(label_nombre), nombre);
    gtk_grid_attach(GTK_GRID(grid), label_nombre, 0, 0, 1, 1);

    label_destino = gtk_label_new(NULL);
    gtk_label_set_text(GTK_LABEL(label_destino), destino);
    gtk_grid_attach(GTK_GRID(grid), label_destino, 0, 1, 1, 1);

    label_horario = gtk_label_new(NULL);
    gtk_label_set_text(GTK_LABEL(label_horario), horario_txt);
    gtk_grid_attach(GTK_GRID(grid), label_horario, 0, 2, 1, 1);

    label_boleto = gtk_label_new(NULL);
    gtk_label_set_text(GTK_LABEL(label_boleto), boleto_tipo);
    gtk_grid_attach(GTK_GRID(grid), label_boleto, 0, 3, 1, 1);

    gtk_widget_show_all(window);
}
void on_button_show_data_clicked(GtkWidget *widget, gpointer data) { // boton de ver los registros

    GtkWidget *parent_window = GTK_WIDGET(data);
    GtkWidget *new_window;
    GtkWidget *label;
    GtkWidget *image11;
    gchar *nombre, *destino, *horario, *boleto;

    // Obtener los datos de la ventana principal
    GtkWidget *grid = gtk_bin_get_child(GTK_BIN(parent_window));
    GtkWidget *entry = gtk_grid_get_child_at(GTK_GRID(grid), 1, 3);
    GtkWidget *combo_destino = gtk_grid_get_child_at(GTK_GRID(grid), 1, 4);
    GtkWidget *combo_horario = gtk_grid_get_child_at(GTK_GRID(grid), 1, 5);
    GtkWidget *combo_boleto = gtk_grid_get_child_at(GTK_GRID(grid), 1, 6);
    
    // CONSEGUIMOS LOS DATOS
    nombre = g_strdup_printf("%s", gtk_entry_get_text(GTK_ENTRY(entry)));
    destino = g_strdup_printf("%s", gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_destino)));
    horario = g_strdup_printf("%s", gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_horario)));
    boleto = g_strdup_printf("%s", gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_boleto)));
    // Enviar la solicitud al servidor
    // Copiar los valores desde los punteros a los miembros de la estructura
    strcpy(nueva_solicitud.nombre, nombre);
    strcpy(nueva_solicitud.destino, destino);
    strcpy(nueva_solicitud.horario, horario);
    strcpy(nueva_solicitud.boleto_tipo, boleto);
    
    nueva_solicitud.tipo = 1; // Tipo de mensaje arbitrario
    if (send(mi_socket, &nueva_solicitud, sizeof(struct solicitud), 0) == -1) {
        perror("Error al enviar la solicitud al servidor");
        exit(EXIT_FAILURE);
    }
    
    printf("Solicitud enviada al servidor.\n");
    
    // Recibir la confirmación del servidor
    numbytes = recv(mi_socket, buffer, SIZE - 1, 0);
    if (numbytes == -1) {
        perror("Error al recibir la confirmación del servidor");
        exit(EXIT_FAILURE);
    }
    buffer[numbytes] = '\0';
    printf("%d bytes recibidos\n", numbytes);
    printf("Recibido: %s\n", buffer);
    
    // Cerrar el socket
    close(mi_socket);

    // Crear nueva ventana para mostrar los datos Y VALIDAR QUE SE MANDARON
    new_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(new_window), "Datos Seleccionados");
    gtk_window_set_default_size(GTK_WINDOW(new_window), 200, 150);

    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(new_window), grid);
    image11 = gtk_image_new_from_file("ARRIBA.png"); // Ruta de la primera imagen
    gtk_grid_attach(GTK_GRID(grid), image11, 0, 0, 1, 1);

    label = gtk_label_new(nombre);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
    g_free(nombre);

    label = gtk_label_new(destino);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);
    g_free(destino);

    label = gtk_label_new(horario);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 3, 1, 1);
    g_free(horario);

    label = gtk_label_new(boleto);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 4, 1, 1);
    g_free(boleto);
    


    GtkWidget *label22;

    // Crear el label con un texto de confirmacion
    label22 = gtk_label_new("\n\n\nBOLETO COMPRADO, DISFRUTE SU VIAJE¡");
    gtk_grid_attach(GTK_GRID(grid), label22, 0, 5, 1, 1);
    gtk_widget_show(label22);
    gtk_widget_show_all(new_window);
}


int main(int argc, char *argv[]) { // funcion main
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *grid;
    GtkWidget *entry_nombre;
    GtkWidget *combo_destino;
    GtkWidget *combo_horario;
    GtkWidget *combo_boleto;
    GtkWidget *spin_horario;
    GtkWidget *label_nombre;
    GtkWidget *label_destino;
    GtkWidget *label_horario;
    GtkWidget *label_boleto;
    GtkWidget *image1, *image2;
    
    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    // SERVIDOR 
    if (argc != 3) {
        printf("error: modo de empleo: cliente ip puerto\n");
        exit(-1);
    }
    
    // Configurar la dirección del servidor
    mi_estructura.sin_family = AF_INET;
    mi_estructura.sin_port = htons(atoi(argv[2]));
    inet_aton(argv[1], &(mi_estructura.sin_addr));
    memset(&(mi_estructura.sin_zero), '\0', 8);
    
    // Crear el socket TCP
    mi_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (mi_socket == -1) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }
    
    // Conectar al servidor
    tam = sizeof(struct sockaddr);
    if (connect(mi_socket, (struct sockaddr *)&mi_estructura, tam) == -1) {
        perror("Error al conectar al servidor");
        exit(EXIT_FAILURE);
    }
    
    
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    // INTERFAZ

    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Crear y agregar la primera imagen en la esquina superior izquierda
    image1 = gtk_image_new_from_file("logo_chico.png"); // Ruta de la primera imagen
    gtk_image_set_pixel_size(GTK_IMAGE(image1), 1.2); // Establecer el tamaño en píxeles
    gtk_grid_attach(GTK_GRID(grid), image1, 0, 0, 1, 1);

    // Crear y agregar la segunda imagen en la esquina superior derecha
    image2 = gtk_image_new_from_file("SISTEMAS_OP.png"); // Ruta de la segunda imagen
    gtk_image_set_pixel_size(GTK_IMAGE(image1), 1.2); // Establecer el tamaño en píxeles
    gtk_grid_attach(GTK_GRID(grid), image2, 1, 0, 1, 1);

    label_nombre = gtk_label_new("Nombre:");
    gtk_grid_attach(GTK_GRID(grid), label_nombre, 0, 3, 1, 1);

    entry_nombre = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entry_nombre, 1, 3, 1, 1);

    label_destino = gtk_label_new("Destino:");
    gtk_grid_attach(GTK_GRID(grid), label_destino, 0, 4, 1, 1);

    combo_destino = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_destino), "Ciudad de México");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_destino), "Guadalajara");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_destino), "Monterrey");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_destino), "Puebla");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_destino), "Tijuana");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_destino), "Mérida");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_destino), "Querétaro");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_destino), "Cancún");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_destino), "Acapulco");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_destino), "Oaxaca");
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_destino), 0);
    gtk_grid_attach(GTK_GRID(grid), combo_destino, 1, 4, 1, 1);

    label_horario = gtk_label_new("Horario:");
    gtk_grid_attach(GTK_GRID(grid), label_horario, 0, 5, 1, 1);

    combo_horario = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_horario), "Matutino 7-8 AM");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_horario), "Matutino 9-10 AM");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_horario), "Matutino 11-12 AM");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_horario), "Vespertino 1-3 PM");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_horario), "Vespertino 3-5 PM");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_horario), "Vespertino 5-7 PM");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_horario), "Nocturno 8-9 PM");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_horario), "Nocturno 10-11 PM");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_horario), "Nocturno 11-12 PM");
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_horario), 0);
    gtk_grid_attach(GTK_GRID(grid), combo_horario, 1, 5, 1, 1);

    label_boleto = gtk_label_new("Tipo de boleto:");
    gtk_grid_attach(GTK_GRID(grid), label_boleto, 0, 6, 1, 1);

    combo_boleto = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_boleto), "Ordinario $50");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_boleto), "1ra Directo $100");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_boleto), "Gl Premier $150");
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_boleto), 0);
    gtk_grid_attach(GTK_GRID(grid), combo_boleto, 1, 6, 1, 1); 
   
   // mandamos los datos al servidor
    button = gtk_button_new_with_label("IMPRIMIR TICKET");
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_show_data_clicked), (gpointer)window);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 9, 2, 1);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}


