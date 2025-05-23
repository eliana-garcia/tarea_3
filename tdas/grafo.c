#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grafo.h"

int is_equal_string(void *key1, void *key2) {
    return strcmp((char *)key1, (char *)key2) == 0;
}

Vertice* crearVertice(const char* id, const char* nombre) {
    Vertice* v = malloc(sizeof(Vertice));
    v->id = strdup(id);
    v->nombre = strdup(nombre);
    v->adyacentes = list_create();
    return v;
}

Arista* crearArista(Vertice* destino, int peso) {
    Arista* a = malloc(sizeof(Arista));
    a->destino = destino;
    a->peso = peso;
    return a;
}

Grafo* crearGrafo() {
    Grafo* g = malloc(sizeof(Grafo));
    g->vertices = map_create(is_equal_string);
    return g;
}

void agregarVertice(Grafo* grafo, const char* id, const char* nombre) {
    if (map_search(grafo->vertices, (void*) id)) return; // Ya existe
    Vertice* v = crearVertice(id, nombre);
    map_insert(grafo->vertices, v->id, v);
}

void agregarArista(Grafo* grafo, const char* origen, const char* destino, int peso) {
    Vertice* vOrigen = (Vertice*) map_search(grafo->vertices, (void*) origen);
    Vertice* vDestino = (Vertice*) map_search(grafo->vertices, (void*) destino);
    if (!vOrigen || !vDestino) return;

    Arista* a = crearArista(vDestino, peso);
    list_pushBack(vOrigen->adyacentes, a);
}

Vertice* buscarVertice(Grafo* grafo, const char* id) {
    return (Vertice*) map_search(grafo->vertices, (void*) id);
}

List* obtenerAdyacentes(Grafo* grafo, const char* id) {
    Vertice* v = (Vertice*) map_search(grafo->vertices, (void*) id);
    if (!v) return NULL;
    return v->adyacentes;
}


void mostrarGrafo(Grafo* grafo) {
    MapPair* pair = map_first(grafo->vertices);
    while (pair) {
        Vertice* v = pair->value;
        printf("Lugar %s (%s):\n", v->nombre, v->id);
        List* ady = v->adyacentes;
        for (Arista* a = list_first(ady); a != NULL; a = list_next(ady)) {
            printf("  -> %s (%s) [Tiempo: %d]\n", a->destino->nombre, a->destino->id, a->peso);
        }
        pair = map_next(grafo->vertices);
    }
}


