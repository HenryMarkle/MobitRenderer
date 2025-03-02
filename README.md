# Mobit Renderer

## Prerequisites

- Git
- CMake
- A C++ compiler
    - For Windows, use [MinGW64 DevKit](https://github.com/skeeto/w64devkit/).


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


## The Source Code

The code is not well-documented, therefore rely on the code being self-documented.
All libraries are stored in `/libs`.

The entry point for the editor is in `src/main.cpp`, and for the auto-renderer is in `src/autorenderermode.cpp`.

I could not compile `raylib` on Windows, therefore I use the pre-compiled files as a temproray solution. 

I do not own a Mac, so I could not compile and test the project on MacOS.

### Strong Guidlines

- The C++ standard will not be upgraded untill the next standard receives as much adoptions and popularity.
- No other language is acceptable, except for `C`.

### Weak Guidelines

#### C++ Standard

- C++17 was chosen specifically as the sweet spot between old, but well supported and new but not widely adopted.
- Despite the number of features the languages supports, only a subset of the features is preferable to the cose sane.

#### Conditional Compilation

There exist the following compiler definitions:

| Compiler Definition | Description |
|---------------------|-------------|
| APP_VERSION | is defined a string to indicate the app version |
| BUILD_TYPE | is defined as a string to indicate the build type: `Release` and `Debug` |
| IS_DEBUG_BUILD | Indicated that the program is compiled in `Debug` mode |
| FEATURE_PALETTES | is defined to enable the use of palettes |
| FEATURE_DATAPACKS | is defined to enable datapacks support |

#### Naming Conventions

- Functions, variables, and namespaces use `snake_case`.
- Types use `PascalCase`.
- Compiler constants (`#define`) use `SCREAMING_SNAKE_CASE`.
- Private class members have a leading underscore '_'.
- **Never** use `using` or `#define` in header files.

#### Pointers

Summary: don't do stupid shit.

- Whereever a shared pointer is used, know that I either couldn't bother managing its lifetime or wanted to save memory from copying data repeatedly.

- The usage of smart pointers does not stop the code from using the underlying raw pointer (i.e `ptr.get()`).

- Whenever a raw pointer is used, whether in a function or a data type, must always be expected to be null.

- A raw pointer that is not null is always assumed to be valid.

- If a data type stores a raw pointer, the data type typically does not manage the pointer, _unless_ it is explicitly stated in the constructor/destructor (and that is the exception and not the rule).

- Pointers must always be initialized, whether to null or a valid memory address.

#### Types

- `typedef`s are discouraged, except for shortening type names.
- `auto` is usually preferred, except for explcit numeric type casting.
    - Example: `float result = 10 / 2`.
