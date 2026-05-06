# IDE Compilador - Trabalho de Compiladores

## Como Rodar

### Passo 1: Configurar o PATH (PowerShell)
```powershell
$env:PATH = "C:\Qt\6.11.0\mingw_64\bin;C:\Qt\Tools\mingw1310_64\bin;" + $env:PATH
```

### Passo 2: Gerar Makefile
```bash
qmake IDE_Compilador.pro
```

### Passo 3: Compilar
```bash
mingw32-make
```

### Passo 4: Executar
```bash
.\build\bin\IDE_Compilador.exe
```