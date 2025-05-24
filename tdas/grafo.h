#ifndef GRAFO_H
#define GRAFO_H

#include "map.h"
#include "list.h"

typedef struct {
    char* id;         // ID del vértice (puede ser número o string)
    char* nombre;     // Nombre del lugar
    List* adyacentes; // Lista de aristas
} Vertice;

typedef struct {
    Vertice* destino;
    int peso; // Tiempo o peso de la arista
} Arista;

typedef struct {
    Map* vertices; // Mapa de id -> Vertice*
} Grafo;

int is_equal_string(void *key1, void *key2);
Grafo* crearGrafo();
void agregarVertice(Grafo* grafo, const char* id, const char* nombre);
void agregarArista(Grafo* grafo, const char* origen, const char* destino, int peso);
Vertice* buscarVertice(Grafo* grafo, const char* id);
List* obtenerAdyacentes(Grafo* grafo, const char* id);
void mostrarGrafo(Grafo* grafo);

#endif
