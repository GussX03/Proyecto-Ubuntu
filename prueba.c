#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <glib.h>

#define COLA_MENSAJES 1234

int mi_socket, nuevo, tam, dinero_total=0;
struct sockaddr_in mi_estructura;
struct solicitud {
    long tipo;
    char nombre[50];
    char destino[50];
    char horario[50];
    char boleto_tipo[50];
    int costo_boleto;
};

void *iniciar_servidor(void *data) {
    int conexiones_manipuladas = 0;
    
    mi_estructura.sin_family = AF_INET;
    mi_estructura.sin_port = 0; // Puerto 0 para que el sistema elija un puerto disponible
    mi_estructura.sin_addr.s_addr = INADDR_ANY;
    printf("Iniciando el servidor...\n");

    // Configurar la dirección del servidor
    mi_estructura.sin_family = AF_INET;
    mi_estructura.sin_port = 0; // Puerto 0 para que el sistema elija un puerto disponible
    mi_estructura.sin_addr.s_addr = INADDR_ANY;
    memset(&(mi_estructura.sin_zero), '\0', 8);
    
    // Crear el socket
    mi_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (mi_socket == -1) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }
    
    // Enlazar el socket a una dirección y puerto específicos
    tam = sizeof(struct sockaddr);
    if (bind(mi_socket, (struct sockaddr *)&mi_estructura, tam) == -1) {
        perror("Error al enlazar el socket");
        exit(EXIT_FAILURE);
    }
    
    // Obtener la dirección y el puerto del socket después de enlazarlo
    getsockname(mi_socket, (struct sockaddr *)&mi_estructura, &tam);
    
    printf("El servidor está escuchando en el puerto: %d\n", ntohs(mi_estructura.sin_port));

    // Configurar el socket para escuchar conexiones entrantes
    if (listen(mi_socket, 10) == -1) { // Permitir hasta 10 conexiones en espera
        perror("Error al escuchar conexiones");
        exit(EXIT_FAILURE);
    }
    
    while (conexiones_manipuladas == 0) { // Esperar hasta 10 conexiones
        printf("Esperando una conexión entrante...\n");
        nuevo = accept(mi_socket, (struct sockaddr *)&mi_estructura, &tam);
        if (nuevo == -1) {
            perror("Error al aceptar la conexión entrante");
            exit(EXIT_FAILURE);
        }

        printf("¡Conexión entrante aceptada!\n");
        
        // Recibir la solicitud del cliente
        struct solicitud solicitud_recibida;
        if (recv(nuevo, &solicitud_recibida, sizeof(struct solicitud), 0) == -1) {
            perror("Error al recibir la solicitud del cliente");
            close(nuevo);
            continue;
        }
        
        // COSTO DE BOLETO COMPARANDO QUE TIPO DE BOLETO ES
        if (strcmp(solicitud_recibida.boleto_tipo, "Ordinario $50") == 0) {
            solicitud_recibida.costo_boleto = 50;
        } else if (strcmp(solicitud_recibida.boleto_tipo, "1ra Directo $100") == 0) {
            solicitud_recibida.costo_boleto = 100;
        } else if (strcmp(solicitud_recibida.boleto_tipo, "Gl Premier $150") == 0) {
            solicitud_recibida.costo_boleto = 150;
        }
  
        // Registrar la solicitud recibida
        printf("Solicitud recibida:\n");
        printf("Nombre : %s\n", solicitud_recibida.nombre);
        printf("Destino: %s\n", solicitud_recibida.destino);
        printf("Horario: %s\n", solicitud_recibida.horario);
        printf("Tipo de boleto: %s\n", solicitud_recibida.boleto_tipo);
        printf("Costo de boleto: %d\n", solicitud_recibida.costo_boleto);
        
        // Calcular dinero total
        dinero_total += solicitud_recibida.costo_boleto;
        
        // Registrar el dinero total recibido hasta ahora
        printf("Dinero total recibido hasta ahora: %d\n", dinero_total);
        
        // Enviar confirmación al cliente
        if (send(nuevo, "Solicitud recibida por el servidor\n", 35, 0) == -1) {
            perror("Error al enviar la confirmación al cliente");
            close(nuevo);
            continue;
        }
        
        // Cerrar la conexión con el cliente
        close(nuevo);
        conexiones_manipuladas++;
    }
    
    // Cerrar el socket principal
    close(mi_socket);

    return NULL;
}

void mostrar_datos(GtkWidget *widget, gpointer data) {
    g_print("Mostrar datos\n");
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *button_start_server;
    GtkWidget *button_show_data;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Interfaz básica");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    gtk_container_add(GTK_CONTAINER(window), grid);

    button_start_server = gtk_button_new_with_label("Iniciar servidor");
    g_signal_connect(button_start_server, "clicked", G_CALLBACK(iniciar_servidor), NULL);
    gtk_grid_attach(GTK_GRID(grid), button_start_server, 0, 0, 1, 1);

    button_show_data = gtk_button_new_with_label("Mostrar datos");
    g_signal_connect(button_show_data, "clicked", G_CALLBACK(mostrar_datos), NULL);
    gtk_grid_attach(GTK_GRID(grid), button_show_data, 1, 0, 1, 1);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
