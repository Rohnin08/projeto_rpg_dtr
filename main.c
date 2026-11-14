#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "estruturas/estruturas.h"
#include "funcoes/funcoes.h"
#include "mecanicas/mecanicas.h"

int main(void)
{
    // Inicializa a semente para a geracao de numeros aleatorios (dados e eventos)
    srand((unsigned int)time(NULL));

    Personagem jogador;
    int opcaoMenu;

    do
    {
        exibirMenuPrincipal();
        if (scanf("%d", &opcaoMenu) != 1)
        {
            printf("\nEntrada invalida! Digite apenas numeros.\n");
            while (getchar() != '\n')
                ; // Limpa o buffer do teclado
            continue;
        }

        if (opcaoMenu == 1)
        {
            int escolhaClasse = 0;

            printf("\n====================================================\n");
            printf(" ESCOLHA SEU EXTERMINADOR\n");
            printf("====================================================\n");
            printf("[1] Ken Matsudaira - Alta Forca e Resistencia\n");
            printf("[2] Satoshi Hasegawa - Alta Agilidade e Precisao\n");
            printf("[3] Hanae Suda - Alta Inteligencia e Dano Espiritual\n");

            // Insiste ate o usuario digitar uma classe valida (KEN=1 a HANAE=3)
            while (1)
            {
                printf("Escolha seu personagem (1-3): ");

                if (scanf("%d", &escolhaClasse) != 1)
                {
                    printf("\nEntrada invalida! Digite apenas numeros (1, 2 ou 3).\n\n");
                    while (getchar() != '\n')
                        ;
                    continue;
                }

                if (escolhaClasse >= KEN && escolhaClasse <= HANAE)
                {
                    break;
                }

                printf("\nPersonagem fora do escopo! Tente novamente (1 a 3).\n\n");
            }

            escolherPersonagem(&jogador, escolhaClasse);

            printf("\n>> Personagem selecionado com sucesso!\n");
            exibirStatusPersonagem(&jogador);
            pausar();

            // Inicializa as 3 missoes usando a struct do estruturas.h
            Missao missoes[3] = {
                {1, "Beco das Almas Perdidas - Encontrar a fenda principal e derrotar Malphas.", 0},
                {2, "Ruinas do Purgatorio - Desbancar as legioes medias e vencer Astaroth.", 0},
                {3, "Trono do Lorde Demonio - Enfrentar Satanael e salvar o mundo!", 0}};

            // Executa as 3 fases em sequencia sem usar goto
            int missoesConcluidas = 0;
            for (int i = 0; i < 3; i++)
            {
                iniciarFase(&jogador, &missoes[i]);

                if (jogador.vidaAtual <= 0)
                {
                    break; // Sai do loop imediatamente se o jogador morrer
                }

                missoes[i].concluida = 1;
                missoesConcluidas++;
            }

            // Verificacao final de vitoria ou derrota
            if (jogador.vidaAtual > 0)
            {
                printf("\n====================================================\n");
                printf(" PARABENS, EXTERMINADOR!\n");
                printf(" Voce derrotou Satanael e fechou os portoes do inferno!\n");
                printf(" Missoes concluidas: %d/3\n", missoesConcluidas);
                printf("====================================================\n");
            }
            else
            {
                printf("\n====================================================\n");
                printf(" GAME OVER: SUA ALMA FOI CONSUMIDA PELAS TREVAS...\n");
                printf(" O submundo prevalecera nesta era. (Missoes: %d/3)\n", missoesConcluidas);
                printf("====================================================\n");
            }

            pausar();
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