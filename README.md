# Planificador Dieciochero / Tarea 1 Sistemas Operativos / Vicente Álvarez / SEC 01

## Descripción
Programa desarrollado en C++ que lee un archivo de actividades (`plan.txt`) organizadas como un Grafo Acíclico Dirigido (DAG). El sistema va ejecutando las tareas respetando cuáles tienen que terminar primero y controlando que no se sobrepase el límite de ejecuciones simultáneas K.

## Explicación de Funciones Implementadas

- `inicializar_mapa()` y `buscar_indice_por_id(int id)`: Administran la tabla de mapeo directo para convertir IDs de tareas en su posición en el arreglo en tiempo constante O(1).
- `dependencias_listas(int idx)`: Revisa si todas las dependencias previas de una tarea están en estado `COMPLETADO`. Devuelve `true` si la tarea ya puede ser ejecutada.
- `tiene_dependencia_fallida(int idx)`: Verifica si alguna de las dependencias asociadas a una tarea terminó en estado `CANCELADO` para transmitir la cancelación en cascada dentro del DAG.
- `ejecutar_tarea(int idx)`: Crea un proceso hijo mediante `fork()`, configura la tubería (`pipe`), simula el tiempo de ejecución con `usleep()` y notifica su finalización escribiendo por el extremo de escritura del pipe.
- `esperar_proceso()`: Bloquea al proceso padre mediante `waitpid()` a la espera de la finalización de cualquier hijo, lee el mensaje transmitido por el pipe y actualiza el estado de la tarea.
- `manejador_sigint(int sig)`: Manejador de la señal `SIGINT` (Ctrl+C). Recorre las tareas en estado `EJECUCION` y les envía la señal `SIGTERM` para finalizar los procesos hijos de forma limpia.
- `leer_plan(const char *nombre_archivo)`: Abre y procesa línea por línea el archivo `plan.txt`. Usa `strtok` para parsear el ID, nombre, tiempo estimado y lista de dependencias de cada tarea.

## Justificación de Decisiones de Diseño

- **Procesos independientes**: Cada tarea del plan corre en su propio proceso hijo usando `fork()`, garantizando el aislamiento total de memoria exigido en la pauta.
- **Comunicación con Pipes**: Cada proceso hijo avisa al proceso padre cuando termina a través de una tubería anónima (`pipe`), permitiendo el paso de mensajes interproceso IPC sin compartir memoria.
- **Búsqueda eficiente en O(1)**: Se optimizó el acceso a tareas usando un mapa/arreglo de acceso directo (`mapa_id_a_idx`), permitiendo ejecutar pruebas masivas de hasta 10.000 tareas sin degrada el rendimiento.
- **Sincronización sin saturar CPU**: El proceso padre usa `waitpid(-1, &status, 0)` para esperar de forma bloqueante a que cualquier proceso hijo termine, evitando el gasto innecesario de CPU (*busy-waiting*).
- **Manejo de errores en cadena**: Si una tarea falla o se cancela, la función `tiene_dependencia_fallida()` omite automáticamente las tareas que dependían de ella, permitiendo que las demás ramas independientes del plan continúen.
- **Interrupción con Ctrl+C**: Se capturó la señal `SIGINT` usando `sigaction`. Si se presiona Ctrl+C, el programa identifica los procesos activos y los cierra de forma segura enviando `SIGTERM`.

## Compilación y Ejecución

Compilación con Makefile:
```bash
make
```
Compilación manual:
```bash
g++ -Wall -Wextra -std=c++17 main.cpp -o planificador
```

Ejecución del programa:
```bash
./planificador plan.txt 2
```
