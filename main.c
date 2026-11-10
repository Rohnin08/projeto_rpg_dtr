#include <stdio.h>
#include "entidades.h"
#include "personagem.h"

int main() {
    // 1. Cria o personagem (chama internamente a obtenção de opção, validação e inicialização)
    Personagem meuHeroi = criarPersonagem();

    // 2. Exibe os atributos do herói gerado na tela
    exibirPersonagem(meuHeroi);

    return 0;
}