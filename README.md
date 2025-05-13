# GraphQuest

Este sistema permite jugar **GraphQuest**, un juego de aventura donde el jugador recorre un **laberinto** representado como un **grafo**. Cada lugar del laberinto es un escenario con decisiones y objetos que se puedan recoger.

El jugador parte desde la **entrada principal** y llegar a la **salida**, eligiendo su camino y recorriendo ítems en el camino. Cada movimiento gasta **tiempo**, y llevar muchos ítems lo hace avanzar más lento.

El objetivo es **llegar a la salida con el mayor puntaje posible**, recogiendo los mejores ítems antes de que se acabe el tiempo.

## Cómo copilar y ejecutar

Este sistema ha sido desarrollado en lenguaje C y puede ejecutarse fácilmente utilizando Visual Studio Code junto con una extensión para C/C++, como C/C++ Extension Pack de Microsoft. Para comenzar a trabajar con el sistema en tu equipo local, sigue estos pasos:

### Requisitos previos:

- Tener instalado [Visual Studio Code](https://code.visualstudio.com/).
- Instalar la extensión **C/C++** (Microsoft).
- Tener instalado un compilador de C (como **gcc**). Si estás en Windows, se recomienda instalar [MinGW](https://www.mingw-w64.org/) o utilizar el entorno [WSL](https://learn.microsoft.com/en-us/windows/wsl/).

### Pasos para compilar y ejecutar:

1. **Descarga y descomprime el** archivo `.zip` en una carpeta de tu elección.
2. **Abre el proyecto en Visual Studio Code**
    - Inicia Visual Studio Code.
    - Selecciona `Archivo > Abrir carpeta...` y elige la carpeta donde descomprimiste el proyecto.
3. **Compila el código**
    - Abre el archivo principal (`tarea2.c`).
    - Abre la terminal integrada (`Terminal > Nueva terminal`).
    - En la terminal, compila el programa con el siguiente comando:
```
gcc tdas/*.c tarea2.c -Wno-unused-result -o tarea2
```
- Para ejecutar el programa, use:

```
./tarea2
```

## Funcionalidades disponibles

- **

*Todas las funciones funcionan correctamente*

