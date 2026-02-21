#include <stdlib.h> // Para malloc e free
#include <ctype.h>  // Para tolower e toupper
#include <stdbool.h> // para true or false
#include <CUnit/Basic.h>
#include <string.h>
#include "jogo.h"

//-------------------------------------- Validação -------------------------//
void teste_validarLinha() {
    CU_ASSERT_TRUE(validarLinha("abc", 3));
    CU_ASSERT_FALSE(validarLinha("abcd", 3));
    CU_ASSERT_FALSE(validarLinha("abc", 4));
    CU_ASSERT_FALSE(validarLinha("abc", 0));
}

void teste_coordenada_valida() {
    Tabuleiro *tabuleiro = criarTabuleiro(3, 3);
    tabuleiro->casas[0][0].letra = 'A';
    tabuleiro->casas[0][1].letra = 'B';
    tabuleiro->casas[0][2].letra = 'C';
    tabuleiro->casas[1][0].letra = 'D';

    CU_ASSERT_TRUE(coordenadaValida(tabuleiro, 'a', 1));
    CU_ASSERT_TRUE(coordenadaValida(tabuleiro, 'c', 3));
    CU_ASSERT_FALSE(coordenadaValida(tabuleiro, 'd', 1));
    CU_ASSERT_FALSE(coordenadaValida(tabuleiro, 'a', 4));

    libertarTabuleiro(tabuleiro);
}

//-------------------------------------- Tabuleiro -------------------------//

void teste_criar_tabuleiro() {
    Tabuleiro *tabuleiro = criarTabuleiro(3, 3);
    CU_ASSERT_PTR_NOT_NULL(tabuleiro);
    CU_ASSERT_EQUAL(tabuleiro->linhas, 3);
    CU_ASSERT_EQUAL(tabuleiro->colunas, 3);

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            CU_ASSERT_EQUAL(tabuleiro->casas[i][j].letra, ' ');
            CU_ASSERT_EQUAL(tabuleiro->casas[i][j].estado, ORIGINAL);
        }
    }

    libertarTabuleiro(tabuleiro);
}

void teste_libertar_tabuleiro() {
    Tabuleiro *tabuleiro = criarTabuleiro(3, 3);
    libertarTabuleiro(tabuleiro);
}

void teste_copiar_tabuleiro() {
    Tabuleiro *original = criarTabuleiro(3, 3);
    original->casas[0][0].letra = 'A';
    original->casas[1][1].letra = 'B';
    original->casas[2][2].letra = 'C';

    Tabuleiro *copia = copiarTabuleiro(original);

    CU_ASSERT_PTR_NOT_NULL(copia);
    CU_ASSERT_EQUAL(copia->linhas, original->linhas);
    CU_ASSERT_EQUAL(copia->colunas, original->colunas);

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            CU_ASSERT_EQUAL(copia->casas[i][j].letra, original->casas[i][j].letra);
            CU_ASSERT_EQUAL(copia->casas[i][j].estado, original->casas[i][j].estado);
        }
    }

    libertarTabuleiro(original);
    libertarTabuleiro(copia);
}
void teste_imprimir_tabuleiro() {
    Tabuleiro *tabuleiro = criarTabuleiro(2, 2);
    tabuleiro->casas[0][0].letra = 'A';
    tabuleiro->casas[0][1].letra = 'B';
    tabuleiro->casas[1][0].letra = 'C';
    tabuleiro->casas[1][1].letra = 'D';
    imprimirTabuleiro(tabuleiro);

    libertarTabuleiro(tabuleiro);
}

//-------------------------------------- Verificação de restrições -------------------------//
void teste_verificar_restricoes() {
    Tabuleiro *tabuleiro = criarTabuleiro(2, 2);
    tabuleiro->casas[0][0].letra = 'A'; tabuleiro->casas[0][0].estado = BRANCO;
    tabuleiro->casas[0][1].letra = 'B'; tabuleiro->casas[0][1].estado = RISCADO;
    tabuleiro->casas[1][0].letra = 'C'; tabuleiro->casas[1][0].estado = BRANCO;
    tabuleiro->casas[1][1].letra = 'D'; tabuleiro->casas[1][1].estado = BRANCO;
    Violacoes violacoes = verificarRestricoes(tabuleiro);

    CU_ASSERT_FALSE(violacoes.regra1Violada);
    CU_ASSERT_FALSE(violacoes.regra2Violada);
    CU_ASSERT_FALSE(violacoes.regra3Violada);
    CU_ASSERT_FALSE(violacoes.regra4Violada);

    libertarTabuleiro(tabuleiro);
}

void teste_existe_caminho_branco() {
    Tabuleiro *tabuleiro = criarTabuleiro(2, 2);

    // Teste com caminho branco
    tabuleiro->casas[0][0].estado = BRANCO;
    tabuleiro->casas[0][1].estado = BRANCO;
    tabuleiro->casas[1][1].estado = BRANCO;
    tabuleiro->casas[1][0].estado = BRANCO;

    CU_ASSERT_TRUE(existeCaminhoBranco(tabuleiro, 0)); // Passa 0 para não mostrar erros

    // Teste sem caminho branco
    tabuleiro->casas[0][0].estado = BRANCO;
    tabuleiro->casas[0][1].estado = RISCADO;
    tabuleiro->casas[1][1].estado = BRANCO;
    tabuleiro->casas[1][0].estado = RISCADO;

    CU_ASSERT_FALSE(existeCaminhoBranco(tabuleiro, 0)); // Passa 0 para não mostrar erros

    libertarTabuleiro(tabuleiro);
}

void teste_imprimir_restricoes() {
    Violacoes violacoes;
    violacoes.regra1Violada = true;
    violacoes.regra2Violada = false;
    violacoes.regra3Violada = true;
    violacoes.regra4Violada = false;

    imprimirViolacoes(violacoes);
}

void teste_nao_existe_restricoes() {
    Tabuleiro *tabuleiro = criarTabuleiro(2, 2);
    tabuleiro->casas[0][0].estado = ORIGINAL;
    tabuleiro->casas[0][1].estado = BRANCO;
    tabuleiro->casas[1][0].estado = RISCADO;


    Violacoes violacoes = verificarRestricoes(tabuleiro);
    CU_ASSERT_FALSE(violacoes.regra1Violada);
    CU_ASSERT_FALSE(violacoes.regra2Violada);
    CU_ASSERT_FALSE(violacoes.regra3Violada);
    CU_ASSERT_FALSE(violacoes.regra4Violada);

    libertarTabuleiro(tabuleiro);
}

//-------------------------------------- Manipulação de casas -------------------------//

void teste_pintar_de_branco() {
    Tabuleiro *tabuleiro = criarTabuleiro(2, 2);
    tabuleiro->casas[0][0].estado = ORIGINAL;

    pintarDeBranco(tabuleiro, 'a', 1);
    CU_ASSERT_EQUAL(tabuleiro->casas[0][0].estado, BRANCO);

    libertarTabuleiro(tabuleiro);
}
void teste_pintar_de_branco_invalida() {
    Tabuleiro *tabuleiro = criarTabuleiro(2, 2);

    pintarDeBranco(tabuleiro, 'd', 1);
    CU_ASSERT_EQUAL(tabuleiro->casas[0][0].estado, ORIGINAL);

    libertarTabuleiro(tabuleiro);
}

void teste_pintar_de_branco_repetida() {
    Tabuleiro *tabuleiro = criarTabuleiro(2, 2);
    tabuleiro->casas[0][0].estado = BRANCO;

    pintarDeBranco(tabuleiro, 'a', 1);
    CU_ASSERT_EQUAL(tabuleiro->casas[0][0].estado, BRANCO);

    libertarTabuleiro(tabuleiro);
}

void teste_pintar_de_branco_riscada() {
    Tabuleiro *tabuleiro = criarTabuleiro(2, 2);
    tabuleiro->casas[0][0].estado = RISCADO;

    pintarDeBranco(tabuleiro, 'a', 1);
    CU_ASSERT_EQUAL(tabuleiro->casas[0][0].estado, BRANCO);

    libertarTabuleiro(tabuleiro);
}

void teste_riscar_casa() {
    Tabuleiro *tabuleiro = criarTabuleiro(2, 2);
    tabuleiro->casas[0][0].estado = ORIGINAL;

    riscarCasa(tabuleiro, 'a', 1);
    CU_ASSERT_EQUAL(tabuleiro->casas[0][0].estado, RISCADO);

    libertarTabuleiro(tabuleiro);
}

void teste_riscar_casa_invalida() {
    Tabuleiro *tabuleiro = criarTabuleiro(2, 2);
    tabuleiro->casas[0][0].estado = ORIGINAL;

    riscarCasa(tabuleiro, 'd', 1);
    CU_ASSERT_EQUAL(tabuleiro->casas[0][0].estado, ORIGINAL);

    libertarTabuleiro(tabuleiro);
}

void teste_riscar_casa_repetida() {
    Tabuleiro *tabuleiro = criarTabuleiro(2, 2);
    tabuleiro->casas[0][0].estado = RISCADO;

    riscarCasa(tabuleiro, 'a', 1);
    CU_ASSERT_EQUAL(tabuleiro->casas[0][0].estado, RISCADO);

    libertarTabuleiro(tabuleiro);
}

void teste_riscar_casa_branco() {
    Tabuleiro *tabuleiro = criarTabuleiro(2, 2);
    tabuleiro->casas[0][0].estado = BRANCO;

    riscarCasa(tabuleiro, 'a', 1);
    CU_ASSERT_EQUAL(tabuleiro->casas[0][0].estado, RISCADO);

    libertarTabuleiro(tabuleiro);
}

void teste_verifica_branco_original() {
    Tabuleiro *tab = criarTabuleiro(3, 3);
    tab->casas[0][0].letra = 'A';
    tab->casas[0][0].estado = ORIGINAL;
    tab->casas[0][1].letra = 'A';
    tab->casas[0][1].estado = BRANCO;

    CU_ASSERT_EQUAL(verificaBrancoOriginal(tab), 1);

    tab->casas[0][1].letra = 'B';
    CU_ASSERT_EQUAL(verificaBrancoOriginal(tab), 0);

    libertarTabuleiro(tab);
}

void teste_verifica_duplicados() {
    Tabuleiro *tab = criarTabuleiro(3, 3);
    tab->casas[1][0].letra = 'X';
    tab->casas[1][0].estado = BRANCO;
    tab->casas[1][2].letra = 'X';
    tab->casas[1][2].estado = BRANCO;

    CU_ASSERT_EQUAL(verificaDuplicados(tab), 1);

    tab->casas[1][2].letra = 'Y';
    CU_ASSERT_EQUAL(verificaDuplicados(tab), 0);

    libertarTabuleiro(tab);
}


//-------------------------------------- Histórico -------------------------//

void teste_salvar_estado() {
    Tabuleiro *tabuleiro = criarTabuleiro(2, 2);
    NodoHistorico *historico = NULL;

    salvarEstado(&historico, tabuleiro);
    CU_ASSERT_PTR_NOT_NULL(historico);

    libertarTabuleiro(tabuleiro);

    // Libera o histórico
    while (historico) {
        NodoHistorico *temp = historico;
        historico = historico->anterior;
        libertarTabuleiro(temp->estado);
        free(temp);
    }
}
void teste_libertar_historico() {
    NodoHistorico *historico = NULL;

    Tabuleiro *tabuleiro = criarTabuleiro(2, 2);
    salvarEstado(&historico, tabuleiro);

    while (historico) {
        NodoHistorico *temp = historico;
        historico = historico->anterior;
        libertarTabuleiro(temp->estado);
        free(temp);
    }

    CU_ASSERT_PTR_NULL(historico);
    libertarTabuleiro(tabuleiro);
}


//-------------------------------------- Jogo -------------------------//

void teste_criar_jogo() {
    Jogo *jogo = criarJogo(3, 3);
    CU_ASSERT_PTR_NOT_NULL(jogo);
    CU_ASSERT_PTR_NOT_NULL(jogo->atual);
    CU_ASSERT_PTR_NOT_NULL(jogo->inicial);
    CU_ASSERT_EQUAL(jogo->atual->linhas, 3);
    CU_ASSERT_EQUAL(jogo->atual->colunas, 3);

    libertarJogo(jogo);
}

void teste_carregar_jogo() {
    Jogo *jogo = carregarJogo("fim"); // Ensure the correct relative path
    CU_ASSERT_PTR_NOT_NULL_FATAL(jogo); // Ensure the test stops if the file is not loaded

    if (jogo != NULL) { // Apenas acessa o jogo se ele não for NULL
        CU_ASSERT_PTR_NOT_NULL(jogo->atual);
        CU_ASSERT_PTR_NOT_NULL(jogo->inicial);
        CU_ASSERT_EQUAL(jogo->atual->linhas, 3);
        CU_ASSERT_EQUAL(jogo->atual->colunas, 3);
        libertarJogo(jogo);
    }
}
void teste_salvar_jogo() {
    Jogo *jogo = criarJogo(3, 3);
    salvarJogo(jogo, "jogo_salvo.txt");

    // Verifica se o arquivo foi criado
    FILE *arquivo = fopen("jogo_salvo.txt", "r");
    CU_ASSERT_PTR_NOT_NULL(arquivo);
    fclose(arquivo);

    libertarJogo(jogo);
}


void teste_iniciar_jogo() {
    Jogo *jogo = criarJogo(3, 3);

    CU_ASSERT_PTR_NOT_NULL(jogo->atual);
    CU_ASSERT_EQUAL(jogo->atual->linhas, 3);
    CU_ASSERT_EQUAL(jogo->atual->colunas, 3);

    libertarJogo(jogo);
}

void teste_libertar_jogo() {
    Jogo *jogo = criarJogo(3, 3);
    salvarJogo(jogo, "jogo_salvo.txt");
    libertarJogo(jogo);

    // Remove o arquivo salvo
    remove("jogo_salvo.txt");
}

int main() {
    CU_initialize_registry();

    CU_pSuite suite = CU_add_suite("Teste de Jogo", NULL, NULL);
    CU_add_test(suite, "Teste de validarLinha", teste_validarLinha);
    CU_add_test(suite, "Teste de coordenadaValida", teste_coordenada_valida);
    CU_add_test(suite, "Teste de criar_tabuleiro", teste_criar_tabuleiro);
    CU_add_test(suite, "Teste de copiar_tabuleiro", teste_copiar_tabuleiro);
    CU_add_test(suite, "Teste de imprimir_tabuleiro", teste_imprimir_tabuleiro);
    CU_add_test(suite, "Teste de verificar_restricoes", teste_verificar_restricoes);
    CU_add_test(suite, "Teste de existe_caminho_branco", teste_existe_caminho_branco);
    CU_add_test(suite, "verificarRestricoes", teste_verificar_restricoes);
    CU_add_test(suite, "verificaBrancoOriginal", teste_verifica_branco_original);
    CU_add_test(suite, "verificaDuplicados", teste_verifica_duplicados);
    CU_add_test(suite, "Teste de imprimir_restricoes", teste_imprimir_restricoes);
    CU_add_test(suite, "Teste de pintar_de_branco", teste_pintar_de_branco);
    CU_add_test(suite, "Teste de pintar_de_branco_invalida", teste_pintar_de_branco_invalida);
    CU_add_test(suite, "Teste de pintar_de_branco_riscada", teste_pintar_de_branco_riscada);
    CU_add_test(suite, "Teste de riscar_casa", teste_riscar_casa);
    CU_add_test(suite, "Teste de riscar_casa_invalida", teste_riscar_casa_invalida);
    CU_add_test(suite, "Teste de salvar_estado", teste_salvar_estado);
    CU_add_test(suite, "Teste de libertar_historico", teste_libertar_historico);
    CU_add_test(suite, "Teste de criar_jogo", teste_criar_jogo);
    CU_add_test(suite, "Teste de carregar_jogo", teste_carregar_jogo);
    CU_add_test(suite, "Teste de salvar_jogo", teste_salvar_jogo);
    CU_add_test(suite, "Teste de libertar_jogo", teste_libertar_jogo);

    CU_basic_run_tests();
    return 0;
}



