#ifndef FUNCOES_H
#define FUNCOES_H

#include "estruturas.h"

void exibirMenuPrincipal(); // Exibe o menu principal do jogo
int rolarDado(int faces); // Rola um dado(d6 para o tabuleiro e d20 para o combate)
void escolherPersonagem(Personagem *p, int escolha); // Configura os atributos do personagem com base na escolha do usuário
void pausar(void); // PPausa a execução do terminal limpado o buffer e guardado o enter do usuário

#endif