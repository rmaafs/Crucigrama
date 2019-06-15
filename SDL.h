#ifndef SDL_h
#define SDL_h
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "Botones.h"
#include <iostream>

using namespace std;

class SDL {
	
	
	private:
		SDL_Window * window;
		SDL_Renderer * renderer;
		SDL_Event event;
		
	public:
		SDL(){//Inicia y abre una ventana para el juego.
			SDL_Init(SDL_INIT_VIDEO);
    		IMG_Init(IMG_INIT_PNG);
			window = SDL_CreateWindow("Crucigrama", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 640, 0);
			renderer = SDL_CreateRenderer(window, -1, 0);
		}
		
		//Limpia todas las imagenes impresas en la pantalla.
		void limpiar(){
			SDL_RenderClear(renderer);
		}
		
		//Dibuja una imagen en la pantalla.
		void dibujar(SDL_Texture *img){
			SDL_RenderCopy(renderer, img, NULL, NULL);
		}
		
		//Dibuja una imagen en la pantalla, editando sus coordenadsa y su imageSize.
		void dibujar(SDL_Texture *img, int x, int y, int sizeX, int sizeY){
			SDL_Rect dstrect;
			dstrect.x = x;
			dstrect.y = y;
			dstrect.w = sizeX;
			dstrect.h = sizeY;
			SDL_RenderCopy(renderer, img, NULL, &dstrect);
		}
		
		//Dibuja una imagen en la pantalla, editando sus coordenadsa y su imageSize y recortes.
		void dibujarTamCorte(SDL_Texture *img, int x, int y, int sizeX, int sizeY, int empezarX, int empezarY, int pixelX, int pixelY){
			SDL_Rect srcrect;
			SDL_Rect dstrect;
			
			srcrect.x = empezarX;
			srcrect.y = empezarY;
			srcrect.w = pixelX;
			srcrect.h = pixelY;
			
			dstrect.x = x;
			dstrect.y = y;
			dstrect.w = sizeX;
			dstrect.h = sizeY;
			
			SDL_RenderCopy(renderer, img, &srcrect, &dstrect);
		}
		
		//Imprime todo lo que se mando a dibujar.
		void render(){
			SDL_RenderPresent(renderer);
		}
		
		//Es similar a un wait
		void esperar(int i){
			SDL_Delay(i);
		}
		
		//Cambiar el color del fondo en formato RGB
		void setColorFondo(int r, int g, int b){
			SDL_SetRenderDrawColor(renderer, r, g, b, 0);
		}
		
		//Destruye alguna imagen para liberar su dirección de memoria.
		//Esta función no se utiliza para borrar una imagen de la pantalla.
		void destruir(SDL_Texture *img){
			SDL_DestroyTexture(img);
		}
		
		//Cerra ventana (Terminar proceso).
		void salir(){
			//SDL_FreeSurface(image);
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			
			IMG_Quit();
			SDL_Quit();
		}
		
		//Cargar una imagen mandando el directorio donde está guardada.
		SDL_Texture* loadImg( const string &str){
			SDL_Surface* surface = IMG_Load( str.c_str() );
		    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		    SDL_FreeSurface(surface);
		    return texture;
		}
		
		//Retorna verdadero si el usuario está intentando cerrar la ventana.
		bool clickeoSalir(){
			SDL_PollEvent(&event);
	        switch (event.type){
	            case SDL_QUIT:
	                return true;
	        }
	        return false;
		}
		
		//Retorna el botón que el usuario está clickeando en el teclado.
		Botones botonClickeado(){
			const Uint8 *keys = SDL_GetKeyboardState(NULL);
			if (keys[SDL_SCANCODE_A]){
				return A;
			} else if (keys[SDL_SCANCODE_D]){
				return D;
			} else if (keys[SDL_SCANCODE_W]){
				return W;
			} else if (keys[SDL_SCANCODE_S]){
				return S;
			} else if (keys[SDL_SCANCODE_Q]){
				return Q;
			} else if (keys[SDL_SCANCODE_E]){
				return E;
			} else if (keys[SDL_SCANCODE_P]){
				return P;
			} else if (keys[SDL_SCANCODE_M]){
				return M;
			} else if (keys[SDL_SCANCODE_K]){
				return K;
			} else if (keys[SDL_SCANCODE_SPACE]){
				return SPACE;
			} else if (keys[SDL_SCANCODE_DOWN]){
				return DOWN;
			} else if (keys[SDL_SCANCODE_UP]){
				return UP;
			} else if (keys[SDL_SCANCODE_ESCAPE]){
				return ESC;
			}
	        return NADA;
		}
};

#endif
