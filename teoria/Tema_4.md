Tema 4: Gestionde archivos

(en principio muchas cosas las hemos visto)


Semantica de consistencia

   que ocurre si dos programas abren un archivo y lo usan a la vez

   forma de solucionarlo:

      semantica unix
         se tiene una unica copia del archivo en memoria principal, y todos los programas que usen ese archivo, trabajan sobre la misma copia en memoria principal

         la escritura es directamente observable

         existe un modo en el que los usuarios comparten el puntero actual de posicion de escritura/lectura de un archivo (hebras e hijos)

      semanticas de sesion

         sistemas de archivos distribuidos (no comparten memoria)

         si se abre un archivo, se trabaja desde la maquina en la que se ha abierto

         la escritura de un archivo no es directamente observable

         cuando un archivo se cierra sus cambios solo se observan en sesiones posteriores


      archivos inmutables
         cuando un archivo se declara compartido, no se puede modificar


Funciones basicas del SIstema de Archivos (SA)

   Tener conocimiento de todos los archivos

   controlar comparticion de archivos y forzar su proteccion

   gestionar espacio del sistema de archivos
      asignar/liberar del espacio en disco

   traducir las direcciones logicas del archivo en direcciones fisicas del disco
      los usuario especifican las partes que quieren leer/escribir en terminos de direcciones logicas relativas al archivo


Diseño software del SA

   dos problemas
      definir como el usuario ve el SA
         definir archivo y sus atributos

         definir operaciones permitidas sobre un archivo

         definir estructura de directorios

      definir algoritmos y estructuras de datos que deben crearse para establecer la correspondencia entre el sistema de archivos logico y los dispositivos fisicos dende se almacenan



Estructura del SA

   organizacion en niveles(capas)


      dispositivo
          ^
          |
      control E/S
          ^
          |
      sistemas de archivos basico
          ^
          |
      modulo de organizacion de archivos
          ^
          |
      sistema logico de archivos
          ^
          |
      programas



      por eficiencia, el SO mantiene una tabla indexada (descriptor de archivos) de archivos abiertos

      bloque de control de archivo : estructura con la informacion de un archivo


Metodos de asignacion de espacio

   contiguo

      cada archivo ocupa un conjunto de bloques contiguos en disco

      ventajas
         sencillo: solo necesita la localizacion de comienzo (n bloque) y la longitud

         buenos tanto al acceso secuencial como el directo

      desventajas
         no se conoce inicialmente el tamaño

         derroche de espacio (problema de asignacion dinamica -> fragmentacion externa)

         los archivos pueden crecer, a no ser que se realice compactacion -> ineficiente

      asociacion logica a fisica
         supongamos bloques de 512 bytes
            direccion logica (DL)/512 -> C(cociente), R(resto)

            bloque a acceder -> C-esimo

            desplazamiento en el bloque -> R+1


   no contiguo

      cada archivo es una lista enlazada de bloques de disco, los cuales pueden estar dispersos

      ventajas
         evita fragmentacion externa

         el archivo puede crecer dinamicamente cuando hay bloques de disco libres -> no es necesario compactar

         basta almacenar el puntero al primer bloque del archivo


      desventajas
         acceso directo no efectivo, necesito ir avanzado por los bloques del archivo, hasta llegar al que quiero

         espacio requerido para los punteros de enlace(soolucion, agrupacion de bloques)

         seguridad por la perdida de punteros (solucion, lista doblemente enlazada)


      asociacion logica a fisica (direccion = 1byte)

            direccion logica (DL)/511 -> C(cociente), R(resto)

            bloque a acceder -> C-esimo

            desplazamiento en el bloque -> R+1

   no contiguo (enlazado)

      tabla de asignacion de archivos (tabla FAT), los punteros los sacamos a una tabla aparte, indexada por num bloques fisicos del SA

      MIRAR DIAPOSITIVAS

      mejora el acceso directo, no necesita cargar las paginas anteriores, se mueve por la tabla que esta en memoria principal , y se trae el bloque de datos que necesite, una vez encontrado


      principales problemas: si se rompe la tabla FAT, perdemos toda la informacion del SA


   no contiguo (indexado)

      todos los punteros a bloques estan juntos en una localizacion concreta: bloque indice

      el directorio tiene la localizacion a este bloque indice y cada archivo tiene asociado su propio bloque indice

      para leer el i-esimo bloque buscamos el puntero en la i-esima entrada del bloque indice

      ventajas:
         buen acceso directo
         no produce fragmentacion externa

      desventajas
         posible desperdicio de espacio en los bloques indices

         tamaño  del bloque indice. Soluciones
            bloques indice enlazados

            bloques indice multinivel
               problema: acceso a disco necesario para recuperar la direccion del bloque para cada nivel de indexacion

               solucion: mantener algunos bloques indices en memoria principal

            esquema combinado(unix)

               MIRAR DIAPOSITIVAS


Gestion del espacio libre

   el sistema mantiene una lista de los bloques que estan libres: lista de espacio libre

   la FAT no necesita ningun metodo
      la propia FAT almacena que bloques estan libres


   a pesar de su nombre, la lista de espacio libre tiene diferentes implimentaciones

      -mapa o vector de bits
         un bit por cada bloque o cluster de archivos, 0 bloque libre, 1 bloque ocupado

         facil encontrar bloque/s libre/s

         ineficiente si no se mantiene en memoria principal


      -lista enlazada
         enlaza los bloques libres del disco, teniendo un puntero al primer bloque, en cada bloque libre se almacena la direccion del siguiente bloque libre

         no derrocha espacio, el espacio usado es espacio no utilizado

         relativamente ineficiente -> no es normal atravear bloques vacios

      -lista enlazada con agrupacion
         misma idea que lista enlazada, solo que en un bloque libre tengo informacion de los n siguienets bloques libres

         obtener informacion de muchos bloques libres es mas rapido

      -cuenta
         cada entrada de la lista: una direccion de bloque libre y un contador con el n de bloques libres que sigue



Implementacio de Directorios

   contenido de una entrada de directorio. CASOS:

      toda en la entrada de informacion del archivo
         -nombre_archivo + atributos + direccion de bloques de datos (como MS-D0S)

      linux, usando inodos
         -nombre_archivo + puntero a estructura de datos (linux)

         se pone el inodo + nombre_archivo, y el inodo guarda la informacion del archivo

         MIRAR DIAPOSITIVAS

   cuando se abre un archivo:
      el SO busca en su directorio la entrada correspondiente

      extrae sus atributos y la localizacion de sus bloques de datos y los coloca en una tabla de memoria principal

      vualquier referencia posterior usa la informacion de dicha tabla



   implementacion de archivos compartidos (enlaces)
      MIRAR DIAPOSITIVAS


Distribucion del Sistema de Archivos
   MIRAR DIAPOSITIVAS


Recuperacion
   como los archivos y directorios se mantienen tanto en MP como en disco, el sistema debe asegurar que un fallo no genere perdida o inconsistencia de datos

   distintas formas
      comprobador de consistencia
         compara los datos en la estructura de directorios con los bloques de datos en disco y trata cualquier inconsistencia

         mas facil en listas enlazadas que con bloques indices

         listas enlazadas >>>>> bloques indices

      usar programas del sistema para realizar copias de seguridad de datos de disco a otros dispositivos y de recuperacion de los archivos perdidos
