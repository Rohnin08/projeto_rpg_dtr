#ifndef FUNCOES_H
#define FUNCOES_H

#include "estruturas.h"

#define TOTAL_PERSONAGENS 3

/* ---------- Interface ---------- */
void exibirBanner(const char *titulo);
void exibirMenuPrincipal(void);
void exibirMenuSelecao(void);
void exibirMenuDificuldade(void);
void exibirStatusPersonagem(const Personagem *p);
void exibirTabuleiro(const Casa tabuleiro[], int tamanho, int posicao);
void exibirTelaVitoria(void);
void exibirTelaDerrota(void);
void limparTela(void);
void pausar(void);

/* ---------- Entrada ---------- */
void limparBuffer(void);
int lerInteiro(void);

/* ---------- Utilitarios e inicializacao ---------- */
int rolarDado(int faces);

/* Limita valor ao intervalo [minimo, maximo] */
int limitar(int valor, int minimo, int maximo);

/* Soma valor a atual sem ultrapassar o maximo */
int restaurar(int atual, int maximo, int valor);

/* Percentual de um valor, arredondado para cima */
int percentualCeil(int valor, double fracao);

/* Retorna 1 se escolha (1 a TOTAL_PERSONAGENS) for valida; 0 caso contrario */
int escolherPersonagem(Personagem *p, int escolha);

#endif /* FUNCOES_H */