#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdio.h>
#include "globales.h"
#include "fisicas.h"
#include "colisiones.h"

/*
bibliotecas (#include)
declaraciones (#define, funciones, variables, eestructuras)
main(){
    cuerpo main
}
definicion de funciones

Ejemplo:
int funcion();

int main() {
    //Cuerpo
}

int funcion () {
    // Cuerpo de funcion
}


*/

void renderizar_pantalla(ALLEGRO_BITMAP *imagen);

// FUNCION PRINCIPAL DEL JUEGO
int main() {
    // Cargar el mapa inicial
    cargar_nivel("nivel1.txt");

    // Inicializacion de Allegro 5 y sus complementos
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

    ALLEGRO_BITMAP *imagen = al_load_bitmap("imagenes/Nocco2.png");
    if (!imagen) return -1;

    al_set_window_title(display, "Mi Juego Modular");

    // Configuracion del reloj (60 FPS)
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

        // Evento de reloj (60 veces por segundo)
        if (evento.type == ALLEGRO_EVENT_TIMER) {
            // Llama a las funciones que se crearon en los otros archivos
            actualizar_fisicas_juego();
            procesar_todas_las_colisiones();

            // Si toca la meta, reubicamos de forma segura en este frame
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

        // Renderizado grafico ordenado
        if (juego.redibujar && al_is_event_queue_empty(queue)) {
            juego.redibujar = false;
            renderizar_pantalla(imagen);
        }
    }

    // Liberacion de memoria
    al_destroy_font(letra_interfaz);
    al_destroy_bitmap(imagen);
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}

// FUNCION PARA DIBUJAR TODO EN PANTALLA
void renderizar_pantalla(ALLEGRO_BITMAP *imagen) {
    // Fondo negro absoluto
    al_clear_to_color(al_map_rgb(0, 0, 0)); 

    // Dibujar la matriz del nivel considerando el movimiento de la camara
    for (int f = 0; f < FILAS; f++) {
        for (int c = 0; c < COLUMNAS; c++) {
            int x_pantalla = (c * TAM_BLOQUE) - juego.camara_x;
            int y_pantalla = f * TAM_BLOQUE;

            if (matriz_nivel[f][c] == '#') {
                al_draw_filled_rectangle(x_pantalla, y_pantalla, x_pantalla + TAM_BLOQUE, y_pantalla + TAM_BLOQUE, al_map_rgb(255, 255, 255));
            }
            else if (matriz_nivel[f][c] == 'X') {
                al_draw_filled_triangle(x_pantalla + TAM_BLOQUE / 2, y_pantalla, x_pantalla, y_pantalla + TAM_BLOQUE, x_pantalla + TAM_BLOQUE, y_pantalla + TAM_BLOQUE, al_map_rgb(255, 50, 50));
            }
            else if (matriz_nivel[f][c] == 'O') {
                al_draw_filled_circle(x_pantalla + TAM_BLOQUE / 2, y_pantalla + TAM_BLOQUE / 2, TAM_BLOQUE / 4, al_map_rgb(255, 215, 0));
            }
            else if (matriz_nivel[f][c] == 'M') {
                al_draw_filled_rectangle(x_pantalla + 5, y_pantalla + 5, x_pantalla + TAM_BLOQUE - 5, y_pantalla + TAM_BLOQUE - 5, al_map_rgb(0, 191, 255));
            }
        }
    }
    
    // Dibujar al jugador desplazado por la camara
    al_draw_scaled_bitmap(
        imagen, 0, 0, al_get_bitmap_width(imagen), al_get_bitmap_height(imagen),
        (jugador.x - juego.camara_x) - jugador.ancho / 2, jugador.y - jugador.alto / 2,
        jugador.ancho, jugador.alto, 0
    );

    // Dibujar al enemigo desplazado por la camara
    al_draw_filled_rectangle(
        (malo.x - juego.camara_x) - malo.ancho / 2, malo.y - malo.alto / 2, 
        (malo.x - juego.camara_x) + malo.ancho / 2, malo.y + malo.alto / 2, 
        al_map_rgb(255, 0, 0)
    );
    
    // Dibujar los textos de la interfaz grafica
    al_draw_textf(letra_interfaz, al_map_rgb(255, 0, 0), 60, 60, ALLEGRO_ALIGN_LEFT, "PUNTOS: %06d", juego.puntos);
    al_draw_textf(letra_interfaz, al_map_rgb(255, 0, 0), ANCHO - 60, 60, ALLEGRO_ALIGN_RIGHT, "TIEMPO: %d", (int)juego.segundos_restantes);
    
    al_flip_display();
}