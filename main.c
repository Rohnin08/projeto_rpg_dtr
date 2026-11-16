#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "estruturas/estruturas.h"
#include "funcoes/funcoes.h"
#include "mecanicas/mecanicas.h"
#include "combate/combate.h"

#define TOTAL_MISSOES 3
#define DADO_MOVIMENTO 6
#define CURA_SANTUARIO 25
#define CASAS_SORTEAVEIS 5 /* CASA_VAZIA ate CASA_ARMADILHA */

/* ====================================================================
 * TABULEIRO: GERACAO E EVENTOS
 * ==================================================================== */

/* Preenche o vetor de casas: eventos sorteados e o chefe na ultima casa */
static void gerarTabuleiro(Casa tabuleiro[])
{
    int i;

    for (i = 0; i < TAM_TABULEIRO; i++)
    {
        tabuleiro[i].idCasa = i + 1;

        if (i == TAM_TABULEIRO - 1)
            tabuleiro[i].tipo = CASA_CHEFE;
        else
            tabuleiro[i].tipo = (TipoCasa)(rand() % CASAS_SORTEAVEIS);
    }
}

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
    int dano = configDificuldade()->danoArmadilha;

    receberDano(p, dano);
    printf("[ARMADILHA] Voce ativou uma runa de malandragem demoniaca! Perdeu %d de HP.\n", dano);
    pausar();
}

static void eventoVazio(void)
{
    printf("[EVENTO VAZIO] Um silencio pesado e o eco de sussurros sombrios cercam o local...\n");
    pausar();
}

/* Executa o evento correspondente ao tipo da casa */
static void executarEvento(Personagem *p, const Casa *casa, int fase)
{
    switch (casa->tipo)
    {
    case CASA_COMBATE:
        eventoCombate(p, fase);
        break;
    case CASA_TESOURO:
        eventoTesouro(p);
        break;
    case CASA_CURA:
        eventoCura(p);
        break;
    case CASA_ARMADILHA:
        eventoArmadilha(p);
        break;
    case CASA_CHEFE:
        eventoChefe(p, fase);
        break;
    case CASA_VAZIA:
    default:
        eventoVazio();
        break;
    }
}

/* ====================================================================
 * TABULEIRO: PERCURSO DA FASE
 * ==================================================================== */

/* Rola o d6, avanca e trava na ultima casa, só avança quando matar o chefão */
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

static void iniciarFase(Personagem *p, const Missao *missao)
{
    Casa tabuleiro[TAM_TABULEIRO];
    int posicao = 0;

    gerarTabuleiro(tabuleiro);

    exibirBanner("INICIO DE FASE");
    printf(" FASE %d\n Missao: %s\n", missao->idFase, missao->descricao);
    exibirTabuleiro(tabuleiro, TAM_TABULEIRO, posicao);
    pausar();

    while (posicao < TAM_TABULEIRO && p->vidaAtual > 0)
    {
        posicao = avancarCasa(posicao);
        exibirTabuleiro(tabuleiro, TAM_TABULEIRO, posicao);
        executarEvento(p, &tabuleiro[posicao - 1], missao->idFase);
    }

    if (p->vidaAtual > 0)
    {
        printf("\n[REGIAO LIBERTADA] Voce concluiu com sucesso a Fase %d!\n", missao->idFase);
        pausar();
    }
}

/* ====================================================================
 * MENUS E CAMPANHA
 * ==================================================================== */

/* Insiste ate o usuario escolher um exterminador valido */
static void selecionarExterminador(Personagem *jogador)
{
    int valido = 0;

    while (!valido)
    {
        exibirMenuSelecao();
        valido = escolherPersonagem(jogador, lerInteiro());

        if (!valido)
        {
            printf("\nPersonagem fora do escopo! Tente novamente (1 a %d).\n", TOTAL_PERSONAGENS);
        }
    }

    printf("\n>> Personagem selecionado com sucesso!\n");
    exibirStatusPersonagem(jogador);
    pausar();
}

/* Insiste ate o usuario escolher uma dificuldade valida */
static void selecionarDificuldade(void)
{
    int escolha;

    do
    {
        exibirMenuDificuldade();
        escolha = lerInteiro();

        if (escolha < DIF_FACIL || escolha > DIF_DIFICIL)
        {
            printf("\nDificuldade invalida! Tente novamente (1 a 3).\n");
        }
    } while (escolha < DIF_FACIL || escolha > DIF_DIFICIL);

    definirDificuldade(escolha);
    printf("\n>> Dificuldade selecionada: %s\n", nomeDificuldade());
    pausar();
}

/* Executa as 3 missoes em sequencia; retorna quantas foram concluidas */
static int executarCampanha(Personagem *jogador)
{
    Missao missoes[TOTAL_MISSOES] = {
        {1, "Beco das Almas Perdidas - Encontrar a fenda principal e derrotar Malphas.", 0},
        {2, "Ruinas do Purgatorio - Desbancar as legioes medias e vencer Astaroth.", 0},
        {3, "Trono do Lorde Demonio - Enfrentar Satanael e salvar o mundo!", 0}};

    int concluidas = 0;

    for (int i = 0; i < TOTAL_MISSOES; i++)
    {
        iniciarFase(jogador, &missoes[i]);

        if (jogador->vidaAtual <= 0)
        {
            break; /* o jogador morreu: encerra a campanha */
        }

        missoes[i].concluida = 1;
        concluidas++;
    }

    return concluidas;
}

static void exibirDesfecho(const Personagem *jogador, int missoesConcluidas)
{
    if (jogador->vidaAtual > 0)
    {
        exibirBanner("PARABENS, EXTERMINADOR!");
        printf(" Voce derrotou Satanael e fechou os portoes do inferno!\n");
        printf(" Missoes concluidas: %d/%d\n", missoesConcluidas, TOTAL_MISSOES);
    }
    else
    {
        exibirBanner("GAME OVER: SUA ALMA FOI CONSUMIDA PELAS TREVAS...");
        printf(" O submundo prevalecera nesta era. (Missoes: %d/%d)\n",
               missoesConcluidas, TOTAL_MISSOES);
    }
    exibirStatusPersonagem(jogador);
    pausar();
}

int main(void)
{
    /* Semente para dados e eventos aleatorios */
    srand((unsigned int)time(NULL));

    Personagem jogador;
    int opcaoMenu;

    do
    {
        exibirMenuPrincipal();
        opcaoMenu = lerInteiro();

        if (opcaoMenu == 1)
        {
            selecionarExterminador(&jogador);
            selecionarDificuldade();
            int concluidas = executarCampanha(&jogador);
            exibirDesfecho(&jogador, concluidas);
        }
        else if (opcaoMenu == 2)
        {
            printf("\nEncerrando Exterminadores do Inferno. Ate logo!\n");
        }
        else
        {
            printf("\nOpcao desconhecida! Tente novamente.\n");
        }

    } while (opcaoMenu != 2);

    return 0;
}