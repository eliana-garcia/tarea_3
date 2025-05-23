
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "tdas/grafo.h"
#include "tdas/list.h"
#include "tdas/map.h"
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
    int arriba, abajo, izquierda, derecha;
    int esFinal;
} Escenario;

typedef struct {
    List* inventario;
    int pesoTotal;
    int puntajeTotal;
    int tiempo;
    int escenarioActual;
} Jugador;

Item* crearItem(const char* nombre, int peso, int valor) {
    Item* item = (Item*)malloc(sizeof(Item));
    item->nombre = strdup(nombre);
    item->peso = peso;
    item->valor = valor;
    return item;
}

Escenario* crearEscenario(char* nombre, char* descripcion) {
    Escenario* escenario = (Escenario*)malloc(sizeof(Escenario));
    escenario->nombre = strdup(nombre);
    escenario->descripcion = strdup(descripcion);
    escenario->items = list_create();
    escenario->arriba = escenario->abajo = escenario->izquierda = escenario->derecha = -1;
    escenario->esFinal = 0;
    return escenario;
}

List* cargarLaberinto(const char* archivo) {
    FILE* fp = fopen(archivo, "r");
    if (!fp) {
        printf("Error al abrir el archivo %s\n", archivo);
        presioneTeclaParaContinuar();
        return NULL;
    }

    List* escenarios = list_create();
    char** campos = leer_linea_csv(fp, ',');
    while ((campos = leer_linea_csv(fp, ',')) != NULL) {
        if (!campos[0]) continue;
        Escenario* escenario = crearEscenario(campos[1], campos[2]);
        if (campos[3] && strlen(campos[3]) > 0) {
            List* items = split_string(campos[3], ";");
            char* item = list_first(items);
            while (item) {
                List* data = split_string(item, ",");
                char* nombre = list_first(data);
                int peso = atoi(list_next(data));
                int valor = atoi(list_next(data));
                list_pushBack(escenario->items, crearItem(nombre, peso, valor));
                item = list_next(items);
            }
        }
        escenario->arriba = atoi(campos[4]);
        escenario->abajo = atoi(campos[5]);
        escenario->izquierda = atoi(campos[6]);
        escenario->derecha = atoi(campos[7]);
        escenario->esFinal = strcmp(campos[8], "Si") == 0;
        list_pushBack(escenarios, escenario);
    }
    fclose(fp);
    return escenarios;
}

Escenario* obtenerEscenario(List* escenarios, int id) {
    int i = 0;
    void* data = list_first(escenarios);
    while (data) {
        if (i == id - 1) return (Escenario*)data;
        data = list_next(escenarios);
        i++;
    }
    return NULL;
}

void mostrarEstado(List* escenarios, Jugador* jugador) {
    Escenario* esc = obtenerEscenario(escenarios, jugador->escenarioActual);
    printf("\n=== %s ===\n", esc->nombre);
    printf("%s\n\n", esc->descripcion);
    printf("Tiempo restante: %d\n", jugador->tiempo);
    printf("Inventario (Peso: %d, Puntaje: %d):\n", jugador->pesoTotal, jugador->puntajeTotal);
    void* it = list_first(jugador->inventario);
    while (it) {
        Item* item = (Item*)it;
        printf("- %s (P: %d, V: %d)\n", item->nombre, item->peso, item->valor);
        it = list_next(jugador->inventario);
    }
    printf("\nÍtems en este escenario:\n");
    it = list_first(esc->items);
    while (it) {
        Item* item = (Item*)it;
        printf("- %s (P: %d, V: %d)\n", item->nombre, item->peso, item->valor);
        it = list_next(esc->items);
    }
    printf("\nDirecciones disponibles:\n");
    if (esc->arriba != -1) printf("1. Arriba\n");
    if (esc->abajo != -1) printf("2. Abajo\n");
    if (esc->izquierda != -1) printf("3. Izquierda\n");
    if (esc->derecha != -1) printf("4. Derecha\n");
}

void recogerItems(Escenario* esc, Jugador* jugador) {
    if (list_size(esc->items) == 0) {
        printf("\nNo hay ítems para recoger.\n");
        presioneTeclaParaContinuar();
        return;
    }

    while (1) {
        limpiarPantalla();
        printf("\nÍtems disponibles para recoger:\n");
        int index = 1;
        void* it = list_first(esc->items);
        while (it) {
            Item* item = (Item*)it;
            printf("%d. %s (Peso: %d, Valor: %d)\n", index, item->nombre, item->peso, item->valor);
            it = list_next(esc->items);
            index++;
        }
        printf("%d. Volver\n", index);
        printf("Elige ítem para recoger o volver: ");
        int opcion;
        scanf("%d", &opcion);
        if (opcion == index) break; // Volver

        if (opcion < 1 || opcion > index) {
            printf("Opción inválida.\n");
            presioneTeclaParaContinuar();
            continue;
        }

        // Sacar ítem de la lista del escenario y pasarlo al jugador
        it = list_first(esc->items);
        int i = 1;
        while (it && i < opcion) {
            it = list_next(esc->items);
            i++;
        }
        if (it) {
            Item* item = (Item*)it;
            list_pushBack(jugador->inventario, item);
            jugador->pesoTotal += item->peso;
            jugador->puntajeTotal += item->valor;
            jugador->tiempo--;

            // Remover el ítem de la lista del escenario
            list_popCurrent(esc->items);
            printf("Has recogido %s.\n", item->nombre);
            presioneTeclaParaContinuar();
        }
    }
}

void descartarItems(Jugador* jugador) {
    if (list_size(jugador->inventario) == 0) {
        printf("\nNo tienes ítems para descartar.\n");
        presioneTeclaParaContinuar();
        return;
    }

    while (1) {
        limpiarPantalla();
        printf("\nÍtems en tu inventario:\n");
        int index = 1;
        void* it = list_first(jugador->inventario);
        while (it) {
            Item* item = (Item*)it;
            printf("%d. %s (Peso: %d, Valor: %d)\n", index, item->nombre, item->peso, item->valor);
            it = list_next(jugador->inventario);
            index++;
        }
        printf("%d. Volver\n", index);
        printf("Elige ítem para descartar o volver: ");
        int opcion;
        scanf("%d", &opcion);
        if (opcion == index) break; // Volver

        if (opcion < 1 || opcion > index) {
            printf("Opción inválida.\n");
            presioneTeclaParaContinuar();
            continue;
        }

        // Sacar ítem de inventario
        it = list_first(jugador->inventario);
        int i = 1;
        while (it && i < opcion) {
            it = list_next(jugador->inventario);
            i++;
        }
        if (it) {
            Item* item = (Item*)it;
            jugador->pesoTotal -= item->peso;
            jugador->puntajeTotal -= item->valor;
            // Liberar memoria si quieres eliminar ítems (opcional)
            free(item->nombre);
            free(item);

            list_popCurrent(jugador->inventario);
            printf("Has descartado %s.\n", item->nombre);
            presioneTeclaParaContinuar();
        }
    }
}



void avanzar(Jugador* jugador, Escenario* esc, int direccion) {
    int siguiente = -1;
    if (direccion == 1) siguiente = esc->arriba;
    else if (direccion == 2) siguiente = esc->abajo;
    else if (direccion == 3) siguiente = esc->izquierda;
    else if (direccion == 4) siguiente = esc->derecha;

    if (siguiente == -1) {
        printf("\nDirección inválida.\n");
        return;
    }

    int costo = (jugador->pesoTotal + 1 ) / 10;
    jugador->tiempo -= costo;
    jugador->escenarioActual = siguiente;
}

void reiniciar(Jugador* jugador) {
    list_clean(jugador->inventario);
    jugador->pesoTotal = 0;
    jugador->puntajeTotal = 0;
    jugador->tiempo = 10;
    jugador->escenarioActual = 1;
}

void menuPrincipal() {
    printf("\n=== Menú Principal ===\n");
    printf("1. Cargar Laberinto\n");
    printf("2. Iniciar Partida\n");
    printf("3. Salir\n> ");
}

void mostrarEstado(List* escenarios, Jugador* jugador);
void menuJuego() {
    printf("\nOpciones:\n");
    printf("1. Recoger Ítems\n");
    printf("2. Descartar Ítems\n");
    printf("3. Avanzar\n");
    printf("4. Reiniciar\n");
    printf("5. Salir\n> ");
}

void destruirEscenario(void* data) {
    Escenario* esc = (Escenario*)data;
    if (!esc) return;

    free(esc->nombre);
    free(esc->descripcion);

    void* it = list_first(esc->items);
    while (it) {
        Item* item = (Item*)it;
        free(item->nombre);
        free(item);
        it = list_next(esc->items);
    }

    list_clean(esc->items);
    free(esc->items);
    free(esc);
}

int main() {
    List* escenarios = NULL;
    Jugador jugador;
    jugador.inventario = list_create();
    jugador.tiempo = 10;
    jugador.pesoTotal = 0;
    jugador.puntajeTotal = 0;
    jugador.escenarioActual = 1;

    int opcion;

    while (1) {
        limpiarPantalla();
        menuPrincipal();
        scanf("%d", &opcion);

        if (opcion == 1) {
            escenarios = cargarLaberinto("graphquest.csv");
            presioneTeclaParaContinuar();
        } else if (opcion == 2 && escenarios) {
            reiniciar(&jugador);
            while (1) {
                limpiarPantalla();
                mostrarEstado(escenarios, &jugador);
                                if (jugador.tiempo <= 0) {
                    printf("\n¡Se acabó el tiempo!\n");
                    printf("Tu puntaje final fue: %d\n", jugador.puntajeTotal);
                    presioneTeclaParaContinuar();
                    break;
                }

                Escenario* esc = obtenerEscenario(escenarios, jugador.escenarioActual);
                if (esc->esFinal) {
                    printf("\n¡Has llegado al escenario final!\n");
                    printf("Tu puntaje final fue: %d\n", jugador.puntajeTotal);
                    presioneTeclaParaContinuar();
                    break;
                }

                menuJuego();
                int opcionJuego;
                scanf("%d", &opcionJuego);

                if (opcionJuego == 1) {
                    recogerItems(esc, &jugador);
                } else if (opcionJuego == 2) {
                    descartarItems(&jugador);
                } else if (opcionJuego == 3) {
                    printf("Elige dirección: ");
                    int dir;
                    scanf("%d", &dir);
                    avanzar(&jugador, esc, dir);
                } else if (opcionJuego == 4) {
                    reiniciar(&jugador);
                    list_clean(escenarios);
                    escenarios = cargarLaberinto("graphquest.csv");
                } else if (opcionJuego == 5) {
                    break;
                }
            }
        } else if (opcion == 3) {
            break;
        } else {
            printf("Debes cargar el laberinto primero.\n");
            presioneTeclaParaContinuar();
        }
    }

    // Liberar memoria
    list_clean(jugador.inventario);
    if (escenarios) {
        void* data = list_first(escenarios);
        while (data) {
            Escenario* esc = (Escenario*)data;
            free(esc->nombre);
            free(esc->descripcion);
            void* item = list_first(esc->items);
            while (item) {
                Item* it = (Item*)item;
                free(it->nombre);
                free(it);
                item = list_next(esc->items);
            }
            list_clean(esc->items);
            free(esc->items);
            free(esc);
            data = list_next(escenarios);
        }
        list_clean(escenarios);
    }

    return 0;
}
