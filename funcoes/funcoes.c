#include <stdio.h>
#include <stdlib.h>
#include "funcoes.h"

void exibirMenuPrincipal()
{
    printf("\n========================================\n");
    printf("     EXTERMINADORES DO INFERNO   \n");
    printf("========================================\n");
    printf("[1] Iniciar Nova Jornada\n");
    printf("[2] Sair\n");
    printf("Escolha uma opcao: ");
}

int rolarDado(int faces)
{
    return (rand() % faces) + 1;
}

void escolherPersonagem(Personagem *p, int escolha)
{
    const Personagem modelos[3] = {
        {
            .nome = "Ken Matsudaira",
            .classe = "Espadachim sobrenatural",
            .roupa = "Camiseta",
            .arma = "Katana Simples",
            .habilidade = "Corte do Juizo Final",
            .vidaMax = 120,
            .vidaAtual = 120,
            .manaMax = 30,
            .manaAtual = 30,
            .forca = 12,
            .defesa = 10,
            .agilidade = 5,
            .inteligencia = 5,
        },
        {
            .nome = "Satoshi Hasegawa",
            .classe = "Atirador espiritual",
            .roupa = "Casaco",
            .arma = "Pistolas gemeas",
            .habilidade = "Rajada de Balas Espirituais",
            .vidaMax = 100,
            .vidaAtual = 100,
            .manaMax = 40,
            .manaAtual = 40,
            .forca = 8,
            .defesa = 7,
            .agilidade = 13,
            .inteligencia = 6,
        },
        {
            .nome = "Hanae Suda",
            .classe = "Maga exorcista",
            .roupa = "Vestido",
            .arma = "Tomo Sagrado",
            .habilidade = "Exorcismo de Chamas Sagradas",
            .vidaMax = 80,
            .vidaAtual = 80,
            .manaMax = 50,
            .manaAtual = 50,
            .forca = 5,
            .defesa = 5,
            .agilidade = 6,
            .inteligencia = 14,
        }};

    // Como o main já validou que 'escolha' está entre 1 e 3, basta subtrair 1
    *p = modelos[escolha - 1];
}
void exibirStatusPersonagem(const Personagem *p)
{
    printf("\n----------------------------------------\n");
    printf(" Exterminador: %s (%s)\n", p->nome, p->classe);
    printf(" HP: %d / %d\n", p->vidaAtual, p->vidaMax);
    printf(" Forca: %d | Defesa: %d | Agilidade: %d | Inteligencia: %d\n",
           p->forca, p->defesa, p->agilidade, p->inteligencia);
    printf(" Arma: %s | Roupa: %s\n", p->arma, p->roupa);
    printf(" Habilidade Especial: %s\n", p->habilidade);
    printf("----------------------------------------\n");
}

void limparTela(void)
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pausar(void)
{
    int c;
    printf("\nPressione ENTER para continuar...");
    while ((c = getchar()) != '\n' && c != EOF)
        ;
    getchar();
    limparTela();
}
