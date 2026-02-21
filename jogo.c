#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "jogo.h"

// --------------------------- Validação ---------------------------

int validarLinha(const char *linha, int colunas) {
    return (int)strlen(linha) == colunas;
}

int coordenadaValida(const Tabuleiro *tab, char col, int lin) {
    return col >= 'a' && col < 'a' + tab->colunas && lin >= 1 && lin <= tab->linhas;
}

// --------------------------- Tabuleiro ---------------------------

Tabuleiro *criarTabuleiro(int linhas, int colunas) {
    Tabuleiro *tab = malloc(sizeof(Tabuleiro));
    tab->linhas = linhas;
    tab->colunas = colunas;
    tab->casas = malloc(linhas * sizeof(Casa *));

    for (int i = 0; i < linhas; i++) {
        tab->casas[i] = malloc(colunas * sizeof(Casa));
        for (int j = 0; j < colunas; j++) {
            tab->casas[i][j].letra = ' ';
            tab->casas[i][j].estado = ORIGINAL;
        }
    }
    return tab;
}

Tabuleiro *copiarTabuleiro(const Tabuleiro *origem) {
    Tabuleiro *destino = malloc(sizeof(Tabuleiro));
    destino->linhas = origem->linhas;
    destino->colunas = origem->colunas;

    // Aloca a matriz bidimensional de casas
    destino->casas = malloc(destino->linhas * sizeof(Casa *));
    for (int i = 0; i < destino->linhas; i++) {
        destino->casas[i] = malloc(destino->colunas * sizeof(Casa));
       //Copia a linha inteira usando memcpy
        memcpy(destino->casas[i], origem->casas[i], destino->colunas * sizeof(Casa));
    }
    // Devolve o novo tabuleiro copiado
    return destino;
}

void imprimirTabuleiro(const Tabuleiro *tab) {
    printf("  ");
    for (int j = 0; j < tab->colunas; j++) {
        printf("%c ", 'a' + j);
    }
    printf("\n");

    for (int i = 0; i < tab->linhas; i++) {
        printf("%d ", i + 1);
        for (int j = 0; j < tab->colunas; j++) {
            Casa casa = tab->casas[i][j];
            if (casa.estado == RISCADO)
                printf("# ");
            else if (casa.estado == BRANCO)
                printf("%c ", toupper(casa.letra));
            else
                printf("%c ", casa.letra);
        }
        printf("\n");
    }
}

void libertarTabuleiro(Tabuleiro *tab) {
    if (!tab) return;
    // Liberta cada linha individualmente
    for (int i = 0; i < tab->linhas; i++) {
        free(tab->casas[i]);
    }
    free(tab->casas);
    // Liberta a estrutura Tabuleiro
    free(tab);
}

void restaurarTabuleiroOriginal(Jogo *jogo) {
    // Liberta o tabuleiro atual
    libertarTabuleiro(jogo->atual);

    // Cria uma nova cópia do tabuleiro inicial
    jogo->atual = copiarTabuleiro(jogo->inicial);
}

// --------------------------- Verificação de restrições-----------------------

Violacoes verificarRestricoes(const Tabuleiro *tabuleiro) {
    Violacoes violacoes = {0, 0, 0, 0};

    if (verificaDuplicados(tabuleiro))
    violacoes.regra1Violada = 1;

    if (verificaBrancoOriginal(tabuleiro))
        violacoes.regra2Violada = 1;

    if (verificaVizinhosRiscado(tabuleiro))
        violacoes.regra3Violada = 1;

    if (!existeCaminhoBranco(tabuleiro,1))
        violacoes.regra4Violada = 1;

    return violacoes;
}


// Verifica se todas as casas brancas do tabuleiro estão ligadas ortogonalmente
int existeCaminhoBranco(const Tabuleiro *tab, int mostrarErros) {
    int total, iInicio, jInicio;

    // Conta quantas casas brancas existem e onde está a primeira
    contarBrancasEPrimeira(tab, &total, &iInicio, &jInicio);

    if (total <= 1)
        return 1; // Se só existir uma ou nenhuma casa branca, estão ligadas por definição
 // Aloca matriz auxiliar para marcar as casas visitadas
 int **visitado = alocarVisitado(tab->linhas, tab->colunas);

 // Conta quantas casas brancas conseguimos visitar a partir da primeira
 contarBrancasLigadas(tab, visitado, iInicio, jInicio);

 int houveIsoladas = 0;

 // Verifica se há casas brancas não visitadas (não ligadas)
 for (int i = 0; i < tab->linhas; i++) {
     for (int j = 0; j < tab->colunas; j++) {
         if (tab->casas[i][j].estado == BRANCO && !visitado[i][j]) {
           if (mostrarErros) {
             printf("Regra 4 violada: Casa %c%d está branca mas não está ligada às restantes.\n",
                    'a' + j, i + 1);
           }
             houveIsoladas = 1;
         }
     }
 }

 // Liberta a memória usada
 libertarVisitado(visitado, tab->linhas);

 return !houveIsoladas; // Retorna 0 se houve alguma casa isolada
}


void contarBrancasEPrimeira(const Tabuleiro *tab, int *total, int *iInicio, int *jInicio) {
 *total = 0;
 *iInicio = -1;
 *jInicio = -1;

 for (int i = 0; i < tab->linhas; i++) {
     for (int j = 0; j < tab->colunas; j++) {
         // Se a casa for branca, aumenta o total e guarda a primeira que encontra
         if (tab->casas[i][j].estado == BRANCO) {
             (*total)++;
             if (*iInicio == -1) {
                 *iInicio = i;
                 *jInicio = j;
                }
            }
        }
    }
}

int **alocarVisitado(int linhas, int colunas) {
    int **visitado = malloc(linhas * sizeof(int *));
    for (int i = 0; i < linhas; i++)
        visitado[i] = calloc(colunas, sizeof(int));
    return visitado;
}

void libertarVisitado(int **visitado, int linhas) {
    for (int i = 0; i < linhas; i++)
        free(visitado[i]);
    free(visitado);
}

int contarBrancasLigadas(const Tabuleiro *tab, int **visitado, int iInicio, int jInicio) {
    typedef struct { int linha, coluna; } Posicao;
    Posicao *fila = malloc(tab->linhas * tab->colunas * sizeof(Posicao));
    int inicio = 0, fim = 0, ligadas = 1;

    fila[fim++] = (Posicao){iInicio, jInicio};
    visitado[iInicio][jInicio] = 1;

    int dx[] = {-1, 0, 1, 0};
    int dy[] = {0, 1, 0, -1};

    while (inicio < fim) {
        Posicao atual = fila[inicio++];
        for (int d = 0; d < 4; d++) {
            int ni = atual.linha + dx[d];
            int nj = atual.coluna + dy[d];

            // Verifica limites, se é branca e se ainda não foi visitada
            if (
                ni >= 0 && ni < tab->linhas &&
                nj >= 0 && nj < tab->colunas &&
                tab->casas[ni][nj].estado == BRANCO &&
                !visitado[ni][nj]
            ) {
                visitado[ni][nj] = 1;
                fila[fim++] = (Posicao){ni, nj};
                ligadas++;
            }
        }
    }

    free(fila);
    return ligadas;
}

// Função que verifica se existem letras brancas repetidas na mesma linha ou coluna do tabuleiro.
int verificaDuplicados(const Tabuleiro *tabuleiro) {
    // Verifica violações na regra 1 (letras duplicadas) em linhas e colunas
    int violacaoLinha = verificarDuplicadosNaLinha(tabuleiro);
    int violacaoColuna = verificarDuplicadosNaColuna(tabuleiro);

    // Retorna verdadeiro se houve violação em qualquer direção
    return violacaoLinha || violacaoColuna;
}

int verificarDuplicadosNaLinha(const Tabuleiro *tabuleiro) {
    int houveViolacao = 0;

    for (int i = 0; i < tabuleiro->linhas; i++) {
        // Array para guardar posições de cada letra na linha
        int colunasComLetra[256][tabuleiro->colunas];
        // Contador de ocorrências de cada letra
        int contador[256] = {0};

        // Percorre todas as colunas da linha atual
        for (int j = 0; j < tabuleiro->colunas; j++) {
            Casa c = tabuleiro->casas[i][j];
            if (c.estado == BRANCO) {
                // Converte a letra para índice no array
                unsigned char letra = (unsigned char)c.letra;
                // Guarda a posição da letra e incrementa seu contador
                colunasComLetra[letra][contador[letra]++] = j;
            }
        }

        // Verifica se alguma letra aparece mais de uma vez
        for (int l = 0; l < 256; l++) {
            if (contador[l] > 1) {
  // Para cada par de casas com a mesma letra
  for (int x = 0; x < contador[l]; x++) {
    for (int y = x + 1; y < contador[l]; y++) {
        // Mostra detalhes da violação
        printf("Regra 1 violada (linha %d): Casa %c%d e %c%d têm letra '%c' em branco\n",
               i + 1,
               'a' + colunasComLetra[l][x], i + 1,
               'a' + colunasComLetra[l][y], i + 1,
               l);
        houveViolacao = 1;
    }
}
}
}
}

return houveViolacao;
}

int verificarDuplicadosNaColuna(const Tabuleiro *tabuleiro) {
int houveViolacao = 0;

for (int j = 0; j < tabuleiro->colunas; j++) {
// Array para guardar posições de cada letra na coluna
int linhasComLetra[256][tabuleiro->linhas];
// Contador de ocorrências de cada letra
int contador[256] = {0};

// Percorre todas as linhas da coluna atual
for (int i = 0; i < tabuleiro->linhas; i++) {
Casa c = tabuleiro->casas[i][j];
if (c.estado == BRANCO) {
// Converte a letra para índice no array
unsigned char letra = (unsigned char)c.letra;
// Guarda a posição da letra e incrementa seu contador
linhasComLetra[letra][contador[letra]++] = i;
}
}

// Verifica se alguma letra aparece mais de uma vez
for (int l = 0; l < 256; l++) {
if (contador[l] > 1) {
for (int x = 0; x < contador[l]; x++) {
    for (int y = x + 1; y < contador[l]; y++) {
        printf("Regra 1 violada (coluna %c): Casa %c%d e %c%d têm letra '%c' em branco\n",
               'a' + j,
               'a' + j, linhasComLetra[l][x] + 1,
               'a' + j, linhasComLetra[l][y] + 1,
               l);
        houveViolacao = 1;
    }
}
}
}
}

return houveViolacao;
}


// Função que verifica se existe alguma casa branca com a mesma letra de uma casa original na mesma linha ou coluna
int verificaBrancoOriginal(const Tabuleiro *tabuleiro) {
int houveViolacao = 0;

for (int i = 0; i < tabuleiro->linhas; i++) {
for (int j = 0; j < tabuleiro->colunas; j++) {
if (tabuleiro->casas[i][j].estado == BRANCO) {
char letra = tabuleiro->casas[i][j].letra;

// Verifica se há letra igual em estado ORIGINAL na mesma linha ou coluna
if (existeOriginalNaLinha(tabuleiro, i, j, letra) ||
    existeOriginalNaColuna(tabuleiro, i, j, letra)) {
    houveViolacao = 1;
}
}
}
}
return houveViolacao;
}

int existeOriginalNaLinha(const Tabuleiro *tab, int linha, int coluna, char letra) {
for (int col = 0; col < tab->colunas; col++) {
if (col != coluna &&
tab->casas[linha][col].estado == ORIGINAL &&
tab->casas[linha][col].letra == letra) {
// Coordenadas: letra da coluna = 'a' + índice, linha = índice + 1
printf("Regra 2 violada: Casa %c%d é BRANCA e há letra '%c' ORIGINAL em %c%d (mesma linha)\n",
    'a' + coluna, linha + 1, letra,
                   'a' + col, linha + 1);
            return 1;
        }
    }
    return 0;
}

int existeOriginalNaColuna(const Tabuleiro *tab, int linha, int coluna, char letra) {
    for (int lin = 0; lin < tab->linhas; lin++) {
        if (lin != linha &&
            tab->casas[lin][coluna].estado == ORIGINAL &&
            tab->casas[lin][coluna].letra == letra) {
            printf("Regra 2 violada: Casa %c%d é BRANCA e há letra '%c' ORIGINAL em %c%d (mesma coluna)\n",
                   'a' + coluna, linha + 1, letra,
                   'a' + coluna, lin + 1);
            return 1;
        }
    }
    return 0;
}

// Função que verifica se alguma casa riscada tem vizinhos que não são brancos.
int verificaVizinhosRiscado(const Tabuleiro *tabuleiro) {
    int houveViolacao = 0;

    // Vetores para deslocar nas 4 direções: cima, direita, baixo, esquerda
    int dx[] = {-1, 0, 1, 0};
    int dy[] = {0, 1, 0, -1};

    // Percorre todas as linhas do tabuleiro
    for (int i = 0; i < tabuleiro->linhas; i++) {
        // Percorre todas as colunas do tabuleiro
        for (int j = 0; j < tabuleiro->colunas; j++) {
            // Verifica se a casa atual está riscada
            if (tabuleiro->casas[i][j].estado == RISCADO) {

                // Para cada casa riscada, verifica os seus 4 vizinhos
                for (int k = 0; k < 4; k++) {
                    // Calcula a linha do vizinho
                    int ni = i + dx[k];

                    // Calcula a coluna do vizinho
                    int nj = j + dy[k];
  // Verifica se o vizinho está dentro dos limites do tabuleiro
  if (ni >= 0 && ni < tabuleiro->linhas &&
    nj >= 0 && nj < tabuleiro->colunas) {

    // Se o vizinho não for BRANCO, a regra 3 está violada
    if (tabuleiro->casas[ni][nj].estado != BRANCO) {
        printf("Regra 3 violada: Casa %c%d é RISCADA mas vizinho %c%d não está branco.\n",
               'a' + j, i + 1, 'a' + nj, ni + 1);
        houveViolacao = 1;
    }
}
}
}
}
}

// Se nenhum vizinho inválido for encontrado, a regra não foi violada
return houveViolacao;
}

void imprimirViolacoes(const Violacoes v) {
if (!v.regra1Violada && !v.regra2Violada && !v.regra3Violada && !v.regra4Violada) {
printf("Não há violações de regras! O tabuleiro está válido.\n");
return;
}
printf("Violações encontradas:\n");
if (v.regra1Violada) printf("- Regra 1: Símbolos brancos duplicados em linhas ou colunas.\n");
if (v.regra2Violada) printf("- Regra 2: Símbolos não riscados após uma réplica branca.\n");
if (v.regra3Violada) printf("- Regra 3: Vizinhos de casas riscadas não são brancos.\n");
if (v.regra4Violada) printf("- Regra 4: Não há caminho entre todas as casas brancas.\n");
}

int verificarPuzzleCompleto(const Tabuleiro *tab) {
// Verifica se não há casas ORIGINAL
for (int i = 0; i < tab->linhas; i++) {
for (int j = 0; j < tab->colunas; j++) {
if (tab->casas[i][j].estado == ORIGINAL) {
return 0;
}
}
}

// Verifica se não há violações
Violacoes v = verificarRestricoes(tab);
if (v.regra1Violada || v.regra2Violada || 
    v.regra3Violada || v.regra4Violada) {
    return 0;
}

return 1;
}

// --------------------------- Inferências comando ´a´---------------------------
int aplicarInferenciaRegra1(Jogo *jogo, Tabuleiro *tab) {
    for (int i = 0; i < tab->linhas; i++) {
        if (corrigirDuplicadoBrancoLinha(jogo, tab, i)) return 1;
    }
    for (int j = 0; j < tab->colunas; j++) {
        if (corrigirDuplicadoBrancoColuna(jogo, tab, j)) return 1;
    }
    return 0;
}

int corrigirDuplicadoBrancoLinha(Jogo *jogo, Tabuleiro *tab, int linha) {
    int contador[256] = {0};
    for (int j = 0; j < tab->colunas; j++) {
        Casa c = tab->casas[linha][j];
        if (c.estado == BRANCO) {
            unsigned char letra = (unsigned char)c.letra;
            if (contador[letra] == 1) {
                // Segunda ocorrência: risca esta
                riscarCasa(jogo, tab, 'a' + j, linha + 1);
                return 1;
            }
            contador[letra]++;
        }
    }
    return 0;
}

int corrigirDuplicadoBrancoColuna(Jogo *jogo, Tabuleiro *tab, int coluna) {
    int contador[256] = {0};
    for (int i = 0; i < tab->linhas; i++) {
        Casa c = tab->casas[i][coluna];
        if (c.estado == BRANCO) {
            unsigned char letra = (unsigned char)c.letra;
            if (contador[letra] == 1) {
                // Segunda ocorrência: risca esta
                riscarCasa(jogo, tab, 'a' + coluna, i + 1);
                return 1;
            }
            contador[letra]++;
        }
    }
    return 0;
}


// Regra 2: branco -> riscar letras iguais na linha ou coluna
int aplicarInferenciaRegra2(Jogo *jogo, Tabuleiro *tab) {
    for (int i = 0; i < tab->linhas; i++) {
        for (int j = 0; j < tab->colunas; j++) {
            if (tab->casas[i][j].estado == BRANCO) {
                char letra = tab->casas[i][j].letra;

                // Linha
                for (int k = 0; k < tab->colunas; k++) {
                    if (k != j &&
                        tab->casas[i][k].estado == ORIGINAL &&
                        tab->casas[i][k].letra == letra) {
                        riscarCasa(jogo, tab, 'a' + k, i + 1);
                        return 1;
                    }
                }

                // Coluna
                for (int k = 0; k < tab->linhas; k++) {
                    if (k != i &&
                        tab->casas[k][j].estado == ORIGINAL &&
                        tab->casas[k][j].letra == letra) {
                        riscarCasa(jogo, tab, 'a' + j, k + 1);
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

// Regra 3: riscado -> pintar vizinhos originais de branco
int aplicarInferenciaRegra3(Jogo *jogo, Tabuleiro *tab) {
    // Vetores para deslocamento nas 4 direções: cima, direita, baixo, esquerda
    int dx[] = {-1, 0, 1, 0};
    int dy[] = {0, 1, 0, -1};

    // Percorre todas as linhas do tabuleiro
    for (int i = 0; i < tab->linhas; i++) {
        // Percorre todas as colunas do tabuleiro
        for (int j = 0; j < tab->colunas; j++) {
            // Verifica se a casa atual está riscada
            if (tab->casas[i][j].estado == RISCADO) {
                // Para cada casa riscada, verifica seus 4 vizinhos
                for (int d = 0; d < 4; d++) {
                    // Calcula a posição do vizinho na direção atual
                    int ni = i + dx[d];
                    int nj = j + dy[d];
                    
                    // Verifica se o vizinho está dentro dos limites do tabuleiro e se é uma casa original
                    if (ni >= 0 && ni < tab->linhas &&
                        nj >= 0 && nj < tab->colunas &&
                        tab->casas[ni][nj].estado == ORIGINAL) {
                        // Aplica a regra 3: pinta o vizinho de branco
                        pintarDeBranco(jogo, tab, 'a' + nj, ni + 1);
                        return 1;   // Retorna 1 para indicar que uma inferência foi aplicada
                    }
                }
            }
        }
    }
    // Retorna 0 se nenhuma inferência foi aplicada
    return 0;
}



// Aplica uma inferência para resolver qualquer uma das violações
int aplicarUmaInferenciaParaViolacoes(Jogo *jogo, Tabuleiro *tab) {
    // Verifica se há alguma violação
    if (aplicarInferenciaRegra1(jogo, tab)) return 1;
    if (aplicarInferenciaRegra2(jogo, tab)) return 1;
    if (aplicarInferenciaRegra3(jogo, tab)) return 1;
    return 0;
}

// ---------------------------------- comando A ----------------------------------
// Aplica inferências até que não haja mais mudanças
int aplicaInferencias(Jogo *jogo, Tabuleiro *tab) {
    int mudou = 0;
    while (aplicarUmaInferenciaParaViolacoes(jogo, tab)) {
        mudou = 1;
    }
    return mudou;
}

//-------------------------comando R -------------------------------------------
int aplicaInferenciasResolve(Jogo *jogo, Tabuleiro *tab) {
    int mudanca = 0;
    mudanca += aplicarInferenciaRegra1(jogo, tab);
    mudanca += aplicarInferenciaRegra2(jogo, tab);
    mudanca += aplicarInferenciaRegra3(jogo, tab);
    mudanca += aplicarInferenciaRegra4(jogo, tab);  
    return mudanca;
}

// Regra 4: pintar vizinho original se houver casas brancas isoladas
int pintarVizinhoOriginal(Jogo *jogo, Tabuleiro *tab, int i, int j) {
    // Vetores para as 4 direções: cima, direita, baixo, esquerda
    int dx[] = {-1, 0, 1, 0};
    int dy[] = {0, 1, 0, -1};

    // Tenta pintar qualquer vizinho original para criar conexão
    // Tentamos pintar de branco qualquer casa original que seja vizinha
    for (int d = 0; d < 4; d++) {
        // Calcula as coordenadas do vizinho na direção atual
        int ni = i + dx[d];
        int nj = j + dy[d];

        // Verifica se o vizinho está dentro dos limites do tabuleiro
        if (ni >= 0 && ni < tab->linhas &&
            nj >= 0 && nj < tab->colunas &&
            // Verifica se o vizinho é uma casa original
            tab->casas[ni][nj].estado == ORIGINAL) {

            // Pinta o vizinho encontrado de branco e retorna 1 para indicar sucesso
            return pintarDeBranco(jogo, tab, 'a' + nj, ni + 1);
        }
    }

    return 0;
}

int aplicarInferenciaRegra4(Jogo *jogo, Tabuleiro *tab) {
    int total, iInicio, jInicio;
    
    // Conta quantas casas brancas existem e encontra a primeira
    contarBrancasEPrimeira(tab, &total, &iInicio, &jInicio);

    // Se houver 0 ou 1 casa branca, não há necessidade de verificar conectividade
    if (total <= 1) return 0;

    // Aloca matriz auxiliar para marcar casas visitadas
    int **visitado = alocarVisitado(tab->linhas, tab->colunas);
    
    // Marca todas as casas brancas que estão conectadas à primeira casa branca
    contarBrancasLigadas(tab, visitado, iInicio, jInicio);

    // Procura por casas brancas isoladas (não visitadas)
    for (int i = 0; i < tab->linhas; i++) {
        for (int j = 0; j < tab->colunas; j++) {
            // Se encontrar uma casa branca que não está conectada
            if (tab->casas[i][j].estado == BRANCO && !visitado[i][j]) {
                // Tenta pintar um vizinho original para criar uma conexão
                if (pintarVizinhoOriginal(jogo, tab, i, j)) {
                    libertarVisitado(visitado, tab->linhas);
                    return 1; // Sucesso: conseguiu pintar um vizinho
                }
            }
        }
    }

    // Libera a memória utilizada
    libertarVisitado(visitado, tab->linhas);
    
    // Nenhuma inferência aplicada
    return 0;
}




// --------------------------- Manipulação de Casas ---------------------------
int pintarDeBranco(Jogo *jogo, Tabuleiro *tab, char col, int lin) {
int j = col - 'a';
int i = lin - 1;

if (i < 0 || i >= tab->linhas || j < 0 || j >= tab->colunas)
    return 0;

// Remove a restrição de estado ORIGINAL
tab->casas[i][j].estado = BRANCO;

// Regista a jogada
jogo->ultimaJogada.linha = i;
jogo->ultimaJogada.coluna = j;
jogo->ultimaJogada.valida = 1;
return 1;
}

int riscarCasa(Jogo *, Tabuleiro *tab, char col, int lin) {
int j = col - 'a';
int i = lin - 1;

if (i < 0 || i >= tab->linhas || j < 0 || j >= tab->colunas)
    return 0;

// Remove a restrição de estado ORIGINAL
tab->casas[i][j].estado = RISCADO;

return 1;
}


//-------------------------Resolver jogo (comando 'r')-------------------------

int resolverJogo(Jogo *jogo, Tabuleiro *tab) {
    if (!jogo || !tab) return 0;

    // Guardar uma cópia antes de modificar
    Tabuleiro *backup = copiarTabuleiro(tab);

    int maxIteracoes = 1000;
    int iter = 0;
    int houveMudanca = 1;

    while (iter++ < maxIteracoes && houveMudanca) {
        houveMudanca = aplicaInferenciasResolve(jogo, tab);
        if (verificarPuzzleCompleto(tab)) {
            libertarTabuleiro(backup);
            return 1;  // Sucesso
        }
    }

    // Se não foi possível resolver, restaurar tabuleiro original
    for (int i = 0; i < backup->linhas; i++) {
        for (int j = 0; j < backup->colunas; j++) {
            tab->casas[i][j] = backup->casas[i][j];
        }
    }

    libertarTabuleiro(backup);
    return 0;  // Falhou
}

// --------------------------- Histórico ---------------------------

void salvarEstado(NodoHistorico **historico, Tabuleiro *tabuleiro) {

    // Cria uma cópia independente do tabuleiro
    Tabuleiro *copia = copiarTabuleiro(tabuleiro);

    // Cria um novo nó para o histórico
    NodoHistorico *novoNodo = malloc(sizeof(NodoHistorico));
    novoNodo->estado = copia;
    novoNodo->anterior = *historico;

    // Atualiza o histórico para apontar para o novo nó
    *historico = novoNodo;

}

int desfazerJogada(Jogo *jogo) {
    if (!jogo->historico) {
        printf("Erro: Não há estados no histórico para desfazer.\n");
        return 0;
    }

    // Obtém o último estado salvo no histórico
    NodoHistorico *ultimo = jogo->historico;

    // Atualiza o histórico para apontar para o nó anterior
    jogo->historico = ultimo->anterior;
 // Libera o tabuleiro atual
 libertarTabuleiro(jogo->atual);

 // Restaura o estado do histórico como o tabuleiro atual
 jogo->atual = ultimo->estado;

 // Libera o nó do histórico
 free(ultimo);

 // Invalida a última jogada, pois ela foi desfeita
 jogo->ultimaJogada.valida = 0;

 return 1;
}



void libertarHistorico(NodoHistorico *hist) {
 while (hist) {
     NodoHistorico *anterior = hist->anterior;
     if (hist->estado) {
         libertarTabuleiro(hist->estado); // Libera o tabuleiro do nó
         hist->estado = NULL;            // Evita uso após liberação
     }
     free(hist); // Libera o nó
     hist = anterior;
 }
}

// --------------------------- Jogo ---------------------------

// Cria um novo jogo com tabuleiros inicial e atual
Jogo *criarJogo(int linhas, int colunas) {
 Jogo *jogo = malloc(sizeof(Jogo));
 jogo->atual = criarTabuleiro(linhas, colunas);
 jogo->inicial = criarTabuleiro(linhas, colunas);
 jogo->historico = NULL; // Não salva o estado inicial automaticamente
 return jogo;
}

// Carrega o estado do jogo de um arquivo
Jogo *carregarJogo(const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) {
        printf("Erro: Não foi possível abrir o arquivo '%s' para leitura.\n", nomeArquivo);
        return NULL;
    }

    int linhas, colunas;
    fscanf(arquivo, "%d %d\n", &linhas, &colunas);

    Jogo *jogo = criarJogo(linhas, colunas);
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            char letra;
            char estado;
            fscanf(arquivo, " %c %c", &letra, &estado);

            jogo->atual->casas[i][j].letra = letra;
            if (estado == 'O') {
                jogo->atual->casas[i][j].estado = ORIGINAL;
            } else if (estado == 'B') {
                jogo->atual->casas[i][j].estado = BRANCO;
            } else if (estado == 'R') {
                jogo->atual->casas[i][j].estado = RISCADO;
            }
        }
    }

    fclose(arquivo);
    jogo->inicial = copiarTabuleiro(jogo->atual);
    return jogo;
}


// Salva o estado atual do jogo em um arquivo
void salvarJogo(const Jogo *jogo, const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "w");
    if (!arquivo) {
        printf("Erro: Não foi possível abrir o arquivo '%s' para escrita.\n", nomeArquivo);
        return;
    }
    // Escreve as dime/nsões do tabuleiro (linhas e colunas) na primeira linha do arquivo
    fprintf(arquivo, "%d %d\n", jogo->atual->linhas, jogo->atual->colunas);
 //Percorre todas as linhas do tabuleiro 
 for (int i = 0; i < jogo->atual->linhas; i++) {
    //Percorre todas as colunas do tabuleiro 
    for (int j = 0; j < jogo->atual->colunas; j++) {
           char estado = ' ';
           // Verifica o estado da casa atual e determina o caractere correspondente
           if (jogo->atual->casas[i][j].estado == ORIGINAL) {
               estado = 'O';
           } else if (jogo->atual->casas[i][j].estado == BRANCO) {
               estado = 'B';
           } else if (jogo->atual->casas[i][j].estado == RISCADO) {
               estado = 'R';
           }
           // Escreve no arquivo a letra e o estado da casa atual, separados por um  espaço
           fprintf(arquivo, "%c %c ", jogo->atual->casas[i][j].letra, estado);
       }
       fprintf(arquivo, "\n");
   }

   fclose(arquivo);
   printf("Jogo salvo com sucesso no arquivo '%s'.\n", nomeArquivo);
}

// Libera a memória alocada para o jogo
void libertarJogo(Jogo *jogo) {
   if (!jogo) return;

   // Libera o tabuleiro atual, se não for compartilhado
   if (jogo->atual) {
       libertarTabuleiro(jogo->atual);
       jogo->atual = NULL;
   }

   // Libera o tabuleiro inicial, se não for compartilhado
   if (jogo->inicial) {
       libertarTabuleiro(jogo->inicial);
       jogo->inicial = NULL;
   }

   // Libera o histórico
   libertarHistorico(jogo->historico);
   free(jogo); // Libera a estrutura do jogo
}

