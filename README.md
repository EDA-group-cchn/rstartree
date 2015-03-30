# Implementación de Árbol R*


## Flujo de Trabajo

Se utilizará el flujo de trabajo
[Feature Branch](https://www.atlassian.com/git/tutorials/comparing-workflows/feature-branch-workflow/).
Una vez que se termina de implementar la característica asignada, se envía un
PullRequest para que la implementación sea revisada y, de estar todo correcto, mezclada. Usar mensajes
de commit descriptivos.


## Estilo de Código

Se seguirá la guía de
[estilo de código de Google](https://google-styleguide.googlecode.com/svn/trunk/cppguide.html).
Adicionalmente, tomar en cuenta lo siguiente:

- Métodos de 1 a 3 líneas (usualmente getters y setters) pueden ser implementados dentro de la
declaración de la clase. Otros métodos deberán ir fuera de la misma.


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
