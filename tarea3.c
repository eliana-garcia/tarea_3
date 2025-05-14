#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "tdas/list.h"
#include "tdas/heap.h"
#include "tdas/extra.h"

/*
// Definición de la estructura para el estado del puzzle
typedef struct {
    int square[3][3]; // Matriz 3x3 que representa el tablero
    int x;    // Posición x del espacio vacío
    int y;    // Posición y del espacio vacío
    List* actions; //Secuencia de movimientos para llegar al estado
} State;

int distancia_L1(State* state) {
    int ev=0;
    int k=1;
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++){
            int val=state->square[i][j];
            if (val==0) continue;
            int ii=(val)/3;
            int jj=(val)%3;
            ev+= abs(ii-i) + abs(jj-j);
        }
    return ev;
}


// Función para imprimir el estado del puzzle
void imprimirEstado(const State *estado) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (estado->square[i][j] == 0)
                printf("x "); // Imprime un espacio en blanco para el espacio vacío
            else
                printf("%d ", estado->square[i][j]);
        }
        printf("\n");
    }
}


int main() {
    // Estado inicial del puzzle
    State estado_inicial = {
        {{0, 2, 8}, // Primera fila (0 representa espacio vacío)
         {1, 3, 4}, // Segunda fila
         {6, 5, 7}, // Tercera fila
         },  
        0, 1   // Posición del espacio vacío (fila 0, columna 1)
    };
    estado_inicial.actions = list_create();

    // Imprime el estado inicial
    printf("Estado inicial del puzzle:\n");
    imprimirEstado(&estado_inicial);

    printf("Distancia L1:%d\n", distancia_L1(&estado_inicial));

    //Ejemplo de heap (cola con prioridad)
    printf("\n***** EJEMPLO USO DE HEAP ******\nCreamos un Heap e insertamos 3 elementos con distinta prioridad\n");
    Heap* heap = heap_create();
    char* data = strdup("Cinco");
    printf("Insertamos el elemento %s con prioridad -5\n", data);
    heap_push(heap, data, -5 prioridad);
    data = strdup("Seis");
    printf("Insertamos el elemento %s con prioridad -6\n", data);
    heap_push(heap, data, -6 prioridad);
    data = strdup("Siete");
    printf("Insertamos el elemento %s con prioridad -7\n", data);
    heap_push(heap, data, -7 prioridad);

    printf("\nLos elementos salen del Heap ordenados de mayor a menor prioridad\n");
    while (heap_top(heap) != NULL){
        printf("Top: %s\n", (char*) heap_top(heap));      
        heap_pop(heap);
    }
    printf("No hay más elementos en el Heap\n");

    char opcion;
    do {
        printf("\n***** EJEMPLO MENU ******\n");
        puts("========================================");
        puts("     Escoge método de búsqueda");
        puts("========================================");
        
        puts("1) Búsqueda en Profundidad");
        puts("2) Buscar en Anchura");
        puts("3) Buscar Mejor Primero");
        puts("4) Salir");
    
        printf("Ingrese su opción: ");
        scanf(" %c", &opcion);
    
        switch (opcion) {
        case '1':
          //dfs(estado_inicial);
          break;
        case '2':
          //bfs(estado_inicial);
          break;
        case '3':
          //best_first(estado_inicial);
          break;
        }
        presioneTeclaParaContinuar();
        limpiarPantalla();

  } while (opcion != '4');

  return 0;
}
*/

typedef struct {
    char* nombre;
    int peso;
    int valor;
} Item;

typedef struct {
    char* nombre;
    char* descripcion;
    List* items;
    int arriba;
    int abajo;
    int izquierda;
    int derecha;
    int esFinal;
} Escenario;

typedef struct {
    List* inventario;
    int pesoTotal;
    int puntajeTotal;
    int tiempo;
    int escenarioActual;
} Jugador;

// Función para crear un nuevo ítem
Item* crearItem(const char* nombre, int peso, int valor) {
    Item* item = (Item*)malloc(sizeof(Item));
    item->nombre = strdup(nombre);
    item->peso = peso;
    item->valor = valor;
    return item;
}

// Función para crear un nuevo escenario
Escenario* crearEscenario(char* nombre, char* descripcion) {
    Escenario* escenario = (Escenario*)malloc(sizeof(Escenario));
    escenario->nombre = strdup(nombre);
    escenario->descripcion = strdup(descripcion);
    escenario->items = list_create();
    escenario->arriba = -1;
    escenario->abajo = -1;
    escenario->izquierda = -1;
    escenario->derecha = -1;
    escenario->esFinal = 0;
    return escenario;
}

// Función para cargar el laberinto desde CSV
List* cargarLaberinto(const char* archivo) {
    FILE* fp = fopen(archivo, "r");
    if (!fp) {
        printf("Error al abrir el archivo %s\n", archivo);
        return NULL;
    }

    List* escenarios = list_create();
    char** campos = leer_linea_csv(fp, ','); // Leer encabezados

    while ((campos = leer_linea_csv(fp, ',')) != NULL) {
        if (!campos[0]) continue;

        Escenario* escenario = crearEscenario(campos[1], campos[2]);
        
        // Procesar ítems
        if (campos[3] && strlen(campos[3]) > 0) {
            List* items = split_string(campos[3], ";");
            char* item = list_first(items);
            while (item != NULL) {
                List* itemData = split_string(item, ",");
                char* nombre = list_first(itemData);
                int peso = atoi(list_next(itemData));
                int valor = atoi(list_next(itemData));
                list_pushBack(escenario->items, crearItem(nombre, peso, valor));
                item = list_next(items);
            }
        }

        escenario->arriba = atoi(campos[4]);
        escenario->abajo = atoi(campos[5]);
        escenario->izquierda = atoi(campos[6]);
        escenario->derecha = atoi(campos[7]);
        escenario->esFinal = (strcmp(campos[8], "Si") == 0);

        list_pushBack(escenarios, escenario);
    }

    fclose(fp);
    return escenarios;
}

// Función para mostrar el estado actual
void mostrarEstado(List* escenarios, Jugador* jugador) {
    Escenario* escenarioActual = NULL;
    int i = 0;
    void* data = list_first(escenarios);
    while (data != NULL) {
        if (i == jugador->escenarioActual) {
            escenarioActual = (Escenario*)data;
            break;
        }
        data = list_next(escenarios);
        i++;
    }

    if (!escenarioActual) return;

    printf("\n=== %s ===\n", escenarioActual->nombre);
    printf("Descripción: %s\n", escenarioActual->descripcion);
    printf("\nÍtems disponibles:\n");
    
    void* itemData = list_first(escenarioActual->items);
    while (itemData != NULL) {
        Item* item = (Item*)itemData;
        printf("- %s (Peso: %d, Valor: %d)\n", item->nombre, item->peso, item->valor);
        itemData = list_next(escenarioActual->items);
    }

    printf("\nTiempo restante: %d\n", jugador->tiempo);
    printf("Inventario (Peso total: %d, Puntaje: %d):\n", jugador->pesoTotal, jugador->puntajeTotal);
    
    itemData = list_first(jugador->inventario);
    while (itemData != NULL) {
        Item* item = (Item*)itemData;
        printf("- %s (Peso: %d, Valor: %d)\n", item->nombre, item->peso, item->valor);
        itemData = list_next(jugador->inventario);
    }

    printf("\nDirecciones disponibles:\n");
    if (escenarioActual->arriba != -1) printf("- Arriba\n");
    if (escenarioActual->abajo != -1) printf("- Abajo\n");
    if (escenarioActual->izquierda != -1) printf("- Izquierda\n");
    if (escenarioActual->derecha != -1) printf("- Derecha\n");
}

// Función para recoger ítems
void recogerItems(List* escenarios, Jugador* jugador) {
    Escenario* escenarioActual = NULL;
    int i = 0;
    void* data = list_first(escenarios);
    while (data != NULL) {
        if (i == jugador->escenarioActual) {
            escenarioActual = (Escenario*)data;
            break;
        }
        data = list_next(escenarios);
        i++;
    }

    if (!escenarioActual || list_size(escenarioActual->items) == 0) {
        printf("No hay ítems para recoger\n");
        return;
    }

    printf("Seleccione el ítem a recoger (0 para cancelar):\n");
    i = 1;
    void* itemData = list_first(escenarioActual->items);
    while (itemData != NULL) {
        Item* item = (Item*)itemData;
        printf("%d. %s (Peso: %d, Valor: %d)\n", i++, item->nombre, item->peso, item->valor);
        itemData = list_next(escenarioActual->items);
    }

    int opcion;
    scanf("%d", &opcion);
    if (opcion == 0) return;

    if (opcion > 0 && opcion <= list_size(escenarioActual->items)) {
        itemData = list_first(escenarioActual->items);
        for (i = 1; i < opcion; i++) {
            itemData = list_next(escenarioActual->items);
        }
        Item* item = (Item*)itemData;
        list_pushBack(jugador->inventario, item);
        jugador->pesoTotal += item->peso;
        jugador->puntajeTotal += item->valor;
        // Restar tiempo
        jugador->tiempo--;
    }
}

// Función principal del juego
void jugar(List* escenarios) {
    Jugador jugador = {
        .inventario = list_create(),
        .pesoTotal = 0,
        .puntajeTotal = 0,
        .tiempo = 20,
        .escenarioActual = 0
    };

    while (1) {
        limpiarPantalla();
        mostrarEstado(escenarios, &jugador);

        printf("\nOpciones:\n");
        printf("1. Recoger ítem\n");
        printf("2. Descartar ítem\n");
        printf("3. Avanzar\n");
        printf("4. Reiniciar partida\n");
        printf("5. Salir\n");
        printf("Seleccione una opción: ");

        int opcion;
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                recogerItems(escenarios, &jugador);
                break;
            case 2:
                // Implementar descarte de ítems
                break;
            case 3:
                // Implementar movimiento
                break;
            case 4:
                // Reiniciar juego
                jugador.pesoTotal = 0;
                jugador.puntajeTotal = 0;
                jugador.tiempo = 20;
                jugador.escenarioActual = 0;
                break;
            case 5:
                return;
        }

        if (jugador.tiempo <= 0) {
            printf("\n¡Se acabó el tiempo! Juego terminado.\n");
            printf("Puntaje final: %d\n", jugador.puntajeTotal);
            presioneTeclaParaContinuar();
            return;
        }
    }
}

int main() {
    List* escenarios = cargarLaberinto("graphquest.csv");
    if (!escenarios) {
        printf("Error al cargar el laberinto\n");
        return 1;
    }

    while (1) {
        limpiarPantalla();
        printf("=== GraphQuest ===\n");
        printf("1. Iniciar partida\n");
        printf("2. Salir\n");
        printf("Seleccione una opción: ");

        int opcion;
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                jugar(escenarios);
                break;
            case 2:
                return 0;
        }
    }

    return 0;
}