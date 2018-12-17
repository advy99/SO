LINUX

#no distincion hebra/proceso - en principio, se usa lo mismo para representarla


#crear procesos - fork o clone (clone para hebras)

hebra : distintos flujos de ejecucion que comparten recursos (memoria, archivos, etc)

proceso: tarea a la que se asignan recursos

cada hebra tiene su task_struct con la informacion necesaria

la llamada clone  tiene un sintaxis (pag 66 diapositivas y 67)

cuando creamos un proceso con clone y queremos una hebra, queremos un proceso con una task_struct, pero con varios flujos de ejecucion

con clone podemos definir que datos van a compartir

los flasgs marcados en la pag 67 son los importantes para crear una hebra, son hebras kernel, ya que no usamos bibliotecas, sino llamadas al sistema

fork() == clone(CHLDN,0) // un fork es un duplicado del padre, pero independiente

para crear la hebra, cremos un hijo donde activamos CLONE_FILES (padre e hijo comparten archivos abiertos), CLONE_FS (comparten informacion del SA), CLONE_SIGHAND (comparten mismo manejadores de señales), CLONE_THREAD( padre e hijo en el mismo grupo de hebras, esa hebra que creo tiene el mismo PID, pero cada hebra distinto TID ), CLONE_VM ( padre e hijo comparten mismo espacio de direcciones )


cada hebra tiene su propio task_struct, pero comparte mm_struct, que contienen la informacion sobre donde esta la memoria, por ejemplo

en el mm_struct esta la informacion del proceso, y es la que comparte el proceso con las hebras

en caso de que sea un proceso hijo, no una hebra (es decir, la creamos con fork), aunque sea una copia del padre NO comparten el mm_struct, por lo que si el hijo modifica algo, no es visible para el padre, ya que son espacios de direcciones distintos

en resumen, se modularizan los struct, para poder compartir informacion y permitir hebras

por ejemplo, las hebras compartirian la estructura de archivos abiertos, pero el hijo la heredaria, haciendo que los cambios de la hebra SI sean visibles para el padre, pero los cambios del hijo NO sean compartidos por el padre

el hijo, a partir de la creacion, tienen espacios de memoria distintos, aunque sean copias identicas en la creacion




el kernel tiene sus propias hebras, que ejecutan codigo del kernel, debido a necesidades de paralelismo, concurrencia, o procesos en segundo plano, estas hebras no tienen mm_struct, su puntero es a NULL, son planificadas y pueden ser expropiadas, se crean con clone, terminan cuando ejecutan do_exit, o porque se decida finalizar a la hebra desde otra parte del kernel


do_fork()	funcion de bajo nivel, que comptueba que los parametros sean correctos, y si lo son, llama a copy_process, cuando se le devuelve el control, llama al planificador a corto plazo

copy_process() se encarga de realizar el trabajo de realizar el proceso hijo como una copia del padre, una vez que crea todo, se devuelve a do_fork y do_fork llama al planificador a corto plazo

se llama al planificador para que se asegure de que se ejecuta el proceso mas prioritario (ya que acabamos de crear un proceso, y puede tener mas prioridad que el que esta en ejecucion)

pasos de copy_process
	
	crea la estructura thread_info donde se almacena la informacion de ejecucion del proceso/hebra que creamos (por ejemplo pila kernel) el thread_info NUNCA se comparte
	crea la task_struct, con valores del padre (se hace una copia) despues se modificara segun si es hebra o proceso
	se cambian los valores de task_struct, como por ejemplo el PID si es un hijo, o el TID si es una hebra o hijo
	se establece el estado del hijo a INITERRUMPIBLE , para asegurarse de que se establece toda la informacion correcta, es como una exclusion mutua
	se establecen valores adecuados de la task_struct con los parametros, por ejemplo PF_SUPERPRIV=0, no permisos de superusuario, o PF_FORKNOEXEC=1, el proceso ha hecho fork, pero no exec, cuando se cambie este, quiere decir que ya no comparte informacion con el padre, ya que se ha ejecutado
	se asigna el nuevo PID, con alloc_pid()
	dependiendo de flags de clone, duplica (hijo) o comparte recurso (hebra), y si es una hebra, se le asigna un TID(Thread ID)
	pasa a estado RUNNING, pasara a listo, pasa a la cola de preparados
	copy_processs() devuelve puntero a la task_struct del nuevo proceso o hebra, se lo devuelve a do_fork

cuando do_fork recibe el puntero (o error, que lo trataria) a la task_struct, llama al planificador a corto plazo

con esto tenemos creado un nuevo proceso o hebra


#terminacion de proceso o hebra

el kernel libera los recursos de la hebra o hijo, y manda una señal al padre, de que un hijo o hebra a acabado

termina con
	 la llamada exit(), o de forma implicita, ejecuta todas sus instrucciones
	recibe una señal, en la que el manejo de la señal sea acabar

la liberacion o hace do_exit()

do_exit()
	activa flag PF_EXITING de la task_struct
	para cada recurso del proceso, se decrementa el contador correspondiente, cada recurso tiene un contador, para saber cuantos procesos estan usando ese recurso, si se queda a 0, libera el recurso (por ejemplo liberar un espacio de memoria), si no vale 0, como esta en uso, no lo puedo liberar
	el valor se pasa como argumento a exit, el valor se almacena en exit_code de la task_struct, hasta que el padre no coja esta informacion, no se libera la task_struct
	se manda una señal al padre de que ha finalizado
	si aun tiene hijos, los hijos pasan a tener de padre init (PID=1)
	se establece el campo exit_state de task_struct a EXIT_ZOMBIE(pasa a zombie)
	se llama a schedule(), planificador a corto plazo, ya que el proceso ha acabado, y ha dejado la CPU libre

como do_exit es lo ultimo que se ejecuta, nunca retorna nada



#
#
#2.3 Planificacion de CPU en LINUX
#
#

Planificador modular: clases de Planificacion

Linux usa una planificacion colas multiples con realimentacion y ademas existe una planificacion entre colas por prioridades apropiativo
La idea es clasificar los procesos par dar una buena respuesta

Linux distingue entre procesos (planificador modular), existen distintas clases de planificacion
	-tiempo real
	-Planificacion limpia o neutra (CFS)
	-Planificacion de la tarea idle (nula, no hay trabajo que hacer) espera a nuevos procesos
	(figura en diapositiva 74)

Linux como es de tipo apropiativo garantiza que ejecuta la tarea con mas prioridad

cada clase de planificacion tiene una prioridad (o rango de prioridades), en orden
	-tiempo real
	-tiempo compartido
	-proceso nulo

algoritmo(s) de planificacion entre clases
algoritmo(s) de planificacion entre procesos

entidad de planificacion: 
	permite agrupar tareas al planificarlas

	se representa mediante la estructura sched_entity

politica de planificacion (diap 76)
	campo policy en la task_struct del proceso

	CFS - fair_sched_class - la normal, la mas justa

		SCHED_NORMAL procesos normales de tiempo compartido
		
		SCHED_BATCH tareas menos importantes, menor compartidas

		SCHED_IDLE prioridad minima


	tiempo real rt_sched_class - cuando se crea, se asigna la politica con la que sera ejecutado, simulando dos colas (existe una forma de cambiarla)

		SCRED_RR uso politica por turnos, Round-Robin

		SCHED_FIFO politica FCFS (FIFO)


Prioridades

	Proceso con mas prioridad -> esta en ejecucion

	se almacena en static_prio

	llega desde [0,99] si es de tiempo real

	[100,139] cualquier otro

	cuanto menor sea, mas prioridad, dando mas prioridad a los de tiempo real

	en tiempo real, la prioridad se asigna al crearlo, y no se modifica

	procesos de tiempo compartido, si se puede variar (orden nice: -20 equivale a 100 y 19 equivale a 139)



Planificador periodico
	se implementa en scheduler_tick, funcion llamada con una frecuencia (entre 1000 y 100 HZ)

	Tareas principales:

		actualizar estadisticas del kernel
		activar metodo planificacion periodico de la clase de planificacion a la que corresponde el proceso actual (task_tick)
		cada clase de planificacion implementa su propia task_tick (cuenta tiempo en CPU) se posicionara el proceso en la cola de preparados dependiendo del tiempo consumido en CPU
		si hay que replanificar, el planificador de la clase concreta, activa el flag RIF_NEED_RESCHED asociado al proceso en su thread_info, avisando de que necesita replanificarse, y llamara al planificador principal

	//recordatorio thread_info almacena informacion sobre la ejecucion

El planificador principal
	se implementa en schedule, se invoca en distintos puntos del ckernel (ej: clone, exit, fork, se bloquea, etc)
	la funcion schedule se invoca de forma explicita
	el kernel comprueba el flag TIF_NEED_RESCHED y hace la llamada al planificador, o continua el proceso si no lo necesita

Actuacion de schedule
	determina la cola actual : runqueue y establece el puntero prev a la task_struct del proceso actual
	actualiza estadisticas y flasg TIF_NEED_RESCHED
	si el proceso estaba en TASK_INTERRUMPIBLE y ha recibido la señal, lo cambia a TASK_RUNNING
	se llama a pick_next_task  para escojer el siguiente proceso, se estableccce next con el puntero a la task_sturct de ese proceso
	si hay cambio de asignacion de CPU, re realiza el cambio de contexto llamando a context_switch (el despachador)

La clase de planificacion CFS

	Idea general:	
		repartir tiempo CPU de forma imparcial, garantizando que todos procesos se van a ejecutar. Dependiendo n procesos, se asignara mas o menos tiempo CPU, mas n procesos, menos tiempo de cpu, para ejecutar todos (quamtum dinamico)

	mantiene datos sobre tiempos consumidos por los procesos

	el kernel calcula el peso, a mayor prioridad estatica, menor peso

	vruntime valor asociado a procesos que muestra el tiempo virtual consumido (tiempo de uso de CPU, prioridad y peso)


	CFS ejecuta el proceso con menor vruntime

	el valor vruntime se actualiza:
		periodicamente (por el planificador periodico)
		llega nuevo proceso ejecutable
		proceso actual se bloquea


	CFS usa un rbtree (red black tree)
		arboles binario, para implementar algoritmo de busqueda eficiente, rama mas a la izquierda el proceso a elegir

	
	cuando un proceso va a bloquearse
		añade a cola asociada al bloqueo

		se establece a TASK_INTERRUMPIBLE o TASK_NONINTERRUMPIBLE

		se elimina del rbtree

		se llama a schedule para elegir nuevo proceso

	cuando un proceso vuelve de estado bloqueado
		se cambia su estado a TASK_RUNNING
		se elimina de la cola del bloque
		se añade a rbtree (se calcula su vruntime)


	resumen : round-robin, pero quamtum dinamico, dependiendo de vruntime

	
Planificacion de tiempo real
	clase de planificacion rt_sched_class
	mas prioritarios que los normales

	asigna round-robin(ahora si, quamtum fijo) o CFS

	tiempo real se escogen por prioridad, no por llegada, aunque esten en colas distintas (uno CFS y otro Round-robin)



Particularidades en SMP
	multiprocesamiento simetrico
		para realizar correctamente planificacion, en cualquier CPU se puede ejecutar codigo kernel

	para realizarlo
		equilibrar cargas CPU
		tener en cuenta afinidad tarea/CPU
		kernel capaz de migrar procesos de una CPU a otra
	
	kernel comprobara equilibro carga CPU, y ha de reequilibrar CPU



