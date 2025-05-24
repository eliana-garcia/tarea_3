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

1. Abre **Visual Studio Code**
2. Abre la terminal integrada (Terminal > Nueva Terminal)
3. en la terminal abre **Git Bash**

- En la terminal, copila el programa en el siguiente comando:
```
gcc tdas/*.c tarea3.c -Wno-unused-result -o GraphQuest
```
- Para ejecutar el programa, use;
```
./GraphQuest
```

## Funcionalidades 

- Cargar laberinto desde un archivo CSV
- Iniciar partida desde la entrada principal
- Moverse entre escenarios y consumir tiempo según el peso del inventario
- Recoger ítems en los escenarios y agregarlos al inventario
- Descartar ítems para reducir peso
- Avanzar en una Dirección
- Finalizar el juego al llegar a la salida o cuando se acaba el tiempo

**se podria mejorar como se muestra el juego, como los menus, el recoger items, mas que nada la presentacion, tambien un error que encontre es que si terminas el juego o lo ganas y vuelves no se libera bien la memoria y te devuelve los resultados anteriores por lo que para que funcione deverias de volver a salir y ejecutar el programa de nuevo**

## Ejemplo de uso 

1. Cargar el laberinto 
    - El jugador empieza cargando el laberinto desde un archivo CSV.

2. Inicia la partida
    - Te muestra la opcion inicial siendo el tiempo disponible y los movimientos que puedes ejecutar.

3. Recoger items
    - Si en el esenario hay items disponibles, puedes recogerlos, estos te daran peso y valor.

4. Descartar items 
    - En cualquier momento puedes descartar los items que has ido recoguiendo a lo largo del camino, te descontara el peso que cargas y valor.

5. Avanzar 
    - En esta opcion tienes que elegir que direcion disponible quieres seguir, para continuar el laberinto.

6. Reinicir partida
    - La partida se reiniciara y se eliminara todo el proceso que tiene.


