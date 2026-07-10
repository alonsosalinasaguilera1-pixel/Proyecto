#include "globales.h"
#include <stdio.h>

// CREACION REAL DE LAS VARIABLES EN MEMORIA
char matriz_nivel[FILAS][COLUMNAS];
ALLEGRO_FONT *letra_interfaz = NULL;
struct Personaje jugador;
struct Enemigo malo;

// INICIALIZACIÓN DE ESTRUCTURA DE JUEGO
struct EstadoJuego juego = {
    .puntos = 0,
    .segundos_restantes = 120.0,
    .nivel_actual = 1,
    .camara_x = 0,
    .salir = false,
    .redibujar = true,
    .teclas = {false, false, false, false},
    .debe_teletransportar = false
};

// FUNCION PARA LEER EL ARCHIVO DE TEXTO DEL NIVEL
void cargar_nivel(const char *nombre_archivo) {
    FILE *archivo = fopen(nombre_archivo, "r");
    if (archivo == NULL) {
        printf("Error: No se pudo abrir %s\n", nombre_archivo);
        return;
    }

    int f = 0;
    int c = 0;
    char ch;

    // Se limpia la matriz con espacios vacios antes de rellenar
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            matriz_nivel[i][j] = ' ';
        }
    }

    // Lee el archivo caracter por caracter ignorando saltos de linea desfasados
    while ((ch = fgetc(archivo)) != EOF && f < FILAS) {
        if (ch == '\n' || ch == '\r') {
            if (c > 0) { 
                f++;
                c = 0;
            }
        } 
        else if (c < COLUMNAS) {
            matriz_nivel[f][c] = ch;
            c++;
        }
    }

    fclose(archivo);
}

// FUNCION PARA BUSCAR LA 'P' Y LA 'E' EN EL MAPA
void inicializar_nivel(void) {
    for (int f = 0; f < FILAS; f++) {
        for (int c = 0; c < COLUMNAS; c++) {
            
            // Si encuentra la P, ubica al jugador de forma correcta
            if (matriz_nivel[f][c] == 'P') {
                jugador.x = (c * TAM_BLOQUE) + (TAM_BLOQUE / 2.0);
                jugador.y = (f * TAM_BLOQUE) + (TAM_BLOQUE / 2.0);
            }
            
            // Si encuentra la E, ubica al malo en su posicion inicial
            if (matriz_nivel[f][c] == 'E') {
                malo.x = (c * TAM_BLOQUE) + (TAM_BLOQUE / 2.0);
                malo.y = (f * TAM_BLOQUE) + (TAM_BLOQUE / 2.0);
            }
        }
    }
    
    // Resetea las fisicas de caida y el tiempo para el nuevo nivel
    jugador.vel_y = 0;
    jugador.en_suelo = false;
    juego.segundos_restantes = 120.0;
}