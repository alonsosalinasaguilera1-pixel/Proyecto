#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
//TODO: Agregar paredes y colisionar con ellas
//TODO: Poner un personaje nuevo con movimiento propio (que suba y baje/izquierda o derecha) y con imagen

#define ANCHO 800
#define ALTO  600

struct cuadrado
{
    float x, y, ancho, alto, velocidad;
}
;
int main()
{
    // Inicialización de Allegro
    struct cuadrado jugador;
    if (!al_init()) return -1;
    if (!al_install_keyboard()) return -1;
    if (!al_init_primitives_addon()) return -1;
    al_init_image_addon();

    // Crear la ventana gráfica
    ALLEGRO_DISPLAY *display = al_create_display(ANCHO, ALTO);
    if (!display) return -1;

    ALLEGRO_BITMAP *imagen = al_load_bitmap("imagenes/gato.png");

    if (!imagen)
    {
       return -1;
    }
    al_set_window_title(display, "Cuadrado 2D con Allegro 5");

    // Configurar reloj (60 FPS) y cola de eventos
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_keyboard_event_source());

    // Posición inicial y tamaño del cuadrado
    jugador.x = ANCHO / 2.0;
    jugador.y = ALTO / 2.0;
    jugador.alto = 50.0; // El cuadrado medirá 50x50 píxeles
    jugador.ancho = 50.0;
    jugador.velocidad = 5.0; // Píxeles que se mueve por fotograma

    // Estado del teclado
    bool teclas[4] = {false, false, false, false};
    enum { UP, DOWN, LEFT, RIGHT };

    bool salir = false;
    bool redibujar = true;

    al_start_timer(timer);

    // Bucle principal
    while (!salir) {
        ALLEGRO_EVENT evento;
        al_wait_for_event(queue, &evento);

        if (evento.type == ALLEGRO_EVENT_TIMER) {
            // Mover el cuadrado según las flechas presionadas
            if (teclas[UP])    
                jugador.y -= jugador.velocidad;
            if (teclas[DOWN])  
                jugador.y += jugador.velocidad;
            if (teclas[LEFT])
                jugador.x -= jugador.velocidad;
            if (teclas[RIGHT])
                jugador.x += jugador.velocidad;

                // Colisión con los bordes de la ventana
             if (jugador.x - jugador.ancho / 2 < 0)
                 jugador.x = jugador.ancho / 2;

            if (jugador.x + jugador.ancho / 2 > ANCHO)
                jugador.x = ANCHO - jugador.ancho / 2;

            if (jugador.y - jugador.alto / 2 < 0)
                jugador.y = jugador.alto / 2;

            if (jugador.y + jugador.alto / 2 > ALTO)
                jugador.y = ALTO - jugador.alto / 2;
                
            redibujar = true;
        }
        else if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (evento.keyboard.keycode) {
                case ALLEGRO_KEY_UP:
                    teclas[UP] = true;
                    break;
                case ALLEGRO_KEY_DOWN:
                    teclas[DOWN] = true;
                    break;
                case ALLEGRO_KEY_LEFT:
                    teclas[LEFT] = true;
                    break;
                case ALLEGRO_KEY_RIGHT:
                    teclas[RIGHT] = true;
                    break;
            }
        }
        else if (evento.type == ALLEGRO_EVENT_KEY_UP) {
            switch (evento.keyboard.keycode) {
                case ALLEGRO_KEY_UP:
                    teclas[UP] = false;
                    break;
                case ALLEGRO_KEY_DOWN:
                    teclas[DOWN] = false;
                    break;
                case ALLEGRO_KEY_LEFT:
                    teclas[LEFT] = false;
                    break;
                case ALLEGRO_KEY_RIGHT:
                    teclas[RIGHT] = false;
                    break;
                case ALLEGRO_KEY_ESCAPE:
                    salir = true;
                    break; // Salir con ESC
            }
        }
        else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            salir = true;
        }

        // Renderizado en pantalla
        if (redibujar && al_is_event_queue_empty(queue)) {
            redibujar = false;
            
            // Fondo negro absoluto
            al_clear_to_color(al_map_rgb(0, 0, 0)); 
            
            // Dibujar cuadrado blanco relleno en las coordenadas (cuad_x, cuad_y)
            al_draw_scaled_bitmap(
            imagen,
            0, 0,
            al_get_bitmap_width(imagen),
            al_get_bitmap_height(imagen),
            jugador.x - jugador.ancho / 2,
            jugador.y - jugador.alto / 2,
            jugador.ancho,
            jugador.alto,
            0
        );
            
            al_flip_display();
        }
    }

    // Limpieza
    al_destroy_bitmap(imagen);
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}
