#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
using namespace std; 

#define MAX_TAREAS 10005 //para prueba de estres
#define MAX_DEPS 50   // cant max de dependencias por tarea

//estados de las tareas
#define PENDIENTE 0
#define PROCESANDO 1
#define COMPLETADO 2

struct Tarea {
int id;
char nombre[1000]; 
int duracion_ms; 
int dependencias[MAX_DEPS]; 

int cant_dependencias; 
int estado; 
};

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
