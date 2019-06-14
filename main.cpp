#include <iostream>
#include <list>
#include "Botones.h"
#include "SDL.h"

using namespace std;
//Convertir el tipo de clase SDL_Texture para poder interpretarlo como "Imagen"
typedef SDL_Texture Imagen;

void mostrarInicio(SDL);
void mostrarInstrucciones(SDL);

int main(int argc, char *args[]) {
	SDL sdl;
	mostrarInicio(sdl);//s
    return 0;
}

	
//Mostrar la pantalla de inicio
void mostrarInicio(SDL sdl){
	Imagen *fondo1 = sdl.loadImg("img/fondo1.png");
	Imagen *fondo2 = sdl.loadImg("img/fondo2.png");
	Imagen *fondo3 = sdl.loadImg("img/fondo3.png");
	
	bool ciclar = true;
	
	int opcion = 1;
	int timerBotones = 0;//Timer para evitar que cuando presionen una tecla, se cambie rápido.
	
	bool mostrarJuego = true;
	
    while (ciclar){
    	if (sdl.clickeoSalir()){
    		break;
		}
		
		timerBotones--;
		
		switch (sdl.botonClickeado()){
        	case UP:
        		if (timerBotones <= 0){
        			timerBotones = 50;
        			if (--opcion < 1) opcion = 3;
				}
        	case DOWN:
        		if (timerBotones <= 0){
        			timerBotones = 50;
        			if (++opcion > 3) opcion = 1;
				}
				break;
			case SPACE:
				ciclar = false;
				if (opcion == 2){
					mostrarInstrucciones(sdl);
				}
				break;
		}
		
        sdl.limpiar();
        sdl.setColorFondo(0, 0, 0);
        
        if (opcion == 1){
        	sdl.dibujar(fondo1);
		} else if (opcion == 2){
        	sdl.dibujar(fondo2);
		} else if (opcion == 3){
        	sdl.dibujar(fondo3);
		}
        
        sdl.render();
        sdl.esperar(7);
    }
    
    sdl.destruir(fondo1);
    sdl.destruir(fondo2);
    sdl.destruir(fondo3);
}

//Mostrar la pantalla de instrucciones
void mostrarInstrucciones(SDL sdl){
	Imagen *fondo1 = sdl.loadImg("img/fondo1.png");
	
	while (true){
    	if (sdl.clickeoSalir()){
    		break;
		}
		
		switch (sdl.botonClickeado()){
			case SPACE:
				
				break;
		}
		
        sdl.limpiar();
        sdl.setColorFondo(0, 0, 0);
        
        sdl.dibujar(fondo1);
        
        sdl.render();
        sdl.esperar(7);
    }
    
    sdl.destruir(fondo1);
}
