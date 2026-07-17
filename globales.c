#include "globales.h"
#include <stdio.h>

// Definiciones de las variables que declaraste como "extern" en el .h
char matriz_nivel[FILAS][COLUMNAS];
ALLEGRO_FONT *letra_interfaz = NULL;
struct Personaje jugador;
struct Enemigo malo;
struct Gaviota gaviota;
struct EstadoJuego juego;

// Definición de los punteros a sprites
ALLEGRO_BITMAP *sprite_palanca_desactivada = NULL;
ALLEGRO_BITMAP *sprite_palanca_activada = NULL;
ALLEGRO_BITMAP *sprite_gaviota1 = NULL;
ALLEGRO_BITMAP *sprite_gaviota2 = NULL;
ALLEGRO_BITMAP *sprite_caca = NULL;

// Implementacion de la funcion cargar_nivel
void cargar_nivel(const char *nombre_archivo) {
    FILE *archivo = fopen(nombre_archivo, "r");
    if (!archivo) return;

    // limpia la matriz
    for (int f = 0; f < FILAS; f++)
        for (int c = 0; c < COLUMNAS; c++)
            matriz_nivel[f][c] = ' ';

    int f = 0, c = 0;
    int caracter;

    while ((caracter = fgetc(archivo)) != EOF) {
        if (caracter == '\n') {
            f++;
            c = 0;
        } else if (f < FILAS && c < COLUMNAS) {
            matriz_nivel[f][c] = (char)caracter;
            c++;
        }
    }
    fclose(archivo);
}

// Implementacion de la funcion actualizar_gaviota
void actualizar_gaviota(void) {
    // Movimiento de izquierda a derecha
    gaviota.x += gaviota.velocidad;
    if (gaviota.x > (COLUMNAS * TAM_BLOQUE) - 100 || gaviota.x < 0) {
        gaviota.velocidad *= -1;
    }
    
    // Timer para disparar caca
    gaviota.timer_disparo++;
    if (gaviota.timer_disparo > 60) { // Dispara cada 1 segundo
        for(int i = 0; i < 5; i++) {
            if (!gaviota.balas[i].activo) {
                gaviota.balas[i].x = gaviota.x;
                gaviota.balas[i].y = gaviota.y;
                gaviota.balas[i].activo = true;
                break;
            }
        }
        gaviota.timer_disparo = 0;
    }

    // Movimiento de las cacas ya disparadas
    for(int i = 0; i < 5; i++) {
        if (gaviota.balas[i].activo) {
            gaviota.balas[i].y += 4.0; // Caen hacia abajo
            if (gaviota.balas[i].y > ALTO) gaviota.balas[i].activo = false;
        }
    }
}

void inicializar_nivel(void) {

    juego.nivel_actual = 1;

    malo.x = -100; 
    malo.y = -100;

    for (int f = 0; f < FILAS; f++) {
        for (int c = 0; c < COLUMNAS; c++) {
            if (matriz_nivel[f][c] == 'P') { 
                jugador.x = c * TAM_BLOQUE + TAM_BLOQUE / 2;
                jugador.y = f * TAM_BLOQUE + TAM_BLOQUE / 2;
            }
            if (matriz_nivel[f][c] == 'E') { 
                malo.x = c * TAM_BLOQUE;
                malo.y = f * TAM_BLOQUE;
            }
            if (matriz_nivel[f][c] == 'G') {
                gaviota.x = c * TAM_BLOQUE;
                gaviota.y = f * TAM_BLOQUE;
            }
        }
    }
}