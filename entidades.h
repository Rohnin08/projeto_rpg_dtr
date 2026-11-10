#ifndef ENTIDADES_H
#define ENTIDADES_H

// Enum que define as classes
typedef enum
{
    CLASSE_GUERREIRO = 1,
    CLASSE_MAGO,
    CLASSE_ARQUEIRO
} TipoClasse;

// struct do Personagem

typedef struct
{
    char nome[50];
    char classe[40];
    int vidaAtual,
        vidaMax,
        forca,
        defesa,
        agilidade,
        inteligencia;
    char arma[40];
    char roupa[40];
    char habilidade[50];
} Personagem; // Struct para guardar as informações do personagem

#endif