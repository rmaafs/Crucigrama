#ifndef Botones_h
#define Botones_h
/*
 Si necesitas agregar una nueva tecla:
 - Agrega la tecla en este enum.
 
 - Ve a SDL.h, en la �ltima funci�n copea un else if y ed�ta:
 			 else if (keys[SDL_SCANCODE_*TuTecla*]){
				return *TuTecla*;
			}
			
 - Ve a Game.h y busca el switch (sdl.botonClickeado()){ dentro de la funci�n iniciar()
   y edita tu if:
   			case *TuTecla*:
				rojo.matar();
				break;
*/
enum Botones {NADA, W, S, A, D, Q, E, P, M, K, SPACE, DOWN, UP, ESC};

#endif
