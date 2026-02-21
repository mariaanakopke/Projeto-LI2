#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "jogo.h"

// Estado global do jogo
Jogo *jogo = NULL;

void iniciarJogo() {
    char nomeArquivo[MAX];
    
    if (jogo) {
        libertarJogo(jogo);
    }

    printf("Digite o nome do arquivo do jogo: ");
    scanf("%s", nomeArquivo);
    
    jogo = carregarJogo(nomeArquivo);
    if (jogo) {
        printf("Jogo carregado com sucesso!\n");
        imprimirTabuleiro(jogo->atual);
    } else {
        printf("Não foi possível carregar o jogo '%s'.\n", nomeArquivo);
    }
}

void processarComando(char tipo) {
    switch (tipo) {
        case 's': comandoSair(); return;
        case 'i': iniciarJogo(); return;
        case 'b': comandoPintar(); return;
        case 'r': comandoRiscar(); return;
        case 'v': comandoVerificar(); return;
        case 'd': comandoDesfazer(); return;
        case 'l': comandoCarregar(); return;
        case 'g': comandoSalvar(); return;
        case 'a': comandoAjuda(); return;
        case 'A': comandoInferirTudo(); return;
        case 'R': comandoResolver(); return;
        default:
            printf("Comando inválido.\n");
            while (getchar() != '\n');
            return;
        }
    }
    void comandoSair() {
        if (jogo) {
            libertarJogo(jogo);
            jogo = NULL;
        }
        printf("Jogo encerrado.\n");
    }
    
    void verificarFimDeJogo() {
        if (verificarPuzzleCompleto(jogo->atual)) {
            printf("\nParabéns!Completaste o puzzle!\n");
            imprimirTabuleiro(jogo->atual);
        }
    }
    
    void comandoPintar() {
        char col;
        int lin;
        if (scanf(" %c%d", &col, &lin) == 2) {
            salvarEstado(&jogo->historico, jogo->atual);
            if (pintarDeBranco(jogo, jogo->atual, col, lin)) {
                jogo->ultimaJogada.linha = lin - 1;
                jogo->ultimaJogada.coluna = col - 'a';
                jogo->ultimaJogada.valida = 1;
            }
            imprimirTabuleiro(jogo->atual);
            verificarFimDeJogo();
        } else {
            printf("Comando inválido.\n");
            while (getchar() != '\n');
        }
    }
    
    void comandoRiscar() {
        char col;
        int lin;
        if (scanf(" %c%d", &col, &lin) == 2) {
            salvarEstado(&jogo->historico, jogo->atual);
            if (riscarCasa(jogo, jogo->atual, col, lin)) {
                jogo->ultimaJogada.linha = lin - 1;
                jogo->ultimaJogada.coluna = col - 'a';
                jogo->ultimaJogada.valida = 1;
            }
            imprimirTabuleiro(jogo->atual);
            verificarFimDeJogo();
        } else {
            printf("Comando inválido.\n");
            while (getchar() != '\n');
        }
    }
    void comandoVerificar() {
        if (!jogo || !jogo->atual) {
            printf("Não há jogo em andamento.\n");
            return;
        }
        Violacoes v = verificarRestricoes(jogo->atual);
        imprimirViolacoes(v);
    }
    
    void comandoDesfazer() {
        if (desfazerJogada(jogo)) {
            printf("Jogada desfeita!\n");
            imprimirTabuleiro(jogo->atual);
        } else {
            printf("Não há jogadas para desfazer.\n");
        }
    }
    
    void comandoCarregar() {
        char nomeArquivo[MAX];
        scanf("%s", nomeArquivo);
    
        if (jogo) {
            libertarJogo(jogo);
        }
    
        jogo = carregarJogo(nomeArquivo);
        if (jogo) {
            printf("Jogo carregado com sucesso!\n");
            imprimirTabuleiro(jogo->atual);
        }
    }
    
    void comandoSalvar() {
        char nomeArquivo[MAX];
    
        if (!jogo || !jogo->atual) {
            printf("Não há jogo em andamento para salvar.\n");
            return;
        }
    
        scanf("%s", nomeArquivo);
        salvarJogo(jogo, nomeArquivo);
    }
    
    void comandoAjuda() {
            if (!jogo || !jogo->atual) {
                printf("Não há jogo em andamento.\n");
                return;
            }
        
            salvarEstado(&jogo->historico, jogo->atual);
        
            int mudou = aplicarUmaInferenciaParaViolacoes(jogo, jogo->atual);
        
            imprimirTabuleiro(jogo->atual);
        
            if (!mudou) {
                printf("Não foi possível fazer mais inferências.\n");
            }
        }
        
    
    
        void comandoInferirTudo() {
            if (!jogo || !jogo->atual) {
                printf("Não há jogo em andamento.\n");
                return;
            }
        
            int mudancas = aplicaInferencias(jogo, jogo->atual);
        
            if (mudancas > 0) {
                salvarEstado(&jogo->historico, jogo->atual);
                imprimirTabuleiro(jogo->atual);
            } else {
                printf("O tabuleiro está estável, nenhuma inferência possível.\n");
            }
        }
        
    
        void comandoResolver() {
            if (!jogo || !jogo->atual) {
                printf("Não há jogo em andamento.\n");
                return;
            }
        
            restaurarTabuleiroOriginal(jogo);
            salvarEstado(&jogo->historico, jogo->atual);
            Tabuleiro *anterior = copiarTabuleiro(jogo->atual);
        
            if (resolverJogo(jogo, jogo->atual)) {
                imprimirTabuleiro(jogo->atual);
            } else {
                // Restaurar tabuleiro anterior se não resolver
                for (int i = 0; i < anterior->linhas; i++) {
                    for (int j = 0; j < anterior->colunas; j++) {
                        jogo->atual->casas[i][j] = anterior->casas[i][j];
                    }
                }
                printf("Não foi possível resolver o jogo.\n");
                imprimirTabuleiro(jogo->atual);
            }
        
            libertarTabuleiro(anterior);
        }        
    
    
    
    void loop() {
        char tipo;
        int firstRun = 1;  // Flag para ver a first run
        int gameStarted = 0;  // Flag para ver se o jogo já começou

        while (1) {
            if (firstRun) {
                printf("\nBem vindo, clique em i para iniciar o jogo> ");
                firstRun = 0;
            } else if (gameStarted) {
                printf("\nInsira o comando> ");
            } else {
                printf("\n> ");
            }
    
            scanf(" %c", &tipo);
    
            if (tipo == 's') {
                comandoSair();
                return;
            }
    
            if (tipo == 'i') {
                gameStarted = 1;
            }
    
            processarComando(tipo);
        }
    }
    
    int main() {
        loop();
        if (jogo) {
            libertarJogo(jogo);
        }
        return 0;
    }
    
                    