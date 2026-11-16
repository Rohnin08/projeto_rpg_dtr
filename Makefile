# Makefile - Exterminadores do Inferno
# Uso: make          (compila)
#      make run      (compila e executa)
#      make clean    (remove o executavel)

CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -pedantic -Iestruturas -Ifuncoes -Imecanicas -Icombate
LDLIBS  = -lm
SRC     = main.c funcoes/funcoes.c mecanicas/mecanicas.c combate/combate.c

ifeq ($(OS),Windows_NT)
    TARGET = exterminadores.exe
    RM     = del /Q
else
    TARGET = exterminadores
    RM     = rm -f
endif

all: $(TARGET)

$(TARGET): $(SRC) estruturas/estruturas.h funcoes/funcoes.h mecanicas/mecanicas.h combate/combate.h
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDLIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	$(RM) $(TARGET)

.PHONY: all run clean