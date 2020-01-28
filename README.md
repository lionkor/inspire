# inspire

*a simple command-line idea collection tool!*


I find myself often lacking ideas for new projects or other things to make, so I made this little **command-line tool** to remember any ideas i get for later!

## How to build

Either run `cmake .` and then `make`, or just use gcc: `gcc main.c -o inspire`

## Usage is simple!

* To add a new idea that you have:

    ```
    $ inspire add [idea]
    ```

    This will read from stdin if you don't supply text, so you can happily pipe into it:
    ```
    $ some_other_command | inspire add
    ```

    Examples:

    ```
    $ inspire add asteroids clone but you're the asteroid
    ```
    ```
    $ inspire add
    My new idea
    ```

* To retrieve a random stored idea:
    
    ```
    $ inspire give
    ```

    Example:

    ```
    $ inspire give
    asteroids clone but you're the asteroid
    ```

    (the ability to remove items directly as a reaction to this is WIP)

* To see all stored ideas:
    
    ```
    $ inspire show
    my first idea!
    finish my game
    work on fixme's in 'ideas'
    asteroids clone but you're the asteroid
    ```

    this can then be piped to `less`, `shuf`, for other processing. 


* To clear all ideas, there is currently no builtin way. The best way is to remove the file `$HOME/.inspire/data`.
