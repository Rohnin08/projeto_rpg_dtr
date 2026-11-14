#ifndef FUNCOES_H
#define FUNCOES_H

#include "estruturas.h"

// Exibe o menu principal do jogo
void exibirMenuPrincipal();
// Rola um dado(d6 para o tabuleiro e d20 para o combate)
int rolarDado(int faces);
// Configura os atributos do personagem com base na escolha do usuário
void escolherPersonagem(Personagem *p, int escolha);
// Exibe as informações do personagem
void exibirStatusPersonagem(const Personagem *p);
// Pausa a execução do terminal limpado o buffer e guardado o enter do usuário
void pausar();
void limparTela();
#endif