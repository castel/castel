# Castel compiler

Please see the [official homepage](http://castel.github.com) for more informations.

## Compilation

You will need LLVM 3.2, Ragel, Lemon, Clang, Boost, Readline and CMake.

The LibCastel is included as a git submodule. Don't forget to init them before starting compilation.

    $> git clone git@github.com:castel/castel
    $> mkdir castel/build
    $> cd castel/build
    $> cmake ..
    $> make

The binary will be generated in the `bin/` directory.

## License

```
* -------------------------------------------------------------------------------
* "THE BEER/PIZZA-WARE LICENSE" (Revision 42):
* <nison.mael@gmail.com> wrote these files. As long as you retain this notice you
* can do whatever you want with this stuff. If we meet some day, and you think
* this stuff is worth it, you can buy me a beer or a pizza in return Maël Nison
* -------------------------------------------------------------------------------
```
