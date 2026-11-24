#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jogo.h"

/* =====================================================================
   DADOS FIXOS: nomes e descricoes de classes e itens
   ===================================================================== */

const char *const nomesClasses[TOTAL_VIGIAS] = {
    "Guerreiro", "Arqueiro", "Curandeira", "Mistica"
};

/* Descricao curta de cada classe, usada na escolha de personagem. */
const char *const descricoesClasses[TOTAL_VIGIAS] = {
    "Resistente: mais vida e golpe forte.",
    "Especialista em dano e percepcao.",
    "Cura a equipe e reduz o medo.",
    "Usa ritual para revelar e paralisar o monstro."
};

const char *const nomesItens[TOTAL_ITENS] = {
    "Ervas medicinais", "Lamparina", "Amuleto", "Ervas ritualisticas"
};

const char *const descricoesItens[TOTAL_ITENS] = {
    "Recuperam 35 pontos de vida de quem usar.",
    "Concede +5 no proximo teste de percepcao.",
    "Reduz em 25 pontos o medo de todo o grupo.",
    "Causa dano e paralisa o Quibungo se ele estiver visivel."
};

/* =====================================================================
   ESTADO E UTILITARIOS DOS VIGIAS
   ===================================================================== */

/* Retorna um numero aleatorio entre 1 e 20. */
static int rolarDado(void) {
    return rand() % 20 + 1;
}

/* Conta quantos vigias ainda conseguem lutar. */
int contarVigiasVivos(const EstadoJogo *jogo) {
    int total = 0;

    for (int i = 0; i < TOTAL_VIGIAS; i++) {
        if (jogo->vigias[i].vivo) {
            total++;
        }
    }

    return total;
}

/* Aumenta o medo de todos os vigias que estao vivos. */
static void aumentarMedo(EstadoJogo *jogo, int quantidade) {
    for (int i = 0; i < TOTAL_VIGIAS; i++) {
        if (jogo->vigias[i].vivo) {
            jogo->vigias[i].medo += quantidade;

            if (jogo->vigias[i].medo > 100) {
                jogo->vigias[i].medo = 100;
            }
        }
    }
}

/* Reduz o medo sem deixar o valor ficar abaixo de zero. */
static void reduzirMedo(EstadoJogo *jogo, int quantidade) {
    for (int i = 0; i < TOTAL_VIGIAS; i++) {
        jogo->vigias[i].medo -= quantidade;

        if (jogo->vigias[i].medo < 0) {
            jogo->vigias[i].medo = 0;
        }
    }
}

/* Prepara os quatro personagens no inicio da campanha. */
static void prepararVigias(EstadoJogo *jogo) {
    strcpy(jogo->vigias[0].nome, "Bento");
    jogo->vigias[0].vidaMaxima = 120;
    jogo->vigias[0].ataque = 22;
    jogo->vigias[0].percepcao = 2;

    strcpy(jogo->vigias[1].nome, "Joaquim");
    jogo->vigias[1].vidaMaxima = 90;
    jogo->vigias[1].ataque = 27;
    jogo->vigias[1].percepcao = 5;

    strcpy(jogo->vigias[2].nome, "Dona Maria");
    jogo->vigias[2].vidaMaxima = 85;
    jogo->vigias[2].ataque = 10;
    jogo->vigias[2].percepcao = 3;

    strcpy(jogo->vigias[3].nome, "Rosa");
    jogo->vigias[3].vidaMaxima = 80;
    jogo->vigias[3].ataque = 12;
    jogo->vigias[3].percepcao = 6;

    for (int i = 0; i < TOTAL_VIGIAS; i++) {
        jogo->vigias[i].vida = jogo->vigias[i].vidaMaxima;
        jogo->vigias[i].medo = 0;
        jogo->vigias[i].vivo = 1;
    }
}

/* =====================================================================
   INICIO DA PARTIDA
   ===================================================================== */

/* Prepara a partida e define quantas pessoas vao jogar. */
void iniciarJogo(EstadoJogo *jogo, const char *nomePerfil) {
    int escolha;
    int jogador;
    int escolhidos[TOTAL_VIGIAS] = {0};

    memset(jogo, 0, sizeof(EstadoJogo));

    jogo->noite = 1;
    jogo->rodada = 1;
    jogo->criancasVivas = TOTAL_CRIANCAS;
    jogo->monstro.vidaMaxima = VIDA_QUIBUNGO;
    jogo->monstro.vida = VIDA_QUIBUNGO;
    strcpy(jogo->monstro.nome, "Quibungo");

    jogo->itens[0] = 3;
    jogo->itens[1] = 2;
    jogo->itens[2] = 2;
    jogo->itens[3] = 2;

    prepararVigias(jogo);

    printf("\n");
    mostrarHistoria();

    textoAnimado("\nA campanha tera cinco noites. Dez criancas dependem dos vigias.\n");
    escolha = lerOpcaoValida("\nQuantas pessoas vao jogar? (1 a 4): ",
                             "numero de jogadores invalido. Escolha de 1 a 4 jogadores.",
                             1, TOTAL_VIGIAS, 1);
    jogo->quantidadeJogadores = escolha;

    mostrarSubtitulo("DIFICULDADE");
    printf("1 - Facil\n");
    printf("2 - Normal\n");
    printf("3 - Dificil\n");
    escolha = lerOpcaoValida("Opcao: ",
                             "dificuldade invalida. Selecione 1 (Facil), 2 (Normal) ou 3 (Dificil).",
                             1, 3, 2);
    jogo->dificuldade = escolha;

    for (jogador = 0; jogador < jogo->quantidadeJogadores; jogador++) {
        char nome[TAM_NOME];
        if (jogador == 0) {
            strncpy(nome, nomePerfil, sizeof(nome) - 1);
            nome[sizeof(nome) - 1] = '\0';
        } else {
            char pergunta[64];
            snprintf(pergunta, sizeof(pergunta), "\nJogador %d, digite seu nome: ", jogador + 1);
            lerNome(nome, sizeof(nome), pergunta);
        }
        printf("\n");
        imprimirLinha('-');
        printf(" Jogador %d (%s), escolha seu personagem:\n", jogador + 1, nome);
        imprimirLinha('-');

        for (int i = 0; i < TOTAL_VIGIAS; i++) {
            if (!escolhidos[i]) {
                printf("%d - %-10s | %s\n", i + 1, nomesClasses[i], descricoesClasses[i]);
            }
        }

        printf("Opcao: ");
        escolha = lerNumero();

        while (escolha < 1 || escolha > TOTAL_VIGIAS ||
               escolhidos[escolha - 1]) {
            if (feof(stdin)) {  /* entrada encerrada: pega o primeiro livre */
                escolha = 1;
                while (escolhidos[escolha - 1]) escolha++;
                break;
            }
            printf("AVISO: escolha invalida ou personagem ja escolhido. Tente novamente: ");
            escolha = lerNumero();
        }

        escolhidos[escolha - 1] = 1;
        jogo->jogadoresHumanos[escolha - 1] = 1;
        strcpy(jogo->nomesJogadores[escolha - 1], nome);
        printf("%s escolheu %s.\n", nome, nomesClasses[escolha - 1]);
    }

    mostrarSubtitulo("NPCs");
    printf("Personagens restantes serao controlados pela inteligencia do jogo.\n");
    for (int i = 0; i < TOTAL_VIGIAS; i++) {
        if (!jogo->jogadoresHumanos[i]) {
            strcpy(jogo->nomesJogadores[i], "NPC");
            printf("  %s sera um NPC.\n", nomesClasses[i]);
        }
    }

    printf("\nA primeira noite vai comecar...\n");
    pausar();
}

/* =====================================================================
   EVENTOS DA RODADA
   ===================================================================== */

/* Cada vigia vivo tenta encontrar o Quibungo (dificuldade DT_PERCEPCAO). */
static void testePercepcao(EstadoJogo *jogo) {
    int encontrou = 0;

    printf("\nOs vigias tentam perceber os sons da mata.\n");

    for (int i = 0; i < TOTAL_VIGIAS; i++) {
        Vigia *v = &jogo->vigias[i];

        if (v->vivo) {
            int penalidade = jogo->bonusPercepcao;
            int resultado = rolarDado() + v->percepcao + penalidade;

            /* O medo muito alto atrapalha a concentracao. */
            if (v->medo >= 80) {
                resultado -= 2;
            }

            printf("  %-11s %2d (DT %d)\n", v->nome, resultado, DT_PERCEPCAO);

            if (resultado >= DT_PERCEPCAO) {
                encontrou = 1;
            }
        }
    }

    if (encontrou) {
        if (!jogo->monstroVisivel) {
            printf("\nUma musica estranha ecoa pelo vilarejo...\n");
            mostrarMusica();
        }
        printf("O QUIBUNGO FOI ENCONTRADO!\n");
        jogo->monstroVisivel = 1;
    } else {
        jogo->monstroVisivel = 0;
        printf("Ninguem conseguiu encontrar a criatura.\n");
    }

    jogo->bonusPercepcao = 0;
}

/* Sorteia um acontecimento para deixar cada rodada diferente. */
static void eventoAleatorio(EstadoJogo *jogo) {
    int chance = rand() % 100;
    int evento;

    if (chance >= 30) {
        return;
    }

    evento = rand() % 4;

    mostrarSubtitulo("EVENTO ALEATORIO");

    if (evento == 0) {
        printf("Um grito vem da mata! Os vigias ficam atentos.\n");
        jogo->bonusPercepcao = 2;
    } else if (evento == 1) {
        printf("Uma chuva forte comeca. A visao fica prejudicada.\n");
        jogo->bonusPercepcao = 3;
    } else if (evento == 2) {
        printf("O vento apaga algumas lamparinas. O grupo fica assustado.\n");
        aumentarMedo(jogo, 10);
    } else {
        printf("Os moradores se escondem e fecham as portas.\n");
    }

    imprimirLinha('-');
}

/* Sorteia a chance de aparecer o evento especial nesta rodada. */
static void sortearEventoAtaque(EstadoJogo *jogo) {
    jogo->eventoAtaque50 = 0;
    if (jogo->criancasVivas > 0 && rand() % 100 < 45) {
        jogo->eventoAtaque50 = 1;
        mostrarTitulo("EVENTO ESPECIAL: 50/50 DO QUIBUNGO SEQUESTRAR!");
    }
}

/* =====================================================================
   ACOES DOS PERSONAGENS
   ===================================================================== */

/* Causa dano ao Quibungo e registra o dano total da campanha. */
static void causarDano(EstadoJogo *jogo, int dano) {
    if (dano < 0) {
        dano = 0;
    }

    jogo->monstro.vida -= dano;
    jogo->danoTotal += dano;

    if (jogo->monstro.vida < 0) {
        jogo->monstro.vida = 0;
    }

    printf("O Quibungo recebeu %d de dano.\n", dano);
    mostrarBarraVida("Quibungo", jogo->monstro.vida, jogo->monstro.vidaMaxima,
                     LARGURA_BARRA_QUIBUNGO);
}

/* Ataque normal do personagem escolhido. */
static void atacar(EstadoJogo *jogo) {
    Vigia *jogador = &jogo->vigias[jogo->classeEscolhida];
    int dano;

    if (!jogador->vivo) {
        printf("Seu personagem esta fora de combate.\n");
        return;
    }

    if (!jogo->monstroVisivel) {
        printf("Voce nao consegue ver o Quibungo. O ataque foi perdido.\n");
        return;
    }

    dano = jogador->ataque + rand() % 8;

    if (jogo->classeEscolhida == 1) {
        dano += 8;
    }

    if (jogo->classeEscolhida == 0) {
        dano += 5;
    }

    if (jogador->medo >= 70) {
        dano -= 7;
        printf("O medo atrapalhou seu ataque!\n");
    }

    causarDano(jogo, dano);
}

/* Protege as criancas contra uma tentativa de sequestro nesta rodada. */
static void protegerCriancas(EstadoJogo *jogo) {
    jogo->criancasProtegidas = 1;
    printf("Voce ficou de guarda perto das casas. As criancas estao protegidas nesta rodada.\n");
}

/* Tenta salvar a crianca que foi levada pelo monstro. */
static void resgatarCrianca(EstadoJogo *jogo) {
    Vigia *jogador = &jogo->vigias[jogo->classeEscolhida];
    int resultado;

    if (!jogo->criancaSequestrada) {
        printf("Nenhuma crianca esta desaparecida neste momento.\n");
        return;
    }

    if (!jogador->vivo) {
        printf("Seu personagem nao pode realizar o resgate.\n");
        return;
    }

    resultado = rolarDado() + jogador->percepcao;

    printf("Teste de resgate: %d (DT %d)\n", resultado, DT_RESGATE);

    if (resultado >= DT_RESGATE) {
        jogo->criancaSequestrada = 0;
        printf("Voce encontrou a crianca e conseguiu traze-la de volta!\n");
        reduzirMedo(jogo, 5);
    } else {
        printf("Voce nao conseguiu encontrar a crianca a tempo.\n");
    }
}

/* Cada classe tem uma habilidade especial. */
static void habilidadeEspecial(EstadoJogo *jogo) {
    Vigia *jogador = &jogo->vigias[jogo->classeEscolhida];

    if (!jogador->vivo) {
        printf("Seu personagem esta fora de combate.\n");
        return;
    }

    if (jogo->classeEscolhida == 0) {
        if (jogo->monstroVisivel) {
            printf("Bento avanca com um golpe pesado!\n");
            causarDano(jogo, 25 + rand() % 8);
        } else {
            printf("O guerreiro prepara o golpe, mas nao encontra o monstro.\n");
        }
    } else if (jogo->classeEscolhida == 1) {
        if (jogo->monstroVisivel) {
            printf("Joaquim mira em um ponto fraco da criatura!\n");
            causarDano(jogo, 22 + rand() % 8);
        } else {
            jogo->bonusPercepcao = 3;
            printf("O arqueiro observa a mata. O proximo teste de percepcao recebe +3.\n");
        }
    } else if (jogo->classeEscolhida == 2) {
        for (int i = 0; i < TOTAL_VIGIAS; i++) {
            Vigia *v = &jogo->vigias[i];

            if (v->vivo) {
                v->vida += 25;
                if (v->vida > v->vidaMaxima) {
                    v->vida = v->vidaMaxima;
                }
            }
        }

        reduzirMedo(jogo, 10);
        printf("A curandeira recuperou a vida e acalmou o grupo.\n");
    } else {
        if (jogo->monstroVisivel) {
            printf("A mistica realiza um ritual. O Quibungo fica paralisado!\n");
            causarDano(jogo, 12);
            jogo->monstroParalisado = 1;
        } else {
            jogo->monstroVisivel = 1;
            printf("O ritual faz a criatura aparecer na escuridao!\n");
            mostrarMusica();
        }
    }
}

/* Usa um item que foi encontrado durante a campanha.
   Retorna 1 se um item foi usado e 0 se o jogador voltou ao menu de acoes. */
static int usarItem(EstadoJogo *jogo) {
    int opcao;
    Vigia *jogador = &jogo->vigias[jogo->classeEscolhida];

    for (;;) {
        mostrarSubtitulo("MOCHILA");
        for (int i = 0; i < TOTAL_ITENS; i++) {
            printf("%d - %s (%d): %s\n", i + 1, nomesItens[i],
                   jogo->itens[i], descricoesItens[i]);
        }
        printf("0 - Voltar\nOpcao: ");

        opcao = lerNumero();

        if (opcao == 0) {
            return 0;
        }

        if (feof(stdin)) {  /* entrada encerrada: evita laco infinito */
            printf("\nAVISO: entrada encerrada. Nenhum item usado.\n");
            return 0;
        }

        if (opcao < 1 || opcao > TOTAL_ITENS) {
            printf("AVISO: opcao invalida. Escolha um item de 1 a %d ou 0 para voltar.\n", TOTAL_ITENS);
        } else if (jogo->itens[opcao - 1] <= 0) {
            printf("AVISO: voce nao tem %s. Escolha outro item.\n", nomesItens[opcao - 1]);
        } else {
            break;
        }
    }

    jogo->itens[opcao - 1]--;

    if (opcao == 1) {
        jogador->vida += 35;
        if (jogador->vida > jogador->vidaMaxima) {
            jogador->vida = jogador->vidaMaxima;
        }
        printf("Voce usou ervas medicinais e recuperou vida.\n");
    } else if (opcao == 2) {
        jogo->bonusPercepcao = 5;
        printf("A lamparina iluminou a mata. Bonus +5 no proximo teste de percepcao.\n");
    } else if (opcao == 3) {
        reduzirMedo(jogo, 25);
        printf("O amuleto trouxe tranquilidade. O medo do grupo diminuiu.\n");
    } else {
        if (jogo->monstroVisivel) {
            causarDano(jogo, 18);
            jogo->monstroParalisado = 1;
            printf("As ervas ritualisticas enfraqueceram a criatura.\n");
        } else {
            jogo->bonusPercepcao = 4;
            printf("A fumaca do ritual se espalhou pela mata. Bonus +4 na percepcao.\n");
        }
    }

    return 1;
}

/* =====================================================================
   TURNOS
   ===================================================================== */

/* Mostra as acoes de um personagem controlado por uma pessoa. */
static void menuAcoes(EstadoJogo *jogo, int personagem) {
    int opcao;
    jogo->classeEscolhida = personagem;

    printf("\n");
    imprimirLinha('-');
    printf(" Sua vez, %s! (%s)\n",
           jogo->nomesJogadores[personagem][0] ? jogo->nomesJogadores[personagem]
                                               : jogo->vigias[personagem].nome,
           jogo->vigias[personagem].nome);
    printf(" VIDA ");
    desenharBarra(jogo->vigias[personagem].vida, jogo->vigias[personagem].vidaMaxima,
                  LARGURA_BARRA_VIDA);
    printf(" %d/%d   MEDO ", jogo->vigias[personagem].vida, jogo->vigias[personagem].vidaMaxima);
    desenharBarra(jogo->vigias[personagem].medo, 100, LARGURA_BARRA_MEDO);
    printf(" %d\n", jogo->vigias[personagem].medo);
    imprimirLinha('-');
    printf("1 - Atacar: causa dano ao Quibungo se ele estiver visivel.\n");
    printf("2 - Proteger: impede o sequestro de crianca nesta rodada.\n");
    printf("3 - Habilidade especial: efeito exclusivo da classe.\n");
    printf("4 - Usar item: consome um recurso da mochila.\n");
    printf("5 - Resgatar: tenta recuperar a crianca sequestrada.\n");
    printf("6 - Salvar e sair: grava o progresso e volta ao menu principal.\n");
    printf("\nEscolha: ");

    opcao = lerNumero();

    switch (opcao) {
        case 1:
            atacar(jogo);
            break;
        case 2:
            protegerCriancas(jogo);
            break;
        case 3:
            habilidadeEspecial(jogo);
            break;
        case 4:
            if (!usarItem(jogo) && !feof(stdin)) {
                menuAcoes(jogo, personagem);  /* voltou: escolhe outra acao */
            }
            break;
        case 5:
            resgatarCrianca(jogo);
            break;
        case 6:
            /* Marca em que ponto da rodada o jogo foi salvo: ao continuar, a
               rodada segue a partir deste personagem, sem refazer sorteios. */
            jogo->retomarRodada = 1;
            jogo->proximoVigia = personagem;
            if (salvarJogo(jogo)) {
                printf("Partida salva com sucesso. Saindo da partida...\n");
                jogo->sairDoJogo = 1;
            } else {
                jogo->retomarRodada = 0;
                jogo->proximoVigia = 0;
                printf("Nao foi possivel salvar a partida. Escolha outra acao.\n");
                menuAcoes(jogo, personagem);
            }
            break;
        default:
            if (feof(stdin)) {  /* entrada encerrada: evita laco infinito */
                printf("Entrada encerrada. Voce perdeu sua acao.\n");
            } else {
                printf("AVISO: opcao invalida. Escolha um numero de 1 a 6.\n");
                menuAcoes(jogo, personagem);
            }
            break;
    }
}

/* Os personagens que nao tem uma pessoa jogando agem como NPCs. */
static void acaoNPC(EstadoJogo *jogo, int personagem) {
    jogo->classeEscolhida = personagem;

    printf("\n[NPC] %s esta agindo...\n", jogo->vigias[personagem].nome);

    if (jogo->criancaSequestrada) {
        printf("[NPC] Vai tentar encontrar a crianca desaparecida.\n");
        resgatarCrianca(jogo);
    } else if (jogo->monstroVisivel) {
        printf("[NPC] Ataca o Quibungo!\n");
        atacar(jogo);
    } else if (personagem == 3) {
        printf("[NPC] Usa sua habilidade para revelar a criatura.\n");
        habilidadeEspecial(jogo);
    } else if (personagem == 2 && jogo->vigias[personagem].vida <
               jogo->vigias[personagem].vidaMaxima) {
        printf("[NPC] Usa sua habilidade de cura.\n");
        habilidadeEspecial(jogo);
    } else if (rand() % 100 < 30) {
        printf("[NPC] Fica de guarda perto das criancas.\n");
        protegerCriancas(jogo);
    } else if (personagem == 1) {
        habilidadeEspecial(jogo);
    } else {
        printf("[NPC] Observa a mata e protege o grupo.\n");
        protegerCriancas(jogo);
    }
}

/* Cada personagem vivo realiza uma acao, seja pessoa ou NPC. */
static void turnoDoGrupo(EstadoJogo *jogo, int inicio) {
    for (int i = inicio; i < TOTAL_VIGIAS; i++) {
        if (!jogo->vigias[i].vivo) {
            continue;
        }

        if (jogo->jogadoresHumanos[i]) {
            menuAcoes(jogo, i);
        } else {
            acaoNPC(jogo, i);
        }

        if (jogo->sairDoJogo || jogo->monstro.vida <= 0 || jogo->criancasVivas <= 0) {
            break;
        }
    }
}

/* Evento especial: 50/50 de o Quibungo tentar sequestrar uma crianca. */
static void tentarSequestro(EstadoJogo *jogo) {
    if (!jogo->eventoAtaque50 || jogo->criancasVivas <= 0) {
        return;
    }

    if (rand() % 2 != 0) {
        printf("O sequestro nao aconteceu desta vez.\n");
        return;
    }

    printf("O Quibungo tentou sequestrar uma crianca!\n");

    if (jogo->criancasProtegidas) {
        printf("O grupo protegeu as casas e impediu o sequestro!\n");
    } else if (!jogo->criancaSequestrada) {
        jogo->criancaSequestrada = 1;
        jogo->rodadaDoSequestro = (jogo->noite - 1) * RODADAS_POR_NOITE + jogo->rodada;
        printf("Uma crianca foi sequestrada! Resgatem-na!\n");
        aumentarMedo(jogo, 15);
    } else {
        printf("Uma crianca ja esta desaparecida.\n");
    }
}

/* Ataque normal do Quibungo contra um vigia sorteado que ainda esteja vivo. */
static void atacarVigia(EstadoJogo *jogo) {
    int escolhido = rand() % TOTAL_VIGIAS;
    int dano = 15 + rand() % 11;
    Vigia *alvo;

    jogo->monstro.comportamento = rand() % 4;
    printf("\nO Quibungo se prepara para atacar!\n");

    while (!jogo->vigias[escolhido].vivo) {
        escolhido = (escolhido + 1) % TOTAL_VIGIAS;
    }
    alvo = &jogo->vigias[escolhido];

    if (jogo->dificuldade == 1) {
        dano -= 5;
    }
    if (jogo->dificuldade == 3) {
        dano += 7;
    }
    if (jogo->monstro.comportamento == 3) {
        dano += 8;
    }

    alvo->vida -= dano;
    aumentarMedo(jogo, 8);
    printf("O Quibungo atacou %s e causou %d de dano!\n", alvo->nome, dano);

    if (alvo->vida <= 0) {
        alvo->vida = 0;
        alvo->vivo = 0;
    }
    mostrarBarraVida(alvo->nome, alvo->vida, alvo->vidaMaxima, LARGURA_BARRA_VIDA);

    if (!alvo->vivo) {
        printf("%s foi derrotado e nao pode mais lutar.\n", alvo->nome);
    }

    if (jogo->monstro.comportamento == 2 && jogo->monstro.vida > 0) {
        jogo->monstroVisivel = 0;
        printf("O Quibungo desapareceu na mata. Sera preciso fazer outro teste!\n");
    }
}

/* Turno do Quibungo: tentativa de sequestro (evento) e ataque normal. */
static void turnoQuibungo(EstadoJogo *jogo) {
    if (jogo->monstro.vida <= 0 || contarVigiasVivos(jogo) <= 0) {
        return;
    }

    mostrarSubtitulo("TURNO DO QUIBUNGO");

    tentarSequestro(jogo);
    jogo->eventoAtaque50 = 0;

    if (jogo->monstroParalisado) {
        printf("\nO Quibungo esta paralisado e perdeu a vez!\n");
        jogo->monstroParalisado = 0;
        return;
    }

    atacarVigia(jogo);
}

/* =====================================================================
   FIM DE RODADA E CAMPANHA
   ===================================================================== */

/* Se a crianca nao for resgatada ate a rodada seguinte, ela se perde. */
static void verificarSequestro(EstadoJogo *jogo) {
    int rodadaAtual = (jogo->noite - 1) * RODADAS_POR_NOITE + jogo->rodada;

    if (jogo->criancaSequestrada &&
        rodadaAtual > jogo->rodadaDoSequestro) {
        jogo->criancaSequestrada = 0;
        jogo->criancasVivas--;
        jogo->criancasPerdidas++;
        printf("\nA crianca nao foi encontrada a tempo. Ela foi perdida.\n");
        aumentarMedo(jogo, 20);
    }
}

/* Verifica se a campanha acabou por derrota ou vitoria. */
static int verificarFim(EstadoJogo *jogo) {
    if (jogo->monstro.vida <= 0) {
        jogo->jogoFinalizado = 1;
        jogo->vitoria = 1;
        return 1;
    }

    if (jogo->criancasVivas <= 0 || contarVigiasVivos(jogo) <= 0) {
        jogo->jogoFinalizado = 1;
        jogo->vitoria = 0;
        return 1;
    }

    return 0;
}

/* Recupera os personagens ao amanhecer, sem sistema de niveis. */
static void evoluirPersonagens(EstadoJogo *jogo) {
    printf("\nO grupo descansou ao amanhecer e recuperou 20 pontos de vida.\n");
    for (int i = 0; i < TOTAL_VIGIAS; i++) {
        Vigia *v = &jogo->vigias[i];
        if (v->vivo) {
            v->vida += 20;
            if (v->vida > v->vidaMaxima) v->vida = v->vidaMaxima;
        }
    }
    reduzirMedo(jogo, 15);
    jogo->itens[0]++;
    jogo->itens[1]++;
    jogo->itens[3]++;
}

/* Controla as cinco noites da campanha. */
void jogar(EstadoJogo *jogo) {
    jogo->sairDoJogo = 0;

    while (!jogo->jogoFinalizado) {
        int retomando = jogo->retomarRodada;
        int inicio = 0;

        limparTela();  /* tela limpa a cada nova rodada / noite */

        if (retomando) {
            /* Save feito no meio da rodada: mantem os sorteios e a protecao
               ja definidos e continua a partir de quem estava jogando. */
            inicio = jogo->proximoVigia;
            jogo->retomarRodada = 0;
            jogo->proximoVigia = 0;
            mostrarSituacao(jogo);
            printf("\n>> Retomando a rodada de onde voce parou...\n");
            if (jogo->eventoAtaque50) {
                printf("(Evento especial desta rodada: 50/50 do Quibungo sequestrar!)\n");
            }
        } else {
            jogo->criancasProtegidas = 0;
            mostrarSituacao(jogo);
            eventoAleatorio(jogo);
            sortearEventoAtaque(jogo);

            if (!jogo->monstroVisivel) {
                testePercepcao(jogo);
            }
        }

        turnoDoGrupo(jogo, inicio);

        if (jogo->sairDoJogo) {
            break;
        }

        if (verificarFim(jogo)) {
            break;
        }

        turnoQuibungo(jogo);
        verificarSequestro(jogo);

        if (verificarFim(jogo)) {
            break;
        }

        printf("\n");
        imprimirLinha('=');
        printf(" Fim da rodada %d da noite %d.\n", jogo->rodada, jogo->noite);
        pausar();

        if (jogo->rodada == RODADAS_POR_NOITE) {
            jogo->noitesConcluidas++;

            if (jogo->noite == TOTAL_NOITES) {
                jogo->jogoFinalizado = 1;
                jogo->vitoria = 0;
                break;
            }

            limparTela();  /* amanhecer: nova tela antes da proxima noite */
            mostrarTitulo("AMANHECER");
            printf("\nO sol comeca a nascer. O grupo sobreviveu a noite %d.\n",
                   jogo->noite);
            evoluirPersonagens(jogo);

            jogo->noite++;
            jogo->rodada = 1;
            jogo->monstroVisivel = 0;
            jogo->monstroParalisado = 0;
            jogo->eventoAtaque50 = 0;
            jogo->monstro.vida += 15;

            if (jogo->monstro.vida > jogo->monstro.vidaMaxima) {
                jogo->monstro.vida = jogo->monstro.vidaMaxima;
            }

            printf("\nA proxima noite sera ainda mais perigosa...\n");
            pausar();
        } else {
            jogo->rodada++;
        }
    }

    if (jogo->sairDoJogo) {
        return;  /* ja foi salvo; volta ao menu principal sem tela final */
    }

    /* O final nao regrava o save: "Continuar jogo salvo" volta ao ponto
       em que o jogador salvou, antes do fim da campanha. */
    mostrarFinal(jogo);
    pausar();
}
