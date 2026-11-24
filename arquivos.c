#include <stdio.h>
#include <string.h>
#include <time.h>
#include "jogo.h"

/* Le a historia de um arquivo de texto. */
void mostrarHistoria(void) {
    FILE *arquivo = fopen("dados/historia.txt", "r");
    char linha[200];

    if (arquivo == NULL) {
        printf("O vilarejo de La na Minha Terra esta em perigo.\n");
        return;
    }

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        for (int i = 0; linha[i] != '\0'; i++) {
            putchar(linha[i]);
            fflush(stdout);
            clock_t inicio = clock();
            while ((double)(clock() - inicio) / CLOCKS_PER_SEC < 0.002) { }
        }
    }

    fclose(arquivo);
}

/* Grava os dados da partida em um arquivo de texto. */
int salvarJogo(EstadoJogo *jogo) {
    FILE *arquivo = fopen("saves/partida.txt", "w");

    if (arquivo == NULL) {
        return 0;
    }

    fprintf(arquivo, "QUIBUNGO_SAVE_5\n");
    fprintf(arquivo, "%d %d %d %d %d\n",
            jogo->noite, jogo->rodada, jogo->classeEscolhida,
            jogo->quantidadeJogadores, jogo->dificuldade);
    for (int i = 0; i < TOTAL_VIGIAS; i++) {
        fprintf(arquivo, "%d\n", jogo->jogadoresHumanos[i]);
        fprintf(arquivo, "%s\n", jogo->nomesJogadores[i]);
    }
    fprintf(arquivo, "%d\n", jogo->eventoAtaque50);
    fprintf(arquivo, "%d %d\n", jogo->retomarRodada, jogo->proximoVigia);

    fprintf(arquivo, "%d %d %d %d\n",
            jogo->criancasVivas, jogo->criancasPerdidas,
            jogo->criancaSequestrada, jogo->rodadaDoSequestro);

    fprintf(arquivo, "%d %d %d %d\n",
            jogo->monstroVisivel, jogo->monstroParalisado,
            jogo->criancasProtegidas, jogo->bonusPercepcao);

    fprintf(arquivo, "%d %d %d %d\n",
            jogo->jogoFinalizado, jogo->vitoria,
            jogo->danoTotal, jogo->noitesConcluidas);

    fprintf(arquivo, "%d %d %d\n",
            jogo->monstro.vida, jogo->monstro.vidaMaxima,
            jogo->monstro.comportamento);

    for (int i = 0; i < TOTAL_ITENS; i++) {
        fprintf(arquivo, "%d ", jogo->itens[i]);
    }
    fprintf(arquivo, "\n");

    for (int i = 0; i < TOTAL_VIGIAS; i++) {
        Vigia *v = &jogo->vigias[i];

        fprintf(arquivo, "%s\n", v->nome);
        fprintf(arquivo, "%d %d %d %d %d %d\n",
                v->vida, v->vidaMaxima, v->ataque, v->percepcao,
                v->medo, v->vivo);
    }

    fclose(arquivo);
    return 1;
}

/* Recupera os dados que foram gravados anteriormente. */
int carregarJogo(EstadoJogo *jogo) {
    FILE *arquivo = fopen("saves/partida.txt", "r");
    char versao[40];

    if (arquivo == NULL) {
        return 0;
    }

    if (fgets(versao, sizeof(versao), arquivo) == NULL ||
        strcmp(versao, "QUIBUNGO_SAVE_5\n") != 0) {
        fclose(arquivo);
        return 0;
    }

    if (fscanf(arquivo, "%d %d %d %d %d",
               &jogo->noite, &jogo->rodada, &jogo->classeEscolhida,
               &jogo->quantidadeJogadores, &jogo->dificuldade) != 5) {
        fclose(arquivo);
        return 0;
    }

    for (int i = 0; i < TOTAL_VIGIAS; i++) {
        if (fscanf(arquivo, "%d", &jogo->jogadoresHumanos[i]) != 1 ||
            fscanf(arquivo, " %29[^\n]", jogo->nomesJogadores[i]) != 1) {
            fclose(arquivo);
            return 0;
        }
    }
    if (fscanf(arquivo, "%d", &jogo->eventoAtaque50) != 1) {
        fclose(arquivo);
        return 0;
    }
    if (fscanf(arquivo, "%d %d", &jogo->retomarRodada, &jogo->proximoVigia) != 2) {
        fclose(arquivo);
        return 0;
    }

    if (fscanf(arquivo, "%d %d %d %d",
               &jogo->criancasVivas, &jogo->criancasPerdidas,
               &jogo->criancaSequestrada, &jogo->rodadaDoSequestro) != 4) {
        fclose(arquivo);
        return 0;
    }

    if (fscanf(arquivo, "%d %d %d %d",
               &jogo->monstroVisivel, &jogo->monstroParalisado,
               &jogo->criancasProtegidas, &jogo->bonusPercepcao) != 4) {
        fclose(arquivo);
        return 0;
    }

    if (fscanf(arquivo, "%d %d %d %d",
               &jogo->jogoFinalizado, &jogo->vitoria,
               &jogo->danoTotal, &jogo->noitesConcluidas) != 4) {
        fclose(arquivo);
        return 0;
    }

    if (fscanf(arquivo, "%d %d %d",
               &jogo->monstro.vida, &jogo->monstro.vidaMaxima,
               &jogo->monstro.comportamento) != 3) {
        fclose(arquivo);
        return 0;
    }

    for (int i = 0; i < TOTAL_ITENS; i++) {
        if (fscanf(arquivo, "%d", &jogo->itens[i]) != 1) {
            fclose(arquivo);
            return 0;
        }
    }

    for (int i = 0; i < TOTAL_VIGIAS; i++) {
        Vigia *v = &jogo->vigias[i];

        if (fscanf(arquivo, " %29[^\n]", v->nome) != 1) {
            fclose(arquivo);
            return 0;
        }

        if (fscanf(arquivo, "%d %d %d %d %d %d",
                   &v->vida, &v->vidaMaxima, &v->ataque, &v->percepcao,
                   &v->medo, &v->vivo) != 6) {
            fclose(arquivo);
            return 0;
        }
    }

    fclose(arquivo);

    if (jogo->noite < 1 || jogo->noite > TOTAL_NOITES ||
        jogo->rodada < 1 || jogo->rodada > RODADAS_POR_NOITE ||
        jogo->classeEscolhida < 0 || jogo->classeEscolhida >= TOTAL_VIGIAS ||
        jogo->quantidadeJogadores < 1 || jogo->quantidadeJogadores > 4 ||
        jogo->retomarRodada < 0 || jogo->retomarRodada > 1 ||
        jogo->proximoVigia < 0 || jogo->proximoVigia >= TOTAL_VIGIAS) {
        return 0;
    }

    return 1;
}
