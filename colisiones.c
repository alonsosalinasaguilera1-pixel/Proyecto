#include "globales.h"
#include "colisiones.h"
#include <stdio.h>

// GESTION DE COLISIONES: PAREDES, SUELO, PINCHOS, MONEDAS Y META
void procesar_todas_las_colisiones(void) {
    
    // 1. CALCULAR INTENTOS DE NUEVA POSICION SEGUN EL TECLADO
    float nueva_x = jugador.x;
    if (juego.teclas[LEFT])  nueva_x -= jugador.velocidad;
    if (juego.teclas[RIGHT]) nueva_x += jugador.velocidad;

    if (juego.teclas[UP] && jugador.en_suelo) {
        jugador.vel_y = -12.0; // Fuerza del salto
        jugador.en_suelo = false;
    }
    
    // Nueva posicion en Y considerando la velocidad de la gravedad calculada en fisicas.c
    float nueva_y = jugador.y + jugador.vel_y;
    if (juego.teclas[DOWN])  nueva_y += jugador.velocidad;

    jugador.en_suelo = false;

    // 2. COLISION EN EL EJE X (IZQUIERDA / DERECHA) CONTRA PAREDES (#)
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
            if (matriz_nivel[fila_cuerpo_sup][col_x_revisar] != '#' && matriz_nivel[fila_cuerpo_inf][col_x_revisar] != '#') {
                jugador.x = nueva_x; // Si hay aire, se confirma el movimiento en X
            }
        }
    }

    // 3. COLISION EN EL EJE Y (ABAJO / CAIDA) CONTRA BLOQUES FIRMES (#)
    int col_pie_izq = (jugador.x - jugador.ancho / 2 + 2) / TAM_BLOQUE;
    int col_pie_der = (jugador.x + jugador.ancho / 2 - 2) / TAM_BLOQUE;
    int fil_pies = (nueva_y + jugador.alto / 2) / TAM_BLOQUE;

    if (fil_pies >= 0 && fil_pies < FILAS && col_pie_izq >= 0 && col_pie_der < COLUMNAS) {
        if (matriz_nivel[fil_pies][col_pie_izq] != '#' && matriz_nivel[fil_pies][col_pie_der] != '#') {
            jugador.y = nueva_y; // Si hay aire abajo, el jugador sigue cayendo
        } else {
            // Encontro suelo firme
            if (jugador.vel_y >= 0) {
                jugador.en_suelo = true;
                jugador.vel_y = 0;
                // Alineacion matematica perfecta con la parte de arriba del bloque
                jugador.y = (fil_pies * TAM_BLOQUE) - (jugador.alto / 2.0);
            }
        }
    }

    // 4. DETECCION POR CASILLAS (CENTRO DEL CUERPO DEL JUGADOR)
    int col_centro = jugador.x / TAM_BLOQUE;
    int fil_centro = jugador.y / TAM_BLOQUE;

    if (fil_centro >= 0 && fil_centro < FILAS && col_centro >= 0 && col_centro < COLUMNAS) {
        
        // Colision con pinchos (X) Regresa al inicio buscando la 'P'
        if (matriz_nivel[fil_centro][col_centro] == 'X') {
            inicializar_nivel();
        }
        
        // Colision con Monedas (O)  Desaparece del mapa y suma 100 puntos
        if (matriz_nivel[fil_centro][col_centro] == 'O') {
            matriz_nivel[fil_centro][col_centro] = ' '; 
            juego.puntos += 100;                               
        }
        
        // Colision con la Meta (M) Cambio de nivel automatico
        if (matriz_nivel[fil_centro][col_centro] == 'M') {
            juego.nivel_actual++; 
            
            char proximo_archivo[20];
            sprintf(proximo_archivo, "nivel%d.txt", juego.nivel_actual);
            
            FILE *test = fopen(proximo_archivo, "r");
            if (test != NULL) {
                fclose(test);
                cargar_nivel(proximo_archivo); 
                juego.debe_teletransportar = true; // Activa la bandera para reubicar en el main
            } else {
                printf("¡Felicidades, ganaste el juego completo!\n");
                juego.salir = true; 
            }
        }
    }

    // 5. COLISIONES DE CONTROL CONTRA LOS BORDES TOTALES DEL MAPA EXTENDIDO
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

    // 6. COLISIONES DE LIMITES PARA EL MOVIMIENTO AUTOMATICO DEL ENEMIGO
    if (malo.x - malo.ancho / 2 < 0) {
        malo.x = malo.ancho / 2;
        malo.direccion = 1; 
    }
    if (malo.x + malo.ancho / 2 > (COLUMNAS * TAM_BLOQUE)) {
        malo.x = (COLUMNAS * TAM_BLOQUE) - malo.ancho / 2;
        malo.direccion = -1; 
    }
}