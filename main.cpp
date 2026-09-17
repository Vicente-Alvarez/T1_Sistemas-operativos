#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
using namespace std; 
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <string>


#define MAX_TAREAS 10005 //para prueba de estres
#define MAX_DEPS 50   // cant max de dependencias por tarea

//para los estados 
enum EstadoTarea {
PENDIENTE,
EJECUCION,
COMPLETADO,
CANCELADO
};

struct Tarea {
int id;
char  nombre[100]; 
int duracion_ms; 
int dependencias[MAX_DEPS]; 

int cant_dependencias; 

EstadoTarea estado; 
pid_t pid; 
int pipe_fd[2]; //0 es lectura y 1 escritura 
};

int procesos_activos=0; 

//arreglo parar almacenar el plan completo 
Tarea plan[MAX_TAREAS];
int total_tareas= 0; 
int limite_K=1; //limite de concurrencia 





void leer_plan(const char *nombre_archivo){

FILE *f = fopen(nombre_archivo, "r");  //abrimos archivo en lectura 
if(!f){ //si no existe el archivo muestra el error 
perror("error al abrir el achivo plan");
 exit(1);  }

char linea[256]; 
while(fgets(linea, sizeof(linea),f)){ //leemos el archivo completo hasta final 
if(strlen(linea) <= 1 || linea [0] == '#') continue;  // aca ignoramos lineas vacias o inicio con hashtag 

Tarea t; 
memset(&t, 0, sizeof(Tarea)); // limpiamos memoria de struc 

t.estado= PENDIENTE;  //aqui todas las tareas inician en pendiente 

char *token= strtok(linea, ":"); // 1 extraemos id : 
if(!token)continue; 
t.id= atoi(token); //con atoi convertimos el texto a un num entero 

token= strtok(NULL, ":"); // 2 extraemos el nombre 
if(!token) continue; 
sscanf(token, " %s", t.nombre);

token= strtok(NULL, ":");// 3 extraemos duracion en milisegundos
if(!token)continue; 
t.duracion_ms= atoi(token); 

if(t.duracion_ms <=0){ //por si no tiene un rango asignado, se asigna aleatorio
t.duracion_ms = 100 + (rand() % 4901); // rango de 100 a 5000
}


token= strtok(NULL, ":");  //4 extraemos lista de dependencias si existen 
if(token){
char *dep= strtok (token, ", \t\n"); 
while(dep!=NULL){
int dep_id=atoi(dep);
if(dep_id>0){
t.dependencias[t.cant_dependencias++]= dep_id; // aqui guardamos depen
} 
dep= strtok(NULL,  ", \t\n"); // avanzamos sgnt dependencia 
}
}

plan[total_tareas++] = t; // guardamos tarea procesada en arreglo plan
}
fclose(f); // cerramos el archivo 
}


int buscar_indice_por_id(int id) {
for (int i = 0; i < total_tareas; i++) {
if (plan[i].id == id) return i;
}
return -1;
}





bool dependencias_listas(int idx){
for(int i=0;i< plan[idx].cant_dependencias;i++){
int dep_id = plan[idx].dependencias[i];
int dep_idx= buscar_indice_por_id(dep_id);
 
if(dep_idx != -1 && plan[dep_idx].estado != COMPLETADO){
return false;//aca se dice que falta al menos una dependencia  
}
}
return true; // si todas las depen ya terminaron 
}







void ejecutar_tarea(int idx){
if(pipe(plan[idx].pipe_fd) == -1){
perror("Error en la creacion de pipe"); 
exit(1); }

pid_t pid=fork(); 

if(pid <0){
perror("Error"); 
exit(1); 
} else if( pid==0){ //hijo
close(plan[idx].pipe_fd[0]); // aca el hijo no lee pipe, solo escribe
usleep(plan[idx].duracion_ms * 1000);  // simula duracion en mili

string mensaje = "OK:" + to_string(plan[idx].id);
write(plan[idx].pipe_fd[1], mensaje.c_str(), mensaje.length());

close(plan[idx].pipe_fd[1]);
        exit(0); // el hijo termino bien
    } else { //aca es el proceso padre
        close(plan[idx].pipe_fd[1]); // el padre solo lee, cierra el lado de escritura
        plan[idx].pid = pid;
        plan[idx].estado = EJECUCION;
        procesos_activos++;

cout << "[INICIO] tarea" << plan[idx].id << " (" << plan[idx].nombre 
             << ") en PID " << pid << endl;
}}







void esperar_proceso() {
int status;
pid_t pid_finalizado = waitpid(-1, &status, 0);// aca bloquea a padre hasta q  algun hijo termine,  asi no se consume toda la cpu

if (pid_finalizado > 0) {
procesos_activos--;

for (int i = 0; i < total_tareas; i++) { // buscamos tarea de pid 
if (plan[i].pid == pid_finalizado && plan[i].estado == EJECUCION) {
char buffer[128] = {0}; //leemos mensaje enviado por pipe 
read(plan[i].pipe_fd[0], buffer, sizeof(buffer) - 1);
close(plan[i].pipe_fd[0]);

if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
plan[i].estado = COMPLETADO;
cout << "[COMPLETADO] Tarea " << plan[i].id << " (" << plan[i].nombre << "). Pipe leido: " << buffer << endl;
} else {
plan[i].estado = CANCELADO;
}
break;
}
}
}
}








int main (int  argc, char *argv[]){

if( argc < 3){ //para validar argumentos necesarios 
cout<<"Uso: "<< argv[0] << "<plan.txt> <K_concurrencia>" <<endl; 
return 1; 
}

srand(time(NULL));
limite_K =atoi(argv[2]); // guardamos k  ingresado 

if(limite_K <=0){
cout << "El limite k debe ser mayor a 0, no valido " << endl; 
return 1; }

leer_plan(argv[1]);
cout<<"Carga y parseo del DAG exitoso "<<endl; 
cout<<"Total tareas cargadas: " <<total_tareas<<", limite k: "<<limite_K<<endl;

for(int i=0; i< total_tareas; i++){
cout << "[ID " << plan[i].id << "] " << plan[i].nombre << ", Duracion: " << plan[i].duracion_ms << " ms"
                  << ", Depende de (" << plan[i].cant_dependencias << " tareas): ";


for (int j=0; j < plan[i].cant_dependencias; j++) {
            cout << plan[i].dependencias[j] << " ";
        }
        cout <<endl;
    }

    return 0;
}
