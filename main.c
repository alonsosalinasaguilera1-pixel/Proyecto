#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdio.h>
#include "globales.h"
#include "fisicas.h"
#include "colisiones.h"

// Declaracion de funciones
void renderizar_pantalla(ALLEGRO_BITMAP *imagen, ALLEGRO_BITMAP *fondo, ALLEGRO_BITMAP *sprite_bloque, ALLEGRO_BITMAP *sprite_moneda, 
    ALLEGRO_BITMAP *sprite_suelo, ALLEGRO_BITMAP *sprite_gato1, ALLEGRO_BITMAP *sprite_gato2, ALLEGRO_BITMAP *sprite_pal_desact, ALLEGRO_BITMAP *sprite_pal_act, 
    ALLEGRO_BITMAP *sprite_gav1, ALLEGRO_BITMAP *sprite_gav2, ALLEGRO_BITMAP *sprite_caca);

// Funcion principal
int main() {
    
    // Inicializacion de Allegro y sus complementos
    if (!al_init()) return -1;
    if (!al_install_keyboard()) return -1;
    if (!al_init_primitives_addon()) return -1;
    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    letra_interfaz = al_create_builtin_font();

    al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
    ALLEGRO_DISPLAY *display = al_create_display(ANCHO, ALTO);
    if (!display) return -1;

    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

    // Carga el mapa
    cargar_nivel("nivel1.txt");

    // Carga los bitmaps
    ALLEGRO_BITMAP *imagen = al_load_bitmap("imagenes/nocco.png");
    if (!imagen) {
        al_destroy_display(display);
        return -1;
    }

    ALLEGRO_BITMAP *fondo = al_load_bitmap("imagenes/fondo1.png");
    ALLEGRO_BITMAP *fondo2 = al_load_bitmap("imagenes/fondo2.png");
    if (!fondo || !fondo2) {
        al_destroy_bitmap(imagen);
        if (fondo) al_destroy_bitmap(fondo);
        al_destroy_display(display);
        return -1;
    }

    ALLEGRO_BITMAP *sprite_bloque = al_load_bitmap("imagenes/bloque11.png");
    ALLEGRO_BITMAP *sprite_moneda = al_load_bitmap("imagenes/moneda_nocco.png");
    ALLEGRO_BITMAP *sprite_suelo = al_load_bitmap("imagenes/suelo1.png");
    ALLEGRO_BITMAP *sprite_gato1 = al_load_bitmap("imagenes/gato1.png");
    ALLEGRO_BITMAP *sprite_gato2 = al_load_bitmap("imagenes/gato2.png");
    
    // Carga de las palancas activada y desactivada
    sprite_palanca_desactivada = al_load_bitmap("imagenes/palanca_desactivada.png");
    sprite_palanca_activada = al_load_bitmap("imagenes/palanca_activada.png");
    sprite_gaviota1 = al_load_bitmap("imagenes/gaviota1.png");
    sprite_gaviota2 = al_load_bitmap("imagenes/gaviota2.png");
    sprite_caca = al_load_bitmap("imagenes/caca.png");

    if (!sprite_bloque || !sprite_moneda || !sprite_suelo || !sprite_gato1 || !sprite_gato2 || 
        !sprite_palanca_desactivada || !sprite_palanca_activada || 
        !sprite_gaviota1 || !sprite_gaviota2 || !sprite_caca) {

        al_destroy_bitmap(imagen);
        al_destroy_bitmap(fondo);
        al_destroy_bitmap(fondo2);
        if (sprite_bloque) al_destroy_bitmap(sprite_bloque);
        if (sprite_moneda) al_destroy_bitmap(sprite_moneda);
        if (sprite_suelo) al_destroy_bitmap(sprite_suelo);
        if (sprite_gato1) al_destroy_bitmap(sprite_gato1);
        if (sprite_gato2) al_destroy_bitmap(sprite_gato2);
        if (sprite_palanca_desactivada) al_destroy_bitmap(sprite_palanca_desactivada);
        if (sprite_palanca_activada) al_destroy_bitmap(sprite_palanca_activada);
        al_destroy_display(display);
        return -1;
    }

    al_set_window_title(display, "Rocco Jump");

    // Marca el ritmo a 60 FPS
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_keyboard_event_source());

    // Propiedades fijas de tamaño y velocidad iniciales
    jugador.alto = 50.0; 
    jugador.ancho = 50.0;
    jugador.velocidad = 5.0; 

    malo.alto = 40.0;
    malo.ancho = 40.0;
    malo.velocidad = 2.0;
    malo.direccion = 1;

    // Buscar las posiciones de inicio en el mapa
    inicializar_nivel();
    al_start_timer(timer);

    // Bucle de juego principal
    while (!juego.salir) {
        ALLEGRO_EVENT evento;
        al_wait_for_event(queue, &evento);

        // Actualizamos el juego 60 veces por segundo
        if (evento.type == ALLEGRO_EVENT_TIMER) {
            actualizar_fisicas_juego();
            procesar_todas_las_colisiones();
            actualizar_gaviota();

            // Si toca la meta, reubica de forma segura en este frame
            if (juego.debe_teletransportar) {
                inicializar_nivel();
                juego.debe_teletransportar = false;
            }

            // Control matematico del scroll de la camara
            juego.camara_x = jugador.x - ANCHO / 2;
            if (juego.camara_x < 0) {
                juego.camara_x = 0;
            }
            if (juego.camara_x > (COLUMNAS * TAM_BLOQUE) - ANCHO) {
                juego.camara_x = (COLUMNAS * TAM_BLOQUE) - ANCHO;
            }

            juego.redibujar = true;
        }
        
        // Eventos cuando se presiona una tecla
        else if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (evento.keyboard.keycode) {
                case ALLEGRO_KEY_UP:    juego.teclas[UP] = true; break;
                case ALLEGRO_KEY_DOWN:  juego.teclas[DOWN] = true; break;
                case ALLEGRO_KEY_LEFT:  juego.teclas[LEFT] = true; break;
                case ALLEGRO_KEY_RIGHT: juego.teclas[RIGHT] = true; break;
            }
        }
        
        // Eventos cuando se suelta una tecla
        else if (evento.type == ALLEGRO_EVENT_KEY_UP) {
            switch (evento.keyboard.keycode) {
                case ALLEGRO_KEY_UP:    juego.teclas[UP] = false; break;
                case ALLEGRO_KEY_DOWN:  juego.teclas[DOWN] = false; break;
                case ALLEGRO_KEY_LEFT:  juego.teclas[LEFT] = false; break;
                case ALLEGRO_KEY_RIGHT: juego.teclas[RIGHT] = false; break;
                case ALLEGRO_KEY_ESCAPE: juego.salir = true; break;
            }
        }
        
        // Evento de cerrar la ventana con la X
        else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            juego.salir = true;
        }

        // Renderizado grafico
        if (juego.redibujar && al_is_event_queue_empty(queue)) {
            juego.redibujar = false;
            
            // Selecciona el fondo adecuado segun el nivel
            ALLEGRO_BITMAP *fondo_actual = fondo;
            if (juego.nivel_actual == 2) {
                fondo_actual = fondo2;
            }
            
            renderizar_pantalla(imagen, fondo_actual, sprite_bloque, sprite_moneda, sprite_suelo, sprite_gato1, sprite_gato2, sprite_palanca_desactivada, 
                                sprite_palanca_activada, sprite_gaviota1, sprite_gaviota2, sprite_caca);
        }
    }

    // Liberacion de memoria
    al_destroy_font(letra_interfaz);
    al_destroy_bitmap(imagen);
    al_destroy_bitmap(fondo);
    al_destroy_bitmap(fondo2);
    al_destroy_bitmap(sprite_bloque);
    al_destroy_bitmap(sprite_moneda);
    al_destroy_bitmap(sprite_suelo);
    al_destroy_bitmap(sprite_gato1);
    al_destroy_bitmap(sprite_gato2);
    al_destroy_bitmap(sprite_palanca_desactivada);
    al_destroy_bitmap(sprite_palanca_activada);
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}

// Definicion de funciones
void renderizar_pantalla(ALLEGRO_BITMAP *imagen, ALLEGRO_BITMAP *fondo, ALLEGRO_BITMAP *sprite_bloque, ALLEGRO_BITMAP *sprite_moneda, ALLEGRO_BITMAP *sprite_suelo, ALLEGRO_BITMAP *sprite_gato1, ALLEGRO_BITMAP *sprite_gato2, ALLEGRO_BITMAP *sprite_pal_desact, ALLEGRO_BITMAP *sprite_pal_act, ALLEGRO_BITMAP *sprite_gav1, ALLEGRO_BITMAP *sprite_gav2, ALLEGRO_BITMAP *sprite_caca) {
    
    // Limpia pantalla
    al_clear_to_color(al_map_rgb(0, 0, 0)); 

    // Dibuja fondo infinito
    int ancho_fondo = al_get_bitmap_width(fondo);
    int total_ancho_nivel = COLUMNAS * TAM_BLOQUE; 
    for (int x_original = 0; x_original < total_ancho_nivel; x_original += ancho_fondo) {
        int x_pantalla = x_original - juego.camara_x; 
        if (x_pantalla + ancho_fondo > 0 && x_pantalla < ANCHO) {
            al_draw_scaled_bitmap(
                fondo, 
                0, 0, ancho_fondo, al_get_bitmap_height(fondo), 
                x_pantalla, 0, ancho_fondo, ALTO, 
                0
            );
        }
    }

    // Dibujar matriz del nivel
    for (int f = 0; f < FILAS; f++) {
        for (int c = 0; c < COLUMNAS; c++) {
            int x_pantalla = (c * TAM_BLOQUE) - juego.camara_x;
            int y_pantalla = f * TAM_BLOQUE;

            // Bloque flotante / Pared ('#')
            if (matriz_nivel[f][c] == '#') {
                al_draw_scaled_bitmap(
                    sprite_bloque, 
                    0, 0, al_get_bitmap_width(sprite_bloque), al_get_bitmap_height(sprite_bloque),
                    x_pantalla, y_pantalla, TAM_BLOQUE, TAM_BLOQUE, 0
                );
            }
            // Suelo plano ('-')
            else if (matriz_nivel[f][c] == '-') {
                al_draw_scaled_bitmap(
                    sprite_suelo, 
                    264, 116, 500, 500,
                    x_pantalla, y_pantalla, TAM_BLOQUE, TAM_BLOQUE, 0
                );
            }
            // Monedas ('O')
            else if (matriz_nivel[f][c] == 'O') {
                al_draw_scaled_bitmap(
                    sprite_moneda, 
                    0, 0, al_get_bitmap_width(sprite_moneda), al_get_bitmap_height(sprite_moneda),
                    x_pantalla, y_pantalla, TAM_BLOQUE, TAM_BLOQUE, 0
                );
            }
            // Dibujar pinchos ('X')
            else if (matriz_nivel[f][c] == 'X') {
                al_draw_filled_triangle(x_pantalla + TAM_BLOQUE / 2, y_pantalla, x_pantalla, y_pantalla + TAM_BLOQUE, x_pantalla + TAM_BLOQUE, y_pantalla + TAM_BLOQUE, al_map_rgb(255, 50, 50));
            }
            // Dibujar palanca desactivada ('L')
            else if (matriz_nivel[f][c] == 'L') {
                al_draw_scaled_bitmap(
                    sprite_pal_desact, 
                    0, 0, al_get_bitmap_width(sprite_pal_desact), al_get_bitmap_height(sprite_pal_desact),
                    x_pantalla, y_pantalla, TAM_BLOQUE, TAM_BLOQUE, 0
                );
            }
            // Dibujar palanca activada ('l')
            else if (matriz_nivel[f][c] == 'l') {
                al_draw_scaled_bitmap(
                    sprite_pal_act, 
                    0, 0, al_get_bitmap_width(sprite_pal_act), al_get_bitmap_height(sprite_pal_act),
                    x_pantalla, y_pantalla, TAM_BLOQUE, TAM_BLOQUE, 0
                );
            }
            // Meta ('M')
            else if (matriz_nivel[f][c] == 'M') {
                // Solo se ve de color si se han activado las 2 palancas
                if (juego.palancas_activadas >= 2) {
                    al_draw_filled_rectangle(x_pantalla + 5, y_pantalla + 5, x_pantalla + TAM_BLOQUE - 5, y_pantalla + TAM_BLOQUE - 5, al_map_rgb(0, 191, 255));
                } else {
                    // Si no estan activadas, se ve un cuadrado gris bloqueando el paso
                    al_draw_filled_rectangle(x_pantalla + 5, y_pantalla + 5, x_pantalla + TAM_BLOQUE - 5, y_pantalla + TAM_BLOQUE - 5, al_map_rgb(80, 80, 80));
                }
            }
        }
    }
    
    // Dibujar el jugador
    al_draw_scaled_bitmap(
        imagen, 
        0, 0, al_get_bitmap_width(imagen), al_get_bitmap_height(imagen), 
        (jugador.x - juego.camara_x) - jugador.ancho / 2, jugador.y - jugador.alto / 2, 
        jugador.ancho, jugador.alto, 
        0
    );

    // Dibujar enemigo gato
    int frame_gato = ((int)(al_get_time() * 8)) % 2;
    ALLEGRO_BITMAP *sprite_actual_gato = (frame_gato == 0) ? sprite_gato1 : sprite_gato2;
    int flags_espejo = (malo.direccion == -1) ? ALLEGRO_FLIP_HORIZONTAL : 0;
    
    al_draw_scaled_bitmap(
        sprite_actual_gato,
        0, 0, al_get_bitmap_width(sprite_actual_gato), al_get_bitmap_height(sprite_actual_gato),
        (malo.x - juego.camara_x) - malo.ancho / 2, malo.y - malo.alto / 2,
        malo.ancho, malo.alto,
        flags_espejo
    );
    // Dibujar gaviota
    int frame_gaviota = ((int)(al_get_time() * 5)) % 2;
    ALLEGRO_BITMAP *sprite_actual_gaviota = (frame_gaviota == 0) ? sprite_gav1 : sprite_gav2;
    al_draw_scaled_bitmap(
        sprite_actual_gaviota, 0, 0, al_get_bitmap_width(sprite_actual_gaviota), al_get_bitmap_height(sprite_actual_gaviota),
        (gaviota.x - juego.camara_x) - 30, gaviota.y - 30, 60, 60, 0
    );

    // Dibujar cacas
    for(int i = 0; i < 5; i++) {
        if(gaviota.balas[i].activo) {
            al_draw_scaled_bitmap(
                sprite_caca, 0, 0, al_get_bitmap_width(sprite_caca), al_get_bitmap_height(sprite_caca),
                (gaviota.balas[i].x - juego.camara_x) - 10, gaviota.balas[i].y - 10, 20, 20, 0
            );
        }
    }

    // Dibujar texto de puntos y tiempo
    al_draw_textf(letra_interfaz, al_map_rgb(255, 0, 0), 60, 60, ALLEGRO_ALIGN_LEFT, "PUNTOS: %06d", juego.puntos);
    al_draw_textf(letra_interfaz, al_map_rgb(255, 0, 0), ANCHO - 60, 60, ALLEGRO_ALIGN_RIGHT, "TIEMPO: %d", (int)juego.segundos_restantes);

    al_flip_display();
}