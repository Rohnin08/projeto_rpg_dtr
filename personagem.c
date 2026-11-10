#include <stdio.h>
#include <string.h>
#include "personagem.h"

TipoClasse obterOpcaoClasse()
{
    int escolha;

    do
    {
        printf("\n========================================\n");
        printf("         ESCOLHA O SEU PERSONAGEM       \n");
        printf("========================================\n");
        printf("[1] Guerreiro\n");
        printf("[2] Mago\n");
        printf("[3] Arqueiro\n");
        printf("Digite sua opcao: ");
        scanf("%d", &escolha);

        if (escolha < 1 || escolha > 3)
        {
            printf("\n[ERRO]: Opcao invalida! Escolha entre 1, 2 ou 3.\n");
        }

    } while (escolha < 1 || escolha > 3);

    return (TipoClasse)escolha;
}

Personagem inicializarPersonagem(TipoClasse opcao)
{
    Personagem p;

    switch (opcao)
    {
    case CLASSE_GUERREIRO:
        strcpy(p.nome, "Guerreiro");
        strcpy(p.classe, "Combate corpo a corpo");
        p.vidaAtual = 120;
        p.vidaMax = 120;
        p.forca = 12;
        p.defesa = 10;
        p.agilidade = 5;
        p.inteligencia = 5;
        strcpy(p.arma, "Espada longa");
        strcpy(p.roupa, "Armadura de placas");
        strcpy(p.habilidade, "Golpe Poderoso");
        break;

    case CLASSE_MAGO:
        strcpy(p.nome, "Mago");
        strcpy(p.classe, "Magia e ataque especial");
        p.vidaAtual = 80;
        p.vidaMax = 80;
        p.forca = 5;
        p.defesa = 5;
        p.agilidade = 6;
        p.inteligencia = 14;
        strcpy(p.arma, "Cajado magico");
        strcpy(p.roupa, "Tunica arcana");
        strcpy(p.habilidade, "Explosao Arcana");
        break;

    case CLASSE_ARQUEIRO:
        strcpy(p.nome, "Arqueiro");
        strcpy(p.classe, "Precisao e agilidade");
        p.vidaAtual = 100;
        p.vidaMax = 100;
        p.forca = 8;
        p.defesa = 7;
        p.agilidade = 13;
        p.inteligencia = 6;
        strcpy(p.arma, "Arco e flechas");
        strcpy(p.roupa, "Armadura de couro");
        strcpy(p.habilidade, "Tiro Preciso");
        break;
    }

    return p;
}

Personagem criarPersonagem()
{
    TipoClasse opcao = obterOpcaoClasse();
    return inicializarPersonagem(opcao);
}

void exibirPersonagem(Personagem p)
{
    printf("\n========================================\n");
    printf("         PERSONAGEM CRIADO              \n");
    printf("========================================\n");
    printf("Classe: %s\n", p.nome);
    printf("Tipo: %s\n", p.classe);
    printf("Vida: %d\n", p.vidaMax);
    printf("Forca: %d | Defesa: %d\n", p.forca, p.defesa);
    printf("Agilidade: %d | Inteligencia: %d\n", p.agilidade, p.inteligencia);
    printf("Arma: %s | Roupa: %s\n", p.arma, p.roupa);
    printf("Habilidade: %s\n", p.habilidade);
    printf("========================================\n");
}