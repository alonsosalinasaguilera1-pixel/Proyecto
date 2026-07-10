#ifndef GLOBALES_H
#define GLOBALES_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <stdbool.h>

// CONSTANTES ORIGINALES DE PANTALLA Y MATRIZ
#define FILAS 27
#define COLUMNAS 150
#define TAM_BLOQUE 40
#define ANCHO 1920
#define ALTO  1080

// ESTADOS DEL TECLADO
enum { UP, DOWN, LEFT, RIGHT };

// ESTRUCTURA ORIGINAL DEL JUGADOR
struct Personaje {
    float x;
    float y;
    float ancho;
    float alto;
    float velocidad;
    float vel_y;
    bool en_suelo;
};

// ESTRUCTURA ORIGINAL DEL ENEMIGO
struct Enemigo {
    float x;
    float y;
    float ancho;
    float alto;
    float velocidad;
    int direccion; 
};  

// NUEVA ESTRUCTURA PARA ORGANIZAR VARIABLES SUELTAS
struct EstadoJuego {
    int puntos;
    float segundos_restantes;
    int nivel_actual;
    float camara_x;
    bool salir;
    bool redibujar;
    bool teclas[4];
    bool debe_teletransportar; // Bandera para el cambio seguro de nivel
};

// VARIABLES COMPARTIDAS CON OTROS ARCHIVOS
extern char matriz_nivel[FILAS][COLUMNAS];
extern ALLEGRO_FONT *letra_interfaz;
extern struct Personaje jugador;
extern struct Enemigo malo;
extern struct EstadoJuego juego;

// FUNCIONES QUE SE COMPARTIRAN
void cargar_nivel(const char *nombre_archivo);
void inicializar_nivel(void);

#endif