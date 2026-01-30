#   Mirage Language Specification

This should hopefully be all I need to implement in the C++ compiler before I can bootstrap the Mirage langauge

---

## 1. Variables
Borrowing heavily from Go and Rust programming langauges lol

### 1.1 Mutable Variables

Mutable variables are declared with the `var` keyword. The type annotation is optional when the type can be inferred
from the initialiser. When no initialiser is present, the type annotation is required.

Unused variables will throw compiler error 

**Syntax:**
```
var <name> <type> = <expr>;   // explicit type + initialiser
var <name>        = <expr>;   // inferred type + initialiser
var <name> <type>;            // explicit type, no initialiser (must be assigned later)
```

**Examples:**
```
var x int = 5;       // explicit type
var y     = 10;      // type inferred as int
var z float;         // declared, assigned later
z = 3.14;
```

---

### 1.2 Constants

Constants are declared with the `const` keyword. Constants are **untyped** when inferred. 
They adapt to the type context in which they are used (Go-style). 
A type annotation pins them to a specific type.

**Syntax:**
```
const <name> <type> = <expr>;   // typed constant
const <name>        = <expr>;   // untyped constant (adapts to usage context)
```

**Examples:**
```
const MAX_SIZE int = 100;   // typed: always int
const PI           = 3.14;  // untyped: fits float32 or float64 as needed
const LIMIT        = 256;   // untyped: fits int8, int16, int32, int64, uint8 as needed

var a int32 = LIMIT; // LIMIT will become int32 here
var b int64 = LIMIT; // LIMIT will become int64 here
var c uint32 = LIMIT; // LIMIT will become uint32 here
var d uint8 = 257; // compile error since 256 doesn't fit in uint8 (0-255) (Go behavior)


// These would be compile errors:
// var unused int = 5;          ERROR: assigned but never read
// var x int;  (never assigned) ERROR: declared but never assigned before use
// MAX_PLAYERS = 16;            ERROR: cannot assign to constant

```

---

### 1.3 Reassignment

Reassignment uses plain `=`. Only `var` variables may be reassigned; reassigning a `const` is a compile-time error.

```
var x int = 5;
x = 10;          // works

const Y = 5;
Y = 10;          // ERROR: cannot assign to constant
```

---

### 1.4 Usage Rules

| Rule                    | Description                                                                             |
|-------------------------|-----------------------------------------------------------------------------------------|
| Declared but never used | Compile error: every variable must be read at least once anywhere after declaration     |
| Used before assigned    | Compile error: a var declared without an initialiser must be assigned before it is read |
| Type mismatch on assign | Compile error: the RHS type must match the declared or inferred type                    |
| Const reassignment      | Compile error: constants are immutable after declaration                                |

---

## 2. Functions

Functions are declared with the `func` keyword. Parameters follow the same pattern as variables. 
The return type is specified after the parameter list using an arrow `->`. 
Functions with no return value omit the arrow.

Functions can return multiple values using an unnamed tuple syntax. 
For reusable named shapes, use a struct instead.

### 2.1 Entry Point

Every Mirage program starts at `main`. It returns an `int` exit code and takes command-line arguments as `[]string`.

```
func main(args []string) -> int {
    // program starts here
    return 0;
}
```

**Syntax:**
```
func <name>(<params>) -> <return_type> {
    <body>
}

func <name>(<params>) {
    <body>
}

func <name>(<params>) -> (<type1>, <type2>, ..., <typeN>) {
    return <expr1>, <expr2>, ..., <exprN>;
}

```

**Examples:**
```
func add(a int, b int) -> int {
    return a + b;
}

func greet(name string) -> string {
    return "Hello, " + name;
}

// No return type - omit the arrow entirely
func printScore(score int) {
    // prints score, returns nothing
}

// Multiple return values - unnamed tuple
func divide(a int, b int) -> (int, int) {
    return a / b, a % b;
}
var quotient, remainder = divide(10, 3);

```

---

## 3. Types

### 3.0 Primitive Types

| Category | Types | Notes                                                                                                                                                                            |
|----------|-------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Signed integers | `int8`, `int16`, `int32`, `int64` |                                                                                                                                                                                  |
| Unsigned integers | `uint8`, `uint16`, `uint32`, `uint64` |                                                                                                                                                                                  |
| Platform integer | `int` | Native word size (64-bit on 64-bit systems) (stealing from Go again or atleast thats the plan)                                                                                   |
| Half-precision floats | `float16`, `bfloat16` | `bfloat16` = [brain float](https://cloud.google.com/blog/products/ai-machine-learning/bfloat16-the-secret-to-high-performance-on-cloud-tpus) // who knows if I'll implement this |
| Floats | `float32`, `float64` |                                                                                                                                                                                  |
| Float alias | `float` | Alias for `float64`                                                                                                                                                              |
| Complex numbers | `complex64`, `complex128` | `complex64` = float32 real + imaginary; `complex128` = float64 real + imaginary; same deal as brainfloat, we'll see if I implmeent                                               |
| Boolean | `bool` | `true` / `false`                                                                                                                                                                 |
| String | `string` | UTF-8 encoded                                                                                                                                                                    |
| Byte | `byte` | Alias for `uint8`                                                                                                                                                                |

### 3.04 Print / IO 

`print` and `println` are built-in functions available without imports (so novel!).

```
print("hello");           // no newline - stdout
println("hello");         // with newline - stdout
eprint("error");          // no newline - stderr
eprintln("error");        // with newline - stderr
println("x = ${x}");     // works with string interpolation! :D
```

Full IO control (stdout/stderr objects, buffered writers, blah blah blah) will be implemented after I boot strap hopefully.

### 3.05 Strings

Strings are UTF-8 encoded and immutable. Use standard C-style escape sequences.

**Escape sequences:**
`\n` newline, `\t` tab, `\\` backslash, `\"` double quote, `\0` null, `\r` carriage return

**String interpolation** with `${}`:
```
var name = "Alice";
var msg = "hello ${name}";
var info = "score: ${score + 1}";   // expressions allowed inside ${}
```

**Multiline strings** use triple quotes. Leading whitespace is stripped based on the indentation of the closing `"""`.
```
var text = """
    this is line one
    this is line two
    """;
// result: "this is line one\nthis is line two\n"
```

**Concatenation** also works with `+`:
```
var full = "hello " + "world";
```

Formatted output for ML and numeric formatting will be provided via `fmt()` in the standard library (if and when I get to it).

### 3.1 Arrays & Slices

#### Fixed Arrays

Fixed arrays have a size known at compile time and live on the stack. The size is part of the type - `[5]int` and `[10]int` are different types.

```
var fixed [5]int = [1, 2, 3, 4, 5];
var kernel [3]float32 = [0.1, 0.5, 0.1];
var rgb [3]uint8 = [255, 128, 0];
var buffer [1024]byte;
```

#### Dynamic Slices

Slices have a size determined at runtime and live on the heap. Under the hood a slice is a pointer to data, a length, and a capacity.

```
var slice []int = [1, 2, 3];
slice.push(4);    // [1, 2, 3, 4]
```

#### Indexing

Zero-indexed. Both arrays and slices use bracket syntax.

```
var first = nums[0];
var last = nums[4];
```

**Out-of-bounds behavior:**

| Method | Behavior                                              |
|--------|-------------------------------------------------------|
| `nums[i]` | Runtime panic if out of bounds                        |
| `nums.get(i)` | Returns `Option<T>` -> `Some(value)` or `None`        |
| `fixed[10]` (constant index) | Compile-time error when both size and index are known |

#### Length

TODO: decide this syntax 
```
var len = nums.len(); 
// or maybe 
var len = nums.len;
```

#### Range Slicing

Slices of arrays/slices using range syntax. **Zero-copy by default** - a slice is a view into the original memory.
Basically Rust Syntax
```
var nums []int = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9];

var a = nums[2..5];     // [2, 3, 4] - exclusive upper bound
var b = nums[2..=5];    // [2, 3, 4, 5] - inclusive upper bound
var c = nums[..3];      // [0, 1, 2] - from start
var d = nums[3..];      // [3, 4, 5, 6, 7, 8, 9] - to end
var e = nums[..];       // full slice
```

Because slices are views, mutating a slice mutates the original as shown:
```
var sub = nums[1..4];   // view into nums
sub[0] = 99;            // also modifies nums[1]
```

For an independent copy:
```
var copy = nums[1..4].clone();  // explicit copy, owns its own memory
```

### 3.15 Error Handling - Result Type

Mirage uses a `Result<T, E>` type for error handling (no exceptions). A `Result` is either `Ok(value)` or `Err(error)`.

**Syntax:**
```
func readFile(path string) -> Result<string, Error> {
    // returns Ok(data) on success, Err(e) on failure
}
```

**Handling errors with match:**
```
match readFile("data.txt") {
    Ok(data) -> println(data),
    Err(e)   -> eprintln("failed: ${e}"),
}
```

**Propagating errors** - functions that return `Result` can use `?` to propagate errors to the caller:
```
func loadConfig(path string) -> Result<Config, Error> {
    var text = readFile(path)?;   // returns Err early if readFile fails
    var config = parse(text)?;
    return Ok(config);
}
```

| Type | Variants | Use case |
|------|----------|----------|
| `Result<T, E>` | `Ok(T)`, `Err(E)` | Operations that can fail |
| `Option<T>` | `Some(T)`, `None` | Values that may or may not exist |

### 3.16 Enums (Algebraic Types)

Enums are declared with the `enum` keyword. Variants can optionally carry associated data. Simple enums (no data) are just a special case of the same syntax.

**Syntax:**
```
enum <name> {
    <Variant>,
    <Variant>(<type>, ...),
}
```

**Examples:**
```
// Simple - no associated data
enum Color {
    Red,
    Green,
    Blue,
}

// With associated data
enum Shape {
    Circle(float),
    Rectangle(float, float),
    Point,
}

var s = Shape::Circle(5.0);
match s {
    Circle(r)        -> println("radius: ${r}"),
    Rectangle(w, h)  -> println("area: ${w * h}"),
    Point            -> println("just a point"),
}
```

`Result` and `Option` are regular enums defined in the standard library:
```
enum Option<T> {
    Some(T),
    None,
}

enum Result<T, E> {
    Ok(T),
    Err(E),
}
```

### 3.17 `::` vs `.` - Namespace vs Instance Access

`::` accesses things inside a namespace or type (modules, enum variants, static methods).
`.` accesses things on an instance.

```
// :: for namespace/type access
math::add(1, 2);              // module function
Shape::Circle(5.0);           // enum variant constructor
Option::Some(42);             // enum variant
String::from("hello");        // static method (future)

// . for instance access
var s = Shape::Circle(5.0);
var r = s.radius;             // instance field
var rounded = data.floor();   // instance method
```

| Operator | Meaning | Exapmle                                              |
|----------|---------|-----------------------------------------------------|
| `::` | Inside this namespace/type | `mod::func`, `Enum::Variant`, `Type::static_method` |
| `.` | On this instance | `obj.field`, `val.method()`                         |

### 3.17b Null - There Is None :P

[Mirage doesn't have **null**](https://www.infoq.com/presentations/Null-References-The-Billion-Dollar-Mistake-Tony-Hoare/). The only way to represent absence is `Option<T>`. Every non-Option type is guaranteed to hold a valid value at all times.

```
var x int = 5;                  // always valid, can never be null
var y Option<int> = None;       // explicit optionality
var z Option<int> = Some(42);

// Must handle both cases to use the value
match y {
    Some(val) -> println("got ${val}"),
    None      -> println("nothing"),
}
```

This eliminates null pointer errors at compile time. If a value might not exist, 
the type system forces you to say so and handle it. 

### 3.18 Generics

Functions, structs, and enums can be parameterized with type variables using `<T>`. 
Type parameters can be constrained with trait bounds using `<T: Bound>`.

**Generic functions:**
```
func max<T: Comparable>(a T, b T) -> T {
    if a > b { return a; }
    return b;
}

func identity<T>(x T) -> T {
    return x;
}
```

**Generic structs:**
```
struct Pair<T, U> {
    first T,
    second U,
}

// Type inference - explicit type params optional when inferable
var p = Pair { first: 1, second: "hello" };              // inferred Pair<int, string>
var p = Pair<int, string> { first: 1, second: "hello" }; // explicit when needed
```

**Generic enums**:
```
enum Option<T> {
    Some(T),
    None,
}

enum Result<T, E: Error> {
    Ok(T),
    Err(E),
}
```

**Multiple bounds** use `+`:
```
func process<T: Comparable + Serializable>(val T) -> T { ... }
```

**Multiple type parameters with bounds**, comma separated:
```
func zip<T: Comparable, U: Serializable>(a T, b U) -> Pair<T, U> { ... }
```

Generics are abstracted at write time but during compile time, the compiler will generate a dedicated implementation 
for each type used.

### 3.2 Structs

Structs are declared with the `struct` keyword. Fields follow the name-before-type pattern, comma separated.

**Declaration:**
```
struct <name> {
    <field> <type>,
    <field> <type>,
}
```

**Instantiation** uses named fields with colons:
```
var <name> = <StructName> { <field>: <expr>, <field>: <expr> };
```

**Examples:**
```
struct Vec2 {
    x float,
    y float,
}

var pos = Vec2 { x: 0.0, y: 0.0 };
var vel = Vec2 { x: 1.5, y: -3.0 };

// Access fields with dot notation
var px = pos.x;
```

### 3.3 Traits / Interfaces

### 3.4 Closures / Lambdas

### 3.5 Iterators

### 3.6 Standard Library

---

## 4. Control Flow

### 4.1 If / Else

Conditions don't need to use parentheses. Braces are **required**, even for a single statement.

**Syntax:**
```
if <condition> {
    <body>
}

if <condition> {
    <body>
} else {
    <body>
}

if <condition> {
    <body>
} else if <condition> {
    <body>
} else {
    <body>
}
```

**Examples:**
```
if x > 10 {
    x = 10;
}

if score >= 100 {
    // win
} else if score >= 50 {
    // decent
} else {
    // try again
}
```


### 4.2 Loops

  Mirage has separate `for` and `while` keywords. Braces are required.

**`for...in` - iterator loop (no parens around clause):**
```
for <name> in <iterable> {
    <body>
}
```

The built-in `range` function generates integer sequences. Exclusive upper bounds.
(Basically Pythons range function lol)


| Form | Description                                |
|------|--------------------------------------------|
| `range(end)` | 0 to end-1, step 1                         |
| `range(start, end)` | start to end-1, step 1                     |
| `range(start, end, step)` | start to end-1 (exclusive), with given step |

Negative step counts downward. `range(10, 0, -1)` yields 10, 9, 8, ... 1.

`for...in` also works over arrays and other iterables.

`while` - condition-only loop (parenthesis not required):
```
while <condition> {
    <body>
}
```

**Examples:**
```
for i in range(10) {
    // 0 through 9
}

for i in range(0, 10, 3) {
    // 0, 3, 6, 9
}

for i in range(10, 0, -1) {
    // 10, 9, 8, ... 1
}

for item in myArray {
    // iterate over elements
}

while x < 100 {
    x = x * 2;
}
```

### 4.3 Break & Continue

`break` exits the innermost loop. `continue` skips to the next iteration. Both work in `for` and `while` loops.

**Labeled breaks/continues** target a specific outer loop using a label followed by a colon (like Java or JS)

```
// Basic
while true {
    if done {
        break;
    }
    continue;
}

// Labeled: break out of a specific outer loop
outer: for i in range(10) {
    for j in range(10) {
        if i + j > 15 {
            break outer;
        }
    }
}
```

### 4.4 Match

`match` is an expression-based multi-branch construct. `_` is the wildcard/default case. 
Each arm uses `->` to map a pattern to its result.

**Syntax:**
```
match <expr> {
    <pattern> -> <expr>,
    <pattern> -> <expr>,
    _         -> <expr>,
}
```

Can be used as an expression:
```
var label = match x {
    1 -> "one",
    2 -> "two",
    _ -> "other",
};
```

Or as a statement with block bodies:
```
match status {
    0 -> {
        // handle success
    },
    _ -> {
        // handle error
    },
}
```

---

## 5. Memory & Pointers
(Basically Rust)
### 5.1 Ownership

Every value in Mirage has exactly one owner. When the owner goes out of scope, the value is dropped 
(deterministic deallocation, not garbage collector). Assignment moves ownership by default.

```
var x = Vec2 { x: 1.0, y: 2.0 };
var y = x;       // x is moved into y - x can no longer be used
// x = ...;      // ERROR: use of moved value
```

### 5.2 Borrowing

References borrow a value without taking ownership. Immutable borrows use `&`, mutable borrows use `&mut`.
The compiler enforces borrowing rules at compile time:

- Any number of immutable borrows `&T` at the same time, OR
- Exactly one mutable borrow `&mut T` - never both simultaneously

```
var x = 5;
var r &int = &x;          // immutable borrow
var m &mut int = &mut x;  // mutable borrow
```

**In function parameters:**
```
func render(v &Vec2) {
    // can read v, cannot modify
}

func update(v &mut Vec2) {
    v.x = v.x + 1.0;   // can modify through mutable borrow
}
```

### 5.3 Raw Pointers (Unsafe)

Raw pointers (`*T`) are available as an escape hatch for low-level operations (SIMD, FFI, custom allocators). 
They require an `unsafe` block - the compiler does not enforce safety rules inside unsafe.

```
unsafe {
    var p *int = &x;
    *p = 10;
}
```

| Pointer type | Safety | Compiler checked |
|-------------|--------|-----------------|
| `&T` | Safe immutable reference | Yes |
| `&mut T` | Safe mutable reference | Yes |
| `*T` | Raw pointer | No - requires `unsafe` |

### 5.4 Concurrency - Channels


  When first learning Go, I really liked their concurrency implementation of ["Don't communicate by sharing memory, 
  instead share memory by communicating"](https://medium.com/@relieved_gold_mole_613/golang-explanation-share-memory-by-communicating-8be944cbf8f8)
  As a result, I decided to steal it for Mirage ^_^



  "Channels" are the main way for tasks to communicate. When sending data through a channel, the ownership transfers
  to the receiver so the sender can no longer touch it. This means two tasks can't modify the same data at the same 
  time



```
var ch = channel<int>();

// sender
ch.send(42);

// receiver
var val = ch.recv();
```

Sending a value into a channel moves it so the sender can no longer access it enforced by the ownership system.

### 5.5 Spawn - Concurrent Tasks

`spawn` launches a concurrent task and returns a handle. The handle can be used to join, check status, 
or cancel the task.

**Syntax:**
```
var handle = spawn {
    <body>
};
```

**Handle methods:**

| Method | Description |
|--------|-------------|
| `handle.join()` | Block until done, returns `Result<T, Error>` |
| `handle.is_done()` | Non-blocking check, returns `bool` |
| `handle.cancel()` | Signal cancellation |

**Spawn with return value:**
```
var handle = spawn {
    return trainEpoch(model, data);
};

match handle.join() {
    Ok(value) -> process(value),
    Err(e)    -> eprintln("task failed: ${e}"),
}
```

**Multiple parallel tasks:**
```
var handles []Handle<Result<Batch, Error>> = [];
for batch in batches {
    handles.push(spawn {
        return processBatch(batch);
    });
}

for h in handles {
    var result = h.join();
}
```

Fire-and-forget - just don't bind the handle, for tasks that don't need to be tracked:
```
spawn {
    ch.send(42);
};
```

---

## 6. Operators

Standard C-style operators. `++` and `--` are **statements only** - can't use inside expressions. This is to prevent
things like `arg[i++] = arg[++j]`

**Arithmetic:**

| Operator | Description |
|----------|-------------|
| `+` | Addition |
| `-` | Subtraction |
| `*` | Multiplication |
| `/` | Division |
| `%` | Modulo |

**Comparison:**

| Operator | Description |
|----------|-------------|
| `==` | Equal |
| `!=` | Not equal |
| `<` | Less than |
| `>` | Greater than |
| `<=` | Less than or equal |
| `>=` | Greater than or equal |

**Logical:**

| Operator | Description |
|----------|-------------|
| `&&` | Logical AND |
| `\|\|` | Logical OR |
| `!` | Logical NOT |

**Bitwise:**

| Operator | Description |
|----------|-------------|
| `&` | Bitwise AND |
| `\|` | Bitwise OR |
| `^` | Bitwise XOR |
| `~` | Bitwise NOT |
| `<<` | Left shift |
| `>>` | Right shift |

**Compound Assignment:**

| Operator | Description |
|----------|-------------|
| `+=` | Add and assign |
| `-=` | Subtract and assign |
| `*=` | Multiply and assign |
| `/=` | Divide and assign |
| `%=` | Modulo and assign |
| `&=` | Bitwise AND and assign |
| `\|=` | Bitwise OR and assign |
| `^=` | Bitwise XOR and assign |
| `<<=` | Left shift and assign |
| `>>=` | Right shift and assign |

**Increment / Decrement (statement only):**
```
i++;    // ok
i--;    // ok
// var x = i++;  ERROR: ++ is a statement, not an expression
```

---

## 7. Comments

Single-line comments use `//`. Multi-line comments use `/* */`. Multi-line comments can be nested.

```
// this is a single-line comment

/* this is a
   multi-line comment */

/* outer /* nested */ still in outer */
```

---

## 8. Modules & File Structure

Mirage uses an explicit module system (stealing from Rust) where one file = one module. 
Modules are declared with `mod` and symbols are brought into scope with `use`.

### 8.1 Declaring Modules

```
mod math;              // declaring that module "math" exists
mod network;
```

### 8.2 Importing Symbols

```
use math::add;         // bring a specific symbol into scope
use math::*;           // wildcard - bring all public symbols
use network::layers::Dense;   // nested module path
```

### 8.3 Visibility

All symbols are **private to their module** by default. Use `pub` to export.

```
// in math.mrg
pub func add(a int, b int) -> int {
    return a + b;
}

func helper() -> int {    // private - only visible within this module
    return 42;
}

pub struct Vec2 {
    x float,
    y float,
}
```

### 8.4 File Structure

One file = one module. The file name determines the module name. Nested directories map to nested modules.

```
project/
├── main.mrg              // entry point
├── math.mrg              // mod math
└── network/
    ├── mod.mrg            // mod network
    └── layers.mrg         // mod network::layers
```

---

## Appendix A: Decision Log

| #     | Decision                                                                                                        |
|-------|-----------------------------------------------------------------------------------------------------------------|
| D-001 | Variables: mutable with `var`, immutable with `const`                                                           |
| D-002 | Declaration: name before type → `var x int = 5`                                                                 |
| D-003 | Type annotation optional when initialiser present; required when not                                            |
| D-004 | Untyped constants: `const x = 5` adapts to usage context (like Go)                                              |
| D-005 | Unused variable policy: declared+never-assigned AND assigned+never-read are both compile errors                 |
| D-006 | Function return type uses arrow syntax: `func add(a int, b int) -> int`                                         |
| D-007 | Void functions omit the arrow entirely: `func doStuff(x int) { ... }`                                           |
| D-008 | Multiple returns via unnamed tuples `-> (int, int)`, or use a struct for reusable shapes                        |
| D-009 | Entry point: `func main(args []string) -> int` - returns exit code, takes args                                  |
| D-010 | If/else: no parens on condition, braces always required                                                         |
| D-011 | `for...in` iterator loop with `range(start, end, step)`; `while` for condition loops; braces required           |
| D-012 | `break` and `continue` supported; labeled breaks/continues for nested loops (`outer: for ...`)                  |
| D-013 | `match` expression (Rust-style) with `->` arms and `_` wildcard; no switch/case                                 |
| D-014 | Comments: `//` single-line and `/* */` multi-line (nestable)                                                    |
| D-015 | Structs: comma-separated fields, instantiation with named fields `Vec2 { x: 0.0, y: 0.0 }`                      |
| D-016 | Ownership model: single owner, move semantics, deterministic deallocation, no garbage collector                 |
| D-017 | Borrowing: `&T` immutable, `&mut T` mutable; raw pointers `*T` requires `unsafe` blocks                         |
| D-018 | Concurrency via channels: `channel<T>()`, `.send()`, `.recv()`; sending moves ownership                         |
| D-019 | C-style operators; `++`/`--` are statements only, not expressions                                               |
| D-020 | Rust-style modules: `mod` to declare, `use` to import, `pub` for visibility, one file = one module              |
| D-021 | Primitive types: int8-64, uint8-64, int, float16/32/64, bfloat16, float, complex64/128, bool, string, byte      |
| D-022 | Fixed arrays `[N]T` (stack) and dynamic slices `[]T` (heap) with `.push()`, `.len`                              |
| D-023 | Indexing: `[]` panics on OOB, `.get()` returns `Option<T>`, compile-time error for constant indices             |
| D-024 | Range slicing `[2..5]` exclusive, `[2..=5]` inclusive; zero-copy views by default, `.clone()` to copy           |
| D-025 | Strings: C-style escapes, `${}` interpolation, `"""` multiline with whitespace stripping, `fmt()` in stdlib     |
| D-026 | `print()`, `println()`, `eprint()`, `eprintln()` as builtins; full IO via stdlib post-bootstrap                 |
| D-027 | Error handling via `Result<T, E>` with `Ok`/`Err`; `?` operator for propagation; no exceptions                  |
| D-028 | Enums with associated data (algebraic types); `Result` and `Option` are regular enums in stdlib                 |
| D-029 | `::` for namespace/type access (modules, variants, statics); `.` for instance access (fields, methods)          |
| D-030 | Generics: `<T>`, trait bounds `<T: Bound>`, `+` for multiple bounds, type inference when unambiguous            |
| D-031 | `spawn` returns a handle with `.join()` (returns Result), `.is_done()`, `.cancel()`; fire-and-forget if unbound |
| D-032 | No null - `Option<T>` with `Some(T)`/`None` is the only way to represent absence                                |
