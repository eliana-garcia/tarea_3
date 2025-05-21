#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "tdas/list.h"
#include "tdas/heap.h"
#include "tdas/extra.h"

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
        presioneTeclaParaContinuar();
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

void descartarItems(Jugador* jugador) {
    if (list_size(jugador->inventario) == 0) {
        printf("No tienes ítems para descartar.\n");
        presioneTeclaParaContinuar();
        return;
    }

    printf("Seleccione el ítem a descartar (0 para cancelar):\n");
    int i = 1;
    void* itemData = list_first(jugador->inventario);
    while (itemData != NULL) {
        Item* item = (Item*)itemData;
        printf("%d. %s (Peso: %d, Valor: %d)\n", i++, item->nombre, item->peso, item->valor);
        itemData = list_next(jugador->inventario);
    }

    int opcion;
    scanf("%d", &opcion);
    if (opcion == 0) return;

    if (opcion > 0 && opcion <= list_size(jugador->inventario)) {
        itemData = list_first(jugador->inventario);
        for (i = 1; i < opcion; i++) {
            itemData = list_next(jugador->inventario);
        }

        Item* item = (Item*)itemData;
        jugador->pesoTotal -= item->peso;
        jugador->puntajeTotal -= item->valor;
        list_popCurrent(jugador->inventario);  // Elimina el ítem
        jugador->tiempo--; // Restar tiempo
    }
}

void avanzar(Jugador* jugador, List* escenarios) {
    Escenario* actual = NULL;
    void* data = list_first(escenarios);
    int i = 0;
    while (data != NULL) {
        if (i == jugador->escenarioActual) {
            actual = (Escenario*)data;
            break;
        }
        data = list_next(escenarios);
        i++;
    }

    if (!actual) return;

    printf("Seleccione la dirección:\n");
    if (actual->arriba != -1) printf("1. Arriba\n");
    if (actual->abajo != -1) printf("2. Abajo\n");
    if (actual->izquierda != -1) printf("3. Izquierda\n");
    if (actual->derecha != -1) printf("4. Derecha\n");
    printf("0. Cancelar\n");

    int opcion;
    scanf("%d", &opcion);

    int nuevoEscenario = -1;
    if (opcion == 1 && actual->arriba != -1) nuevoEscenario = actual->arriba;
    else if (opcion == 2 && actual->abajo != -1) nuevoEscenario = actual->abajo;
    else if (opcion == 3 && actual->izquierda != -1) nuevoEscenario = actual->izquierda;
    else if (opcion == 4 && actual->derecha != -1) nuevoEscenario = actual->derecha;
    else if (opcion == 0) return;
    else {
        printf("Dirección inválida.\n");
        presioneTeclaParaContinuar();
        return;
    }

    jugador->escenarioActual = nuevoEscenario;

    // Cálculo del tiempo gastado
    int tiempoGastado = (jugador->pesoTotal + 1 + 9) / 10; // ceil(peso + 1 / 10)
    jugador->tiempo -= tiempoGastado;

    // Verificar si es escenario final
    Escenario* nuevo = NULL;
    i = 0;
    data = list_first(escenarios);
    while (data != NULL) {
        if (i == jugador->escenarioActual) {
            nuevo = (Escenario*)data;
            break;
        }
        data = list_next(escenarios);
        i++;
    }

    if (nuevo && nuevo->esFinal) {
        printf("\n¡Has llegado al escenario final!\n");
        printf("Puntaje final: %d\n", jugador->puntajeTotal);
        printf("Ítems en el inventario:\n");

        void* itemData = list_first(jugador->inventario);
        while (itemData != NULL) {
            Item* item = (Item*)itemData;
            printf("- %s (Peso: %d, Valor: %d)\n", item->nombre, item->peso, item->valor);
            itemData = list_next(jugador->inventario);
        }

        presioneTeclaParaContinuar();
        exit(0);
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
                descartarItems(&jugador);
                break;
            case 3:
                avanzar(&jugador, escenarios);
                break;
            case 4:
                jugador.pesoTotal = 0;
                jugador.puntajeTotal = 0;
                jugador.tiempo = 20;
                jugador.escenarioActual = 0;
                list_clean(jugador.inventario);
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