#include<iostream>
#include<string>
#include<cstdlib>
#include<ctime>
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_mixer.h>

#define ALTURA_VENTANA 1000
#define ANCHURA_VENTANA 1500
#define VELOCIDAD 80
#define VELOCIDAD_PELOTA 20
#define LIMITE1 11
#define LIMITE2 21

using namespace std;

SDL_Rect destino_circulo;
SDL_Rect destino_raqueta;
SDL_Rect destino_raqueta2;
SDL_Rect destino_red;

int y_raqueta = 100;
int y_raqueta2 = 100;

int circulo_ancho = 50;
int circulo_alto = 50;

int posX = (ANCHURA_VENTANA - circulo_ancho) / 2;
int posY = (ALTURA_VENTANA - circulo_alto) / 2;

int dx = VELOCIDAD_PELOTA;
int dy = VELOCIDAD_PELOTA;

int jugador1 = 0;
int jugador2 = 0;

int limite;

int main(int argc, char* argv[]) {
    srand(time(NULL));

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        SDL_Log("Error al inicializar SDL: %s", SDL_GetError());
        return 1;
    }

    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        SDL_Log("Error al inicializar SDL_mixer: %s", Mix_GetError());
        return 1;
    }

    Mix_Chunk* sonidoRebote = Mix_LoadWAV("sonidos/boing.wav");
    if (!sonidoRebote) {
        SDL_Log("No se pudo cargar el sonido: %s", Mix_GetError());
    }

    SDL_Window* ventana = SDL_CreateWindow("Huguini Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ANCHURA_VENTANA, ALTURA_VENTANA, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderizador = SDL_CreateRenderer(ventana, -1, SDL_RENDERER_ACCELERATED);

    if(!renderizador) {
        SDL_Log("La creación del renderizador falló: %s", SDL_GetError());
        SDL_DestroyWindow(ventana);
        SDL_Quit();
        return 1;
    }

    SDL_Surface* imagenCirculoSuperficie = IMG_Load("texturas/círculo.png");
    SDL_Surface* imagenRaquetaSuperficie = IMG_Load("texturas/raqueta.jpeg");
    SDL_Surface* imagenRaquetaSuperficie2 = IMG_Load("texturas/raqueta.jpeg");
    SDL_Surface* imagenRedSuperficie = IMG_Load("texturas/red.png");

    bool salir = false;

    while(!salir) {
        SDL_Event evento;

        while(SDL_PollEvent(&evento)) {
            SDL_Keycode keycode;
            switch(evento.type) {
                case SDL_QUIT:
                    salir = true;
                    break;

                case SDL_KEYDOWN:
                    keycode = evento.key.keysym.sym;

                    if(keycode == SDLK_w) {
                        y_raqueta -= VELOCIDAD;
                    } else if(keycode == SDLK_s) {
                        y_raqueta += VELOCIDAD;
                    } else if(keycode == SDLK_DOWN) {
                        y_raqueta2 += VELOCIDAD;
                    } else if(keycode == SDLK_UP) {
                        y_raqueta2 -= VELOCIDAD;
                    }

                default:
                    break;
            }
        }

        posX += dx;
        posY += dy;

        if (posY <= 0 || posY + circulo_alto >= ALTURA_VENTANA) {
            dy = -dy;
            if (sonidoRebote) Mix_PlayChannel(-1, sonidoRebote, 0);
        }

        SDL_Rect pelotaRect = {posX, posY, circulo_ancho, circulo_alto};
        SDL_Rect raqueta1 = {0, y_raqueta, 20, 200};
        SDL_Rect raqueta2 = {1475, y_raqueta2, 20, 200};

        if (SDL_HasIntersection(&pelotaRect, &raqueta1)) {
            dx = abs(dx);
            if (sonidoRebote) Mix_PlayChannel(-1, sonidoRebote, 0);
        }

        if (SDL_HasIntersection(&pelotaRect, &raqueta2)) {
            dx = -abs(dx);
            if (sonidoRebote) Mix_PlayChannel(-1, sonidoRebote, 0);
        }

        destino_circulo = {posX, posY, circulo_ancho, circulo_alto};
        destino_raqueta = {0, y_raqueta, 20, 200};
        destino_raqueta2 = {1475, y_raqueta2, 20, 200};
        destino_red = {740, 0, 20, 1000};

        SDL_Texture* CirculoTextura = SDL_CreateTextureFromSurface(renderizador, imagenCirculoSuperficie);
        SDL_Texture* RaquetaTextura = SDL_CreateTextureFromSurface(renderizador, imagenRaquetaSuperficie);
        SDL_Texture* RaquetaTextura2 = SDL_CreateTextureFromSurface(renderizador, imagenRaquetaSuperficie2);
        SDL_Texture* RedTextura = SDL_CreateTextureFromSurface(renderizador, imagenRedSuperficie);

        SDL_SetRenderDrawColor(renderizador, 0, 0, 0, 255);
        SDL_RenderClear(renderizador);

        if(RedTextura) {
            SDL_RenderCopy(renderizador, RedTextura, NULL, &destino_red);
            SDL_DestroyTexture(RedTextura);
        }

        if(RaquetaTextura) {
            SDL_RenderCopy(renderizador, RaquetaTextura, NULL, &destino_raqueta);
            SDL_DestroyTexture(RaquetaTextura);
        }

        if(RaquetaTextura2) {
            SDL_RenderCopy(renderizador, RaquetaTextura2, NULL, &destino_raqueta2);
            SDL_DestroyTexture(RaquetaTextura2);
        }

        if(CirculoTextura) {
            SDL_RenderCopy(renderizador, CirculoTextura, NULL, &destino_circulo);
            SDL_DestroyTexture(CirculoTextura);
        }

        SDL_RenderPresent(renderizador);

        if(posX >= 1480) {
            jugador1++;
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                                     "¡El jugador 1 gana un punto!",
                                     "¡El jugador 1 gana un punto!",
                                     NULL);
            limite++;
            posX = (ANCHURA_VENTANA - circulo_ancho) / 2;
            posY = (ALTURA_VENTANA - circulo_alto) / 2;
            dx = -VELOCIDAD_PELOTA;
            dy = (rand() % 2 == 0) ? VELOCIDAD_PELOTA : -VELOCIDAD_PELOTA;
        }

        if(posX <= -5) {
            jugador2++;
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                                     "¡El jugador 2 gana un punto!",
                                     "¡El jugador 2 gana un punto!",
                                     NULL);
            limite++;
            posX = (ANCHURA_VENTANA - circulo_ancho) / 2;
            posY = (ALTURA_VENTANA - circulo_alto) / 2;
            dx = VELOCIDAD_PELOTA;
            dy = (rand() % 2 == 0) ? VELOCIDAD_PELOTA : -VELOCIDAD_PELOTA;
        }

        int resta = jugador1 - jugador2;

        if((limite == LIMITE1 || limite == LIMITE2) && abs(resta) >= 2) {
            if(jugador1 > jugador2) {
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                                         "¡El jugador 1 ha ganado!",
                                         "¡El jugador 1 ha ganado!",
                                         NULL);
            } else {
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                                         "¡El jugador 2 ha ganado!",
                                         "¡El jugador 2 ha ganado!",
                                         NULL);
            }
            salir = true;
        }

        SDL_Delay(16);
    }

    Mix_FreeChunk(sonidoRebote);
    Mix_CloseAudio();
    SDL_DestroyWindow(ventana);
    SDL_Quit();
    return 0;
}
