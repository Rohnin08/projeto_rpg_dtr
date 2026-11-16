#ifndef COMBATE_H
#define COMBATE_H

#include "estruturas.h"

/* ---------- Dificuldade ---------- */
#define DIF_FACIL 1
#define DIF_NORMAL 2
#define DIF_DIFICIL 3

typedef struct
{
    const char *nome;
    double escalaVida;  /* base do pow() que escala a vida por fase */
    double danoMonstro; /* multiplicador do dano causado pelos demonios */
    int danoArmadilha;  /* dano direto das runas demoniacas */
} ConfigDificuldade;

/* Define a dificuldade (valores fora de 1-3 sao ajustados) */
void definirDificuldade(int dificuldade);

/* Parametros da dificuldade atual */
const ConfigDificuldade *configDificuldade(void);

/* Nome da dificuldade atual ("Facil", "Normal" ou "Dificil") */
const char *nomeDificuldade(void);

/* ---------- Monstros ---------- */

/* Preenche o monstro conforme a fase (1-3) e o indice (0-1 comuns, 2 chefe) */
void gerarMonstro(Monstro *m, int fase, int indice);

/* ---------- Combate ---------- */

/* Combate por turnos. ehChefe = 1 bloqueia a fuga */
void executarCombate(Personagem *p, Monstro *m, int fase, int ehChefe);

#endif /* COMBATE_H */