#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "estruturas.h"
#include "funcoes.h"
#include "mecanicas.h"

/* CONSTANTES */

#define CUSTO_ESPECIAL_BASE 10

/* Criticos e falhas no d20 */
#define ACERTO_CRITICO 20
#define FALHA_CRITICA 1
#define MULT_CRITICO 2

/* Esquiva do jogador: 5% por ponto de Agilidade acima do demonio (max 50%) */
#define ESQUIVA_POR_AGI 5
#define ESQUIVA_MAX 50

/* Fuga: 50% base +/- 5% por ponto de diferenca de Agilidade (20% a 90%) */
#define FUGA_BASE 50
#define FUGA_POR_AGI 5
#define FUGA_MIN 20
#define FUGA_MAX 90

/* DADOS ESTATICOS */

/* [Classe 0-2][Chefe 0-1] */
static const Arma armas[3][2] = {
    /* Ken Matsudaira */
    {{"Katana Amaldicoada Muramasa", 0, 0, 3, 0, 0, 0},
     {"Espada Divina Ame-no-Habakiri", 0, 0, 4, 0, 1, 0}},
    /* Satoshi Hasegawa */
    {{"Pistolas de Prata Consagrada", 0, 0, 2, 0, 2, 0},
     {"Canhoes Arcanos Seraphim", 0, 0, 3, 0, 2, 0}},
    /* Hanae Suda */
    {{"Grimorio das Chamas Puras", 0, 5, 0, 0, 0, 3},
     {"Tomo Supremo Akashico", 0, 10, 0, 0, 0, 4}}};

/* [Classe 0-2][Nivel 0-2]: nivel 0 e a roupa inicial, 1 e 2 vem dos chefes */
static const Roupa roupas[3][3] = {
    /* Ken Matsudaira */
    {{"Camiseta", 0, 0, 0, 0, 0, 0, 0, 0, 0, "Nenhum"},
     {"Haori do Exterminador", 10, 0, 0, 2, 0, 0, 2, 0, 0,
      "Couraca Espiritual: -2 de dano recebido em cada ataque"},
     {"Armadura Kabuto Sagrada", 15, 0, 1, 3, 0, 0, 4, 0, 0,
      "Couraca Espiritual: -4 de dano recebido em cada ataque"}},
    /* Satoshi Hasegawa */
    {{"Casaco", 0, 0, 0, 0, 0, 0, 0, 0, 0, "Nenhum"},
     {"Sobretudo Runico", 0, 0, 0, 1, 2, 0, 0, 10, 0,
      "Passos Fantasmas: +10% de chance de esquiva"},
     {"Manto das Mil Balas", 0, 0, 1, 0, 3, 0, 0, 20, 0,
      "Passos Fantasmas: +20% de chance de esquiva"}},
    /* Hanae Suda */
    {{"Vestido", 0, 0, 0, 0, 0, 0, 0, 0, 0, "Nenhum"},
     {"Vestido de Vestal Consagrado", 5, 0, 0, 0, 0, 2, 0, 0, 2,
      "Fluxo Arcano: habilidade especial custa 2 MP a menos"},
     {"Veste Astral das Chamas Puras", 0, 10, 0, 0, 0, 3, 0, 0, 4,
      "Fluxo Arcano: habilidade especial custa 4 MP a menos"}}};

/* ====================================================================
 * CLASSE E ROUPA
 * ==================================================================== */

/* Indice da classe (0 Ken, 1 Satoshi, 2 Hanae) a partir do nome */
static int indiceClasse(const Personagem *p)
{
    if (strcmp(p->nome, "Satoshi Hasegawa") == 0)
        return 1;
    if (strcmp(p->nome, "Hanae Suda") == 0)
        return 2;
    return 0;
}

/* Roupa atualmente equipada (assume a inicial se o nome nao for encontrado) */
static const Roupa *roupaEquipada(const Personagem *p)
{
    int classe = indiceClasse(p);
    int i;

    for (i = 0; i < 3; i++)
    {
        if (strcmp(p->roupa, roupas[classe][i].nome) == 0)
            return &roupas[classe][i];
    }
    return &roupas[classe][0];
}

int custoEspecial(const Personagem *p)
{
    return limitar(CUSTO_ESPECIAL_BASE - roupaEquipada(p)->reducaoCustoEspecial,
                   1, CUSTO_ESPECIAL_BASE);
}

int reducaoDanoRoupa(const Personagem *p)
{
    return roupaEquipada(p)->reducaoDano;
}

/* ATAQUE, ESQUIVA E FUGA */

/* Dano = ataque - defesa, com minimo garantido de 1 */
static int calcularDano(double ataque, int defesa)
{
    return (int)fmax(1.0, ataque - defesa);
}

int resolverAtaque(int dado, double ataque, int defesa, int *critico)
{
    int dano;

    *critico = 0;
    if (dado == FALHA_CRITICA)
        return 0;

    dano = calcularDano(ataque, defesa);
    if (dado == ACERTO_CRITICO)
    {
        dano *= MULT_CRITICO;
        *critico = 1;
    }
    return dano;
}

int chanceEsquiva(const Personagem *p, const Monstro *m)
/*Utiliza uma operação para */
{
    int chance = ESQUIVA_POR_AGI * (p->agilidade - m->agilidade) + roupaEquipada(p)->esquivaExtra;

    return limitar(chance, 0, ESQUIVA_MAX);
}

int chanceFuga(const Personagem *p, const Monstro *m)
{
    return limitar(FUGA_BASE + FUGA_POR_AGI * (p->agilidade - m->agilidade), FUGA_MIN, FUGA_MAX);
}

int sortearChance(int chance)
{
    return (rand() % 100) < chance;
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

/* Soma bonus fixos aos atributos e restaura HP/MP */
static void aplicarBonusFixo(Personagem *p, int vida, int mana, int forca,
                             int defesa, int agilidade, int inteligencia)
{
    p->vidaMax += vida;
    p->vidaAtual = p->vidaMax;
    p->manaMax += mana;
    p->manaAtual = p->manaMax;
    p->forca += forca;
    p->defesa += defesa;
    p->agilidade += agilidade;
    p->inteligencia += inteligencia;
}

/* Imprime apenas os bonus diferentes de zero */
static void exibirBonusFixo(const char *origem, int vida, int mana, int forca,
                            int defesa, int agilidade, int inteligencia)
{
    printf(">> Bonus da %s:", origem);
    if (vida)
        printf(" +%d HP", vida);
    if (mana)
        printf(" +%d MP", mana);
    if (forca)
        printf(" +%d Forca", forca);
    if (defesa)
        printf(" +%d Defesa", defesa);
    if (agilidade)
        printf(" +%d Agilidade", agilidade);
    if (inteligencia)
        printf(" +%d Inteligencia", inteligencia);
    printf("\n");
}

/* Equipa a arma do chefe (0 ou 1) e aplica os bonus fixos */
static void evoluirArma(Personagem *p, int indiceChefe)
{
    const Arma *a = &armas[indiceClasse(p)][indiceChefe];

    strcpy(p->arma, a->nome);
    aplicarBonusFixo(p, a->vida, a->mana, a->forca, a->defesa, a->agilidade, a->inteligencia);

    printf(">> NOVA ARMA EQUIPADA: %s!\n", a->nome);
    exibirBonusFixo("arma", a->vida, a->mana, a->forca, a->defesa, a->agilidade, a->inteligencia);
}

/* Equipa a roupa do nivel (indiceChefe + 1) e aplica os bonus fixos */
static void evoluirRoupa(Personagem *p, int indiceChefe)
{
    const Roupa *r = &roupas[indiceClasse(p)][indiceChefe + 1];

    strcpy(p->roupa, r->nome);
    aplicarBonusFixo(p, r->vida, r->mana, r->forca, r->defesa, r->agilidade, r->inteligencia);

    printf(">> NOVA VESTIMENTA EQUIPADA: %s!\n", r->nome);
    exibirBonusFixo("roupa", r->vida, r->mana, r->forca, r->defesa, r->agilidade, r->inteligencia);
    printf(">> EFEITO ESPECIAL: %s\n", r->efeito);
}

static void recompensaChefe(Personagem *p, int fase)
{
    /* Fase 1 -> +10% | Fase 2 -> +20% */
    int indiceChefe = (fase == 2) ? 1 : 0;
    int porcentagem = (fase == 2) ? 20 : 10;
    double mult = 1.0 + porcentagem / 100.0;

    exibirBanner("RECOMPENSA DE CHEFE: ESSENCIA DEMONIACA ABSORVIDA!");

    aplicarBonusPercentual(p, mult);

    printf(">> TODOS os seus atributos aumentaram em +%d%% e seu HP/MP foram restaurados!\n",
           porcentagem);

    /* Os bonus fixos de arma e roupa entram depois do percentual, sem serem multiplicados */
    evoluirArma(p, indiceChefe);
    evoluirRoupa(p, indiceChefe);

    exibirStatusPersonagem(p);
    pausar();
}

static void recompensaFinal(void)
{
    exibirBanner("O SENHOR DO SUBMUNDO FOI SELADO DEFINITIVAMENTE!");
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