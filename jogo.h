#ifndef JOGO_H
#define JOGO_H

#include <stdio.h>

#define MAX 100

// --------------------------- Estado de cada casa ---------------------------
typedef enum {
    ORIGINAL, // letra minúscula
    BRANCO,   // letra maiúscula
    RISCADO   // '#'
} EstadoCasa;

// --------------------------- Estrutura de uma casa ---------------------------
typedef struct {
    char letra;        // Letra original da casa
    EstadoCasa estado; // Estado atual da casa
} Casa;

// --------------------------- Estrutura do tabuleiro ---------------------------
typedef struct {
    Casa **casas;      // Matriz dinâmica de casas
    int linhas;
    int colunas;
} Tabuleiro;

// --------------------------- Lista ligada para histórico ---------------------------
typedef struct NodoHistorico {
    Tabuleiro *estado;             // Cópia do estado do tabuleiro
    struct NodoHistorico *anterior; // Ponteiro para o estado anterior
} NodoHistorico;

// --------------------------- Estado global do jogo ---------------------------

typedef struct {
    int linha;
    int coluna;
    int valida;
} Jogada;

typedef struct {
    Tabuleiro *atual;
    Tabuleiro *inicial;
    Tabuleiro *solucao;
    NodoHistorico *historico;
    Jogada ultimaJogada;
} Jogo;

// --------------------------- Verificação de restrições ---------------------------
typedef struct {
    int regra1Violada;  // Símbolo branco duplicado em linha/coluna
    int regra2Violada;  // Símbolo não riscado após um branco
    int regra3Violada;  // Vizinhos de casa riscada não brancos
    int regra4Violada;  // Sem caminho entre casas brancas
} Violacoes;

// --------------------------- Funções principais ---------------------------

/* Validação */
int validarLinha(const char *linha, int colunas);
int coordenadaValida(const Tabuleiro *tab, char col, int lin);

/* Tabuleiro */
Tabuleiro *criarTabuleiro(int linhas, int colunas);
void libertarTabuleiro(Tabuleiro *tab);
Tabuleiro *copiarTabuleiro(const Tabuleiro *origem);
void imprimirTabuleiro(const Tabuleiro *tab);

/* Verificação de restrições */
Violacoes verificarRestricoes(const Tabuleiro *tab);
int existeCaminhoBranco(const Tabuleiro *tab, int mostrarErros);
int verificaBrancoOriginal(const Tabuleiro *);
int existeOriginalNaLinha(const Tabuleiro *tab, int linha, int coluna, char letra);
int existeOriginalNaColuna(const Tabuleiro *tab, int linha, int coluna, char letra);
int verificaDuplicados(const Tabuleiro *);
int verificarDuplicadosNaLinha(const Tabuleiro *tabuleiro);
int verificarDuplicadosNaColuna(const Tabuleiro *tabuleiro);
int verificaVizinhosRiscado(const Tabuleiro *);
void contarBrancasEPrimeira(const Tabuleiro *, int *, int *, int *);
int **alocarVisitado(int, int);
void libertarVisitado(int **, int);
int contarBrancasLigadas(const Tabuleiro *, int **, int, int);
void imprimirViolacoes(const Violacoes v);
int verificarPuzzleCompleto(const Tabuleiro *tab);

/* Manipulação de casas */
int pintarDeBranco(Jogo *jogo, Tabuleiro *tab, char col, int lin);
int riscarCasa(Jogo *jogo, Tabuleiro *tab, char col, int lin);


/* Inferência automática */
int aplicaInferencias(Jogo *jogo, Tabuleiro *tab);
int inferirRiscadosPorBrancos(Jogo *jogo, Tabuleiro *tab);
int inferirBrancosPorVizinhos(Jogo *jogo, Tabuleiro *tab);

/* Resolver jogo*/
int resolverJogo(Jogo *jogo, Tabuleiro *tab);

/* Histórico */
void salvarEstado(NodoHistorico **historico, Tabuleiro *estadoAtual);
int desfazerJogada(Jogo *jogo);
void libertarHistorico(NodoHistorico *histo);

/* Jogo */
Jogo *criarJogo(int linhas, int colunas);
Jogo *carregarJogo(const char *nomeArquivo);
void salvarJogo(const Jogo *jogo, const char *nomeArquivo);
void libertarJogo(Jogo *jogo); 
void restaurarTabuleiroOriginal(Jogo *jogo);
void comandoPintar();
void verificarFimDeJogo();
void comandoRiscar();
void comandoVerificar();
void comandoDesfazer();
void comandoCarregar();
void comandoSalvar();
void comandoAjuda(void);
void comandoInferirTudo(void);
void comandoResolver(void);

#endif

