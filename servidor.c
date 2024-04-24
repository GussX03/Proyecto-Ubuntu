#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define COLA_MENSAJES 1234

struct solicitud {
    long tipo;
    char nombre[50];
    char destino[50];
    char horario[50];
    char boleto_tipo[50];
    int costo_boleto;
};

int main(int argc, char *argv[]) {
    int mi_socket, nuevo, tam, dinero_total=0;
    struct sockaddr_in mi_estructura;
    struct solicitud solicitud_recibida;
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
    if (listen(mi_socket, 5) == -1) {
        perror("Error al escuchar conexiones");
        exit(EXIT_FAILURE);
    }
    
    while (1) { // Bucle infinito para tratar conexiones
        nuevo = accept(mi_socket, (struct sockaddr *)&mi_estructura, &tam);
        if (nuevo == -1) {
            perror("Error al aceptar la conexión entrante");
            exit(EXIT_FAILURE);
        }
        
        // Recibir la solicitud del cliente
        if (recv(nuevo, &solicitud_recibida, sizeof(struct solicitud), 0) == -1) {
            perror("Error al recibir la solicitud del cliente");
            close(nuevo);
            continue;
        }
        // COSTO DE BOLETO COMPARANDO QUE TIPO DE BOLETO ES

        if (strcmp(solicitud_recibida.boleto_tipo, "Ordinario $50") == 0) {
            solicitud_recibida.costo_boleto = 50;
          }else if(strcmp(solicitud_recibida.boleto_tipo, "1ra Directo $100") == 0) {
            solicitud_recibida.costo_boleto = 100;
          }else if(strcmp(solicitud_recibida.boleto_tipo, "Gl Premier $150") == 0) {
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
    }

    // Cerrar el socket principal (esto nunca se alcanza debido al bucle infinito)
    close(mi_socket);

    return 0;
}

