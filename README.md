given the file:
```test.mir
return 67;
```
lexer needs to get tokens along the lines of:
```
RETURN
INT_LIT -> 67
SEMI
```

then into assembly: 
```
global start
start:
mov rax, 60
mov rd, 67
syscall
```