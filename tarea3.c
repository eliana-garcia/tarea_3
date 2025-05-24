#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "tdas/grafo.h"
#include "tdas/list.h"
#include "tdas/map.h"
#include "tdas/extra.h"

// Definición de estructuras
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

// Funcion para crea un nuevo item con los valores dados y reserva memoria para él.
Item* crearItem(const char* nombre, int peso, int valor) {
    // Reservar memoria para un nuevo Item
    Item* item = (Item*)malloc(sizeof(Item));
    // Copiar el nombre recibido al campo nombre del item
    item->nombre = strdup(nombre);
    // Asignar el peso y el valor al nuevo item
    item->peso = peso;
    item->valor = valor;
    // Devolver el puntero al item creado
    return item;
}

// Funcion que crea un nuevo escenario con nombre, descripción y conexiones vacías.
Escenario* crearEscenario(char* nombre, char* descripcion) {
    // Reservar memoria para un nuevo Escenario
    Escenario* escenario = (Escenario*)malloc(sizeof(Escenario));
    // Copiar el nombre y la descripción recibidos
    escenario->nombre = strdup(nombre);
    escenario->descripcion = strdup(descripcion);
    // Inicializar la lista de ítems del escenario
    escenario->items = list_create();
    // Inicializar las conexiones del escenario con otros
    escenario->arriba = escenario->abajo = escenario->izquierda = escenario->derecha = -1;
    // Por defecto no es el escenario final
    escenario->esFinal = 0;
    // Devolver el puntero al escenario creado
    return escenario;
}

// Función para cargar el laberinto desde un archivo CSV y devuelve una lista de escenarios.
List* cargarLaberinto(const char* archivo) {
    // Intentar abrir el archivo en modo lectura
    FILE* fp = fopen(archivo, "r");
    if (!fp) {
        printf("Error al abrir el archivo %s\n", archivo);
        presioneTeclaParaContinuar();
        return NULL;
    }
    // Crear la lista donde se almacenarán los escenarios
    List* escenarios = list_create();
    // Leer la primera línea (cabecera) y descartarla
    char** campos = leer_linea_csv(fp, ',');
    // Leer línea por línea hasta el final del archivo
    while ((campos = leer_linea_csv(fp, ',')) != NULL) {
        // Ignorar líneas vacías o mal formadas
        if (!campos[0]) continue;
        // Crear un nuevo escenario con nombre y descripción extraídos del CSV
        Escenario* escenario = crearEscenario(campos[1], campos[2]);
        // Procesar la lista de items si existe
        if (campos[3] && strlen(campos[3]) > 0) {
            // Separar items por ";"
            List* items = split_string(campos[3], ";");
            // Iterar sobre cada item de la lista
            char* item = list_first(items);
            while (item) {
                // Cada item contiene datos separados por ",": nombre, peso, valor
                List* data = split_string(item, ",");
                char* nombre = list_first(data);
                int peso = atoi(list_next(data));
                int valor = atoi(list_next(data));
                // Crear el item y añadirlo a la lista de items del escenario
                list_pushBack(escenario->items, crearItem(nombre, peso, valor));
                // Avanzar al siguiente item
                item = list_next(items);
            }
        }
         // Asignar las conexiones del escenario con los índices indicados en CSV
        escenario->arriba = atoi(campos[4]);
        escenario->abajo = atoi(campos[5]);
        escenario->izquierda = atoi(campos[6]);
        escenario->derecha = atoi(campos[7]);
        // Determinar si es el escenario final (comparar con "Si")
        escenario->esFinal = strcmp(campos[8], "Si") == 0;
        // Añadir el escenario a la lista general de escenarios
        list_pushBack(escenarios, escenario);
    }
    // Cerrar el archivo una vez terminado
    fclose(fp);
    // Devolver la lista completa de escenarios cargados
    return escenarios;
}

// Función para obtener un escenario específico de la lista de escenarios 
Escenario* obtenerEscenario(List* escenarios, int id) {
    int i = 0;
    // Obtener el primer elemento de la lista
    void* data = list_first(escenarios);
    // Recorrer la lista hasta encontrar la posición 
    while (data) {
        // El ID es base 1, por eso se compara con i + 1
        if (i == id - 1) return (Escenario*)data;
        // Avanzar al siguiente elemento
        data = list_next(escenarios);
        i++;
    }
    // Si no se encuentra el escenario con el ID dado, retornar NULL
    return NULL;
}

void mostrarEstado(List* escenarios, Jugador* jugador) {
    // Obtener el escenario actual del jugador
    Escenario* esc = obtenerEscenario(escenarios, jugador->escenarioActual);
    // Mostrar nombre y descripción del escenario actual
    printf("\n=== %s ===\n", esc->nombre);
    printf("%s\n\n", esc->descripcion);
    // Mostrar tiempo restante del jugador
    printf("Tiempo restante: %d\n", jugador->tiempo);
    // Mostrar inventario actual, con peso y puntaje total
    printf("Inventario (Peso: %d, Puntaje: %d):\n", jugador->pesoTotal, jugador->puntajeTotal);
    // Verificar si el inventario está vacío
    if (!list_first(jugador->inventario)) {
        printf("  - Vacío\n");
    } else {
        // Recorrer e imprimir cada ítem en el inventario
        void* it = list_first(jugador->inventario);
        while (it) {
            Item* item = (Item*)it;
            printf("  - %s (P: %d, V: %d)\n", item->nombre, item->peso, item->valor);
            it = list_next(jugador->inventario);
        }
    }
    // Mostrar ítems disponibles en el escenario actual
    printf("\n Ítems disponibles en este escenario:\n");
    if (!list_first(esc->items)) {
        printf("  - No hay ítems en este lugar.\n");
    } else {
        // Recorrer e imprimir cada ítem del escenario
        void* it = list_first(esc->items);
        while (it) {
            Item* item = (Item*)it;
            printf("  - %s (P: %d, V: %d)\n", item->nombre, item->peso, item->valor);
            it = list_next(esc->items);
        }
    }
    // Mostrar las direcciones disponibles para moverse desde el escenario actual
    printf("\nDirecciones disponibles:\n");
    if (esc->arriba != -1) printf("1. Arriba\n");
    if (esc->abajo != -1) printf("2. Abajo\n");
    if (esc->izquierda != -1) printf("3. Izquierda\n");
    if (esc->derecha != -1) printf("4. Derecha\n");
}

void recogerItems(Escenario* esc, Jugador* jugador) {
    // Verificar si hay ítems disponibles en el escenario
    if (list_size(esc->items) == 0) {
        printf("\nNo hay ítems para recoger.\n");
        presioneTeclaParaContinuar();
        return;
    }
    // Mostrar los ítems disponibles y permitir al jugador elegir uno
    while (1) {
        limpiarPantalla();
        printf("\nÍtems disponibles para recoger:\n");
        // Mostrar todos los ítems disponibles en el escenario
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
        if (scanf("%d", &opcion) != 1) {
            printf("Entrada inválida.\n");
            presioneTeclaParaContinuar();
            // Limpiar buffer para evitar ciclo infinito
            while(getchar() != '\n');
            continue;
        }
        // Salir del ciclo si elige "Volver"
        if (opcion == index) break; 
        // Validar opción ingresada
        if (opcion < 1 || opcion > index) {
            printf("Opción inválida.\n");
            presioneTeclaParaContinuar();
            continue;
        }
        // Recorrer la lista hasta el ítem seleccionado
        it = list_first(esc->items);
        int i = 1;
        while (it && i < opcion) {
            it = list_next(esc->items);
            i++;
        }
        if (it) {
            Item* item = (Item*)it;
            // Agregar ítem al inventario del jugador
            list_pushBack(jugador->inventario, item);
            // Actualizar estado del jugador
            jugador->pesoTotal += item->peso;
            jugador->puntajeTotal += item->valor;
            jugador->tiempo--;
            // Eliminar ítem del escenario
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
        if (scanf("%d", &opcion) != 1) {
            printf("Entrada inválida.\n");
            presioneTeclaParaContinuar();
            while(getchar() != '\n'); // limpiar buffer
            continue;
        }
        if (opcion == index) break; 
        if (opcion < 1 || opcion > index) {
            printf("Opción inválida.\n");
            presioneTeclaParaContinuar();
            continue;
        }
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
            jugador->tiempo--;
            // Eliminar nodo actual de la lista antes de liberar el item
            list_popCurrent(jugador->inventario);
            // Ahora liberar la memoria del item
            free(item->nombre);
            free(item);
            printf("Has descartado el ítem seleccionado.\n");
            presioneTeclaParaContinuar();
        }
    }
}

int avanzar(Jugador* jugador, Escenario* esc, int direccion, int idEscenarioFinal) {
    int siguiente = -1;
    // Determinar el ID del escenario al que se quiere mover
    switch (direccion) {
        case 1: siguiente = esc->arriba; break;
        case 2: siguiente = esc->abajo; break;
        case 3: siguiente = esc->izquierda; break;
        case 4: siguiente = esc->derecha; break;
        default:
            printf("\nDirección inválida.\n");
            presioneTeclaParaContinuar();
            return 0;
    }
    // Calcular el costo de moverse en función del peso cargado
    int costo = (int)ceil((jugador->pesoTotal + 1) / 10.0);
    jugador->tiempo -= costo;
    // Verificar si el jugador se quedó sin tiempo
    if (jugador->tiempo <= 0) {
        return 1; // Se quedó sin tiempo
    }
    // Actualizar escenario actual
    jugador->escenarioActual = siguiente;
    // Verificar si llegó al escenario final
    if (jugador->escenarioActual == idEscenarioFinal) {
        return 2;// Victoria
    }
    return 0;// Movimiento normal
}

// Libera toda la memoria asociada a la lista de escenarios.
void liberarEscenarios(List* escenarios) {
    void* ptr = list_first(escenarios);
    while (ptr) {
        Escenario* esc = (Escenario*)ptr;
        // Libera cada item del escenario
        void* itemPtr = list_first(esc->items);
        while (itemPtr) {
            Item* item = (Item*)itemPtr;
            free(item->nombre);// Libera el nombre del item
            free(item);// Libera la estructura del item
            itemPtr = list_next(esc->items);
        }
        // Limpia y libera la lista de items
        list_clean(esc->items);
        free(esc->items);
        // Libera los strings del escenario
        free(esc->nombre);
        free(esc->descripcion);
        // Libera la estructura del escenario
        free(esc);
        ptr = list_next(escenarios);// Avanza al siguiente escenario
    }
    // Limpia la lista de escenarios
    list_clean(escenarios);
}

// Libera toda la memoria asociada al jugador, incluyendo su inventario.
void liberarJugador(Jugador* jugador) {
    // Libera cada item del inventario del jugador
    void* it = list_first(jugador->inventario);
    while (it) {
        Item* item = (Item*)it;
        free(item); // Solo se libera el item, no su nombre
        it = list_next(jugador->inventario);
    }
    // Limpia y libera la lista de inventario
    list_clean(jugador->inventario);
    free(jugador->inventario);
    // Libera la estructura del jugador
    free(jugador);
}

void reiniciarJuego(List** escenarios, Jugador** jugador, const char* nombreArchivo) {
    // Liberar escenarios antiguos
    liberarEscenarios(*escenarios);
    *escenarios = cargarLaberinto(nombreArchivo);
    // Liberar jugador anterior
    liberarJugador(*jugador);
    // Crear nuevo jugador
    *jugador = (Jugador*)malloc(sizeof(Jugador));
    (*jugador)->inventario = list_create();
    (*jugador)->pesoTotal = 0;
    (*jugador)->puntajeTotal = 0;
    (*jugador)->tiempo = 20;
    (*jugador)->escenarioActual = 1;
}

// Función para mostrar el menú del juego
void menuJuego() {
    printf("\nOpciones:\n");
    printf("1. Recoger Ítems\n");
    printf("2. Descartar Ítems\n");
    printf("3. Avanzar\n");
    printf("4. Reiniciar\n");
    printf("5. Salir\n> ");
}

void jugarLaberinto(List* escenarios, Jugador* jugador) {
    // Buscar el ID del escenario final 
    int idEscenarioFinal = -1;
    int n = list_size(escenarios);
    for (int i = 0; i < n; i++) {
        Escenario* e = obtenerEscenario(escenarios, i+1);
        if (e->esFinal) {
            idEscenarioFinal = i + 1;
            break;
        }
    }
    // Bucle principal del juego
    while (1) {
        limpiarPantalla();// Limpiar pantalla para mostrar nuevo estado
        mostrarEstado(escenarios, jugador);// Mostrar estado actual del jugador y escenario
        // Si se quedó sin tiempo, termina el juego
        if (jugador->tiempo <= 0) {
            printf("\n¡Se acabó el tiempo! Has perdido.\n");
            presioneTeclaParaContinuar();
            return;
        }
        // Mostrar opciones del menú del juego
        menuJuego();
        int opcion;
        scanf("%d", &opcion);// Leer opción ingresada por el jugador
        // Obtener el escenario actual según ID
        Escenario* esc = obtenerEscenario(escenarios, jugador->escenarioActual);
        // Opción seleccionada
        switch (opcion) {
            case 1:
                // Recoger ítems del escenario actual
                recogerItems(esc, jugador);
                break;
            case 2:
                // Descarta ítems del inventario
                descartarItems(jugador);
                break;
            case 3: {
                // Avanzar hacia otra dirección
                printf("Elige dirección: ");
                int dir;
                scanf("%d", &dir);
                // Ejecutar movimiento y verificar el resultado
                int resultado = avanzar(jugador, esc, dir, idEscenarioFinal);
                if (resultado == 1) {
                    printf("\n¡Te quedaste sin tiempo durante el movimiento!\n");
                    presioneTeclaParaContinuar();
                    return;
                } else if (resultado == 2) {
                    printf("\n¡Felicidades! Llegaste al escenario final.\n");
                    printf("Puntaje final: %d\n", jugador->puntajeTotal);
                    presioneTeclaParaContinuar();
                    return;
                }
                break;
            }
            case 4:
                // Reiniciar juego 
                reiniciarJuego(&escenarios, &jugador, "graphquest.csv");
                printf("¡Juego reiniciado!\n");
                break;
            case 5:
                // Salir del juego y liberar memoria
                printf("\nGracias por jugar GraphQuest.\n");
                if (escenarios) list_clean(escenarios);
                list_clean(jugador->inventario);
                exit(0);
                break;               
            default:
                // Opción no válida
                printf("Opción inválida.\n");
                presioneTeclaParaContinuar();
        }
    }
}

// Función para mostrar el menú principal
void menuPrincipal() {
    printf("\n=== Menú Principal ===\n");
    printf("1. Cargar Laberinto\n");
    printf("2. Iniciar Partida\n");
    printf("3. Salir\n> ");
}

int main() {
    // Lista de escenarios cargados desde el archivo
    List* escenarios = NULL;
    // Inicialización del jugador
    Jugador jugador;
    jugador.inventario = list_create();// Crear lista vacía para el inventario
    jugador.tiempo = 20;// Tiempo disponible inicial
    jugador.pesoTotal = 0;// Peso total de ítems en el inventario
    jugador.puntajeTotal = 0; // Puntaje acumulado
    jugador.escenarioActual = 1;// ID del escenario inicial
    int opcion;
     // Bucle principal del menú
    while (1) {
        limpiarPantalla();// Limpia la pantalla para mostrar el menú desde cero
        menuPrincipal();// Muestra las opciones del menú
        printf("Seleccione una opción: ");
        // Leer la opción del usuario, validando que sea un entero
        if (scanf("%d", &opcion) != 1) {
            printf("Entrada inválida.\n");
            presioneTeclaParaContinuar();
            continue;
        }
        switch (opcion) {
            case 1:
            // Cargar laberinto desde archivo CSV
                escenarios = cargarLaberinto("graphquest.csv");
                if (!escenarios) {
                    printf("Error al cargar el laberinto.\n");
                } else {
                    printf("Laberinto cargado con éxito.\n");
                }
                presioneTeclaParaContinuar();
                break;
            case 2:
            // Iniciar juego si el laberinto ya fue cargado
                if (!escenarios) {
                    printf("Primero debes cargar el laberinto.\n");
                    presioneTeclaParaContinuar();
                } else {
                    jugarLaberinto(escenarios, &jugador);
                }
                break;
            case 3:
            // Salir del juego
                printf("Gracias por jugar GraphQuest.\n");
                if (escenarios) list_clean(escenarios);
                list_clean(jugador.inventario);
                return 0;
            default:
            // Opción inválida
                printf("Opción inválida.\n");
                presioneTeclaParaContinuar();
                break;
        }
    }
    return 0;
}

