#include <iostream>
#include <list>
#include <windows.h>
#include "Botones.h"
#include "SDL.h"
#define NEGRO 0
#define AZULFF 1
#define VERDEF 2
#define AQUAF 3
#define ROJOF 4
#define MORADO 5
#define AMARILLOF 6
#define GRIS 7
#define GRISF 8
#define AZULF 9
#define VERDE 10
#define AQUA 11
#define ROJO 12
#define ROSA 13
#define AMARILLO 14
#define BLANCO 15

using namespace std;
//Convertir el tipo de clase SDL_Texture para poder interpretarlo como "Imagen"
typedef SDL_Texture Imagen;

struct Pregunta {
	int num;
	string respuesta;
};

void gotoxy(int, int);
void gotoxy(int, int, string);
void setColor(int);
void ocultarCursor();
void mostrarInicio(SDL);
void mostrarInstrucciones(SDL);
void iniciarJuego(SDL);
void llenarPreguntas(list<Pregunta>&);
Pregunta preguntar();
bool existePregunta(Pregunta, list<Pregunta>);
void eliminar(list<Pregunta>&, Pregunta);

int main(int argc, char *args[]) {
	SDL sdl;
	//mostrarInicio(sdl);//s
	iniciarJuego(sdl);
    return 0;
}

//Mostrar la pantalla de juego
void iniciarJuego(SDL sdl){
	list<Pregunta> preguntas;
	Imagen *fondo1 = sdl.loadImg("img/crucigrama.png");
	
	llenarPreguntas(preguntas);
	
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
        
        Pregunta p = preguntar();
        if (existePregunta(p, preguntas)){
        	eliminar(preguntas, p);
		} else {
			
		}
		
		cout << endl << "TOTAL: " << preguntas.size() << endl;
		system("PAUSE");
    }
    
    sdl.destruir(fondo1);
}

/*
	Preguntar al usuario por consola el número que quiere responder.
	Se retornará un struct con el número y la respuesta que tecleó.
*/
Pregunta preguntar(){
	system("cls");
	fflush(stdin);
	int num;
	string respuesta;
	setColor(AMARILLOF);
	cout << "Por favor, ingresa el numero que quieres llenar: ";
	setColor(AMARILLO);
	cin >> num;
	setColor(VERDEF);
	cout << endl << endl << "Ingresa la respuesta de la pregunta " << num << endl;
	setColor(VERDE);
	cin >> respuesta;
	cout << endl << endl;
	Pregunta p;
	p.num = num;
	p.respuesta = respuesta;
	return p;
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
				if (opcion == 1){
					iniciarJuego(sdl);
				} else if (opcion == 2){
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

bool existePregunta(Pregunta p, list<Pregunta> preguntas){
	for(list <Pregunta> :: iterator it = preguntas.begin(); it != preguntas.end(); ++it){
		Pregunta i = *it;
		if (i.num == p.num && i.respuesta == p.respuesta){
			return true;
		}
	}
	return false;
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

//Llenar la lista de preguntas con todo lo que tiene el crucigrama.
void llenarPreguntas(list<Pregunta> &preguntas){
	Pregunta p;
	p.num = 1;
	p.respuesta = "rodrigo";
	preguntas.push_back(p);
}

//Función para eliminar una pregunta de la lista de preguntas.
void eliminar(list<Pregunta> &preguntas, Pregunta p){
	list <Pregunta> :: iterator it = preguntas.begin();
	while (it != preguntas.end()){
		Pregunta i = *it;
	    if (i.num == p.num && i.respuesta == p.respuesta){
	        it = preguntas.erase(it);// erase and go to next
	        cout << endl << "Borrando..." << endl;
	    } else {
	        ++it;
	    }
	}
}

//En la consola, Poner el puntero en las coordenadas...
void gotoxy(int x,int y){
    HANDLE hcon;  
    hcon = GetStdHandle(STD_OUTPUT_HANDLE);  
    COORD dwPos;  
    dwPos.X = x;  
    dwPos.Y= y;  
    SetConsoleCursorPosition(hcon,dwPos);  
}

//En la consola, Poner el puntero en las coordenadas e imprimir texto.
void gotoxy(int x,int y, string i){
    gotoxy(x, y);
    cout << i;
}

//Cambiar el color del texto.
void setColor(int n){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), n); 
}

//Ocultar el cursor.
void ocultarCursor(){
	HANDLE hCon;
	hCon = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cci;
	cci.dwSize = 50;
	cci.bVisible = FALSE;
	SetConsoleCursorInfo(hCon, &cci);
}
