Instrucciones de Uso
Ejecutar el Servidor

Para ejecutar el servidor, sigue estos pasos:

    Compila el programa del servidor utilizando el siguiente comando:

    bash

gcc -o servidor servidor.c

Ejecuta el servidor:

bash

./servidor /servidor

Una vez iniciado, el servidor mostrará un mensaje indicando en qué puerto está escuchando. Por ejemplo:

yaml

    El servidor está escuchando en el puerto: 39479

Ejecutar la Interfaz del Cliente

Para ejecutar la interfaz del lado del cliente, sigue estos pasos:

    Asegúrate de tener instalada la biblioteca de la interfaz. Puedes instalarla ejecutando:

    bash

sudo apt-get install libgtk-3-dev

Compila el programa de la interfaz utilizando el siguiente comando:

bash

gcc -o interfaz interfaz.c

Ejecuta la interfaz del cliente, especificando el host y el puerto del servidor. Por ejemplo:

bash

    ./interfaz localhost 39479

Recuerda reemplazar "localhost" con la dirección IP o el nombre de host del servidor, y "39479" con el puerto que te haya indicado el servidor.
