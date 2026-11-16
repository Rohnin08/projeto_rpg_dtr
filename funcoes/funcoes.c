#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "estruturas.h"
#include "funcoes.h"

#define POCOES_INICIAIS 3
#define ETERES_INICIAIS 3

/* ====================================================================
 * DADOS ESTATICOS
 * vidaAtual, manaAtual e inventario sao definidos em escolherPersonagem
 * ==================================================================== */

static const Personagem modelos[TOTAL_PERSONAGENS] = {
    {.nome = "Ken Matsudaira",
     .tipo = KEN,
     .classe = "Espadachim sobrenatural",
     .roupa = "Camiseta",
     .arma = "Katana Simples",
     .habilidade = "Corte do Juizo Final",
     .vidaMax = 120,
     .manaMax = 30,
     .forca = 12,
     .defesa = 10,
     .agilidade = 5,
     .inteligencia = 5},
    {.nome = "Satoshi Hasegawa",
     .tipo = SATOSHI,
     .classe = "Atirador espiritual",
     .roupa = "Casaco",
     .arma = "Pistolas Gemeas",
     .habilidade = "Rajada de Balas Espirituais",
     .vidaMax = 100,
     .manaMax = 40,
     .forca = 8,
     .defesa = 7,
     .agilidade = 13,
     .inteligencia = 6},
    {.nome = "Hanae Suda",
     .tipo = HANAE,
     .classe = "Maga exorcista",
     .roupa = "Vestido",
     .arma = "Tomos Sagrados",
     .habilidade = "Exorcismo de Chamas Sagradas",
     .vidaMax = 80,
     .manaMax = 60,
     .forca = 5,
     .defesa = 5,
     .agilidade = 6,
     .inteligencia = 14}};

/* ====================================================================
 * ENTRADA
 * ==================================================================== */

/* Descarta o que sobrou na linha de entrada */
void limparBuffer(void)
{
    int c;

    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

/* Le um inteiro, repetindo ate a entrada ser valida. Sempre consome a linha. */
int lerInteiro(void)
{
    int valor;
    int lidos;

    while ((lidos = scanf("%d", &valor)) != 1)
    {
        if (lidos == EOF)
        {
            printf("\nEntrada encerrada. Saindo do jogo.\n");
            exit(EXIT_SUCCESS);
        }
        printf("Entrada invalida! Tente novamente: ");
        limparBuffer();
    }
    limparBuffer();
    return valor;
}

/* ====================================================================
 * INTERFACE
 * ==================================================================== */

void exibirBanner(const char *titulo)
{
    printf("\n====================================================\n");
    printf(" %s\n", titulo);
    printf("====================================================\n");
}

void exibirMenuPrincipal(void)
{
    printf("\n========================================\n");
    printf("     EXTERMINADORES DO INFERNO   \n");
    printf("========================================\n");
    printf("[1] Iniciar Nova Jornada\n");
    printf("[2] Sair\n");
    printf("Escolha uma opcao: ");
}

void exibirMenuSelecao(void)
{
    int i;

    printf("\n========================================\n");
    printf("     ESCOLHA SEU EXTERMINADOR\n");
    printf("========================================\n");

    for (i = 0; i < TOTAL_PERSONAGENS; i++)
    {
        printf("[%d] %s - %s\n", i + 1, modelos[i].nome, modelos[i].classe);
        printf("    HP %d | MP %d | For %d | Def %d | Agi %d | Int %d\n",
               modelos[i].vidaMax, modelos[i].manaMax, modelos[i].forca,
               modelos[i].defesa, modelos[i].agilidade, modelos[i].inteligencia);
        printf("    Habilidade: %s\n", modelos[i].habilidade);
    }
    printf("Escolha seu exterminador: ");
}

void exibirMenuDificuldade(void)
{
    exibirBanner("ESCOLHA A DIFICULDADE");
    printf("[1] Facil   - Demonios mais fracos e armadilhas leves\n");
    printf("[2] Normal  - Equilibrio original\n");
    printf("[3] Dificil - Demonios mais fortes e armadilhas pesadas\n");
    printf("Escolha a dificuldade: ");
}

void exibirStatusPersonagem(const Personagem *p)
{
    printf("\n----------------------------------------\n");
    printf(" Exterminador: %s (%s)\n", p->nome, p->classe);
    printf(" Nivel: %d | XP: %d\n", p->nivel, p->xp);
    printf(" HP: %d / %d | MP: %d / %d\n", p->vidaAtual, p->vidaMax, p->manaAtual, p->manaMax);
    printf(" Forca: %d | Defesa: %d | Agilidade: %d | Inteligencia: %d\n",
           p->forca, p->defesa, p->agilidade, p->inteligencia);
    printf(" Arma: %s | Roupa: %s\n", p->arma, p->roupa);
    printf(" Habilidade Especial: %s\n", p->habilidade);
    printf(" Pocoes: %d | Eteres: %d\n", p->pocoes, p->eteres);
    printf("----------------------------------------\n");
}

/* Simbolo de cada tipo de casa no mapa do tabuleiro */
static char simboloCasa(TipoCasa tipo)
{
    switch (tipo)
    {
    case CASA_COMBATE:
        return 'C';
    case CASA_TESOURO:
        return 'T';
    case CASA_CURA:
        return '+';
    case CASA_ARMADILHA:
        return '!';
    case CASA_CHEFE:
        return 'B';
    default:
        return '.';
    }
}

void exibirTabuleiro(const Casa tabuleiro[], int tamanho, int posicao)
{
    int i;

    printf("\nMapa: ");
    for (i = 0; i < tamanho; i++)
    {
        printf("[%c]", (i + 1 == posicao) ? '@' : simboloCasa(tabuleiro[i].tipo));
    }
    printf("\n  C=Combate T=Tesouro +=Cura !=Armadilha .=Vazia B=Chefe @=Voce\n");
}

void exibirTelaVitoria(void)
{
    exibirBanner("VITORIA! O INFERNO FOI SELADO E A CIDADE ESTA A SALVO!");
    printf(" Os Exterminadores do Inferno cumpriram seu dever sagrado.\n");
}

void exibirTelaDerrota(void)
{
    exibirBanner("DERROTA... AS TREVAS ENGOLIRAM O EXTERMINADOR.");
    printf(" Mas toda alma pode retornar para uma nova jornada.\n");
}

void limparTela(void)
/*Função que serve para limpar a tela*/
{
#ifdef _WIN32
    system("cls");
#else
    /* Sequencia ANSI: limpa a tela e volta o cursor ao inicio */
    printf("\033[2J\033[H");
    fflush(stdout);
#endif
}

/* Pressupoe que o buffer de entrada esta limpo (lerInteiro ja garante isso) */
void pausar(void)
{
    printf("\nPressione ENTER para continuar...");
    fflush(stdout);
    limparBuffer();
    limparTela();
}

/* ====================================================================
 * UTILITARIOS E INICIALIZACAO
 * ==================================================================== */

int rolarDado(int faces)
{
    if (faces < 1)
        return 1;
    return (rand() % faces) + 1;
}

int limitar(int valor, int minimo, int maximo)
{
    if (valor < minimo)
        return minimo;
    if (valor > maximo)
        return maximo;
    return valor;
}

int restaurar(int atual, int maximo, int valor)
{
    return (atual + valor > maximo) ? maximo : atual + valor;
}

int percentualCeil(int valor, double fracao)
{
    return (int)ceil(valor * fracao);
}

int escolherPersonagem(Personagem *p, int escolha)
{
    if (escolha < 1 || escolha > TOTAL_PERSONAGENS)
        return 0;

    *p = modelos[escolha - 1];
    p->vidaAtual = p->vidaMax;
    p->manaAtual = p->manaMax;
    p->pocoes = POCOES_INICIAIS;
    p->eteres = ETERES_INICIAIS;
    p->nivel = 1;
    p->xp = 0;
    return 1;
}