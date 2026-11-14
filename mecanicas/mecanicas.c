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
    double fator = pow(1.25, fase - 1);

    // Ordem: {vidaBase, forca, defesa, agilidade}
    const int atributosBase[3][3][4] = {
        // Fase 1
        {{30, 8, 4, 5}, {45, 11, 6, 8}, {70, 14, 8, 10}},
        // Fase 2
        {{60, 15, 9, 6}, {75, 18, 10, 14}, {100, 22, 12, 12}},
        // Fase 3
        {{90, 24, 14, 9}, {120, 28, 18, 4}, {180, 35, 20, 15}}};

    const char *nomesMonstros[3][3] = {
        // Fase 1
        {"Diabrete", "Possuido Infernal", "Malphas, O Vigia das Fendas"},
        // Fase 2
        {"Hellhound", "Sucubo", "Astaroth, O Duque das Cinzas"},
        // Fase 3
        {"Berserker Abissal", "Guardiao de Obsidiana", "Satanael, O Principe do Submundo"}};

    int fIdx = fase - 1;
    int mIdx = indice;

    if (fIdx < 0)
        fIdx = 0;
    if (fIdx > 2)
        fIdx = 2;
    if (mIdx < 0)
        mIdx = 0;
    if (mIdx > 2)
        mIdx = 2;

    strcpy(m->nome, nomesMonstros[fIdx][mIdx]);

    int vidaBase = atributosBase[fIdx][mIdx][0];
    m->vidaMax = (int)(vidaBase * fator);
    m->vidaAtual = m->vidaMax;

    m->forca = atributosBase[fIdx][mIdx][1];
    m->defesa = atributosBase[fIdx][mIdx][2];
    m->agilidade = atributosBase[fIdx][mIdx][3];
}

void executarCombate(Personagem *p, Monstro *m, int fase, int ehChefe)
{
    p->manaAtual = p->manaMax; // Restaura o MP no inicio de cada combate
    const int CUSTO_ESPECIAL = 10;

    printf("\n====================================================\n");
    printf(" ENCONTRO COM DEMONIO: %s\n", m->nome);
    printf("====================================================\n");

    while (p->vidaAtual > 0 && m->vidaAtual > 0)
    {
        printf("\n--- Status do Combate ---\n");
        printf("Seu HP: %d/%d | Seu MP: %d/%d | %s HP: %d/%d\n",
               p->vidaAtual, p->vidaMax, p->manaAtual, p->manaMax, m->nome, m->vidaAtual, m->vidaMax);
        printf("[1] Atacar Normal\n");
        printf("[2] Usar Habilidade Especial (%s) [Custo: %d MP]\n", p->habilidade, CUSTO_ESPECIAL);
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
            int dado = rolarDado(20);
            int ataqueTotal = dado + p->forca;
            int dano = (int)fmax(1.0, (double)(ataqueTotal - m->defesa));
            m->vidaAtual -= dano;

            printf(">> Voce atacou! Rolou %d no d20 + forca (%d) = Ataque %d.\n", dado, p->forca, ataqueTotal);
            printf(">> Causou %d de dano espiritual em %s!\n", dano, m->nome);
        }
        else if (acao == 2)
        {
            if (p->manaAtual < CUSTO_ESPECIAL)
            {
                printf(">> MP INSUFICIENTE! Voce nao tem energia espiritual para usar '%s'!\n", p->habilidade);
                continue;
            }

            p->manaAtual -= CUSTO_ESPECIAL;

            int dado = rolarDado(20);
            int ataqueTotal = dado + p->inteligencia + p->forca;
            int dano = (int)fmax(1.0, (double)((ataqueTotal * 1.5) - m->defesa));
            m->vidaAtual -= dano;

            printf(">> Usou '%s' (-%d MP)! Rolou %d.\n", p->habilidade, CUSTO_ESPECIAL, dado);
            printf(">> Dano critico de exorcismo de %d em %s!\n", dano, m->nome);
        }
        else if (acao == 3)
        {
            if (ehChefe)
            {
                printf(">> Voce nao pode fugir de um General Demoniaco!\n");
            }
            else
            {
                printf(">> Voce recuou taticamente do combate! (-15 HP)\n");
                p->vidaAtual -= 15;
                pausar();
                return;
            }
        }
        else
        {
            printf("Acao desconhecida! O demonio aproveitou sua hesitacao.\n");
        }

        // Turno do Demonio (caso ele sobreviva)
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
        aplicarRecompensa(p, m, fase, ehChefe);
    }
}

void aplicarRecompensa(Personagem *p, Monstro *m, int fase, int ehChefe)
{
    (void)m; // Evita warning de parametro nao utilizado

    if (ehChefe)
    {
        printf("\n====================================================\n");
        printf(" RECOMPENSA DE CHEFE: ESSENCIA DEMONIACA PURIFICADA!\n");
        printf("====================================================\n");

        // Aumenta todos os atributos em 10% (arredondando para cima via ceil)
        p->vidaMax = (int)ceil(p->vidaMax * 1.10);
        p->vidaAtual = p->vidaMax;
        p->manaMax = (int)ceil(p->manaMax * 1.10);
        p->manaAtual = p->manaMax;
        p->forca = (int)ceil(p->forca * 1.10);
        p->defesa = (int)ceil(p->defesa * 1.10);
        p->agilidade = (int)ceil(p->agilidade * 1.10);
        p->inteligencia = (int)ceil(p->inteligencia * 1.10);

        // Tabela de evolucao de armas: [Classe 0-2][Fase 0-2]
        const char *armasEvoluidas[3][3] = {
            // Ken Matsudaira
            {"Katana Amaldicoada Muramasa", "Lamina Espiritual Kusanagi", "Espada Divina Ame-no-Habakiri"},
            // Satoshi Hasegawa
            {"Pistolas de Prata Consagrada", "Revolveres Duplos Cerberus", "Canhoes Arcanos Seraphim"},
            // Hanae Suda
            {"Grimorio das Chamas Puras", "Codex Selado de Salomao", "Tomo Supremo Akashico"}};

        int classeIdx = 0; // Ken
        if (strcmp(p->nome, "Satoshi Hasegawa") == 0)
            classeIdx = 1;
        else if (strcmp(p->nome, "Hanae Suda") == 0)
            classeIdx = 2;

        int faseIdx = (fase >= 1 && fase <= 3) ? fase - 1 : 0;
        strcpy(p->arma, armasEvoluidas[classeIdx][faseIdx]);

        printf(">> TODOS os seus atributos aumentaram em +10%% e seu HP/MP foram restaurados!\n");
        printf(">> NOVA ARMA EQUIPADA: %s!\n", p->arma);
        exibirStatusPersonagem(p);
        pausar();
        return;
    }

    // Recompensa padrao para demonios comuns
    printf(">> Recompensa espiritual obtida!\n");

    int atributoSorteado = rand() % 5;
    int bonus = (rand() % 3) + 2;

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
        p->vidaAtual = p->vidaMax;
        printf(">> EVOLUCAO: Sua Vida Maxima aumentou! HP totalmente restaurado (%d)\n", p->vidaMax);
    }
    pausar();
}

void iniciarFase(Personagem *p, const Missao *missao)
{
    printf("\n====================================================\n");
    printf(" FASE %d\n", missao->idFase);
    printf(" Missao: %s\n", missao->descricao);
    printf("====================================================\n");
    pausar();

    int posicao = 0;

    while (posicao < TAM_TABULEIRO && p->vidaAtual > 0)
    {
        int dadoMovimento = rolarDado(6);
        posicao += dadoMovimento;
        if (posicao >= TAM_TABULEIRO)
        {
            posicao = TAM_TABULEIRO;
        }

        printf("\n----------------------------------------------------\n");
        printf("Voce avancou %d casas no territorio. Posicao atual: %d/%d\n", dadoMovimento, posicao, TAM_TABULEIRO);

        int tipoEvento = rand() % 5;

        if (posicao == TAM_TABULEIRO)
        {
            printf("[CONFRONTO FINAL] Voce encontrou o general demoniaco desta regiao!\n");
            Monstro boss;
            gerarMonstro(&boss, missao->idFase, 2);
            executarCombate(p, &boss, missao->idFase, 1);
        }
        else if (tipoEvento == 0)
        {
            printf("[EVENTO DE COMBATE] Um demonio selvagem apareceu para bloquear seu caminho!\n");
            Monstro m;
            int indiceMonstro = rand() % 2;
            gerarMonstro(&m, missao->idFase, indiceMonstro);
            executarCombate(p, &m, missao->idFase, 0);
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
            p->manaAtual = p->manaMax;
            printf("[EVENTO DE CURA] Voce encontrou um santuario de purificacao! HP (%d/%d) e MP (%d/%d) restaurados.\n",
                   p->vidaAtual, p->vidaMax, p->manaAtual, p->manaMax);
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
        printf("\n[REGIAO LIBERTADA] Voce concluiu com sucesso a Fase %d!\n", missao->idFase);
        pausar();
    }
}