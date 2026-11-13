#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "estruturas.h"
#include "funcoes.h"
#include "mecanicas.h"

#define TAM_TABULEIRO 15

void gerarMonstro(Monstro *m, int fase, int indice)
{
    // Aplica o fator de escala exponencial exigido pela prova usando math.h (pow)
    double fator = pow(1.25, fase - 1);

    if (fase == 1)
    {
        if (indice == 0)
        {
            strcpy(m->nome, "Diabrete das Sombras");
            m->vidaMax = (int)(30 * fator);
            m->vidaAtual = m->vidaMax;
            m->forca = 8;
            m->defesa = 4;
            m->agilidade = 5;
        }
        else if (indice == 1)
        {
            strcpy(m->nome, "Possuido Infernal");
            m->vidaMax = (int)(45 * fator);
            m->vidaAtual = m->vidaMax;
            m->forca = 11;
            m->defesa = 6;
            m->agilidade = 8;
        }
        else
        {
            strcpy(m->nome, "Malphas, O Vigia das Fendas (Chefe)");
            m->vidaMax = (int)(70 * fator);
            m->vidaAtual = m->vidaMax;
            m->forca = 14;
            m->defesa = 8;
            m->agilidade = 10;
        }
    }
    else if (fase == 2)
    {
        if (indice == 0)
        {
            strcpy(m->nome, "Cao de Caca do Inferno");
            m->vidaMax = (int)(60 * fator);
            m->vidaAtual = m->vidaMax;
            m->forca = 15;
            m->defesa = 9;
            m->agilidade = 6;
        }
        else if (indice == 1)
        {
            strcpy(m->nome, "Sucubo das Cinzas");
            m->vidaMax = (int)(75 * fator);
            m->vidaAtual = m->vidaMax;
            m->forca = 18;
            m->defesa = 10;
            m->agilidade = 14;
        }
        else
        {
            strcpy(m->nome, "Astaroth, O Duque das Cinzas (Chefe)");
            m->vidaMax = (int)(100 * fator);
            m->vidaAtual = m->vidaMax;
            m->forca = 22;
            m->defesa = 12;
            m->agilidade = 12;
        }
    }
    else
    {
        if (indice == 0)
        {
            strcpy(m->nome, "Berserker Abissal");
            m->vidaMax = (int)(90 * fator);
            m->vidaAtual = m->vidaMax;
            m->forca = 24;
            m->defesa = 14;
            m->agilidade = 9;
        }
        else if (indice == 1)
        {
            strcpy(m->nome, "Guardiao de Obsidiana");
            m->vidaMax = (int)(120 * fator);
            m->vidaAtual = m->vidaMax;
            m->forca = 28;
            m->defesa = 18;
            m->agilidade = 4;
        }
        else
        {
            strcpy(m->nome, "Belz, O Principe do Submundo (Chefe Final)");
            m->vidaMax = (int)(180 * fator);
            m->vidaAtual = m->vidaMax;
            m->forca = 35;
            m->defesa = 20;
            m->agilidade = 15;
        }
    }
}

void executarCombate(Personagem *p, Monstro *m)
{
    printf("\n====================================================\n");
    printf(" ENCONTRO COM DEMONIO: %s\n", m->nome);
    printf("====================================================\n");

    while (p->vidaAtual > 0 && m->vidaAtual > 0)
    {
        printf("\n--- Status do Combate ---\n");
        printf("Seu HP: %d/%d | %s HP: %d/%d\n", p->vidaAtual, p->vidaMax, m->nome, m->vidaAtual, m->vidaMax);
        printf("[1] Atacar Normal\n");
        printf("[2] Usar Habilidade Especial (%s)\n", p->habilidade);
        printf("[3] Tentar Fugir\n");
        printf("Escolha sua acao: ");

        int acao;
        if (scanf("%d", &acao) != 1)
        {
            printf("Entrada invalida!\n");
            while (getchar() != '\n')
                ;
            continue;
        }

        if (acao == 1)
        {
            // Ataque Normal (d20 + Força vs Defesa do Monstro)
            int dado = rolarDado(20);
            int ataqueTotal = dado + p->forca;
            // Usa fmax para garantir dano mínimo de 1 (exigência de math.h)
            int dano = (int)fmax(1.0, (double)(ataqueTotal - m->defesa));
            m->vidaAtual -= dano;

            printf(">> Voce atacou! Rolou %d no d20 + forca (%d) = Ataque %d.\n", dado, p->forca, ataqueTotal);
            printf(">> Causou %d de dano espiritual em %s!\n", dano, m->nome);
        }
        else if (acao == 2)
        {
            // Habilidade Especial (bônus baseado em Inteligência e Força)
            int dado = rolarDado(20);
            int ataqueTotal = dado + p->inteligencia + p->forca;
            int dano = (int)fmax(1.0, (double)((ataqueTotal * 1.5) - m->defesa));
            m->vidaAtual -= dano;

            printf(">> Usou a habilidade '%s'! Rolou %d.\n", p->habilidade, dado);
            printf(">> Dano critico de exorcismo de %d em %s!\n", dano, m->nome);
        }
        else if (acao == 3)
        {
            printf(">> Voce recuou taticamente do combate!\n");
            p->vidaAtual -= 15; // Penalidade por fuga
            return;
        }
        else
        {
            printf("Acao desconhecida! O demonio aproveitou sua hesitacao.\n");
        }

        // Turno do Demônio (caso ele sobreviva)
        if (m->vidaAtual > 0)
        {
            int dadoM = rolarDado(20);
            int ataqueMonstro = dadoM + m->forca;
            int danoMonstro = (int)fmax(1.0, (double)(ataqueMonstro - p->defesa));
            p->vidaAtual -= danoMonstro;

            printf(">> %s contra-atacou! Rolou %d + forca (%d) = Ataque %d.\n", m->nome, dadoM, m->forca, ataqueMonstro);
            printf(">> Voce sofreu %d de dano sombrio!\n", danoMonstro);
        }
    }

    if (p->vidaAtual > 0)
    {
        printf("\n[VITORIA] Voce exorcizou %s com sucesso!\n", m->nome);
        aplicarRecompensa(p, m);
    }
}

void aplicarRecompensa(Personagem *p, Monstro *m)
{
    printf(">> Recompensa espiritual obtida!\n");

    // Sorteio aleatório de qual atributo vai subir (0 a 4)
    int atributoSorteado = rand() % 5;
    int bonus = (rand() % 3) + 2; // Bônus entre 2 e 4

    if (atributoSorteado == 0)
    {
        p->forca += bonus;
        printf(">> EVOLUCAO: Sua Forca aumentou em +%d (Total: %d)\n", bonus, p->forca);
    }
    else if (atributoSorteado == 1)
    {
        p->defesa += bonus;
        printf(">> EVOLUCAO: Sua Defesa aumentou em +%d (Total: %d)\n", bonus, p->defesa);
    }
    else if (atributoSorteado == 2)
    {
        p->agilidade += bonus;
        printf(">> EVOLUCAO: Sua Agilidade aumentou em +%d (Total: %d)\n", bonus, p->agilidade);
    }
    else if (atributoSorteado == 3)
    {
        p->inteligencia += bonus;
        printf(">> EVOLUCAO: Sua Inteligencia aumentou em +%d (Total: %d)\n", bonus, p->inteligencia);
    }
    else
    {
        p->vidaMax += (bonus * 5);
        p->vidaAtual = p->vidaMax; // Cura total ao upar vida máxima
        printf(">> EVOLUCAO: Sua Vida Maxima aumentou! HP totalmente restaurado (%d)\n", p->vidaMax);
    }
    pausar();
}

void iniciarFase(Personagem *p, int numeroFase, const char *nomeFase, const char *missao)
{
    printf("\n====================================================\n");
    printf(" FASE %d: %s\n", numeroFase, nomeFase);
    printf(" Missao: %s\n", missao);
    printf("====================================================\n");
    pausar();

    int posicao = 0;

    // Loop do tabuleiro da fase atual
    while (posicao < TAM_TABULEIRO && p->vidaAtual > 0)
    {
        int dadoMovimento = rolarDado(6);
        posicao += dadoMovimento;
        if (posicao >= TAM_TABULEIRO)
        {
            posicao = TAM_TABULEIRO; // Trava na última casa do tabuleiro (Covil do Chefe)
        }

        printf("\n----------------------------------------------------\n");
        printf("Voce avancou %d casas no territorio. Posicao atual: %d/%d\n", dadoMovimento, posicao, TAM_TABULEIRO);

        // Sorteia o tipo de evento da casa (0: Combate, 1: Tesouro, 2: Cura, 3: Armadilha, 4: Vazio)
        int tipoEvento = rand() % 5;

        // Se cair na última casa (chefe), o combate contra o líder da fase é obrigatório
        if (posicao == TAM_TABULEIRO)
        {
            printf("[CONfronto FINAL] Voce encontrou o general demoniaco desta regiao!\n");
            Monstro boss;
            gerarMonstro(&boss, numeroFase, 2); // Índice 2 é sempre o Chefe da Fase
            executarCombate(p, &boss);
        }
        else if (tipoEvento == 0)
        {
            printf("[EVENTO DE COMBATE] Um demonio selvagem apareceu para bloquear seu caminho!\n");
            Monstro m;
            int indiceMonstro = rand() % 2; // Sorteia entre os demônios comuns (0 ou 1)
            gerarMonstro(&m, numeroFase, indiceMonstro);
            executarCombate(p, &m);
        }
        else if (tipoEvento == 1)
        {
            printf("[EVENTO DE TESOURO] Voce encontrou um amuleto sagrado esquecido!\n");
            p->forca += 1;
            p->inteligencia += 1;
            printf(">> Sua sintonia espiritual aumentou (+1 Forca, +1 Inteligencia).\n");
            pausar();
        }
        else if (tipoEvento == 2)
        {
            int cura = 25;
            p->vidaAtual = (p->vidaAtual + cura > p->vidaMax) ? p->vidaMax : p->vidaAtual + cura;
            printf("[EVENTO DE CURA] Voce encontrou um santuario de purificacao! HP recuperado (%d/%d).\n", p->vidaAtual, p->vidaMax);
            pausar();
        }
        else if (tipoEvento == 3)
        {
            int danoArmadilha = 12;
            p->vidaAtual -= danoArmadilha;
            printf("[ARMADILHA] Voce ativou uma runa de malandragem demoniaca! Perdeu %d de HP.\n", danoArmadilha);
            pausar();
        }
        else
        {
            printf("[EVENTO VAZIO] Um silencio pesado e o eco de sussurros sombrios cercam o local...\n");
            pausar();
        }

        if (p->vidaAtual <= 0)
            break;
    }

    if (p->vidaAtual > 0)
    {
        printf("\n[REGIAO LIBERTADA] Voce concluiu com sucesso a Fase %d!\n", numeroFase);
        pausar();
    }
}