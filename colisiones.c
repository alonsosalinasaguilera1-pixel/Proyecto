#include "globales.h"
#include "colisiones.h"
#include <stdio.h>
#include <math.h>

// Gestion de colisiones: PAREDES, SUELO, PINCHOS, MONEDAS Y META
void procesar_todas_las_colisiones(void) {
    
    // Calcular intentos de nueva posicion segun el teclado
    float nueva_x = jugador.x;
    if (juego.teclas[LEFT])  nueva_x -= jugador.velocidad;
    if (juego.teclas[RIGHT]) nueva_x += jugador.velocidad;

    if (juego.teclas[UP] && jugador.en_suelo) {
        jugador.vel_y = -10.0; // Fuerza del salto
        jugador.en_suelo = false;
    }
    
    // Nueva posicion en Y considerando la velocidad de la gravedad calculada en fisicas.c
    float nueva_y = jugador.y + jugador.vel_y;
    if (juego.teclas[DOWN])  nueva_y += jugador.velocidad;

    jugador.en_suelo = false;

    // Colision en el eje X (IZQUIERDA / DERECHA) contra paredes (# y -)
    int fila_cuerpo_sup = (jugador.y - jugador.alto / 2 + 5) / TAM_BLOQUE;
    int fila_cuerpo_inf = (jugador.y + jugador.alto / 2 - 5) / TAM_BLOQUE;
    
    if (juego.teclas[LEFT] || juego.teclas[RIGHT]) {
        int col_x_revisar = 0;
        if (nueva_x < jugador.x) {
            col_x_revisar = (nueva_x - jugador.ancho / 2) / TAM_BLOQUE; // Moviendose a la izquierda
        } else {
            col_x_revisar = (nueva_x + jugador.ancho / 2) / TAM_BLOQUE; // Moviendose a la derecha
        }

        if (fila_cuerpo_sup >= 0 && fila_cuerpo_inf < FILAS && col_x_revisar >= 0 && col_x_revisar < COLUMNAS) {
            
            char celda_sup = matriz_nivel[fila_cuerpo_sup][col_x_revisar];
            char celda_inf = matriz_nivel[fila_cuerpo_inf][col_x_revisar];

            if (celda_sup != '#' && celda_sup != '-' && celda_inf != '#' && celda_inf != '-') {
                jugador.x = nueva_x; // Si hay aire, confirma el movimiento en X
            }
        }
    }

    // Colision en el eje Y (ABAJO / CAIDA) contra bloques firmes (# y -)
    int col_pie_izq = (jugador.x - jugador.ancho / 2 + 2) / TAM_BLOQUE;
    int col_pie_der = (jugador.x + jugador.ancho / 2 - 2) / TAM_BLOQUE;

    if (jugador.vel_y >= 0) {
        // Colision cuando cae (suelo)
        int fil_pies = (nueva_y + jugador.alto / 2) / TAM_BLOQUE;

        if (fil_pies >= 0 && fil_pies < FILAS && col_pie_izq >= 0 && col_pie_der < COLUMNAS) {
            char celda_pie_izq = matriz_nivel[fil_pies][col_pie_izq];
            char celda_pie_der = matriz_nivel[fil_pies][col_pie_der];

            // Si en los pies no hay '#' ni '-' el jugador sigue cayendo
            if (celda_pie_izq != '#' && celda_pie_izq != '-' && celda_pie_der != '#' && celda_pie_der != '-') {
                jugador.y = nueva_y; 
            } else {
                // Encontro suelo firme ('#' o '-')
                jugador.en_suelo = true;
                jugador.vel_y = 0;
                // Colision con la parte de arriba del bloque
                jugador.y = (fil_pies * TAM_BLOQUE) - (jugador.alto / 2.0);
            }
        }
    } else {
        // Colision cuando sube en el salto (techo / cabeza)
        int fil_cabeza = (nueva_y - jugador.alto / 2) / TAM_BLOQUE;

        if (fil_cabeza >= 0 && fil_cabeza < FILAS && col_pie_izq >= 0 && col_pie_der < COLUMNAS) {
            char celda_cab_izq = matriz_nivel[fil_cabeza][col_pie_izq];
            char celda_cab_der = matriz_nivel[fil_cabeza][col_pie_der];

            // Si arriba hay aire, sube normal
            if (celda_cab_izq != '#' && celda_cab_izq != '-' && celda_cab_der != '#' && celda_cab_der != '-') {
                jugador.y = nueva_y;
            } else {
                // Golpeo un bloque con la cabeza
                jugador.vel_y = 0;
                jugador.y = ((fil_cabeza + 1) * TAM_BLOQUE) + (jugador.alto / 2.0);
            }
        }
    }

    // Deteccion por casillas
    int col_centro = jugador.x / TAM_BLOQUE;
    int fil_centro = jugador.y / TAM_BLOQUE;

    if (fil_centro >= 0 && fil_centro < FILAS && col_centro >= 0 && col_centro < COLUMNAS) {
        
        // Colision con pinchos, regresa al inicio buscando la 'P'
        if (matriz_nivel[fil_centro][col_centro] == 'X') {
            inicializar_nivel();
        }
        
        // Colision con monedas, desaparece del mapa y suma 100 puntos
        if (matriz_nivel[fil_centro][col_centro] == 'O') {
            matriz_nivel[fil_centro][col_centro] = ' '; 
            juego.puntos += 100;                               
        }
        
        // Colision al pasar sobre una palanca desactivada ('L')
        if (matriz_nivel[fil_centro][col_centro] == 'L') {
            juego.palancas_activadas++; // Suma una palanca al contador intuitivo
            matriz_nivel[fil_centro][col_centro] = 'l'; // Cambia a 'l' minuscula para cambiar de sprite
        }
        
        // Colision con la meta, cambio de nivel automatico
        if (matriz_nivel[fil_centro][col_centro] == 'M') {
            // Solo avanza si el contador llego a las 2 palancas necesarias
            if (juego.palancas_activadas >= 2) {
                juego.nivel_actual++; 
                
                char proximo_archivo[20];
                sprintf(proximo_archivo, "nivel%d.txt", juego.nivel_actual);
                
                FILE *test = fopen(proximo_archivo, "r");
                if (test != NULL) {
                    fclose(test);
                    cargar_nivel(proximo_archivo); 
                    juego.debe_teletransportar = true; // Activa la bandera para reubicar en el main
                } else {
                    printf("¡Felicidades, ganaste el juego!\n");
                    juego.salir = true; 
                }
            }
        }
    }

    // Colision entre el jugador y el enemigo gato
    float jug_izq = jugador.x - jugador.ancho / 2;
    float jug_der = jugador.x + jugador.ancho / 2;
    float jug_sup = jugador.y - jugador.alto / 2;
    float jug_inf = jugador.y + jugador.alto / 2;

    float malo_izq = malo.x - malo.ancho / 2;
    float malo_der = malo.x + malo.ancho / 2;
    float malo_sup = malo.y - malo.alto / 2;
    float malo_inf = malo.y + malo.alto / 2;

    if (jug_der > malo_izq && jug_izq < malo_der && jug_inf > malo_sup && jug_sup < malo_inf) {
        inicializar_nivel();
    }

    for(int i = 0; i < 5; i++) {
        if(gaviota.balas[i].activo) {
            float dx = jugador.x - gaviota.balas[i].x;
            float dy = jugador.y - gaviota.balas[i].y;
            float dist_cuadrada = (dx * dx) + (dy * dy);

            // Si la distancia es menor a 25 al cuadrado (625)
            if(dist_cuadrada < 625.0) { 
                inicializar_nivel(); 
            }
        }
    }

    // Colisiones de control contra los bordes totales del mapa extendido
    if (jugador.x - jugador.ancho / 2 < 0) {
        jugador.x = jugador.ancho / 2;
    }
    if (jugador.x + jugador.ancho / 2 > (COLUMNAS * TAM_BLOQUE)) {
        jugador.x = (COLUMNAS * TAM_BLOQUE) - jugador.ancho / 2;
    }
    if (jugador.y - jugador.alto / 2 < 0) {
        jugador.y = jugador.alto / 2;
    }
    if (jugador.y + jugador.alto / 2 > ALTO) {
        jugador.y = ALTO - jugador.alto / 2;
        jugador.en_suelo = true;
        jugador.vel_y = 0;
    }

    // Colisiones de limites y paredes para el movimiento automatico del enemigo
    
    // Calcula que casilla tiene el gato justo enfrente segun su direccion
    int col_frente_gato = 0;
    if (malo.direccion == 1) {
        // Si va a la derecha, revisa el bloque que toca su borde derecho
        col_frente_gato = (malo.x + malo.ancho / 2 + 2) / TAM_BLOQUE;
    } else {
        // Si va a la izquierda, revisa el bloque que toca su borde izquierdo
        col_frente_gato = (malo.x - malo.ancho / 2 - 2) / TAM_BLOQUE;
    }
    
    int fil_centro_gato = malo.y / TAM_BLOQUE;

    // Si la casilla de enfrente esta dentro de la matriz, verificamos si es solida ('#' o '-')
    if (fil_centro_gato >= 0 && fil_centro_gato < FILAS && col_frente_gato >= 0 && col_frente_gato < COLUMNAS) {
        char celda_enfrente = matriz_nivel[fil_centro_gato][col_frente_gato];
        
        if (celda_enfrente == '#' || celda_enfrente == '-') {
            // Si hay pared enfrente, invierte la direccion
            malo.direccion = -malo.direccion; 
        }
    }

    // Colisiones de limites bordes del mapa
    if (malo.x - malo.ancho / 2 < 0) {
        malo.x = malo.ancho / 2;
        malo.direccion = 1; 
    }
    if (malo.x + malo.ancho / 2 > (COLUMNAS * TAM_BLOQUE)) {
        malo.x = (COLUMNAS * TAM_BLOQUE) - malo.ancho / 2;
        malo.direccion = -1; 
    }
}