#ifndef MECANICAS_H
#define MECANICAS_H

#include "estruturas.h"

/* Numero de casas de cada fase (a ultima e o covil do chefe) */
#define TAM_TABULEIRO 15

/* Preenche o monstro conforme a fase (1-3) e o indice (0-1 comuns, 2 chefe) */
void gerarMonstro(Monstro *m, int fase, int indice);

/* Combate por turnos. ehChefe = 1 bloqueia a fuga */
void executarCombate(Personagem *p, Monstro *m, int fase, int ehChefe);

/* Recompensa apos vencer um demonio (comum ou chefe) */
void aplicarRecompensa(Personagem *p, Monstro *m, int fase, int ehChefe);

/* Percorre o tabuleiro linear de uma fase ate o chefe ou ate o jogador cair */
void iniciarFase(Personagem *p, const Missao *missao);

#endif /* MECANICAS_H */