#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include "jogo.h"

/* =====================================================================
   TELA: limpeza, pausa, textos e barras
   ===================================================================== */

/* Limpa a tela do terminal (Windows ou terminais ANSI). */
void limparTela(void) {
#ifdef _WIN32
    system("cls");
#else
    printf("\033[2J\033[H");
#endif
}

/* Pausa a tela ate o jogador apertar ENTER. */
void pausar(void) {
    printf("\nPressione ENTER para continuar...");
    getchar();
}

/* Exibe textos com efeito de digitacao, sem bibliotecas externas. */
void textoAnimado(const char *texto) {
    while (*texto) {
        putchar(*texto++);
        fflush(stdout);
        clock_t inicio = clock();
        while ((double)(clock() - inicio) / CLOCKS_PER_SEC < 0.012) { }
    }
}

/* Imprime uma linha horizontal com a largura da tela. */
void imprimirLinha(char caractere) {
    for (int i = 0; i < LARGURA_TELA; i++) {
        putchar(caractere);
    }
    putchar('\n');
}

/* Titulo centralizado entre duas linhas de "=". */
void mostrarTitulo(const char *texto) {
    int espacos = (LARGURA_TELA - (int)strlen(texto)) / 2;

    if (espacos < 0) {
        espacos = 0;
    }

    printf("\n");
    imprimirLinha('=');
    printf("%*s%s\n", espacos, "", texto);
    imprimirLinha('=');
}

/* Subtitulo centralizado no meio de uma linha de "-". */
void mostrarSubtitulo(const char *texto) {
    int sobra = LARGURA_TELA - (int)strlen(texto) - 2;
    int esquerda;

    if (sobra < 2) {
        sobra = 2;
    }
    esquerda = sobra / 2;

    printf("\n");
    for (int i = 0; i < esquerda; i++) {
        putchar('-');
    }
    printf(" %s ", texto);
    for (int i = 0; i < sobra - esquerda; i++) {
        putchar('-');
    }
    putchar('\n');
}

/* Desenha uma barra no formato [#####-----]. Se ainda ha algum valor, a barra
   mostra pelo menos um "#". */
void desenharBarra(int atual, int maximo, int largura) {
    int cheio = 0;

    if (maximo > 0 && atual > 0) {
        cheio = (int)((long)atual * largura / maximo);
        if (cheio < 1) {
            cheio = 1;
        }
        if (cheio > largura) {
            cheio = largura;
        }
    }

    putchar('[');
    for (int i = 0; i < largura; i++) {
        putchar(i < cheio ? '#' : '-');
    }
    putchar(']');
}

/* Uma linha "ROTULO  [####----] atual/maximo". */
void mostrarBarraVida(const char *rotulo, int atual, int maximo, int largura) {
    printf("  %-11s ", rotulo);
    desenharBarra(atual, maximo, largura);
    printf(" %d/%d\n", atual, maximo);
}

/* =====================================================================
   ENTRADA: leitura segura de numeros e nomes
   ===================================================================== */

/* Le uma linha e converte para inteiro. Retorna -1 se a linha nao for
   apenas um numero (letras, vazio, "3abc", "1 2", valor gigante). */
int lerNumero(void) {
    char linha[64];
    char *fim;
    long valor;

    if (fgets(linha, sizeof(linha), stdin) == NULL) {
        return -1;
    }

    if (strchr(linha, '\n') == NULL && !feof(stdin)) {
        int c;  /* linha maior que o buffer: descarta o resto */
        while ((c = getchar()) != '\n' && c != EOF) { }
        return -1;
    }

    errno = 0;
    valor = strtol(linha, &fim, 10);
    if (fim == linha || errno != 0 || valor < 0 || valor > 1000000) {
        return -1;
    }
    while (*fim == ' ' || *fim == '\t' || *fim == '\r') fim++;
    if (*fim != '\n' && *fim != '\0') {
        return -1;
    }
    return (int)valor;
}

/* Le um numero inteiro dentro de [minimo, maximo]. Se o valor for invalido
   (letra, vazio ou fora do intervalo), exibe um aviso e pergunta de novo. */
int lerOpcaoValida(const char *mensagem, const char *aviso,
                   int minimo, int maximo, int padraoEntradaEncerrada) {
    for (;;) {
        printf("%s", mensagem);
        int valor = lerNumero();

        if (valor >= minimo && valor <= maximo) {
            return valor;
        }
        if (feof(stdin)) {
            printf("\nAVISO: entrada encerrada. Usando a opcao %d.\n", padraoEntradaEncerrada);
            return padraoEntradaEncerrada;
        }
        printf("AVISO: %s\n", aviso);
    }
}

/* Verifica se o nome e um texto valido: so letras, espacos, hifen, apostrofo
   ou ponto, com pelo menos uma letra. Bytes acima de 127 (acentos em UTF-8)
   sao aceitos como letras. */
static int nomeValido(const char *nome) {
    int temLetra = 0;

    for (const unsigned char *c = (const unsigned char *)nome; *c; c++) {
        if (isalpha(*c) || *c >= 128) {
            temLetra = 1;
        } else if (*c != ' ' && *c != '-' && *c != '\'' && *c != '.') {
            return 0;
        }
    }
    return temLetra;
}

/* Le o nome do jogador e repete a pergunta ate receber um texto valido.
   Rejeita entrada vazia, so espacos, numeros e simbolos. */
void lerNome(char *destino, size_t tamanho, const char *mensagem) {
    char linha[256];

    for (;;) {
        printf("%s", mensagem);

        if (fgets(linha, sizeof(linha), stdin) == NULL) {
            printf("\nAVISO: entrada encerrada. Usando o nome padrao \"Vigia\".\n");
            strncpy(destino, "Vigia", tamanho - 1);
            destino[tamanho - 1] = '\0';
            return;
        }

        size_t len = strcspn(linha, "\n");
        if (linha[len] == '\0' && !feof(stdin)) {
            int c;  /* linha maior que o buffer: descarta o resto */
            while ((c = getchar()) != '\n' && c != EOF) { }
        }
        linha[len] = '\0';

        /* Remove espacos no inicio e no fim. */
        char *ini = linha;
        while (*ini == ' ' || *ini == '\t' || *ini == '\r') ini++;
        size_t n = strlen(ini);
        while (n > 0 && (ini[n - 1] == ' ' || ini[n - 1] == '\t' || ini[n - 1] == '\r')) {
            ini[--n] = '\0';
        }

        if (n == 0) {
            printf("AVISO: o nome nao pode ficar vazio. Digite novamente.\n");
        } else if (!nomeValido(ini)) {
            printf("AVISO: nome invalido. Use apenas letras (sem numeros ou simbolos). Digite novamente.\n");
        } else if (n >= tamanho) {
            printf("AVISO: nome muito longo (maximo %d caracteres). Digite novamente.\n", (int)tamanho - 1);
        } else {
            strcpy(destino, ini);
            return;
        }
    }
}

/* =====================================================================
   TELAS DO JOGO: musica, situacao da rodada e final da campanha
   ===================================================================== */

/* Mostra a letra quando os vigias encontram a criatura. */
void mostrarMusica(void) {
    mostrarTitulo("MUSICA DO QUIBUNGO");
    printf("\n");

    printf("Cuidado ao andar na floresta\n");
    printf("Cuidado quando o Sol se por\n");
    printf("Quando a luz do dia vai embora\n");
    printf("O vento chora e sopra o terror\n");
    printf("O que a mata guarda escondido\n");
    printf("E melhor que fique assim\n");
    printf("O que o nosso olho nao enxerga\n");
    printf("E o que traz nosso triste fim\n\n");

    printf("Medo, o medo\n");
    printf("Trava minhas pernas aqui\n");
    printf("Medo, forte medo\n");
    printf("Nao me deixa sair daqui\n");
    printf("Medo, o medo\n");
    printf("Veio me tirar do mundo enfim\n");
    printf("Medo, forte medo\n");
    printf("E o que traz o Quibungo ate mim\n\n");

    printf("Medo, o medo\n");
    printf("Trava minhas pernas aqui\n");
    printf("Medo, forte medo\n");
    printf("Nao me deixa sair daqui\n");
    printf("Medo, o medo\n");
    printf("Veio me tirar do mundo enfim\n");
    printf("Medo, forte medo\n");
    printf("E o que traz o Quibungo ate mim\n\n");
    imprimirLinha('=');
}

/* Painel com noite, Quibungo, criancas, vigias (vida e medo) e itens. */
void mostrarSituacao(const EstadoJogo *jogo) {
    char item[2][40];

    printf("\n");
    imprimirLinha('=');
    printf(" NOITE %d/%d  -  RODADA %d/%d  |  Noites concluidas: %d/%d\n",
           jogo->noite, TOTAL_NOITES, jogo->rodada, RODADAS_POR_NOITE,
           jogo->noitesConcluidas, TOTAL_NOITES);
    imprimirLinha('=');

    printf("\n");
    mostrarBarraVida("QUIBUNGO", jogo->monstro.vida, jogo->monstro.vidaMaxima,
                     LARGURA_BARRA_QUIBUNGO);
    mostrarBarraVida("CRIANCAS", jogo->criancasVivas, TOTAL_CRIANCAS,
                     TOTAL_CRIANCAS);

    if (jogo->criancaSequestrada) {
        printf("\n  !! ALERTA: uma crianca foi sequestrada! Tente resgata-la !!\n");
    }

    printf("\n  VIGIAS\n");
    imprimirLinha('-');
    for (int i = 0; i < TOTAL_VIGIAS; i++) {
        const Vigia *v = &jogo->vigias[i];

        printf("  %-11s", v->nome);
        if (v->vivo) {
            printf("VIDA ");
            desenharBarra(v->vida, v->vidaMaxima, LARGURA_BARRA_VIDA);
            printf(" %3d/%-3d  MEDO ", v->vida, v->vidaMaxima);
            desenharBarra(v->medo, 100, LARGURA_BARRA_MEDO);
            printf(" %3d\n", v->medo);
        } else {
            printf("(fora de combate)\n");
        }
    }
    imprimirLinha('-');

    printf("  ITENS\n");
    for (int i = 0; i < TOTAL_ITENS; i += 2) {
        for (int k = 0; k < 2; k++) {
            snprintf(item[k], sizeof(item[k]), "%s: %d",
                     nomesItens[i + k], jogo->itens[i + k]);
        }
        printf("  %-30s%s\n", item[0], item[1]);
    }
}

/* Mostra o resultado da campanha e os dados da partida. */
void mostrarFinal(const EstadoJogo *jogo) {
    mostrarTitulo("FIM DA CAMPANHA");

    if (jogo->vitoria) {
        if (jogo->criancasVivas == TOTAL_CRIANCAS &&
            contarVigiasVivos(jogo) == TOTAL_VIGIAS) {
            printf("FINAL PERFEITO! Todos foram salvos e o Quibungo foi derrotado.\n");
        } else if (jogo->criancasVivas > 0) {
            printf("VITORIA! O Quibungo foi derrotado, mas houve perdas.\n");
        }
    } else if (jogo->criancasVivas == 0) {
        printf("FINAL TRAGICO: nenhuma crianca restou no vilarejo.\n");
    } else if (contarVigiasVivos(jogo) == 0) {
        printf("DERROTA: todos os vigias cairam diante do Quibungo.\n");
    } else {
        printf("O Quibungo sobreviveu as cinco noites e continua a rondar a mata.\n");
    }

    printf("\n");
    mostrarBarraVida("QUIBUNGO", jogo->monstro.vida, jogo->monstro.vidaMaxima,
                     LARGURA_BARRA_QUIBUNGO);
    mostrarBarraVida("CRIANCAS", jogo->criancasVivas, TOTAL_CRIANCAS,
                     TOTAL_CRIANCAS);
    mostrarBarraVida("VIGIAS", contarVigiasVivos(jogo), TOTAL_VIGIAS,
                     TOTAL_VIGIAS);

    printf("\n  Noites concluidas: %d/%d\n", jogo->noitesConcluidas, TOTAL_NOITES);
    printf("  Criancas perdidas: %d\n", jogo->criancasPerdidas);
    printf("  Dano total causado ao Quibungo: %d\n", jogo->danoTotal);
    imprimirLinha('=');
}
