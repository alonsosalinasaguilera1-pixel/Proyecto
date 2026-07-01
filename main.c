#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include<stdio.h>
#include<stdbool.h>

//TODO: Agregar monedas al archivo del nivel
//TODO: Dibujar monedas desde la matriz del nivel
//TODO: Detectar colision entre jugador y moneda
//TODO: Hacer que la moneda desaparezca al ser recolectada
//TODO: Aumentar puntaje al recoger monedas
//TODO: Mostrar puntaje en pantalla
//TODO: Implementar tiempo por nivel
//TODO: Mostrar tiempo en pantalla
//TODO: Crear al menos 3 niveles adicionales: nivel2.txt, nivel3.txt, nivel4.txt
//TODO: Reiniciar posicion del jugador al comenzar cada nivel
//TODO: Mantener puntaje y tiempo acumulado entre niveles

#define FILAS 27
#define COLUMNAS 150
#define TAM_BLOQUE 40
#define ANCHO 1920
#define ALTO  1080

char matriz_nivel[FILAS][COLUMNAS];
float camara_x = 0;
struct Personaje

{
    float x, y, ancho, alto, velocidad;
    float vel_y;
    bool en_suelo;
};

struct Enemigo
{
    float x, y, ancho, alto, velocidad;
    int direccion; //sirve para saber hacia donde se mueve solo
};  

void cargar_nivel() {
    FILE *archivo = fopen("nivel1.txt", "r");
    
    // si el archivo no existe o no se encuentra, avisa en la consola
    if (archivo == NULL) {
        printf("Error: No se pudo abrir el archivo nivel1.txt\n");
        return;
    }

    int f = 0; // contador de filas
    int c = 0; // contador de columnas
    char caracter;

    // leemos el archivo caracter por caracter hasta el final (EOF)
    while ((caracter = fgetc(archivo)) != EOF && f < FILAS) {
        // ignoramos los saltos de linea para que no descuadren la matriz
        if (caracter != '\n' && caracter != '\r') {
            matriz_nivel[f][c] = caracter;
            c++; // pasa a la siguiente columna
            
            // si llegamos al final de la columna (20), pasamos a la siguiente fila
            if (c >= COLUMNAS) {
                c = 0;
                f++;
            }
        }
    }

    fclose(archivo); // cerramos el archivo al terminar
}

int main()
{

    
    // inicialización de Allegro
    struct Personaje jugador;
    cargar_nivel();

    printf("--- COMPROBANDO MATRIZ CARGADA ---\n");
    for (int f = 0; f < FILAS; f++) {
        for (int c = 0; c < COLUMNAS; c++) {
            printf("%c", matriz_nivel[f][c]);
        }
        printf("\n"); 
    }
    printf("----------------------------------\n");

    if (!al_init()) return -1;
    if (!al_install_keyboard()) return -1;
    if (!al_init_primitives_addon()) return -1;
    al_init_image_addon();

    // configurar Allegro para que abra en pantalla completa
    al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
    // crear la ventana grafica
    ALLEGRO_DISPLAY *display = al_create_display(ANCHO, ALTO);
    if (!display) return -1;

    ALLEGRO_BITMAP *imagen = al_load_bitmap("imagenes/Nocco2.png");

    if (!imagen)
    {
       return -1;
    }
    al_set_window_title(display, "Cuadrado 2D con Allegro 5");

    // configurar reloj (60 FPS) y cola de eventos
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_keyboard_event_source());

    // estado del teclado
    enum { UP, DOWN, LEFT, RIGHT };

    bool salir = false;
    bool redibujar = true;

    al_start_timer(timer);

    // crear la variable del enemigo
    struct Enemigo malo;

    // buscar las posiciones de ambos en el mapa
    for (int f = 0; f < FILAS; f++) {
        for (int c = 0; c < COLUMNAS; c++) {
            
            // si encuentra la P, ubica al jugador
            if (matriz_nivel[f][c] == 'P') {
                jugador.x = (c * TAM_BLOQUE) + (TAM_BLOQUE / 2.0);
                jugador.y = (f * TAM_BLOQUE) + (TAM_BLOQUE / 2.0);
            }
            
            // si encuentra la E, ubica al malo dentro del mismo bucle
            if (matriz_nivel[f][c] == 'E') {
                malo.x = (c * TAM_BLOQUE) + (TAM_BLOQUE / 2.0);
                malo.y = (f * TAM_BLOQUE) + (TAM_BLOQUE / 2.0);
            }
            
        }
    }

    // propiedades de tamaño y velocidad
    jugador.alto = 50.0; 
    jugador.ancho = 50.0;
    jugador.velocidad = 5.0; 
    jugador.vel_y = 0.0;     
    jugador.en_suelo = false;

    malo.alto = 40.0;
    malo.ancho = 40.0;
    malo.velocidad = 2.0;
    malo.direccion = 0;

    // estado del teclado
    bool teclas[4] = {false, false, false, false};

    // bucle principal
    while (!salir) {
        ALLEGRO_EVENT evento;
        al_wait_for_event(queue, &evento);

        if (evento.type == ALLEGRO_EVENT_TIMER) {
            float fuerza_gravedad = 0.5; 
            
            // si no esta en el suelo, la gravedad lo hace caer
            if (!jugador.en_suelo) {
                jugador.vel_y += fuerza_gravedad;
            } else {
                jugador.vel_y = 0; 
            }

            // guardamos la posicion original y calculamos a donde intenta ir en X
            float nueva_x = jugador.x;
            if (teclas[LEFT])  nueva_x -= jugador.velocidad;
            if (teclas[RIGHT]) nueva_x += jugador.velocidad;

            // control de salto
            if (teclas[UP] && jugador.en_suelo) {
                jugador.vel_y = -12.0; // fuerza del salto
                jugador.en_suelo = false;
            }

            // calculamos a donde intenta ir en Y debido a la gravedad y controles
            float nueva_y = jugador.y + jugador.vel_y;
            if (teclas[DOWN])  nueva_y += jugador.velocidad;

            // control de colisiones
            jugador.en_suelo = false;

            // movimiento y colision en el eje X (izquierda / derecha)
            int fila_cuerpo_sup = (jugador.y - jugador.alto / 2 + 5) / TAM_BLOQUE;
            int fila_cuerpo_inf = (jugador.y + jugador.alto / 2 - 5) / TAM_BLOQUE;
            
            if (teclas[LEFT] || teclas[RIGHT]) {
                int col_x_revisar = 0;
                if (nueva_x < jugador.x) {
                    // moviendose a la izquierda
                    col_x_revisar = (nueva_x - jugador.ancho / 2) / TAM_BLOQUE;
                } else {
                    // moviendose a la derecha
                    col_x_revisar = (nueva_x + jugador.ancho / 2) / TAM_BLOQUE;
                }

                if (fila_cuerpo_sup >= 0 && fila_cuerpo_inf < FILAS && col_x_revisar >= 0 && col_x_revisar < COLUMNAS) {
                    if (matriz_nivel[fila_cuerpo_sup][col_x_revisar] != '#' && matriz_nivel[fila_cuerpo_inf][col_x_revisar] != '#') {
                        jugador.x = nueva_x;
                    }
                }
            }

            int col_pie_izq = (jugador.x - jugador.ancho / 2 + 2) / TAM_BLOQUE;
            int col_pie_der = (jugador.x + jugador.ancho / 2 - 2) / TAM_BLOQUE;
            int fil_pies = (nueva_y + jugador.alto / 2) / TAM_BLOQUE;

            if (fil_pies >= 0 && fil_pies < FILAS && col_pie_izq >= 0 && col_pie_der < COLUMNAS) {
                if (matriz_nivel[fil_pies][col_pie_izq] != '#' && matriz_nivel[fil_pies][col_pie_der] != '#') {
                    jugador.y = nueva_y; // si hay aire, cae
                } else {
                    // encontro suelo firme
                    if (jugador.vel_y >= 0) {
                        jugador.en_suelo = true;
                        jugador.vel_y = 0;
                        // alineacion matemática perfecta con la superficie del bloque
                        jugador.y = (fil_pies * TAM_BLOQUE) - (jugador.alto / 2.0);
                    }
                }
            }

            // detectar colision con pinchos
            int col_centro = jugador.x / TAM_BLOQUE;
            int fil_centro = jugador.y / TAM_BLOQUE;

            if (fil_centro >= 0 && fil_centro < FILAS && col_centro >= 0 && col_centro < COLUMNAS) {
                if (matriz_nivel[fil_centro][col_centro] == 'X') {
                    // si toca un pincho, lo mandamos al inicio buscando la 'P'
                    for (int f = 0; f < FILAS; f++) {
                        for (int c = 0; c < COLUMNAS; c++) {
                            if (matriz_nivel[f][c] == 'P') {
                                jugador.x = (c * TAM_BLOQUE) + (TAM_BLOQUE / 2.0);
                                jugador.y = (f * TAM_BLOQUE) + (TAM_BLOQUE / 2.0);
                                jugador.vel_y = 0;
                                jugador.en_suelo = false;
                            }
                        }
                    }
                }
            }

            // implementar el scroll de la camara
            // centramos la camara en la posición X del jugador
            camara_x = jugador.x - ANCHO / 2;

            // ponemos limites para que la camara no grabe fuera del mapa izquierdo
            if (camara_x < 0) {
                camara_x = 0;
            }
            // ni fuera del mapa derecho (150 columnas * 40 tamaño - ancho de pantalla)
            if (camara_x > (COLUMNAS * TAM_BLOQUE) - ANCHO) {
                camara_x = (COLUMNAS * TAM_BLOQUE) - ANCHO;
            }

            // movimiento automatico del enemigo (malo)
            if (malo.direccion == 0) {
                malo.direccion = 1;
            }

            // colision con los bordes de la ventana de juego
            if (jugador.x - jugador.ancho / 2 < 0)
                jugador.x = jugador.ancho / 2;

            if (jugador.x + jugador.ancho / 2 > ANCHO)
                jugador.x = ANCHO - jugador.ancho / 2;

            if (jugador.y - jugador.alto / 2 < 0)
                jugador.y = jugador.alto / 2;

            if (jugador.y + jugador.alto / 2 > ALTO) {
                jugador.y = ALTO - jugador.alto / 2;
                jugador.en_suelo = true;
                jugador.vel_y = 0;
            }

            // movimiento automatico del enemigo (malo)
            if (malo.direccion == 0) {
                malo.direccion = 1;
            }

            malo.x += malo.velocidad * malo.direccion;

            if (malo.x - malo.ancho / 2 < 0) {
                malo.x = malo.ancho / 2;
                malo.direccion = 1; 
            }
            if (malo.x + malo.ancho / 2 > ANCHO) {
                malo.x = ANCHO - malo.ancho / 2;
                malo.direccion = -1; 
            }
                
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
                    break; // salir con ESC
            }
        }
        else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            salir = true;
        }

      // renderizado en pantalla
        if (redibujar && al_is_event_queue_empty(queue)) {
            redibujar = false;
            
            // fondo negro absoluto
            al_clear_to_color(al_map_rgb(0, 0, 0)); 

            for (int f = 0; f < FILAS; f++) {
                for (int c = 0; c < COLUMNAS; c++) {
                    // calculamos la posicion absoluta en el mundo y le restamos la camara
                    int x_pantalla = (c * TAM_BLOQUE) - camara_x;
                    int y_pantalla = f * TAM_BLOQUE;

                    // si en esa posición hay una pared (#)
                    if (matriz_nivel[f][c] == '#') {
                        // dibujamos el bloque blanco desplazado por la camara
                        al_draw_filled_rectangle(
                            x_pantalla, 
                            y_pantalla, 
                            x_pantalla + TAM_BLOQUE, 
                            y_pantalla + TAM_BLOQUE, 
                            al_map_rgb(255, 255, 255)
                        );
                    }
                    // dibujar pinchos (X) desplazados por la camara
                    else if (matriz_nivel[f][c] == 'X') {
                        al_draw_filled_triangle(
                            x_pantalla + TAM_BLOQUE / 2, y_pantalla,
                            x_pantalla, y_pantalla + TAM_BLOQUE,
                            x_pantalla + TAM_BLOQUE, y_pantalla + TAM_BLOQUE,
                            al_map_rgb(255, 50, 50)
                        );
                    }
                }
            }
            
            // dibujar el personaje restando la posición de la camara en X
            al_draw_scaled_bitmap(
                imagen,
                0, 0,
                al_get_bitmap_width(imagen),
                al_get_bitmap_height(imagen),
                (jugador.x - camara_x) - jugador.ancho / 2,
                jugador.y - jugador.alto / 2,
                jugador.ancho,
                jugador.alto,
                0
            );

            // dibujar al enemigo restando la posicion de la camara en X
            al_draw_filled_rectangle(
                (malo.x - camara_x) - malo.ancho / 2,
                malo.y - malo.alto / 2, 
                (malo.x - camara_x) + malo.ancho / 2,
                malo.y + malo.alto / 2, 
                al_map_rgb(255, 0, 0)
            );
            
            al_flip_display();
        }
    }

    // limpieza
    al_destroy_bitmap(imagen);
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}
