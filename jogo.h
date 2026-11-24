#ifndef JOGO_H
#define JOGO_H

#include <stddef.h>

#define TOTAL_VIGIAS 4
#define TOTAL_CRIANCAS 10
#define TOTAL_NOITES 5
#define RODADAS_POR_NOITE 8
#define TAM_NOME 30
#define TOTAL_ITENS 4

/* Regras e valores do jogo. */
#define VIDA_QUIBUNGO 1000
#define DT_PERCEPCAO 15
#define DT_RESGATE 12

/* Aparencia do terminal. */
#define LARGURA_TELA 72
#define LARGURA_BARRA_QUIBUNGO 40
#define LARGURA_BARRA_VIDA 20
#define LARGURA_BARRA_MEDO 10

/* Dados de cada personagem do grupo. */
typedef struct {
    char nome[TAM_NOME];
    int vida;
    int vidaMaxima;
    int ataque;
    int percepcao;
    int medo;
    int vivo;
} Vigia;

/* Dados do monstro principal. */
typedef struct {
    char nome[TAM_NOME];
    int vida;
    int vidaMaxima;
    int comportamento;
} Quibungo;

/* Guarda todas as informacoes necessarias para continuar a partida. */
typedef struct {
    int noite;
    int rodada;
    int classeEscolhida;
    int quantidadeJogadores;
    int jogadoresHumanos[TOTAL_VIGIAS];
    char nomesJogadores[TOTAL_VIGIAS][TAM_NOME];
    int eventoAtaque50;
    int dificuldade;
    int criancasVivas;
    int criancasPerdidas;
    int criancaSequestrada;
    int rodadaDoSequestro;
    int monstroVisivel;
    int monstroParalisado;
    int criancasProtegidas;
    int bonusPercepcao;
    int jogoFinalizado;
    int retomarRodada;  /* 1 = o save foi feito no meio de uma rodada */
    int proximoVigia;   /* quem age primeiro ao retomar essa rodada */
    int sairDoJogo;   /* nao e salvo: so sinaliza "salvar e sair" */
    int vitoria;
    int danoTotal;
    int noitesConcluidas;
    int itens[TOTAL_ITENS];
    Vigia vigias[TOTAL_VIGIAS];
    Quibungo monstro;
} EstadoJogo;

/* Dados do jogo (definidos em jogo.c). */
extern const char *const nomesClasses[TOTAL_VIGIAS];
extern const char *const descricoesClasses[TOTAL_VIGIAS];
extern const char *const nomesItens[TOTAL_ITENS];
extern const char *const descricoesItens[TOTAL_ITENS];

/* jogo.c: regras e campanha. */
void iniciarJogo(EstadoJogo *jogo, const char *nomePerfil);
void jogar(EstadoJogo *jogo);
int contarVigiasVivos(const EstadoJogo *jogo);

/* interface.c: entrada do jogador. */
int lerNumero(void);
int lerOpcaoValida(const char *mensagem, const char *aviso,
                   int minimo, int maximo, int padraoEntradaEncerrada);
void lerNome(char *destino, size_t tamanho, const char *mensagem);
void pausar(void);

/* interface.c: elementos visuais. */
void limparTela(void);
void textoAnimado(const char *texto);
void imprimirLinha(char caractere);
void mostrarTitulo(const char *texto);
void mostrarSubtitulo(const char *texto);
void desenharBarra(int atual, int maximo, int largura);
void mostrarBarraVida(const char *rotulo, int atual, int maximo, int largura);
void mostrarMusica(void);
void mostrarSituacao(const EstadoJogo *jogo);
void mostrarFinal(const EstadoJogo *jogo);

/* arquivos.c: historia e salvamento. */
void mostrarHistoria(void);
int salvarJogo(EstadoJogo *jogo);
int carregarJogo(EstadoJogo *jogo);

#endif
