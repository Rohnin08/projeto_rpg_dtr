#ifndef MECANICAS_H
#define MECANICAS_H

#include "estruturas.h"

void gerarMonstro(Monstro *m, int fase, int indice);
void executarCombate(Personagem *p, Monstro *m, int fase, int ehChefe);
void aplicarRecompensa(Personagem *p, Monstro *m, int fase, int ehChefe);
void iniciarFase(Personagem *p, const Missao *missao);

#endif