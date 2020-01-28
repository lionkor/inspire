# inspire

*a simple command-line idea collection tool!*


I find myself often lacking ideas for new projects or other things to make, so I made this little **command-line tool** to remember any ideas i get for later!


## Usage is simple!

* To add a new idea that you have:

    ```
    $ inspire add <idea>
    ```

    Example:

    ```
    $ inspire add asteroids clone but you're the asteroid
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
