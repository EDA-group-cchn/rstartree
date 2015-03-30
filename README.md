# Implementación de Árbol R*


## Flujo de Trabajo

Se utilizará el flujo de trabajo
[Feature Branch](https://www.atlassian.com/git/tutorials/comparing-workflows/feature-branch-workflow/).
Una vez que se termina de implementar la característica asignada, se envía un
PullRequest para que la implementación sea revisada y, de estar todo correcto, mezclada. Usar mensajes
de commit descriptivos.


## Compilación y Ejecución

Primera vez:
    
    $ mkdir build
    
Cada vez que cambia `CMakeLists.txt`:

    $ cd build
    $ cmake ..
    
Cada vez que cambian las fuentes:
    
    $ cd build
    $ make
    $ ./rstartree
