# ThisFunc-Interpreter

Interpreter for **ThisFunc**, a small functional programming language focusing on clarity, purity, and immutability.

---

## Features

* Pure functions, recursion, and immutability
* Dynamic function definition and runtime evaluation
* Lists, arithmetic, logical operations, and conditionals
* Lightweight, minimal, and easy to extend

---

## Installation

Clone the repository:

```
git clone https://github.com/AceEmo/ThisFunc-Interpreter.git
cd ThisFunc-Interpreter
```

Compile (if C++):

```
g++ -std=c++17 -O2 main.cpp -o thisfunc
```

Run the interpreter:

```
./thisfunc
```

---

## Usage

**REPL Example:**

```
> add(2,3)
5
> fact <- if(eq(#0,0),1,mul(#0,fact(sub(#0,1))))
> fact(5)
120
> myList <- list(1,2,3)
> head(myList())
1
> tail(myList())
[2,3]
```

---

## Interpreter Structure

1. **Lexer:** Tokenizes the source code.
2. **Parser:** Builds the Abstract Syntax Tree (AST).
3. **AST:** Represents literals, variables, operations, conditionals, function calls, and lists.
4. **Evaluator:** Traverses the AST, computes values, handles recursion and function calls.

---

## Built-in Functions

* Arithmetic: `add`, `sub`, `mul`, `div`, `pow`, `sqrt`
* Logical/Comparison: `eq`, `le`, `nand`
* Conditional: `if(cond, then, else)`
* Lists: `list(...)`, `head(list)`, `tail(list)`, `length(list)`

---

## Examples

Factorial:

```
fact <- if(eq(#0,0),1,mul(#0,fact(sub(#0,1))))
fact(5)  ; returns 120
```

Fibonacci:

```
fib <- div(sub(pow(add(1,sqrt(5)),#0),pow(sub(1,sqrt(5)),#0)),mul(pow(2,#0),sqrt(5)))
fib(12) ; returns 144
```

Lists:

```
myList <- list(10,20,30)
length(myList()) ; 3
head(myList())   ; 10
tail(myList())   ; [20,30]
```

---
