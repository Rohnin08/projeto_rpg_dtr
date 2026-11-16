#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

// Numero de casas de cada fase (a ultima e o covil do chefe)
#define TAM_TABULEIRO 15

// Enum que define as classes
typedef enum
{
    KEN = 1,
    SATOSHI,
    HANAE
} TipoPersonagem;

// Tipos de casa do tabuleiro (de CASA_VAZIA a CASA_ARMADILHA podem ser sorteadas;
// CASA_CHEFE e sempre a ultima casa da fase)
typedef enum
{
    CASA_VAZIA,
    CASA_COMBATE,
    CASA_TESOURO,
    CASA_CURA,
    CASA_ARMADILHA,
    CASA_CHEFE
} TipoCasa;

// Acoes de combate (correspondem as opcoes numeradas do menu)
typedef enum
{
    ACAO_ATACAR = 1,
    ACAO_HABILIDADE,
    ACAO_POCAO,
    ACAO_ETER,
    ACAO_FUGIR,
    ACAO_DEFENDER
} AcaoCombate;

// struct do Personagem
typedef struct
{
    char nome[50];
    TipoPersonagem tipo; // Classe do personagem (enum)
    char classe[40];     // Nome da classe para exibicao
    int vidaAtual,
        vidaMax,
        manaAtual,
        manaMax,
        forca,
        defesa,
        agilidade,
        inteligencia;
    int xp, nivel;
    char arma[40];
    char roupa[40];
    char habilidade[50];
    int pocoes, eteres;
} Personagem; // Struct para guardar as informações do personagem

typedef struct
{
    const char *nome;
    int vida, mana, forca, defesa, agilidade, inteligencia; /* bonus ao equipar */
    int reducaoDano;                                        /* dano fixo absorvido por ataque */
    int esquivaExtra;                                       /* % somado a chance de esquiva */
    int reducaoCustoEspecial;                               /* MP a menos na habilidade especial */
    const char *efeito;
} Roupa;

typedef struct
{
    const char *nome;
    int vida, mana, forca, defesa, agilidade, inteligencia;
} Arma;

typedef struct
{
    char nome[50];
    char descricao[120];
    int nivel;        // Nivel do demonio (cresce com a fase; chefes tem nivel maior)
    int xpRecompensa; // XP concedido ao jogador ao derrota-lo
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
    int idCasa;    // Posicao da casa no tabuleiro (1 a TAM_TABULEIRO)
    TipoCasa tipo; // Define o tipo de casa: cura, armadilha, combate, tesouro, etc.
} Casa;

#endif