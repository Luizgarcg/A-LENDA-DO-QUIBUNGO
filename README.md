A LENDA DO QUIBUNGO — RPG EM C PURO

SOBRE O JOGO
Uma aventura de RPG em C na qual até quatro pessoas controlam vigias que
precisam proteger dez crianças durante cinco noites e derrotar o Quibungo.
Os espaços de jogadores não ocupados por pessoas são controlados por NPCs.

REQUISITOS
- GCC ou MinGW
- Padrão C99
- Não utiliza bibliotecas externas

COMPILAR
Abra o terminal na pasta principal do projeto:

gcc -std=c99 -Wall -Wextra -pedantic main.c jogo.c interface.c arquivos.c -o quibungo

No Windows: .\quibungo.exe
No Linux: ./quibungo

ARQUIVOS
- main.c: login do jogador e menu principal.
- jogo.h: estruturas, constantes (regras e aparência) e declarações.
- jogo.c: regras, personagens, eventos, combate, turnos e campanha.
- interface.c: tela e entrada (barras de vida com #, títulos, painel da rodada,
  limpeza de tela, leitura segura de números e nomes).
- arquivos.c: leitura da história e salvamento/carregamento.
- dados/historia.txt: narrativa apresentada no início.
- saves/: arquivo de progresso da campanha.

VISUAL
Vida do Quibungo, das crianças e dos vigias aparece em barras no formato
[########----]. O medo dos vigias usa a mesma barra. O tamanho das barras e
da tela é ajustável pelas constantes LARGURA_* em jogo.h.

SISTEMAS E REGRAS
1. Campanha com cinco noites e oito rodadas por noite.
2. Dez crianças precisam ser protegidas.
3. Até quatro pessoas podem jogar; personagens restantes são NPCs.
4. A identificação inicial registra o nome do jogador nesta execução. Não há
   conta on-line nem senha: trata-se de um perfil local simples.
5. O Quibungo começa com 1000 pontos de vida.
6. Os ataques normais do Quibungo acontecem independentemente do evento.
7. A cada rodada há 45% de chance de surgir o evento especial de sequestro.
   Quando aparece, uma chance de 50/50 determina se a tentativa de sequestro
   acontece. O evento não substitui o ataque normal.
8. Proteger as crianças impede o sequestro naquela rodada.
9. Uma criança sequestrada pode ser resgatada antes do fim da rodada seguinte.
10. O Quibungo pode desaparecer; os vigias precisam encontrá-lo novamente.
11. Testes de percepção usam D20 e dificuldade 15.
12. Testes de resgate usam D20 e dificuldade 12.
13. Existem quatro itens: ervas medicinais, lamparina, amuleto e ervas
    ritualísticas.
14. Há três dificuldades: fácil, normal e difícil.
15. Ao amanhecer, os vigias vivos recuperam vida, o medo diminui e o grupo
    encontra alguns recursos.
16. Não existe sistema de níveis de personagem.

CONTAGEM DAS NOITES
A campanha possui exatamente cinco noites, cada uma com oito rodadas. A tela
do terminal é limpa no início de cada nova rodada e a cada amanhecer.
Cada noite possui oito rodadas. Ao concluir a oitava rodada, o contador de
noites concluídas aumenta em um. A campanha termina após a quinta noite caso
o Quibungo ainda não tenha sido derrotado.

SALVAMENTO
Escolha “Salvar e sair” durante a ação de um personagem; o progresso é gravado e o jogo volta ao menu principal. Para continuar,
selecione “Continuar jogo salvo” no menu principal. O formato atual é
QUIBUNGO_SAVE_5; arquivos de versões anteriores não são compatíveis.
Se o save for feito no meio de uma rodada, ao continuar ela segue a partir do
personagem que estava jogando, sem repetir ações nem refazer sorteios.
O fim da campanha não sobrescreve o save: ao continuar, o jogo volta ao ponto
em que o jogador salvou pela última vez.

OBSERVAÇÃO
Execute o jogo a partir da pasta principal do projeto para que os caminhos
relativos de dados/ e saves/ funcionem corretamente.
