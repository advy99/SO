# Tema 3: Gestión de Memoria

Todas las referencias a diapositivas se refieren a los apuntes usados por Patricia, profesora de SO de la UGR, pero que usa unas diapositiva con derechos de autor, luego me es imposible poner capturas o imágenes de estas.

## Jerarquía de memoria

Se basa en dos principios sobre memoria:

1. Menor cantidad -  acceso mas rapido (memoria cache)
2. Mayor cantidad - menor coste por byte (memoria principal)

Los elementos más frecuentes interesa almacenarlos en la memoria más rápida.


## Conceptos sobre caches

### Definición
Copia que puede ser accedida mas rapidamente que el original

### Idea

Hacer que los accesos frecuentes a parte de la memoria sean eficientes

Existen varias situaciones con la cache:

1. Acierto de cache: item necesario en la cache (no hay que acceder a memoria)
2. Fallo de cache: item no esta en cache, hay que realizar un acceso completo a memoria


Tiempo de Acceso Efectivo = probabilidad_acierto \* coste_acierto + probabilidad_fallo \* coste_fallo


Esto se puede realizar gracias a la localidad de los programas, es decir, es muy probable que si un programa necesita un elemento en una dirección de memoria N, más adelante necesite datos con direccion en un intervalo cercano a N.

## Objetivos generales de la gestion de memoria

1. Organización: como esta dividida

2. Gestión: esquemas, estrategias de asignacion, sustitucion y busqueda

3. Protección


## Asignación contigua y no contigua

### Asignación contigua

Los datos se asigna de forma secuencial en mamoria

### Asignación no contigua

Un programa puede estar repartido en distintas zonas de la memoria

#### Tipos

1. Paginación

2. Segmentación

3. Segmentación paginada

(Son explicadas más adelante)


## Intercambio (swapping)

Se basa en intercambiar el espacio de direcciones de un proceso que no se este ejecutando de memoria principal a un almacenamiento secundario. Ha de ser rapido para que no afecte a la gestión de memoria.

Se usara un intercambiador, con las siguientes reponsabilidades:

1. Seleccionar procesos para retirarlos de MP (memoria principal) (planificador a medio plazo)
2. Seleccionar procesos para incorporarlos a MP
3. Gestionar y asignar el espacio de intercambio




## Memoria virtual

### Organización

Se utiliza cuando el tamaño de programa, datos, pila y demas excede la cantidad de memoria física.

Se usan almacenamiento a dos niveles:

1. Memoria principal : partes necesarias en un momento dado

2. Memoria secundaria : espacio de direcciones completo

Es necesario saber que se encuentra en memoria, para saber que es necesario rescatar de la memoria secundaria.

Permite aumentar el grado de multiprogramación


## Unidad de Gestion de Memoria (MMU en ingles)

Es un dispositivo que traduce direcciones virtuales a direcciones físicas, es gestionado por el SO.

En el esquema más simple de MMU, el valor del registro base se añade a cada dirección generada por el proceso de usuario al mismo tiempo que es enviado a memoria.

El programa de usuario solo trata direcciones lógicas, nunca reales.

Pasos:

1. CPU genera direccion virtual
2. MMU comprueba la dirección
3. Si no es legal, genera excepcion
4. Si es legal, comprueba que esta cargada en memoria principal
5. En caso de no estar cargada en MP, genera excepcion para cargarla



## Paginacion

El espacio de direcciones fisicas de un proceso no es contiguo

La memoria física se divide en bloques de tamaño fijo (marcos de paágina)

El espacio lógico se divide en bloques del mismo tamaño, denomidadas paginas (puede que no todas las paginas esten cargadas en memoria)

Existe una tabla de paginas por cada proceso.

Existe una tabla de ubucación en disco, una entrada por cada página, cada entrada almacena donde esta cada página en disco.

Existe una tabla de marcos de pagina, contiene informacion sobre cada marco de página, usada por el SO.

### Contenido de la tabla de paginas

1. Numero de marco
2. Bit de presencia: si esta a 1 nos dice si la pagina es valida y esta cargada en memoria principal

  ejemplo de pagina invalida : memoria reservada pero sin usar entre el heap y la pila

3. Bit de modificacion: nos dice si se ha modificado la página en memoria principal, para saber si tenemos que salvar en el disco secundario cuando se retire la página

 ejemplo: páginas de codigo, el codigo nunca se modifica, nunca lo vamos a tener que sobreescribir

4. Modo de acceso (bits de protección)


### Tratamiento falta de pagina

Pasos:

1. Bloquear el proceso

2. Encontrar ubicación en disco de la página solicitada (usando tabla de ubicación en disco)

3. Encontrar marco libre, si no hubiera, se podria optar por desplazar una página de MP, es decir, intercambiarlas, si esto no ocurre o no es posible, se bloquea hasta que exista un marco libre en MP.

4. Inicia E/S a disco para cargarla (para traerla a memoria principal)

5. Actualiza tabla de páginas

6. Desbloquea el proceso

7. Reinicia la operación que genero la falta de página


### Implementacion de la tabla de pagnas (diapositiva 22)

Siempre se mantiene en memoria principal

Existe un registro base de la tabla de paginas (RBTP) que apunta a la tabla de paginas

Cada acceso a instruccion o dato requiere dos accesos a memoria.

Ejemplo:

		direccion virtual : 32 bits
		tam pagina : 4Kbytes (2^12 bytes)
		tam campo desplazamiento = 12 bits
		tam numero pagina virtual = 20 bits

		numero de paginas virtuales = 2^20 = 1 048 576

	Problema:

		Solucion : Paginación multinivel


### Paginación multinivel

Se basa en paginar las tablas de páginas, es decir, tendremos una tabla que apuntara a otras tablas de páginas.

Ejemplo:
 Paginación a dos niveles, se mantiene la del nivel 1 siempre cargada, y el segundo nivel se cargan las partes necesarias


#### Ejemplo: Paginacion a dos niveles

Dividimos la tabla de paginas en partes del tamaño de una página

La direccion lógica se divide en :

1. Número de página (primer nivel)
		Número de página primer nivel -> p1 (k)
		Desplazamiento de página (nos marcara la de segundo nivel) -> p2 (n-k)
2. Desplazamiento de pagina (dentro del segundo nivel):  d (m bits)

Sintaxis:

		[p1 | p2 | d]

## Segmentación

División en segmentos independientes

Sintaxis:

	<numero_segmento, desplazamiento>


### Tabla de segmentos

Usa direcciones bidimensionales.

Cada entrada contiene:

1. Bit de presencia
2. Bit de modificación
3. Bit de protección
4. Base : direccion fiisca donde reside el segmento en memoria
5. Tamaño : longitud del segmento

La dirección es legal si:

		s < STLR (segmento menor que longitud de tabla de segmentos)



## Segmantacion paginada

Se combinan paginación y segmentación

El espacio de direcciones lo dividimos en segmentos, y cada segmento se gestiona con una tabla de paginas.

No todo el segmento tiene que cargarse en memoria principal, y las páginas del segmento no tienen poque estar contiguas en memoria.


## Memoria virtual

### Gestión

Gestiona la Memoria Virtual con paginación

Criterios de clasificacion respecto a:

1. Políticas de asignación: fija o variable
2. Políticas de busqueda (recuperación)

	* Paginación por demanda (cuando el proceso lo necesite se carga la pagina)
	* Paginación anticipada (SO prevee que paginas va a necisitar y las carga) (!= prepaginacion -> la prepaginacion es la carga inicial de paginas)

3. Políticas de sustitucion (reemplazo)

	* Sustitución local
	* Sustitución global



Independientemente de la política de sustitución, han de cumplir:

1. Páginas limpias frente a sucias
	* Se pretende reducir E/S a discos, las sucias hay que guardar copia

2. Páginas compartidas
	* Se pretende reducir el número de faltas de página

3. Páginas especiales
	* Algunos marcos pueden estar bloqueados (ej: buffer de E/S mientras realiza una transferencia)



### Influencia del tamaño de página

Páginas pequeñas:

* Aumento del número de páginas
* Aumento del número de transferencias, es decir, muchas operaciones de E/S
* Reducen la fragmentación interna

Páginas grandes:

* Carga mucha información no usada en MP
* Aumenta la fragmentación interna

Hay que buscar un equilibrio en el tamaño de las páginas


## Algoritmos de sustitución

Combinaciones de algoritmos:

* Asignación fija y sustitución local (número fijo de marcos, sustitución entre páginas del mismo proceso)

* Asignación variable y sustitución local (número variable de marcos de páginas, pero sustitucion solo entre sus páginas)

* Asignación variable y sustitución global (número variable marcos de páginas, sustitucion entre todas páginas de todos procesos)

### Algoritmos:

Se ejecutan cuando se produce falta de página y no hay espacio para mas paginas

1. Óptimo

 * Sustituye la página que no se va a referencia en un futuro, o la que se referencie más tarde (es teorico, nunca se podra realizar, ya que necesitamos saber las paginas que vamos a referenciar en el futuro)

2. FIFO

	* Sustituye la página más antigua

3. LRU

	* Sustituye la página que fue objeto de la referencia más antigua, debemos almacenar cuando fue referenciada, y cuando quiera sustituirla, tiene que buscar por todas las páginas (es muy costoso)

4. Algoritmo de reloj (aproximacion LRU)

	* La idea es la misma, se escoge una de las que se referenciaron hace más tiempo (no tiene por que ser la última)

	* Cada página tiene asociada un bit de referencia R (lo pone a 1 el hardware (el MMU con la traducción de memoria lógica a física) )

		- Si una pagina tiene bit de referencia 1, ha sido usada al menos una vez

		- Tenemos un puntero a la pagina que se ha usado hace mas tiempo (la mas antigua, la que llevamos mas tiempo sin acceder)

	Pasos:

		1. Consulta la página del marco actual

		2. Si R = 0 : hace tiempo que no se ha usado (si no, tendria R = 1), la escoge para la sustitución e incrementa la posición del puntero.

			Si R = 1 : ponemos R = 0, e incrementamos el puntero a la siguiente posición



Orden de algoritmos (de mejor a peor)

1. Optimo (imposible de realizar)
2. LRU
3. Reloj
4. FIFO


## Comportamiento de los programas

Caracteristicas:

1. Localidad: conjunto de páginas frecuentemente consultadas

	* Tipos

		* Temporal: una posición referenciada recientemente tiene probabilidad alta de ser referenciada en un futuro próximo.

		* Espacial: si una posición de memoria ha sido referenciada, es altamente probable que las zonas adyacentes sean referenciadas en un futuro próximo.

2. Conjunto de trabajo (WS):

	* Conjunto de páginas que son referenciadas frecuentemente en un determinado intervalo de tiempo

	* El objetivo es mantener el conjunto de trabajo de un proceso en memoria

	* Propiedades:

		1. Un proceso segun se ejecuta, va variando sus conjuntos de trabajo

		2. Los conjuntos de trabajo son variables, según el estado del proceso

		3. Hay periodos de transición entre los conjuntos de trabajo, en los que tendre páginas de un WS anterior, hasta que cargo las del nuevo WS.

		4. Difieren unos de otros sustancialmente

	* Teoría del conjunto de trabajo:

		* Un proceso solo puede ejecutarse si tiene su conjunto de trabajo esta en memoria principal.

		* Una página no puede retirarse de memoria principal si esta en el conjunto de trabajo de un proceso.


## Hiperpaginacion (ver diapositivas para ejemplo)

Problema de la paginación:

* Si tenemos mucho grado de multiprogramación (muchos procesos en memoria), llegamos a un valor umbral en el que el numero de falta de páginas se dispara, y disminuye la productividad

* La CPU usa más tiempo en resolver faltas de página, que ejecutando procesos.


Formas de evitarla:

1. Asegurar que cada proceso tiene asignado un espacio en relación a su comportamiento (asignación de memoria dinámica) (algoritmos de asignación variables)

	* A cada proceso, se le asigna el espacio estrictamente necesario para que funcione correctamente

2. Actuar directamente sobre el grado de multiprogramación (cambiar número de procesos máximos cargados en memoria) (algoritmos de regulación de carga)

	* Ej: algoritmo de 50%, si la carga del sistema pasa de ese %, no se cargan mas procesos, y si se pasa del tope, se descargan procesos en memoria secundaria.




## Algoritmo basado en el modelo del WS (working set / espacio de trabajo)

En cada referencia se determina el conjunto de trabajo, es decir, las páginas referenciadas en el intervalo ]t -x , t] y solo esas páginas son mantenidas en MP

	* t = instante de tiempo actual
	* x = tiempo establecido por el algoritmo

El problema de este algoritmo es que necesita mucho coste de CPU.

## Algoritmo FFP (Frecuencia de Falta de Pagina)

		elimina paginas que no se han referenciado entre el instante de falta de pagina actual, y el instante de la ultima falta de pagina, si el instante es pequeño, quiere decir que tiene poca memoria asignada, y el nuevo WS es mas grande, asi que simplemente añade un marco de pagina
		si no hubiera mas marcos de pagina, se bloquea el proceso hasta que quede algun marco de pagina libre

		para formulas mirar diapositivas

		se comporta mal en las etapas de transicion de WS, ya que añadiria nuevas paginas, sin borrar las antiguas (mirar formula)




Gestion de memoria en Linux

	gestion de memoria a bajo nivel
		pagina fisica : unidad basica de gestion de memoria
			struct_page

			tiene la informacion de lo que contiene un marco de pagina en memoria principal

			tiene:
				flags (pagina sucio, bloqueada(no se puede sustituir), etc)
				count (numero de paginas que tienen asignado ese marco de pagina) (varios procesos pueden estar usando la misma pagina, este contador lleva la cuenta ) (ej bibliotecas compartidas)
				direccion virtuales y fisicas


		una pagina puede ser usada por:
			cache de paginas
			datos privados
			proyeccion de tabla de paginas de un proceso
			espacio de direcciones de un proceso
			datos del kernel alojados dinamicamente
			codigo del kernel

			(todo lo que pueda cargar en memoria)

		distintas interfaces, funciones para gestionar paginas, solicitarlas, liberarlas, etc...

		MIRAR DIAPOSITIVAS


		zonas de memoria
			el tipo gfp_t permite especificar el tipo de memoria mediante tres categorias de flags
				modificadores de accion
				modificadores de zona
				tipos

			ejemplos de tipos:
				GFP_KERNEL solicitud de memoria para el kernel
				GFP_USER solicitar memoria para el espacio de usuario de un proceso


Cache de bloques. Organizacion

	la asignacion y liberacion de estructuras de datos es una de las operaciones mas comunes en un kernel dde SO

	para agilizar la solicitud/liberacion de memoria linux usa el nivel de bloques

	(NO CONFUNDIR CACHE CON LA CACHE DEL PROCESADOR)

	el nivel de bloques actua como un nivel de cache de estructuras generico
		existe una cache para cada tipo de estructura distinta
			pe : task_struc cache, o inode cache
				en lugar de crear una task_struct para cada proceso, reutilizo la task_struct en la cache, de un proceso que ha finalizado

		cada cache contiene multiples bloques constituidos por una o mas paginas fisicas contiguas

		cada bloque alloja estructuras del tipo correspondiente a la cache

	Funcionamiento

		tres estados
			lleno
			parcial
			vacio

		cuando el kernel solicita una nueva estructura:
			se satisface desde un bloque parcial

			si no, se satisface a partir del vacio

			si no existe ninguno vacio, se crea uno nuevo, y la solicitud se satisface con este nuevo bloque


Espacio de direcciones de proceso
	espacio de direcciones de proceso es el espacio de direcciones de los procesos ejecutandose en modo usuario. Linux usa memoria virtual (VM)

	A cada proceso se asigna espacio de memoria plano de 32 o 64 bits unico. No obstante se puede compartir el espacio de memoria (CLONE_VM para hebras)

	el proceso solo tiene permiso para acceder a determinados intervalos de direcciones de memoria, denominados areas de memoria.

	holi (Mari saluda)


	¿Que puede contener un area de memoria?

		mapa de memoria de la seccion de codigo (text section)

		mapa de memoria de

		(DIAPOSITIVAS, ME FALTA TIEMPO)

	el espacio de direcciones de memoria de un proceso en linux se almacena con el descriptor de memoria (DIAPOSITIVAS, PAG 64)

		cuando hacemos un hilo, apunta al mismo mm_struct, luego si liberamos al hilo, tenemos que tener en cuenta otros hilos con el mismo mm_struct, para no liberar memoria que la esten usando otros hilos
		con hijos esto no ocurre, ya que con hijos no comparten el mismo espacio de direcciones


	¿como se asigna descriptor de memoria?
		con un fork , se crea una copia

		con clone, se comparte el descriptor

	¿como se libera el descriptro de memoria?

		nucleo decrementa el contador mm_users incluido en el mm_struct, si llega a 0, se decrementa mm_count, si mm_count vale 0, se libera la mm_struct en la cache


	un area de memoria (struct vm_area_struct) describe un intervalo contiguo del espacio de direcciones

		contiene la mm_struct
		vm_start (inicio de la VM)
		vm_end (fin de la VM)
		vm_flags
		vm_operations


	utilizando el archivo /proc/<pid>/maps podemos ver las VMAs de un proceso

	el formato es:
		start-end permisiion offset major:minor inode file

		start-end direccion de comienzo y fin de la VMA
		permission permmisos de acceso al conjunto de paginas de la VMA
		offset
		major:minor sistema de archivo logico donde esta el ejecutable desde donde creamos el proceso
		inode numero de inodos del archivo
		file nombre del archivo



	¿como se crea un intervalo de direcciones?
		do_mmap()
			expandir una VMA ya existente

			crear una nueva VMA que represente el nuevo intervalo de direcciones


	¿como se eliminar un intevalo de direcciones?

		do_munmap()
			eliminar intervalo de direcciones


	linux implimenta una paginacion multinivel de 3 niveles
	las direcciones virtuales deben convertirse a direcciones fisicas mediante tablas de paginas, en linux tenemos 3 niveles de tablas de paginas

		tabla de paginas mas alto nive, es el directorio global de paginas (PGD, page global directory) pgd_t

		tabla de paginas medio nivel,

		tabla de paginas de bajo nivel

		(mirar diapositivas)


cache de paginas . concepto

	cache de paginas constituida por paginas fisicas de RAM, y los contenidos se conrresponden a bloques fisicos de disco

	tamaño de la cache de paginas es dinamico

	el dispositivo sobre el que se realiza la tecnica de cache se denomina almacen de respaldo

	L/E de datos de/a disco

	fuentes de datos para la cache: archivos regulares, de dispositivos, y archivos proyectados en memoria

	pasos:
		de disco, cogemos los datos y los mandamos  a cache en MP, la L/E se realizara en MP, minimizando los tiempos de L/E

		el objetivo es minimizar el tiempo de E/S o E/L en los archivos

		si solo leo una vez, no optimizamos nada, incluso malgastamos en duplicados



seleccion de victimas de pagina

	LRU. requiere mantener la informacion de cuando se accede a cada pagina y seleccionar las paginas ocn el tiempo mas antiguo. EL problema es el acceso a archivos una unica vez

	linux soluciona el problema usando dos listas pseudo-LRU active list e inactive list
		active list :
			no pueden ser seleccionadas como victimas y se añaden paginas accedidas solamente si residen en la inactive list



		inactive list:
			pueden ser seleccionadas como victimas

		la cola de la inactive list sera los que llevan tiempo sin referenciar, al leer una pagina de memoria, metemos la pagina en la inactive list  y la mas antigua (la primera de la lista) la liberamos

		si el bloque que hay en la entrada de la inactive list se llama dos veces, pasa a la  active list, si la active list, se llena, el ultimo de la active list pasa a la inactive list



cache de paginas operaciones

	una pagina puede contener varios bloques de disco posiblemente no contiguos

	cache de paginas en linux usa una estructura para gestionar entradas de cache y operaciones de E/S de paginas addres_space

	L/E de paginas en cache

	hebras de estructura retardada.
