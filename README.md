# Planificador Dieciochero / Tarea 1 Sistemas Operativos / Vicente Álvarez / SEC 01

## Descripción
Programa desarrollado en C++ que lee un archivo de actividades (`plan.txt`) organizadas como un Grafo Acíclico Dirigido (DAG). El sistema va ejecutando las tareas respetando cuáles tienen que terminar primero y controlando que no se sobrepase el límite de ejecuciones simultáneas K.

## Explicación de Funciones Implementadas

- `buscar_posicion(int id)`: Busca el índice dentro del arreglo de tareas a partir de su ID para facilitar el acceso a los datos de cada actividad.
- `dependencias_listas(int pos)`: Revisa si todas las dependencias previas de una tarea están en estado `COMPLETADO`. Devuelve `true` si la tarea ya puede ser ejecutada.
- `tiene_dependencia_fallida(int pos)`: Verifica si alguna de las dependencias asociadas a una tarea terminó en estado `ERROR` o `CANCELADO` para transmitir la cancelación en el DAG.
- `manejar_sigint(int sig)`: Manejador de la señal `SIGINT` (Ctrl+C). Recorre la lista de procesos activos en estado `PROCESANDO` y les envía la señal `SIGTERM` para cerrarlos ordenadamente.
- `leer_plan(const char *nombre_archivo)`: Abre y procesa línea por línea el archivo `plan.txt`. Usa `strtok` para parsear el ID, nombre, tiempo estimado y lista de dependencias de cada tarea.
- `ejecutar_hijo(int pos)`: Código que ejecuta cada proceso hijo creado con `fork()`. Cierra el extremo de lectura de la tubería, simula la duración con `usleep()` y notifica su término escribiendo por el `pipe`.
- `ejecutar_simulacion()`: Bucle principal que administra el ciclo de vida del DAG. Lanza procesos con `fork()`, controla el límite de concurrencia K, lee confirmaciones por `pipe` y se bloquea con `waitpid()` para evitar el consumo de CPU.

## Justificación de Decisiones de Diseño

- **Procesos independientes**: Cada tarea del plan corre en su propio proceso hijo usando `fork()`, garantizando el aislamiento total de memoria exigido en la tarea.
- **Comunicación con Pipes**: Cada proceso hijo avisa al proceso padre cuando termina a través de una tubería anónima (`pipe`), permitiendo el paso de mensajes interproceso sin depender de variables compartidas.
- **Sincronización sin saturar la CPU**: El proceso padre usa `waitpid(-1, &status, 0)` para esperar de forma bloqueante a que cualquier proceso hijo termine, evitando el gasto innecesario de CPU (*busy-waiting*).
- **Manejo de errores en cadena**: Si una tarea falla o se cancela, la función `tiene_dependencia_fallida()` omite automáticamente las tareas que dependían de ella, permitiendo que las demás ramas independientes del plan continúen.
- **Interrupción con Ctrl+C**: Se capturó la señal `SIGINT` usando `sigaction`. Si se presiona Ctrl+C, el programa identifica los procesos activos y los cierra de forma segura enviando `SIGTERM`.

## Compilación y Ejecución

Compilación con Makefile / Sintaxis manual: g++ -Wall -Wextra -std=c++17 main.cpp -o planificador -lpthread
```bash
make
```

Ejecucion del programa / Sintaxis: ./planificador plan.txt K
```bash
./planificador plan.txt 2
```
