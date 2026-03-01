# Mirage

A compiler for the Mirage programming language, written in C++. Will boot strap eventually :P

## Dependencies

- CMake (3.10+)
- C++17 compiler** (g++ or clang++)
- NASM*
- Clang

### Install on Ubuntu/Debian

```bash
sudo apt install cmake g++ nasm clang
```

## Build

```bash
cmake -B build -S .
cmake --build build
```

## Run

```bash
./build/MirageC <input.mir>
```

## Example

```
// hello.mir
func main(args []string) -> int {
    println("hello world");
    return 0;
}
```

```bash
./build/MirageC hello.mir && ./program
```
