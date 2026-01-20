# MirageC Language Specification


---

## 1. Variables
Borrowing heavily from Go programming langauge lol

### 1.1 Mutable Variables

Mutable variables are declared with the `var` keyword. The type annotation is optional when the type can be inferred from the initialiser. When no initialiser is present, the type annotation is required.

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

Constants are declared with the `const` keyword. Constants are **untyped** when inferred. They adapt to the type context in which they are used (Go-style). A type annotation pins them to a specific type.

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
var d uint8 = 257; // compile error since 256 doesn't fit in uint8 (0-255) 


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


---

## 3. Types
  

---

## 4. Control Flow
  

---

## 5. Memory & Pointers
  

---

## 6. Operators
  

---

## 7. Comments
  

---

## 8. Modules & File Structure
  

---

## Appendix A: Decision Log

| #     | Decision                                                                                        |
|-------|-------------------------------------------------------------------------------------------------|
| D-001 | Variables: mutable with `var`, immutable with `const`                                           |
| D-002 | Declaration: name before type → `var x int = 5`                                                 |
| D-003 | Type annotation optional when initialiser present; required when not                            |
| D-004 | Untyped constants: `const x = 5` adapts to usage context (like Go)                              |
| D-005 | Unused variable policy: declared+never-assigned AND assigned+never-read are both compile errors |
