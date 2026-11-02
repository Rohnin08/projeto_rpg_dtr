#include <stdio.h>
#include <string.h>

typedef struct
{
    char classe[50];
    int vida_inicial;
    int forca;
    int defesa;
    int agilidade;
    int inteligencia;
    char roupa[50];
    char arma[50];
    char habilidade_especial[50];

} Personagem; // Strutc que servira de base para criação dos personagens do jogo

typedef struct
{
    char nome[50];
    int vida;
    int ataque;
    int defesa;
    int nivel;
    int recompença;
    char breve_descricao[100];

} Monstro; // Struct que servira como base para criação dos monstros

int main()
{
    // Definição do personagem guerreiro
    Personagem guerreiro;

    strcpy(guerreiro.classe, "Combate corpo a corpo");
    guerreiro.vida_inicial = 120;
    guerreiro.forca = 12;
    guerreiro.defesa = 10;
    guerreiro.inteligencia = 5;
    strcpy(guerreiro.roupa, "Armadura de placas");
    strcpy(guerreiro.arma, "Espada Longa");
    strcpy(guerreiro.habilidade_especial, "Golpe Poderoso");

    return 0;
}