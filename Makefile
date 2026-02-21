# Compilador
CC = gcc

# Flags do compilador
CFLAGS = -Wall -Wextra -O2 -g

# Ficheiros fonte
SRC_MAIN = main.c jogo.c
SRC_TEST = testes.c jogo.c

# Executáveis
BIN_MAIN = jogo
BIN_TEST = testes

# Regras principais
all: $(BIN_MAIN) $(BIN_TEST)

# Compilar o executável principal
$(BIN_MAIN): $(SRC_MAIN)
	$(CC) $(CFLAGS) -o $@ $^

# Compilar os testes
$(BIN_TEST): $(SRC_TEST)
	$(CC) $(CFLAGS) -o $@ $^

# Executar o programa
run: $(BIN_MAIN)
	./$(BIN_MAIN)

# Executar os testes
runtest: $(BIN_TEST)
	./$(BIN_TEST)

# Limpar os binários
clean:
	rm -f $(BIN_MAIN) $(BIN_TEST)
	
.PHONY: all clean run runtest
