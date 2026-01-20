# MirageC — Sample Program
*idk what i'm doing so i'm going to decide on syntax before implementing any more*

---

This file shows sample code using the Mirage syntax, sections are annotated with decisions

---

## Variables
(heavily taking inspiration from Go variable syntax and behavior)
```mirage
// Typed mutable variable
var score int = 0;

// Type-inferred mutable variable
var name = "Alice";

// Declare now, assign later (type required)
var result int;
result = 42;

// Typed constant
const MAX_PLAYERS int = 8;

// Untyped constant- should adapt to the context
const GRAVITY = 9.81;   // used as float32 or float64 wherever it appears
const LIMIT   = 256;   // used as int16, int32, int64, uint32 wherever it appears

// example:
var a int32 = LIMIT; // LIMIT will become int32 here
var b int64 = LIMIT; // LIMIT will become int64 here
var c uint32 = LIMIT; // LIMIT will become uint32 here
var d uint8 = LIMIT; // compile error since 257 doesn't fit in uint8 (0-255) 


// These would be compile errors:
// var unused int = 5;          ERROR: assigned but never read
// var x int;  (never assigned) ERROR: declared but never assigned before use
// MAX_PLAYERS = 16;            ERROR: cannot assign to constant
```

---

## Functions

```mirage

```

---

## Control Flow  

```mirage

```

---

## Structs  

```mirage

```
