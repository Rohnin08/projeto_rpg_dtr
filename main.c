#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "estruturas.h"
#include "funcoes.h"
#include "mecanicas.h"

int main(void) {
    // Inicializa a semente para a geração de números aleatórios (dados e eventos)
    srand((unsigned int)time(NULL));

    Personagem jogador;
    int opcaoMenu;

    do {
        exibirMenuPrincipal();
        if (scanf("%d", &opcaoMenu) != 1) {
            printf("\nEntrada invalida! Digite apenas numeros.\n");
            while (getchar() != '\n'); // Limpa o buffer do teclado
            continue;
        }

        if (opcaoMenu == 1) {
            int escolhaClasse;
            printf("\n====================================================\n");
            printf(" ESCOLHA SEU EXTERMINADOR\n");
            printf("====================================================\n");
            printf("[1] Ken Matsudaira - Alta Forca e Resistencia\n");
            printf("[2] Satoshi Hasegawa - Alta Agilidade e Precisao\n");
            printf("[3] Hanae Suda - Alta Inteligencia e Dano Espiritual\n");
            printf("Escolha seu personagem (1-3): ");

            if (scanf("%d", &escolhaClasse) != 1) {
                printf("\nOpcao invalida! Retornando ao menu...\n");
                while (getchar() != '\n');
                continue;
            }

            // Valida e inicializa o personagem escolhido
            if (escolhaClasse < 1 || escolhaClasse > 3) {
                escolhaClasse = 3; // Padrão se o usuário digitar algo errado
            }

            escolherPersonagem(&jogador, escolhaClasse);
            
            printf("\n>> Personagem selecionado com sucesso!\n");
            exibirStatusPersonagem(&jogador);
            pausar();

            // ==========================================
            // FASE 1: Beco das Almas Perdidas
            // ==========================================
            iniciarFase(&jogador, 1, "Beco das Almas Perdidas", "Encontrar a fenda principal e derrotar Malphas.");
            if (jogador.vidaAtual <= 0) {
                goto fim_jogo;
            }

            // ==========================================
            // FASE 2: Ruínas do Purgatório
            // ==========================================
            iniciarFase(&jogador, 2, "Ruinas do Purgatorio", "Desbancar as legioes medias e vencer Astaroth.");
            if (jogador.vidaAtual <= 0) {
                goto fim_jogo;
            }

            // ==========================================
            // FASE 3: Trono do Lorde Demônio
            // ==========================================
            iniciarFase(&jogador, 3, "Trono do Lorde Demonio", "Enfrentar Belz, O Principe do Submundo e salvar o mundo!");

            // Verificação final de vitória
            fim_jogo:
            if (jogador.vidaAtual > 0) {
                printf("\n====================================================\n");
                printf(" PARABENOS, EXTERMINADOR!\n");
                printf(" Voce derrotou Belz e fechou os portoes do inferno!\n");
                printf("====================================================\n");
            } else {
                printf("\n====================================================\n");
                printf(" GAME OVER: SUA ALMA FOI CONSUMIDA PELAS TREVAS...\n");
                printf(" O submundo prevalecerá nesta era.\n");
                printf("====================================================\n");
            }

            pausar();
        } 
        else if (opcaoMenu == 2) {
            printf("\nEncerrando Exterminadores do Inferno. Ate logo!\n");
        } 
        else {
            printf("\nOpcao desconhecida! Tente novamente.\n");
        }

    } while (opcaoMenu != 2);

    return 0;
}