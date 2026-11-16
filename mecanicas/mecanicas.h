#ifndef MECANICAS_H
#define MECANICAS_H

#include "estruturas.h"

/*
 * Regras numericas do jogo, sem entrada/saida de combate:
 * dano, criticos, esquiva, fuga, efeitos de roupas, XP/nivel e recompensas.
 */

/* Aplicam dano via ponteiro (a vida nunca fica negativa) */
void receberDano(Personagem *p, int dano);
void danoNoMonstro(Monstro *m, int dano);

/*
 * Resolve um ataque considerando o d20:
 *  - 1 natural: falha critica (retorna 0)
 *  - 20 natural: acerto critico (dano dobrado, *critico = 1)
 *  - demais: dano = ataque - defesa, minimo 1
 */
int resolverAtaque(int dado, double ataque, int defesa, int *critico);

/* Custo em MP da habilidade especial (reduzido por algumas roupas) */
int custoEspecial(const Personagem *p);

/* Dano fixo absorvido pela roupa equipada em cada ataque recebido */
int reducaoDanoRoupa(const Personagem *p);

/* Chance (%) de o jogador esquivar de um ataque do demonio (inclui bonus da roupa) */
int chanceEsquiva(const Personagem *p, const Monstro *m);

/* Chance (%) de o jogador conseguir fugir */
int chanceFuga(const Personagem *p, const Monstro *m);

/* Retorna 1 com probabilidade chance% */
int sortearChance(int chance);

/* Concede XP e sobe de nivel quando atinge o limite (atualiza atributos) */
void ganharXP(Personagem *p, int xp);

/* Recompensa apos vencer um demonio (XP + bonus comum ou de chefe) */
void aplicarRecompensa(Personagem *p, Monstro *m, int fase, int ehChefe);

#endif /* MECANICAS_H */