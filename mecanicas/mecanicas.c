#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "estruturas.h"
#include "funcoes.h"
#include "mecanicas.h"

/* ====================================================================
 * CONSTANTES
 * ==================================================================== */

#define CUSTO_ESPECIAL 10
#define CUSTO_POCAO 5 /* a documentacao preve 5 MP; use 0 para desativar */
#define PENALIDADE_FUGA 15
#define DANO_ARMADILHA 12
#define CURA_SANTUARIO 25
#define FRACAO_ITEM 0.25
#define MULT_ESPECIAL 1.5
#define DADO_COMBATE 20
#define DADO_MOVIMENTO 6

/* Eventos sorteados nas casas intermediarias */
#define EV_COMBATE 0
#define EV_TESOURO 1
#define EV_CURA 2
#define EV_ARMADILHA 3
#define EV_VAZIO 4
#define TOTAL_EVENTOS 5

/* ====================================================================
 * DADOS ESTATICOS (criados uma unica vez)
 * ==================================================================== */

/* Ordem: {vidaBase, forca, defesa, agilidade} */
static const int atributosBase[3][3][4] = {
    /* Fase 1 */
    {{30, 8, 4, 5}, {45, 11, 6, 8}, {70, 14, 8, 10}},
    /* Fase 2 */
    {{60, 15, 9, 6}, {75, 18, 10, 14}, {100, 22, 12, 12}},
    /* Fase 3 */
    {{90, 24, 14, 9}, {120, 28, 18, 4}, {180, 35, 20, 15}}};

static const char *nomesMonstros[3][3] = {
    /* Fase 1 */
    {"Diabrete", "Possuido Infernal", "Malphas, O Vigia das Fendas"},
    /* Fase 2 */
    {"Hellhound", "Sucubo", "Astaroth, O Duque das Cinzas"},
    /* Fase 3 */
    {"Berserker Abissal", "Guardiao de Obsidiana", "Satanael, O Principe do Submundo"}};

/* [Classe 0-2][Chefe 0-1] */
static const char *armasEvoluidas[3][2] = {
    {"Katana Amaldicoada Muramasa", "Espada Divina Ame-no-Habakiri"},
    {"Pistolas de Prata Consagrada", "Canhoes Arcanos Seraphim"},
    {"Grimorio das Chamas Puras", "Tomo Supremo Akashico"}};

/* ====================================================================
 * UTILITARIOS INTERNOS
 * ==================================================================== */

static int limitar(int valor, int minimo, int maximo)
{
    if (valor < minimo)
        return minimo;
    if (valor > maximo)
        return maximo;
    return valor;
}

/* Dano = ataque - defesa, com minimo garantido de 1 */
static int calcularDano(double ataque, int defesa)
{
    return (int)fmax(1.0, ataque - defesa);
}

/* Percentual de um valor, arredondado para cima */
static int percentualCeil(int valor, double fracao)
{
    return (int)ceil(valor * fracao);
}

/* Soma valor a atual sem ultrapassar o maximo */
static int restaurar(int atual, int maximo, int valor)
{
    return (atual + valor > maximo) ? maximo : atual + valor;
}

/* Le um inteiro do teclado; retorna -1 se a entrada for invalida */
static int lerAcao(void)
{
    int valor;

    if (scanf("%d", &valor) != 1)
    {
        printf("Entrada invalida!\n");
        while (getchar() != '\n')
            ;
        return -1;
    }
    while (getchar() != '\n')
        ;
    return valor;
}

/* Indice da classe (0 Ken, 1 Satoshi, 2 Hanae) a partir do nome */
static int indiceClasse(const Personagem *p)
{
    if (strcmp(p->nome, "Satoshi Hasegawa") == 0)
        return 1;
    if (strcmp(p->nome, "Hanae Suda") == 0)
        return 2;
    return 0;
}

/* ====================================================================
 * GERACAO DE MONSTROS
 * ==================================================================== */

void gerarMonstro(Monstro *m, int fase, int indice)
{
    int fIdx = limitar(fase - 1, 0, 2);
    int mIdx = limitar(indice, 0, 2);
    double fator = pow(1.25, fIdx);

    strcpy(m->nome, nomesMonstros[fIdx][mIdx]);

    m->vidaMax = (int)(atributosBase[fIdx][mIdx][0] * fator);
    m->vidaAtual = m->vidaMax;
    m->forca = atributosBase[fIdx][mIdx][1];
    m->defesa = atributosBase[fIdx][mIdx][2];
    m->agilidade = atributosBase[fIdx][mIdx][3];
}

/* ====================================================================
 * COMBATE
 * ==================================================================== */

static void exibirMenuCombate(const Personagem *p, const Monstro *m)
{
    printf("\n--- Status do Combate ---\n");
    printf("Seu HP: %d/%d | Seu MP: %d/%d | %s HP: %d/%d\n",
           p->vidaAtual, p->vidaMax, p->manaAtual, p->manaMax,
           m->nome, m->vidaAtual, m->vidaMax);
    printf("[1] Atacar Normal\n");
    printf("[2] Usar Habilidade Especial (%s) [Custo: %d MP]\n",
           p->habilidade, CUSTO_ESPECIAL);
    printf("[3] Usar Pocao (+%d HP / 25%%) [Custo: %d MP] [Restantes: %d]\n",
           percentualCeil(p->vidaMax, FRACAO_ITEM), CUSTO_POCAO, p->pocoes);
    printf("[4] Usar Eter (+%d MP / 25%%) [Restantes: %d]\n",
           percentualCeil(p->manaMax, FRACAO_ITEM), p->eteres);
    printf("[5] Tentar Fugir\n");
    printf("Escolha sua acao: ");
}

/* Cada acao retorna 1 se gastou o turno, 0 se o jogador deve escolher de novo */

static int acaoAtacar(Personagem *p, Monstro *m)
{
    int dado = rolarDado(DADO_COMBATE);
    int ataqueTotal = dado + p->forca;
    int dano = calcularDano(ataqueTotal, m->defesa);

    m->vidaAtual -= dano;

    printf(">> Voce atacou! Rolou %d no d20 + forca (%d) = Ataque %d.\n",
           dado, p->forca, ataqueTotal);
    printf(">> Causou %d de dano espiritual em %s!\n", dano, m->nome);
    return 1;
}

static int acaoHabilidade(Personagem *p, Monstro *m)
{
    int dado, ataqueTotal, dano;

    if (p->manaAtual < CUSTO_ESPECIAL)
    {
        printf(">> MP INSUFICIENTE! Voce nao tem energia espiritual para usar '%s'!\n",
               p->habilidade);
        return 0;
    }

    p->manaAtual -= CUSTO_ESPECIAL;

    dado = rolarDado(DADO_COMBATE);
    ataqueTotal = dado + p->inteligencia + p->forca;
    dano = calcularDano(ataqueTotal * MULT_ESPECIAL, m->defesa);
    m->vidaAtual -= dano;

    printf(">> Usou '%s' (-%d MP)! Rolou %d.\n", p->habilidade, CUSTO_ESPECIAL, dado);
    printf(">> Dano critico de exorcismo de %d em %s!\n", dano, m->nome);
    return 1;
}

static int acaoPocao(Personagem *p)
{
    int cura = percentualCeil(p->vidaMax, FRACAO_ITEM);

    if (p->pocoes <= 0)
    {
        printf(">> Voce nao possui mais Pocoes!\n");
        return 0;
    }
    if (p->vidaAtual == p->vidaMax)
    {
        printf(">> Seu HP ja esta no maximo!\n");
        return 0;
    }
    if (p->manaAtual < CUSTO_POCAO)
    {
        printf(">> MP INSUFICIENTE! A Pocao exige %d MP para canalizar a cura.\n", CUSTO_POCAO);
        return 0;
    }

    p->pocoes--;
    p->manaAtual -= CUSTO_POCAO;
    p->vidaAtual = restaurar(p->vidaAtual, p->vidaMax, cura);

    printf(">> Voce bebeu uma Pocao (+%d HP, -%d MP)! HP atual: %d/%d (Restam %d pocoes).\n",
           cura, CUSTO_POCAO, p->vidaAtual, p->vidaMax, p->pocoes);
    return 1;
}

static int acaoEter(Personagem *p)
{
    int recupera = percentualCeil(p->manaMax, FRACAO_ITEM);

    if (p->eteres <= 0)
    {
        printf(">> Voce nao possui mais Eteres!\n");
        return 0;
    }
    if (p->manaAtual == p->manaMax)
    {
        printf(">> Seu MP ja esta no maximo!\n");
        return 0;
    }

    p->eteres--;
    p->manaAtual = restaurar(p->manaAtual, p->manaMax, recupera);

    printf(">> Voce usou um Eter (+%d MP)! MP atual: %d/%d (Restam %d eteres).\n",
           recupera, p->manaAtual, p->manaMax, p->eteres);
    return 1;
}

/* Retorna 1 se conseguiu fugir (combate termina); 0 caso contrario */
static int acaoFugir(Personagem *p, int ehChefe)
{
    if (ehChefe)
    {
        printf(">> Voce nao pode fugir de um General Demoniaco!\n");
        return 0;
    }

    printf(">> Voce recuou taticamente do combate! (-%d HP)\n", PENALIDADE_FUGA);
    p->vidaAtual -= PENALIDADE_FUGA;
    pausar();
    return 1;
}

static void turnoDemonio(Personagem *p, const Monstro *m)
{
    int dado = rolarDado(DADO_COMBATE);
    int ataque = dado + m->forca;
    int dano = calcularDano(ataque, p->defesa);

    p->vidaAtual -= dano;

    printf(">> %s contra-atacou! Rolou %d + forca (%d) = Ataque %d.\n",
           m->nome, dado, m->forca, ataque);
    printf(">> Voce sofreu %d de dano sombrio!\n", dano);
}

void executarCombate(Personagem *p, Monstro *m, int fase, int ehChefe)
{
    p->manaAtual = p->manaMax; /* MP restaurado no inicio de cada combate */

    printf("\n====================================================\n");
    printf(" ENCONTRO COM DEMONIO: %s\n", m->nome);
    printf("====================================================\n");

    while (p->vidaAtual > 0 && m->vidaAtual > 0)
    {
        int gastouTurno = 1;

        exibirMenuCombate(p, m);

        switch (lerAcao())
        {
        case 1:
            gastouTurno = acaoAtacar(p, m);
            break;
        case 2:
            gastouTurno = acaoHabilidade(p, m);
            break;
        case 3:
            gastouTurno = acaoPocao(p);
            break;
        case 4:
            gastouTurno = acaoEter(p);
            break;
        case 5:
            if (acaoFugir(p, ehChefe))
                return; /* fuga bem-sucedida: sem recompensa */
            gastouTurno = 0;
            break;
        case -1:
            gastouTurno = 0; /* entrada invalida: nao pune o jogador */
            break;
        default:
            printf("Acao desconhecida! O demonio aproveitou sua hesitacao.\n");
            break;
        }

        if (gastouTurno && m->vidaAtual > 0)
            turnoDemonio(p, m);
    }

    if (p->vidaAtual > 0)
    {
        printf("\n[VITORIA] Voce exorcizou %s com sucesso!\n", m->nome);
        aplicarRecompensa(p, m, fase, ehChefe);
    }
}

/* ====================================================================
 * RECOMPENSAS
 * ==================================================================== */

static void aplicarBonusPercentual(Personagem *p, double mult)
{
    p->vidaMax = (int)ceil(p->vidaMax * mult);
    p->vidaAtual = p->vidaMax;
    p->manaMax = (int)ceil(p->manaMax * mult);
    p->manaAtual = p->manaMax;
    p->forca = (int)ceil(p->forca * mult);
    p->defesa = (int)ceil(p->defesa * mult);
    p->agilidade = (int)ceil(p->agilidade * mult);
    p->inteligencia = (int)ceil(p->inteligencia * mult);
}

static void evoluirArma(Personagem *p, int indiceChefe)
{
    strcpy(p->arma, armasEvoluidas[indiceClasse(p)][indiceChefe]);
}

static void recompensaChefe(Personagem *p, int fase)
{
    /* Fase 1 -> +10% | Fase 2 -> +20% */
    int indiceChefe = (fase == 2) ? 1 : 0;
    int porcentagem = (fase == 2) ? 20 : 10;
    double mult = 1.0 + porcentagem / 100.0;

    printf("\n====================================================\n");
    printf(" RECOMPENSA DE CHEFE: ESSENCIA DEMONIACA ABSORVIDA!\n");
    printf("====================================================\n");

    aplicarBonusPercentual(p, mult);
    evoluirArma(p, indiceChefe);

    printf(">> TODOS os seus atributos aumentaram em +%d%% e seu HP/MP foram restaurados!\n",
           porcentagem);
    printf(">> NOVA ARMA EQUIPADA: %s!\n", p->arma);
    exibirStatusPersonagem(p);
    pausar();
}

static void recompensaFinal(void)
{
    printf("\n====================================================\n");
    printf(" O SENHOR DO SUBMUNDO FOI SELADO DEFINITIVAMENTE!\n");
    printf("====================================================\n");
    pausar();
}

static void recompensaComum(Personagem *p)
{
    int atributo = rand() % 5;
    int bonus = (rand() % 3) + 2;

    printf(">> Recompensa espiritual obtida!\n");

    switch (atributo)
    {
    case 0:
        p->forca += bonus;
        printf(">> EVOLUCAO: Sua Forca aumentou em +%d (Total: %d)\n", bonus, p->forca);
        break;
    case 1:
        p->defesa += bonus;
        printf(">> EVOLUCAO: Sua Defesa aumentou em +%d (Total: %d)\n", bonus, p->defesa);
        break;
    case 2:
        p->agilidade += bonus;
        printf(">> EVOLUCAO: Sua Agilidade aumentou em +%d (Total: %d)\n", bonus, p->agilidade);
        break;
    case 3:
        p->inteligencia += bonus;
        printf(">> EVOLUCAO: Sua Inteligencia aumentou em +%d (Total: %d)\n", bonus, p->inteligencia);
        break;
    default:
        p->vidaMax += bonus * 5;
        p->vidaAtual = p->vidaMax;
        printf(">> EVOLUCAO: Sua Vida Maxima aumentou! HP totalmente restaurado (%d)\n", p->vidaMax);
        break;
    }
    pausar();
}

void aplicarRecompensa(Personagem *p, Monstro *m, int fase, int ehChefe)
{
    (void)m; /* evita warning de parametro nao utilizado */

    if (!ehChefe)
        recompensaComum(p);
    else if (fase >= 3)
        recompensaFinal();
    else
        recompensaChefe(p, fase);
}

/* ====================================================================
 * EVENTOS DO TABULEIRO
 * ==================================================================== */

static void eventoChefe(Personagem *p, int fase)
{
    Monstro boss;

    printf("[CONFRONTO FINAL] Voce encontrou o general demoniaco desta regiao!\n");
    gerarMonstro(&boss, fase, 2);
    executarCombate(p, &boss, fase, 1);
}

static void eventoCombate(Personagem *p, int fase)
{
    Monstro m;

    printf("[EVENTO DE COMBATE] Um demonio selvagem apareceu para bloquear seu caminho!\n");
    gerarMonstro(&m, fase, rand() % 2);
    executarCombate(p, &m, fase, 0);
}

static void eventoTesouro(Personagem *p)
{
    printf("[EVENTO DE TESOURO] Voce encontrou um amuleto sagrado esquecido!\n");
    p->forca += 1;
    p->inteligencia += 1;
    printf(">> Sua sintonia espiritual aumentou (+1 Forca, +1 Inteligencia).\n");

    if (rand() % 2 == 0)
    {
        p->pocoes++;
        printf("Item bonus encontrado: +1 Pocao (Total: %d)!\n", p->pocoes);
    }
    else
    {
        p->eteres++;
        printf("Item bonus encontrado: +1 Eter (Total: %d)!\n", p->eteres);
    }
    pausar();
}

static void eventoCura(Personagem *p)
{
    p->vidaAtual = restaurar(p->vidaAtual, p->vidaMax, CURA_SANTUARIO);
    p->manaAtual = p->manaMax;

    printf("[EVENTO DE CURA] Voce encontrou um santuario de purificacao! HP (%d/%d) e MP (%d/%d) restaurados.\n",
           p->vidaAtual, p->vidaMax, p->manaAtual, p->manaMax);
    pausar();
}

static void eventoArmadilha(Personagem *p)
{
    p->vidaAtual -= DANO_ARMADILHA;
    printf("[ARMADILHA] Voce ativou uma runa de malandragem demoniaca! Perdeu %d de HP.\n",
           DANO_ARMADILHA);
    pausar();
}

static void eventoVazio(void)
{
    printf("[EVENTO VAZIO] Um silencio pesado e o eco de sussurros sombrios cercam o local...\n");
    pausar();
}

static void sortearEvento(Personagem *p, int fase)
{
    switch (rand() % TOTAL_EVENTOS)
    {
    case EV_COMBATE:
        eventoCombate(p, fase);
        break;
    case EV_TESOURO:
        eventoTesouro(p);
        break;
    case EV_CURA:
        eventoCura(p);
        break;
    case EV_ARMADILHA:
        eventoArmadilha(p);
        break;
    default:
        eventoVazio();
        break;
    }
}

/* Rola o d6, avanca e trava na ultima casa */
static int avancarCasa(int posicao)
{
    int dado = rolarDado(DADO_MOVIMENTO);

    posicao += dado;
    if (posicao > TAM_TABULEIRO)
        posicao = TAM_TABULEIRO;

    printf("\n----------------------------------------------------\n");
    printf("Voce avancou %d casas no territorio. Posicao atual: %d/%d\n",
           dado, posicao, TAM_TABULEIRO);
    return posicao;
}

/* ====================================================================
 * GAME LOOP DA FASE
 * ==================================================================== */

void iniciarFase(Personagem *p, const Missao *missao)
{
    int posicao = 0;

    printf("\n====================================================\n");
    printf(" FASE %d\n", missao->idFase);
    printf(" Missao: %s\n", missao->descricao);
    printf("====================================================\n");
    pausar();

    while (posicao < TAM_TABULEIRO && p->vidaAtual > 0)
    {
        posicao = avancarCasa(posicao);

        if (posicao == TAM_TABULEIRO)
            eventoChefe(p, missao->idFase);
        else
            sortearEvento(p, missao->idFase);
    }

    if (p->vidaAtual > 0)
    {
        printf("\n[REGIAO LIBERTADA] Voce concluiu com sucesso a Fase %d!\n", missao->idFase);
        pausar();
    }
}