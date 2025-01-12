# Mobit Renderer

## Preparation

Clone the repository

```bash
git clone https://github.com/HenryMarkle/MobitRenderer.git --recursive
```

## Windows

To compile for windows, move the '/build' directory and run the command

```bash
cmake .. -G "MinGW Makefiles"
```
To hide the console window when launching the program, execute this instead

```bash
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
```

Then run the command

```bash
mingw32-make
```

## Linux

in the root directory of the project, the command

```bash
cmake --build build
```