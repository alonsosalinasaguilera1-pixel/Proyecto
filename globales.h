#ifndef GLOBALES_H
#define GLOBALES_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <stdbool.h>

// Constantes originales de pantalla y matriz
#define FILAS 27
#define COLUMNAS 150
#define TAM_BLOQUE 40
#define ANCHO 1920
#define ALTO  1080

// Estados del teclado
enum { UP, DOWN, LEFT, RIGHT };

// Estructura original del jugador
struct Personaje {
    float x;
    float y;
    float ancho;
    float alto;
    float velocidad;
    float vel_y;
    bool en_suelo;
};

// Estructura original del enemigo
struct Enemigo {
    float x;
    float y;
    float ancho;
    float alto;
    float velocidad;
    int direccion; // -1 izquierda, 1 derecha
    int frame_animacion; 
};  

// Estructura para el proyectil de la gaviota
struct Proyectil {
    float x, y;
    bool activo;
};

// Estructura para la gaviota
struct Gaviota {
    float x, y;
    float velocidad;
    float timer_disparo;
    struct Proyectil balas[5]; // Arreglo basico para 5 cacas
};

// Estructura para organizar variables sueltas
struct EstadoJuego {
    int puntos;
    float segundos_restantes;
    int nivel_actual;
    float camara_x;
    bool salir;
    bool redibujar;
    bool teclas[4];
    bool debe_teletransportar; // Bandera para el cambio seguro de nivel
    
    int palancas_activadas;
};

// Variables compartidas con otros archivos
extern char matriz_nivel[FILAS][COLUMNAS];
extern ALLEGRO_FONT *letra_interfaz;
extern struct Personaje jugador;
extern struct Enemigo malo;
extern struct Gaviota gaviota;
extern struct EstadoJuego juego;

// Declaracion compartida de los sprites
extern ALLEGRO_BITMAP *sprite_palanca_desactivada;
extern ALLEGRO_BITMAP *sprite_palanca_activada;
extern ALLEGRO_BITMAP *sprite_gaviota1;
extern ALLEGRO_BITMAP *sprite_gaviota2;
extern ALLEGRO_BITMAP *sprite_caca;

// Funciones que se compartiran
void cargar_nivel(const char *nombre_archivo);
void inicializar_nivel(void);

// Funcion para actualizar la gaviota
void actualizar_gaviota(void); 

#endif