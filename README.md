# Mobit Renderer

## Prerequisites

- A C++ compiler
    - For Windows, use [MinGW64 DevKit](https://github.com/skeeto/w64devkit/).

- CMake

## Preparation

Clone the repository

```bash
git clone https://github.com/HenryMarkle/MobitRenderer.git --recursive
```

## Building

Follow the steps depending on your operating system

### Windows

To compile for windows, move to the `/build` directory and run the command

```bash
cmake .. -G "MinGW Makefiles"
```

The following compile definitions are available on configuration:

- `-DFEATURE_PALETTES=1`: for enabling palettes support.
- `-DFEATURE_DATAPACKS=1`: for enabling data packs support.
- `-DCMAKE_BUILD_TYPE`: for defining build type, which accepts either `Release` or `Debug`.
    - When compiled for `Debug`, a console window will appear on Windows when launching the program, additional debugging features are enabled.

Example of enabling all of them on configuration:

```bash
cmake .. -G "MinGW Makefiles" -DFEATURE_PALETTES=1 -DFEATURE_DATAPACKS=1 -DCMAKE_BUILD_TYPE=Release
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

## Usage

The program can be used as an editor or as a renderer.
Simply by double-clicking the program, it launches in editor-renderer hybrid mode, allowing to open projects, edit and renderer them with ImGui user interface.

The program can also be invoked from the terminal, launching the auto-renderer mode, by simply providing the path of the level you want to render as the last argument.

When auto-renderer is invoked, the renderer automatically loads the project, renderers it, and outputs it without requiring any further user interaction. 

Type `.\MobitRenderer.exe --help` to view all the options available.

## Considerations

Please consider the following things, when using building and using this project (especially if you're coming from the Discord modding community).

### Work-in-Progress

This project is still a work-in-progress (WIP). Expect bugs and crashes. When reporting bugs, it is preferrable to use the project with debugging configuration.

### Minimal editing features

While the program is technically an editor, it is by no means meant to be a replacement of any of the recent level editors out there, as Rained has won the competition. Expect more features to come, but nothing too fancy.

### User data validation

The project has varying degrees of user-fault toleration, while I'm doing my best to keep deserialization reasonable, if the program fails to parse your `Init` or project files, it is your fault:

- Every Init.txt file must begin with a category.
- Duplicate definitions are ignored.
- Props that conflict with tiles are ignores.

- Project files MUST include all lines, including but not limited to: geometry, tiles, effects, cameras, water, props. I know that the officail and community editors sometimes leave out some parts, but that won't be accepted here.

### Parity with the official (and community) editors

This project does not prioritize accomplishing the exact same results as with the official, community, or Drizzle renderers. It may be acheived in the future though.

### Technical knowledge

This project is not designed for people who do not know basic coding skills, such as C/C++ programming languages, Git & GitHub, and CMake.

