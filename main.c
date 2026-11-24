#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "jogo.h"

/* Mostra o menu principal. */
static void mostrarMenu(void) {
    mostrarTitulo("A LENDA DO QUIBUNGO");
    printf("  1 - Novo jogo\n");
    printf("  2 - Continuar jogo salvo\n");
    printf("  0 - Sair\n");
    imprimirLinha('-');
    printf("Escolha: ");
}

/* Identifica o jogador e chama as funcoes do jogo pelo menu principal. */
int main(void) {
    char nomePerfil[TAM_NOME];
    int opcao;

    srand((unsigned int)time(NULL));

    mostrarTitulo("LOGIN - A LENDA DO QUIBUNGO");
    lerNome(nomePerfil, sizeof(nomePerfil), "Digite seu nome de jogador: ");
    printf("\nBem-vindo, %s!\n", nomePerfil);

    do {
        mostrarMenu();

        opcao = lerNumero();
        if (feof(stdin)) {
            opcao = 0;  /* entrada encerrada: evita laco infinito */
        }

        if (opcao == 1) {
            EstadoJogo jogo;

            iniciarJogo(&jogo, nomePerfil);
            jogar(&jogo);
        } else if (opcao == 2) {
            EstadoJogo jogo;

            if (carregarJogo(&jogo)) {
                printf("\nPartida carregada!\n");
                pausar();
                jogar(&jogo);
            } else {
                printf("\nNao foi possivel carregar a partida.\n");
                pausar();
            }
        } else if (opcao != 0) {
            printf("\nAVISO: opcao invalida. Escolha 1, 2 ou 0.\n");
        }
    } while (opcao != 0);

    printf("\nAte a proxima, vigia!\n");
    return 0;
}
