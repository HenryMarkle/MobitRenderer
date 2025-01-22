# Mobit Renderer

## Preparation

Clone the repository

```bash
git clone https://github.com/HenryMarkle/MobitRenderer.git --recursive
```

## Building

Follow the steps depending on your operating system

### Windows

To compile for windows, move the `/build` directory and run the command

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

### Linux

in the root directory of the project, the command

```bash
cmake --build build
```

---

The output should be in `/build/bin`.
Copy `Assets` and `Data` from the source directory to `/build/bin`.
