# ThisFunc-Interpreter
# Interpreter for 'thisFunc' - A Functional Language

Welcome to the repository for the **Interpreter for 'thisFunc'**, a functional programming language designed to support key functional programming concepts. This interpreter processes and executes programs written in 'thisFunc,' a language focusing on clarity, purity, and immutability.

## Features

- **Functional Syntax**: Supports core functional programming features such as higher-order functions, recursion, and immutability.
- **Dynamic Function Definition**: Functions can be defined and called at runtime.
- **Simple and Lightweight**: The interpreter is minimal, focusing on simplicity and clarity in its design and execution.
- **Extensible**: Easy to extend with new features, such as additional built-in functions or error-handling mechanisms.

## Installation

### Prerequisites

Make sure you have the following installed on your machine:

- [C++ Compiler](https://gcc.gnu.org/) (e.g., GCC or Clang)
- [CMake](https://cmake.org/) (for building the project)

### Steps

1. Clone the repository:
    ```bash
    git clone https://github.com/your-username/thisFunc-interpreter.git
    cd thisFunc-interpreter
    ```

2. Build the project using CMake:
    ```bash
    mkdir build
    cd build
    cmake ..
    make
    ```

3. After building, you can run the interpreter using the following command:
    ```bash
    ./thisFunc-interpreter
    ```

## Usage

To use the interpreter, simply provide a `.tf` file (a 'thisFunc' source code file) as an argument:

```bash
./thisFunc-interpreter path/to/your/file.tf
