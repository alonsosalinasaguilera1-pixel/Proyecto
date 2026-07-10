#include "globales.h"
#include "fisicas.h"

// LOGICA DE TIEMPO, GRAVEDAD Y MOVIMIENTO DEL ENEMIGO
void actualizar_fisicas_juego(void) {
    float fuerza_gravedad = 0.5; 
    
    // Control del reloj del juego (Tiempo en reversa)
    if (juego.segundos_restantes > 0) {
        juego.segundos_restantes -= (1.0 / 60.0); 
    } else {
        juego.segundos_restantes = 0; 
    }
    
    // Si el jugador no esta en el suelo, la gravedad lo hace caer
    if (!jugador.en_suelo) {
        jugador.vel_y += fuerza_gravedad;
    } else {
        jugador.vel_y = 0; 
    } 

    // Movimiento automatico del enemigo (malo)
    if (malo.direccion == 0) {
        malo.direccion = 1;
    }
    malo.x += malo.velocidad * malo.direccion;
}