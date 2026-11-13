#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

void escolherPersonagem(Personagem *p, int escolha)
{
    switch (escolha)
    {
    case KEN:
        strcpy(p->nome, "Ken Matsudaira");
        strcpy(p->classe, "Espadachim sobrenatural");
        strcpy(p->roupa, "Camiseta");
        strcpy(p->arma, "Katana Simples");
        strcpy(p->habilidade, "Corte do Juizo Final");
        p->vidaMax = 120;
        p->vidaAtual = 120;
        p->forca = 12;
        p->defesa = 10;
        p->agilidade = 5;
        p->inteligencia = 5;
        break;

    case SATOSHI:
        strcpy(p->nome, "Satoshi Hasegawa");
        strcpy(p->classe, "Atirador espiritual");
        strcpy(p->roupa, "Casaco");
        strcpy(p->arma, "Pistolas gemeas");
        strcpy(p->habilidade, "Rajada de Balas Espirituais");
        p->vidaMax = 100;
        p->vidaAtual = 100;
        p->forca = 8;
        p->defesa = 7;
        p->agilidade = 13;
        p->inteligencia = 6;
        break;

    case HANAE:
    default:
        strcpy(p->nome, "Hanae Suda");
        strcpy(p->classe, "Maga exorcista");
        strcpy(p->roupa, "Vestido");
        strcpy(p->arma, "Tomo Sagrado");
        strcpy(p->habilidade, "Exorcismo de Chamas Sagradas");
        p->vidaMax = 80;
        p->vidaAtual = 80;
        p->forca = 5;
        p->defesa = 5;
        p->agilidade = 6;
        p->inteligencia = 14;
        break;
    }
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

int rolarDado(int faces)
{
    return (rand() % faces) + 1;
}

void pausar(void)
{
    int c;
    printf("\nPressione ENTER para continuar...");
    while ((c = getchar()) != '\n' && c != EOF)
        ;
    getchar();
}