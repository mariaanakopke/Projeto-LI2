# Projeto de Jogo — LI2 

Jogo de tabuleiro implementado em C com sistema de histórico, carregamento de ficheiros e funcionalidades interativas.

Este projeto implementa um jogo onde o jogador manipula um tabuleiro com diferentes estados de casas (original, branco, riscado). O jogo suporta carregamento de ficheiros, histórico de jogadas com undo/redo e salvamento de estados.

## Estrutura do Projeto

- `main.c` — interface principal do jogo e menu interativo
- `jogo.c` — lógica principal do jogo e manipulação do tabuleiro
- `jogo.h` — definições de estruturas e protótipos de funções
- `testes.c` — testes unitários do jogo
- `Makefile` — automatização de compilação e execução
- `j4.txt` — ficheiro de exemplo para carregar jogo


## Como compilar e executar

### Compilação com Makefile (recomendado)

```bash
# Compilar tudo (jogo principal + testes)
make all

# Compilar apenas o jogo principal
make jogo

# Compilar apenas os testes
make testes
```

### Execução

```bash
# Executar o jogo principal
make run
# ou
./jogo

# Executar os testes
make runtest
# ou
./testes
```

### Limpeza

```bash
# Remover executáveis
make clean
```

## Como jogar

1. Execute o programa: `./jogo`
2. Escolha "Iniciar novo jogo" no menu
3. Digite o nome do ficheiro (ex: `j4.txt`)
4. Use os comandos do menu para interagir com o tabuleiro
5. O jogo suporta undo/redo e salvamento automático

## Estrutura do Código

- **Tabuleiro**: matriz dinâmica de casas com estados (original/branco/riscado)
- **Histórico**: lista ligada para undo/redo de jogadas
- **Jogo**: estrutura principal que contém tabuleiro e histórico

## Debugging

Se encontrar problemas:

1. Compile com flags de debug: `gcc -Wall -Wextra -g -o jogo main.c jogo.c`
2. Use `gdb` para debugging: `gdb ./jogo`
3. Execute os testes para verificar funcionalidades: `./testes`
4. Verifique se o ficheiro de jogo (ex: `j4.txt`) existe e tem formato válido


---


