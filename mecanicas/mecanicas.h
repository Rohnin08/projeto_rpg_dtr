#ifndef MECANICAS_H
#define MECANICAS_H

#include "estruturas.h"

// Gera o monstro com base na fase atual
void gerarMonstro(Monstro *m, int fase, int indice);
// Controla o loop de combate por turnos entre o personagem e o monstro
void executarCombate(Personagem *p, Monstro *m);
// Concede XP e distribui um bônus aleatório de atributo ao derrotar um monstro
void aplicarRecompensa(Personagem *p, Monstro *m);
// Gerencia o tabuleiro linear da fase, a rolagem do d6 e os eventos de cada casa
void iniciarFase(Personagem *p, int numeroFase, const char *nomeFase, const char *missao);

#endif