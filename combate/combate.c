#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "estruturas.h"
#include "funcoes.h"
#include "mecanicas.h"
#include "combate.h"

/* CONSTANTES */

#define CUSTO_POCAO 5
#define PENALIDADE_FUGA 15
#define FRACAO_ITEM 0.25
#define FRACAO_DEFESA_MP 0.10
#define MULT_ESPECIAL 1.5
#define DADO_COMBATE 20

enum
{
    FUGA_BLOQUEADA, /* nao gasta o turno */
    FUGA_FALHOU,    /* gasta o turno e custa HP */
    FUGA_SUCESSO    /* encerra o combate */
};

/* DIFICULDADE */

static const ConfigDificuldade configsDificuldade[3] = {
    {"Facil", 1.15, 0.85, 8},
    {"Normal", 1.25, 1.00, 12},
    {"Dificil", 1.40, 1.20, 16}};

static int dificuldadeAtual = DIF_NORMAL;

void definirDificuldade(int dificuldade)
{
    dificuldadeAtual = limitar(dificuldade, DIF_FACIL, DIF_DIFICIL);
}

const ConfigDificuldade *configDificuldade(void)
{
    return &configsDificuldade[dificuldadeAtual - 1];
}

const char *nomeDificuldade(void)
{
    return configDificuldade()->nome;
}

/* MONSTROS */

/* Ordem: {vidaBase, forca, defesa, agilidade} */
static const int atributosBase[3][3][4] = {
    /* Fase 1 */
    {{25, 8, 4, 5}, {35, 11, 6, 8}, {60, 14, 8, 10}},
    /* Fase 2 */
    {{55, 15, 9, 6}, {65, 18, 10, 14}, {90, 22, 12, 12}},
    /* Fase 3 */
    {{85, 24, 14, 9}, {110, 28, 18, 4}, {150, 35, 20, 15}}};

static const char *nomesMonstros[3][3] = {
    /* Fase 1 */
    {"Diabrete", "Possuido Infernal", "Malphas, O Vigia das Fendas"},
    /* Fase 2 */
    {"Hellhound", "Sucubo", "Astaroth, O Duque das Cinzas"},
    /* Fase 3 */
    {"Berserker Abissal", "Guardiao de Obsidiana", "Satanael, O Principe do Submundo"}};

void gerarMonstro(Monstro *m, int fase, int indice)
{
    int fIdx = limitar(fase - 1, 0, 2);
    int mIdx = limitar(indice, 0, 2);
    double fator = pow(configDificuldade()->escalaVida, fIdx);

    strcpy(m->nome, nomesMonstros[fIdx][mIdx]);

    m->vidaMax = (int)(atributosBase[fIdx][mIdx][0] * fator);
    m->vidaAtual = m->vidaMax;
    m->forca = atributosBase[fIdx][mIdx][1];
    m->defesa = atributosBase[fIdx][mIdx][2];
    m->agilidade = atributosBase[fIdx][mIdx][3];
}

/* INICIATIVA E MENU */

/* Define quem age primeiro: maior Agilidade; empate decidido no d20.
 * Retorna 1 se o demonio age primeiro. */
static int monstroAgePrimeiro(const Personagem *p, const Monstro *m)
{
    int primeiro;

    printf(">> INICIATIVA: Agilidade %d (voce) x %d (%s)", p->agilidade, m->agilidade, m->nome);

    if (p->agilidade != m->agilidade)
    {
        primeiro = m->agilidade > p->agilidade;
        printf(".\n");
    }
    else
    {
        int dadoJogador, dadoMonstro;

        do
        {
            dadoJogador = rolarDado(DADO_COMBATE);
            dadoMonstro = rolarDado(DADO_COMBATE);
        } while (dadoJogador == dadoMonstro);

        primeiro = dadoMonstro > dadoJogador;
        printf(" - empate! Desempate no d20: %d (voce) x %d (demonio).\n", dadoJogador, dadoMonstro);
    }

    if (primeiro)
        printf(">> %s e mais rapido e age primeiro!\n", m->nome);
    else
        printf(">> Voce e mais rapido e age primeiro!\n");

    return primeiro;
}

static void exibirMenuCombate(const Personagem *p, const Monstro *m, int ehChefe)
{
    printf("\n--- Status do Combate ---\n");
    printf("Seu HP: %d/%d | Seu MP: %d/%d | %s HP: %d/%d\n",
           p->vidaAtual, p->vidaMax, p->manaAtual, p->manaMax,
           m->nome, m->vidaAtual, m->vidaMax);
    printf("Chance de esquiva: %d%%\n", chanceEsquiva(p, m));
    printf("[1] Atacar Normal\n");
    printf("[2] Usar Habilidade Especial (%s) [Custo: %d MP]\n",
           p->habilidade, custoEspecial(p));
    printf("[3] Usar Pocao (+%d HP / 25%%) [Custo: %d MP] [Restantes: %d]\n",
           percentualCeil(p->vidaMax, FRACAO_ITEM), CUSTO_POCAO, p->pocoes);
    printf("[4] Usar Eter (+%d MP / 25%%) [Restantes: %d]\n",
           percentualCeil(p->manaMax, FRACAO_ITEM), p->eteres);
    if (ehChefe)
        printf("[5] Tentar Fugir [BLOQUEADO contra Chefes]\n");
    else
        printf("[5] Tentar Fugir [Chance: %d%%]\n", chanceFuga(p, m));
    printf("[6] Defender (dano pela metade + %d MP)\n",
           percentualCeil(p->manaMax, FRACAO_DEFESA_MP));
    printf("Escolha sua acao: ");
}

/* ACOES DO JOGADOR */
// Cada acao retorna 1 se gastou o turno, 0 se o jogador deve escolher de novo

static int acaoAtacar(Personagem *p, Monstro *m)
{
    int dado = rolarDado(DADO_COMBATE);
    int ataqueTotal = dado + p->forca;
    int critico;
    int dano = resolverAtaque(dado, ataqueTotal, m->defesa, &critico);

    printf(">> Voce atacou! Rolou %d no d20 + forca (%d) = Ataque %d.\n",
           dado, p->forca, ataqueTotal);

    if (dano == 0)
    {
        printf(">> FALHA CRITICA! Seu golpe errou o alvo!\n");
        return 1;
    }

    if (critico)
        printf(">> ACERTO CRITICO! Dano dobrado!\n");

    m->vidaAtual -= dano;
    printf(">> Causou %d de dano espiritual em %s!\n", dano, m->nome);
    return 1;
}

static int acaoHabilidade(Personagem *p, Monstro *m)
{
    int custo = custoEspecial(p);
    int dado, ataqueTotal, dano, critico;

    if (p->manaAtual < custo)
    {
        printf(">> MP INSUFICIENTE! Voce nao tem energia espiritual para usar '%s'!\n",
               p->habilidade);
        return 0;
    }

    p->manaAtual -= custo;

    dado = rolarDado(DADO_COMBATE);
    ataqueTotal = dado + p->inteligencia + p->forca;
    dano = resolverAtaque(dado, ataqueTotal * MULT_ESPECIAL, m->defesa, &critico);

    printf(">> Usou '%s' (-%d MP)! Rolou %d.\n", p->habilidade, custo, dado);

    if (dano == 0)
    {
        printf(">> FALHA CRITICA! A energia espiritual se dispersou sem atingir o alvo!\n");
        return 1;
    }

    if (critico)
        printf(">> ACERTO CRITICO! Dano dobrado!\n");

    m->vidaAtual -= dano;
    printf(">> Dano critico de exorcismo de %d em %s!\n", dano, m->nome);
    return 1;
}

static int acaoPocao(Personagem *p)
/*Emplementa a ação de tomar a poção, decrementando 1 para cada poção constumida*/
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
/*Emplementa a ação de tomar o eter, decrementando 1 para cada eter constumido*/
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

static int acaoFugir(Personagem *p, const Monstro *m, int ehChefe)
/* Fuga por chance (baseada na Agilidade). Falhar custa HP e o turno. */
{
    int chance;

    if (ehChefe)
    {
        printf(">> Voce nao pode fugir de um General Demoniaco!\n");
        return FUGA_BLOQUEADA;
    }

    chance = chanceFuga(p, m);

    if (sortearChance(chance))
    {
        printf(">> Voce recuou taticamente e escapou do combate! (chance de %d%%)\n", chance);
        pausar();
        return FUGA_SUCESSO;
    }

    printf(">> A fuga falhou! %s bloqueou sua retirada! (-%d HP)\n", m->nome, PENALIDADE_FUGA);
    p->vidaAtual -= PENALIDADE_FUGA;
    return FUGA_FALHOU;
}

static int acaoDefender(Personagem *p, int *defendendo)
/* Defender: o proximo ataque do demonio causa metade do dano e recupera um pouco de MP */
{
    int mp = percentualCeil(p->manaMax, FRACAO_DEFESA_MP);

    p->manaAtual = restaurar(p->manaAtual, p->manaMax, mp);
    *defendendo = 1;

    printf(">> Voce assumiu postura defensiva! (+%d MP, MP atual: %d/%d)\n",
           mp, p->manaAtual, p->manaMax);
    printf(">> O proximo ataque de inimigo causara metade do dano.\n");
    return 1;
}

/* TURNO DO DEMONIO */

static void turnoDemonio(Personagem *p, const Monstro *m, int *defendendo)
{
    int dado = rolarDado(DADO_COMBATE);
    int ataque = dado + m->forca;
    int critico, dano;

    printf(">> %s atacou! Rolou %d + forca (%d) = Ataque %d.\n",
           m->nome, dado, m->forca, ataque);

    if (sortearChance(chanceEsquiva(p, m)))
    {
        printf(">> Voce ESQUIVOU do ataque com sua agilidade!\n");
        *defendendo = 0;
        return;
    }

    dano = resolverAtaque(dado, ataque, p->defesa, &critico);

    if (dano == 0)
    {
        printf(">> FALHA CRITICA! %s errou o golpe!\n", m->nome);
    }
    else
    {
        dano = (int)ceil(dano * configDificuldade()->danoMonstro);

        if (reducaoDanoRoupa(p) > 0)
        {
            dano = (int)fmax(1.0, (double)(dano - reducaoDanoRoupa(p)));
            printf(">> Sua vestimenta absorveu parte do impacto!\n");
        }

        if (critico)
            printf(">> ACERTO CRITICO do demonio! Dano dobrado!\n");

        if (*defendendo)
        {
            dano = (dano + 1) / 2;
            printf(">> Sua postura defensiva reduziu o dano pela metade!\n");
        }

        p->vidaAtual -= dano;
        printf(">> Voce sofreu %d de dano sombrio!\n", dano);
    }

    *defendendo = 0;
}

/* LOOP DE COMBATE */

void executarCombate(Personagem *p, Monstro *m, int fase, int ehChefe)
/*Função que execulta a mecanica de combate*/
{
    int defendendo = 0;
    int novaRodada = 1;
    int demonioPrimeiro;

    p->manaAtual = p->manaMax; /* MP restaurado no inicio de cada combate */

    printf("\n====================================================\n");
    printf(" ENCONTRO COM DEMONIO: %s\n", m->nome);
    printf("====================================================\n");

    demonioPrimeiro = monstroAgePrimeiro(p, m);

    while (p->vidaAtual > 0 && m->vidaAtual > 0)
    {
        int gastouTurno = 1;

        /* Se o demonio e mais rapido, ele ataca no inicio de cada rodada */
        if (novaRodada)
        {
            novaRodada = 0;
            if (demonioPrimeiro)
            {
                turnoDemonio(p, m, &defendendo);
                if (p->vidaAtual <= 0)
                    break;
            }
        }

        exibirMenuCombate(p, m, ehChefe);

        switch (lerInteiro())
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
        {
            int fuga = acaoFugir(p, m, ehChefe);

            if (fuga == FUGA_SUCESSO)
                return; /* fuga bem-sucedida: sem recompensa */
            gastouTurno = (fuga == FUGA_FALHOU);
            break;
        }
        case 6:
            gastouTurno = acaoDefender(p, &defendendo);
            break;
        default:
            printf("Acao desconhecida! O demonio aproveitou sua hesitacao.\n");
            break;
        }

        if (gastouTurno)
        {
            /* Se o jogador e mais rapido, o demonio responde apos a acao */
            if (!demonioPrimeiro && m->vidaAtual > 0 && p->vidaAtual > 0)
                turnoDemonio(p, m, &defendendo);
            novaRodada = 1;
        }
    }

    if (p->vidaAtual > 0)
    {
        printf("\n[VITORIA] Voce exorcizou %s com sucesso!\n", m->nome);
        aplicarRecompensa(p, m, fase, ehChefe);
    }
}