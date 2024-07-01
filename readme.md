> hellow, word
```byte
'01' '03' '01' '02' '00' '0d' '00' '00' '00' 'h' 'e' 'l' 'l' 'o' 'w' ',' '20' 'w' 'o' 'r' 'd' '0d' '0a' '01' '00'
```

## '.co' file instructions 

### TEXT (2)
`[TEXT][size][text(size)]`
```c
* signature *
(1)     - stdout (only TEXT)
(2)     - stdin (only TEXT)
(3-255) - template vars
```

### WRITE (3)

`[WRITE][signature][TEXT|INT|FLOAT]`
```c
* signature *
(1)     - stdout (only TEXT)
(2)     - stdin (only TEXT)
(3-255) - template vars
```
### INPUT (4)

`[INPUT][signature]`
```c
* signature
  (3-255) - template vars
```

### VAR (5)

`[VAR][signature][size]`
```c
* signature (3-255)
* size (4U)
```

### WRITEex (6)

`[WRITEex][signature(4U)][TEXT|INT|FLOAT]`
```c
* signature *
(1)          - stdout
(2)          - stdin
(UINT32_MAX) - template vars
```

### VARex (7)
`[VAR][signature(4U)][size]`
```c
* signature (UINT32_MAX)
* size (4U)
```


### INT (8)
`[INT][size][value(size)]`
```c
* size (4U)
* value (size)
```

### FLOAT (9)

`[FLAOT][size][value(size)]`
```c
* size (4U)
* value (size)
```

### FRAC (10)

`[FRAC][TOP][BOT]`
```c
* TOP (4U)
* BOT (4U)
```

### ADD (11)

`[ADD][FSIGN][SSIGN]`
```c
* FSIGN (3-255) 
* SSIGN (3-255) 
```
```c
FSIGN = FSIGN + SSING
```

### SUB (12)

`[SUB][FSIGN][SSIGN]`
```c
* FSIGN (3-255) 
* SSIGN (3-255) 
```
```c
FSIGN = FSIGN - SSING
```

### DIV (13)

`[DIV][FSIGN][SSIGN]`
```c
* FSIGN (3-255) 
* SSIGN (3-255)
```
```c
FSIGN = FSIGN / SSING 
```

### MUL (14)

`[MUL][FSIGN][SSIGN]`
```c
* FSIGN (3-255) 
* SSIGN (3-255) 
```
```c
FSIGN = FSIGN * SSING
```

### MOD (15)

`[MUL][FSIGN][SSIGN]`
```c
* FSIGN (3-255) 
* SSIGN (3-255) 
```
```c
FSIGN = FSIGN % SSING 
```

### MOV (16)

`[MOV][FSIGN][SSIGN]`
```c
* FSIGN (3-255) 
* SSIGN (3-255) 
```
```c
FSIGN -> SSING 
```

### MOVL (17)

`[MOVL][FSIGN][SSIGN]`
```c
* FSIGN (UINT32_MAX) 
* SSIGN (3-255) 
```
```c
FSIGN -> SSING
```

### LMOV (18)

`[LMOV][FSIGN][SSIGN]`
```c
* FSIGN (3-255) 
* SSIGN (UINT32_MAX) 
```
```c
FSIGN -> SSING
```


### LMOVL (19)

`[LMOVL][FSIGN][SSIGN]`
```c
* FSIGN (UINT32_MAX) 
* SSIGN (UINT32_MAX) 
```
```c
FSIGN -> SSING 
```