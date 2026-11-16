#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "estruturas.h"
#include "funcoes.h"
#include "mecanicas.h"
#include "combate.h"

/* ====================================================================
 * CONSTANTES
 * ==================================================================== */

#define CUSTO_POCAO 5 /* a documentacao preve 5 MP; use 0 para desativar */
#define PENALIDADE_FUGA 15
#define FRACAO_ITEM 0.25
#define FRACAO_DEFESA_MP 0.10 /* MP recuperado ao Defender */
#define MULT_ESPECIAL 1.5
#define DADO_COMBATE 20
#define TIROS_RAJADA 3 /* disparos da habilidade do Satoshi */

/* Resultados possiveis de uma tentativa de fuga */
enum
{
    FUGA_BLOQUEADA, /* nao gasta o turno */
    FUGA_FALHOU,    /* gasta o turno e custa HP */
    FUGA_SUCESSO    /* encerra o combate */
};

/* ====================================================================
 * DIFICULDADE
 * ==================================================================== */

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

/* ====================================================================
 * MONSTROS
 * ==================================================================== */

/* Ordem: {vidaBase, forca, defesa, agilidade, xpRecompensa} */
static const int atributosBase[3][3][5] = {
    /* Fase 1 */
    {{30, 8, 4, 5, 20}, {45, 11, 6, 8, 30}, {70, 14, 8, 10, 100}},
    /* Fase 2 */
    {{60, 15, 9, 6, 45}, {75, 18, 10, 14, 55}, {100, 22, 12, 12, 160}},
    /* Fase 3 */
    {{90, 24, 14, 9, 80}, {120, 28, 18, 4, 95}, {180, 35, 20, 15, 300}}};

static const char *nomesMonstros[3][3] = {
    /* Fase 1 */
    {"Diabrete", "Possuido Infernal", "Malphas, O Vigia das Fendas"},
    /* Fase 2 */
    {"Hellhound", "Sucubo", "Astaroth, O Duque das Cinzas"},
    /* Fase 3 */
    {"Berserker Abissal", "Guardiao de Obsidiana", "Satanael, O Principe do Submundo"}};

static const char *descricoesMonstros[3][3] = {
    /* Fase 1 */
    {"Agil e esguio, especializado em emboscadas rapidas.",
     "Humano mutado por um parasita demoniaco, com forca bruta elevada.",
     "Demonio alado com cranio de corvo que comanda a abertura dos portais."},
    /* Fase 2 */
    {"Fera infernal envolta em chamas negras com alto poder de ataque.",
     "Entidade ilusoria de alta agilidade e ataques precisos.",
     "General demoniaco de armadura pesada e poder destrutivo elevado."},
    /* Fase 3 */
    {"Tita de ferro vulcanico movido por furia destrutiva.",
     "Constructo colossal de rocha infernal com defesa massiva.",
     "O soberano supremo do inferno: poder maximo e resistencia extrema."}};

void gerarMonstro(Monstro *m, int fase, int indice)
{
    int fIdx = limitar(fase - 1, 0, 2);
    int mIdx = limitar(indice, 0, 2);
    double fator = pow(configDificuldade()->escalaVida, fIdx);

    strcpy(m->nome, nomesMonstros[fIdx][mIdx]);
    strcpy(m->descricao, descricoesMonstros[fIdx][mIdx]);

    /* Nivel: 1,1,2 na fase 1; 3,3,4 na fase 2; 5,5,6 na fase 3 */
    m->nivel = 2 * fIdx + 1 + (mIdx == 2);

    m->vidaMax = (int)ceil(atributosBase[fIdx][mIdx][0] * fator);
    m->vidaAtual = m->vidaMax;
    m->forca = atributosBase[fIdx][mIdx][1];
    m->defesa = atributosBase[fIdx][mIdx][2];
    m->agilidade = atributosBase[fIdx][mIdx][3];
    m->xpRecompensa = atributosBase[fIdx][mIdx][4];
}

/* ====================================================================
 * INICIATIVA E MENU
 * ==================================================================== */

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

/* Descricao curta do efeito da habilidade especial de cada classe */
static const char *descricaoHabilidade(const Personagem *p)
{
    switch (p->tipo)
    {
    case KEN:
        return "ignora 50% da defesa do alvo";
    case SATOSHI:
        return "3 disparos, cada um com seu d20";
    default:
        return "cura metade do dano causado";
    }
}

static void exibirMenuCombate(const Personagem *p, const Monstro *m, int ehChefe)
{
    printf("\n--- Status do Combate ---\n");
    printf("Seu HP: %d/%d | Seu MP: %d/%d | %s HP: %d/%d\n",
           p->vidaAtual, p->vidaMax, p->manaAtual, p->manaMax,
           m->nome, m->vidaAtual, m->vidaMax);
    printf("Chance de esquiva: %d%%\n", chanceEsquiva(p, m));
    printf("[%d] Atacar Normal\n", ACAO_ATACAR);
    printf("[%d] Habilidade Especial: %s (%s) [Custo: %d MP]\n",
           ACAO_HABILIDADE, p->habilidade, descricaoHabilidade(p), custoEspecial(p));
    printf("[%d] Usar Pocao (+%d HP / 25%%) [Custo: %d MP] [Restantes: %d]\n",
           ACAO_POCAO, percentualCeil(p->vidaMax, FRACAO_ITEM), CUSTO_POCAO, p->pocoes);
    printf("[%d] Usar Eter (+%d MP / 25%%) [Restantes: %d]\n",
           ACAO_ETER, percentualCeil(p->manaMax, FRACAO_ITEM), p->eteres);
    if (ehChefe)
        printf("[%d] Tentar Fugir [BLOQUEADO contra Chefes]\n", ACAO_FUGIR);
    else
        printf("[%d] Tentar Fugir [Chance: %d%%]\n", ACAO_FUGIR, chanceFuga(p, m));
    printf("[%d] Defender (dano pela metade + %d MP)\n",
           ACAO_DEFENDER, percentualCeil(p->manaMax, FRACAO_DEFESA_MP));
    printf("Escolha sua acao: ");
}

/* ====================================================================
 * ACOES DO JOGADOR
 * Cada acao retorna 1 se gastou o turno, 0 se o jogador deve escolher de novo
 * ==================================================================== */

/* Mostra o resultado de um golpe e aplica o dano. Retorna o dano causado. */
static int aplicarGolpe(Monstro *m, int dano, int critico)
{
    if (dano == 0)
    {
        printf(">> FALHA CRITICA! O golpe errou o alvo!\n");
        return 0;
    }

    if (critico)
        printf(">> ACERTO CRITICO! Dano dobrado!\n");

    danoNoMonstro(m, dano);
    printf(">> Causou %d de dano em %s!\n", dano, m->nome);
    return dano;
}

static int acaoAtacar(Personagem *p, Monstro *m)
{
    int dado = rolarDado(DADO_COMBATE);
    int ataqueTotal = dado + p->forca;
    int critico;
    int dano = resolverAtaque(dado, ataqueTotal, m->defesa, &critico);

    printf(">> Voce atacou! Rolou %d no d20 + forca (%d) = Ataque %d.\n",
           dado, p->forca, ataqueTotal);
    aplicarGolpe(m, dano, critico);
    return 1;
}

/* Ken: golpe devastador que ignora metade da defesa do alvo */
static void habilidadeKen(Personagem *p, Monstro *m)
{
    int dado = rolarDado(DADO_COMBATE);
    double ataque = (dado + p->forca + p->inteligencia) * MULT_ESPECIAL;
    int critico;
    int dano = resolverAtaque(dado, ataque, m->defesa / 2, &critico);

    printf(">> Rolou %d no d20. A lamina atravessa metade da defesa do demonio!\n", dado);
    aplicarGolpe(m, dano, critico);
}

/* Satoshi: rajada de disparos, cada um com seu proprio d20 (critico e falha por tiro) */
static void habilidadeSatoshi(Personagem *p, Monstro *m)
{
    int i;

    for (i = 1; i <= TIROS_RAJADA && m->vidaAtual > 0; i++)
    {
        int dado = rolarDado(DADO_COMBATE);
        int ataque = dado + p->forca + p->agilidade / 4;
        int critico;
        int dano = resolverAtaque(dado, ataque, m->defesa, &critico);

        printf(">> Disparo %d: d20 = %d, ataque %d.\n", i, dado, ataque);
        aplicarGolpe(m, dano, critico);
    }
}

/* Hanae: chamas sagradas que causam dano e curam metade do dano causado */
static void habilidadeHanae(Personagem *p, Monstro *m)
{
    int dado = rolarDado(DADO_COMBATE);
    double ataque = (dado + p->inteligencia + p->forca) * MULT_ESPECIAL;
    int critico;
    int dano = resolverAtaque(dado, ataque, m->defesa, &critico);
    int causado;

    printf(">> Rolou %d no d20. Chamas sagradas envolvem o demonio!\n", dado);
    causado = aplicarGolpe(m, dano, critico);

    if (causado > 0)
    {
        int cura = (causado / 2 > 0) ? causado / 2 : 1;

        p->vidaAtual = restaurar(p->vidaAtual, p->vidaMax, cura);
        printf(">> As chamas purificam sua alma: +%d HP (HP atual: %d/%d)\n",
               cura, p->vidaAtual, p->vidaMax);
    }
}

static int acaoHabilidade(Personagem *p, Monstro *m)
{
    int custo = custoEspecial(p);

    if (p->manaAtual < custo)
    {
        printf(">> MP INSUFICIENTE! Voce nao tem energia espiritual para usar '%s'!\n",
               p->habilidade);
        return 0;
    }

    p->manaAtual -= custo;
    printf(">> Usou '%s' (-%d MP)!\n", p->habilidade, custo);

    switch (p->tipo)
    {
    case KEN:
        habilidadeKen(p, m);
        break;
    case SATOSHI:
        habilidadeSatoshi(p, m);
        break;
    default:
        habilidadeHanae(p, m);
        break;
    }
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

/* Fuga por chance (baseada na Agilidade). Falhar custa HP e o turno. */
static int acaoFugir(Personagem *p, const Monstro *m, int ehChefe)
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
    receberDano(p, PENALIDADE_FUGA);
    return FUGA_FALHOU;
}

/* Defender: o proximo ataque do demonio causa metade do dano e recupera um pouco de MP */
static int acaoDefender(Personagem *p, int *defendendo)
{
    int mp = percentualCeil(p->manaMax, FRACAO_DEFESA_MP);

    p->manaAtual = restaurar(p->manaAtual, p->manaMax, mp);
    *defendendo = 1;

    printf(">> Voce assumiu postura defensiva! (+%d MP, MP atual: %d/%d)\n",
           mp, p->manaAtual, p->manaMax);
    printf(">> O proximo ataque de inimigo causara metade do dano.\n");
    return 1;
}

/* ====================================================================
 * TURNO DO DEMONIO
 * ==================================================================== */

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

        receberDano(p, dano);
        printf(">> Voce sofreu %d de dano sombrio!\n", dano);
    }

    *defendendo = 0;
}

/* ====================================================================
 * LOOP DE COMBATE
 * ==================================================================== */

void executarCombate(Personagem *p, Monstro *m, int fase, int ehChefe)
{
    int defendendo = 0;
    int novaRodada = 1;
    int demonioPrimeiro;

    p->manaAtual = p->manaMax; /* MP restaurado no inicio de cada combate */

    printf("\n====================================================\n");
    printf(" ENCONTRO COM DEMONIO: %s (Nivel %d)\n", m->nome, m->nivel);
    printf(" %s\n", m->descricao);
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
        case ACAO_ATACAR:
            gastouTurno = acaoAtacar(p, m);
            break;
        case ACAO_HABILIDADE:
            gastouTurno = acaoHabilidade(p, m);
            break;
        case ACAO_POCAO:
            gastouTurno = acaoPocao(p);
            break;
        case ACAO_ETER:
            gastouTurno = acaoEter(p);
            break;
        case ACAO_FUGIR:
        {
            int fuga = acaoFugir(p, m, ehChefe);

            if (fuga == FUGA_SUCESSO)
                return; /* fuga bem-sucedida: sem recompensa */
            gastouTurno = (fuga == FUGA_FALHOU);
            break;
        }
        case ACAO_DEFENDER:
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