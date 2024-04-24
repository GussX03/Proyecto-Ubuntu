Ejecutar el servidor: 
gcc -o servidor servidor.c
/servidor
Te dira en que puerto estara escuchando:
El servidor está escuchando en el puerto: 39479
entonces al ejecutar la interfaz del lado del cliente sera:
sudo apt-get install libgtk-3-dev -- para instalar la biblioteca de la interfaz
gcc -o interfaz interfaz.c -- compilar el programa
./interfaz localhost 39479 
