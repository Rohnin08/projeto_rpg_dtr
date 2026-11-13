#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

// Enum que define as classes
typedef enum
{
    KEN = 1,
    SATOSHI,
    HANAE
} TipoPersonagem;

typedef enum
{
    TIPO_ESPECTRO = 1,
    TIPO_BESTA,
    TIPO_CHEFE

} TipoMonstro;

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

typedef struct
{
    char nome[50];
    int vidaAtual,
        vidaMax,
        forca,
        defesa,
        agilidade;
} Monstro;

typedef struct
{
    int idFase;          // Identificador da fase (1, 2, ou 3)
    char descricao[250]; // Texto descritivo da missão (ex: "Derrotar Hell's")
    int concluida;       // Estado da missão: só fica True se for concluida(1), caso o contrario fica False(0)
} Missao;

typedef struct
{
    int idCasa;
    char tipo[20]; // Define o tipo de casa: cura, armadilha, batalha, tesouro, etc.
} Casa;

#endif