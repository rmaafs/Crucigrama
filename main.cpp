#include <iostream>
#include <list>
#include <chrono>
#include <ctime>
#include <windows.h>
#include <fstream>
#include <iostream>
#include <algorithm>
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

struct Cuadro {
	int pregunta;
	int subpregunta;//Si hay otra frase que ya ha desbloqueado esa palabra, no tapar la letra que ya se descubrió.
	int x, y;
};

struct Pregunta {
	int num;
	string respuesta;
};

struct Record {
	char nombre[20];
	int time;
};

void setColor(int);
void ocultarCursor();
void mostrarInicio(SDL);
void mostrarInstrucciones(SDL);
void mostrarRecords(SDL);
void iniciarJuego(SDL);
void llenarPreguntas(list<Pregunta>&);
void agregarPregunta(int, string, list<Pregunta> &);
void llenarRecords(list<Record>&);
void llenarCuadros(list<Cuadro> &);
void agregarCuadro(int, int, int, list<Cuadro> &);
void agregarCuadro(int, int, int, int, list<Cuadro> &);
void dibujarCuadros(SDL, Imagen*, list<Cuadro> &);
Pregunta preguntar();
void desactivarCuadros(int, list<Cuadro> &);
bool existePregunta(Pregunta, list<Pregunta>);
void eliminar(list<Pregunta>&, Pregunta);
void guardarRecord(list<Record> &, auto);
void registrarRecord(list<Record> &, Record&);
void imprimirRecords();
int pedirNumero();
bool isNumber(const string&);

int main(int argc, char *args[]) {
	ocultarCursor();//Ocultar el cursor que parpadea en consola.
	SDL sdl;
	mostrarInicio(sdl);
    return 0;
}

//Mostrar la pantalla de juego
void iniciarJuego(SDL sdl){
	list<Pregunta> preguntas;
	list<Record> records;
	list<Cuadro> cuadros;
	
	Imagen *fondo1 = sdl.loadImg("img/tablero.png");
	Imagen *cuadro = sdl.loadImg("img/cuadro.png");
	
	llenarPreguntas(preguntas);
	llenarRecords(records);
	llenarCuadros(cuadros);
	
	auto start = std::chrono::system_clock::now();//Empezar a contar el tiempo
    
	
	while (true){
    	if (sdl.clickeoSalir()){//Si el jugador ha precionado la X para cerrar la ventana...
    		break;//Terminar el ciclo para que no se imrpima.
		}
		
        sdl.limpiar();
        sdl.setColorFondo(0, 0, 0);
        
        sdl.dibujar(fondo1);
        dibujarCuadros(sdl, cuadro, cuadros);
        
        sdl.render();
        sdl.esperar(7);
        
        Pregunta p = preguntar();
        if (existePregunta(p, preguntas)){//Si la respuesta que contestó, existe...
        	eliminar(preguntas, p);//Eliminar la pregunta de la lista de preguntas.
        	desactivarCuadros(p.num, cuadros);//Eliminar los cuadros blancos que tapan la respuesta.
        	cout << "CORRECTO!" << endl;
        	if (preguntas.size() == 0) break;
		} else {
			setColor(ROJO);
			cout << endl << endl << "Respuesta incorrecta." << endl;
		}

		system("PAUSE");
    }
    
    if (preguntas.size() == 0){
    	//Como el juego ha terminado porque ha completado todas las preguntas...
    	guardarRecord(records, start);//Guardar su record.
	}
    
    sdl.destruir(fondo1);
}

void desactivarCuadros(int pregunta, list<Cuadro> &cuadros){
	
	list <Cuadro> :: iterator it = cuadros.begin();
	while (it != cuadros.end()){
		Cuadro c = *it;
	    if (c.pregunta == pregunta || c.subpregunta == pregunta){
	        it = cuadros.erase(it);
	    } else {
	        ++it;
	    }
	}
}

//Función que le pregunta el nombre al jugador, para guardarlo en el archivo de records.
void guardarRecord(list<Record> &records, auto start){
	auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
              
    Record r;
    system("cls");
    setColor(VERDE);
    cout << "Felicidades, completaste el Crucigrama en " << (int) elapsed_seconds.count() << " segundos." << endl << endl;
    setColor(AMARILLOF);
	cout << "Porfavor, ingresa tu nombre: ";
	setColor(AMARILLO);
	cin >> r.nombre;
	r.time = (int) elapsed_seconds.count();
	registrarRecord(records, r);
	system("cls");
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
	num = pedirNumero();
	setColor(VERDEF);
	cout << endl << endl << "Ingresa la respuesta de la pregunta " << num << endl;
	setColor(VERDE);
	cin >> respuesta;
	
	//Convertimos la respuesta a todas letras minúsculas.
	transform(respuesta.begin(), respuesta.end(), respuesta.begin(), ::tolower);
	
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
	
	bool ciclar = true;//true = Imprimir la pantalla de inicio.
	
	int opcion = 1;//Opción que tiene seleccionada actualmente.
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
        			timerBotones = 40;
        			if (++opcion > 3) opcion = 1;
				}
				break;
			case SPACE:
				//Si la opción que ha escogido es...
				if (opcion == 1){
					iniciarJuego(sdl);
				} else if (opcion == 2){
					mostrarInstrucciones(sdl);
				} else if (opcion == 3){
					mostrarRecords(sdl);
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

//Retornará verdadero si la pregunta existe en la lista de preguntas.
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
	Imagen *fondo1 = sdl.loadImg("img/instrucciones.png");
	
	bool ciclar = true;
	
	while (ciclar){
    	if (sdl.clickeoSalir()){
    		break;
		}
		
		switch (sdl.botonClickeado()){
			case ESC:
				ciclar = false;
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

//Mostrar la pantalla de records
void mostrarRecords(SDL sdl){
	Imagen *fondo1 = sdl.loadImg("img/records.png");
	
	bool ciclar = true;//Variable para imprimir la pantalla cuando esté activada.
	
	imprimirRecords();//Mostrar records por consola.
	
	while (ciclar){
    	if (sdl.clickeoSalir()){
    		break;
		}
		
		switch (sdl.botonClickeado()){
			case ESC:
				ciclar = false;
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

//Agregar el numero y respuesta, a la lista de preguntas.
void agregarPregunta(int num, string respuesta, list<Pregunta> &preguntas){
	Pregunta p;
	p.num = num;
	p.respuesta = respuesta;
	preguntas.push_back(p);
}

//Función que lee los records del archivo y los manda a un list de Records.
void llenarRecords(list<Record> &records){
	ifstream origen("records.txt", ios::binary);
	Record p;
	while (origen.read((char*) &p, sizeof(Record))){
        records.push_back(p);
    }
	origen.close();
}

//Función que registra un record al archivo de records.
void registrarRecord(list<Record> &records, Record &r){
	records.push_back(r);
	ofstream infile("records.txt", ios::binary);
	for(auto iter = records.begin(); iter!=records.end(); iter++){
	    Record rec = *iter;
	    infile.write((char*) &rec, sizeof(Record));
	}
	infile.close();
}

//Imprimir los records en consola.
void imprimirRecords(){
	list<Record> records;
	llenarRecords(records);
	setColor(VERDE);
	cout << "Nombre\t\tTiempo" << endl << endl;
	for(list <Record> :: iterator it = records.begin(); it != records.end(); ++it){
		Record i = *it;
		setColor(AMARILLOF);
		cout.width(16);
		cout << left << i.nombre;
		setColor(AMARILLO);
		cout << i.time << " segundos" << endl;
	}
}

//Función para eliminar una pregunta de la lista de preguntas.
void eliminar(list<Pregunta> &preguntas, Pregunta p){
	list <Pregunta> :: iterator it = preguntas.begin();
	while (it != preguntas.end()){
		Pregunta i = *it;
	    if (i.num == p.num && i.respuesta == p.respuesta){
	        it = preguntas.erase(it);
	    } else {
	        ++it;
	    }
	}
}

int pedirNumero(){
	string s;
	while (true){
		cin >> s;
		if (isNumber(s)){
			return stoi(s);
		} else {
			setColor(ROJO);
			cout << endl << "Por favor ingresa un numero valido." << endl;
			setColor(AMARILLO);
		}
	}
}

bool isNumber( const string& s ){
  bool hitDecimal=0;
  for(char c : s){
    if( c=='.' && !hitDecimal ) // 2 '.' in string mean invalid
      hitDecimal=1; // first hit here, we forgive and skip
    else if( !isdigit( c ) ) 
      return 0 ; // not ., not 
  }
  return 1 ;
}

//Cambiar el color del texto.
void setColor(int n){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), n); 
}

//Ocultar el cursor que parpadea en la consola.
void ocultarCursor(){
	HANDLE hCon;
	hCon = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cci;
	cci.dwSize = 50;
	cci.bVisible = FALSE;
	SetConsoleCursorInfo(hCon, &cci);
}

void dibujarCuadros(SDL sdl, Imagen *imagen, list<Cuadro> &cuadros){
	for(auto iter = cuadros.begin(); iter!=cuadros.end(); iter++){
	    Cuadro c = *iter;
	    sdl.dibujar(imagen, c.x, c.y, 12, 12);
	}
}

//Agregar un cuadro a la lista de cuadros, mandando x & y.
void agregarCuadro(int pregunta, int x, int y, list<Cuadro> &cuadros){
	agregarCuadro(pregunta, 0, x, y, cuadros);
}

//Agregar un cuadro a la lista de cuadros, mandando x & y y la subpregunta.
void agregarCuadro(int pregunta, int subpregunta, int x, int y, list<Cuadro> &cuadros){
	Cuadro c;
	c.pregunta = pregunta;
	c.subpregunta = subpregunta;
	c.x = x;
	c.y = y;
	cuadros.push_back(c);
}

//Tapar las letras del crucigrama con cuadros blancos.
void llenarCuadros(list<Cuadro> &cuadros){
	//Cuadros de la pregunta regunta 1
	agregarCuadro(1, 611, 10, cuadros);
	agregarCuadro(1, 611, 32, cuadros);
	agregarCuadro(1, 611, 53, cuadros);
	agregarCuadro(1, 611, 75, cuadros);
	agregarCuadro(1, 611, 95, cuadros);
	agregarCuadro(1, 19, 611, 117, cuadros);
	agregarCuadro(1, 611, 137, cuadros);
	//------------------------------
	agregarCuadro(2, 722, 11, cuadros);
	agregarCuadro(2, 3, 745, 11, cuadros);
	agregarCuadro(2, 768, 11, cuadros);
	agregarCuadro(2, 789, 11, cuadros);
	//------------------------------
	agregarCuadro(3, 745, 32, cuadros);
	agregarCuadro(3, 745, 53, cuadros);
	agregarCuadro(3, 13, 745, 74, cuadros);
	agregarCuadro(3, 745, 96, cuadros);
	agregarCuadro(3, 745, 116, cuadros);
	agregarCuadro(3, 745, 137, cuadros);
	
	agregarCuadro(4, 856, 11, cuadros);
	agregarCuadro(4, 878, 11, cuadros);
	agregarCuadro(4, 900, 11, cuadros);
	agregarCuadro(4, 922, 11, cuadros);
	agregarCuadro(4, 945, 11, cuadros);
	agregarCuadro(4, 967, 11, cuadros);
	agregarCuadro(4, 989, 10, cuadros);
	agregarCuadro(4, 5, 967, 11, cuadros);//---
	agregarCuadro(4, 1013, 11, cuadros);
	
	agregarCuadro(5, 968, 32, cuadros);
	agregarCuadro(5, 10, 968, 53, cuadros);//---
	agregarCuadro(5, 968, 74, cuadros);
	agregarCuadro(5, 17, 968, 96, cuadros);
	agregarCuadro(5, 968, 116, cuadros);
	agregarCuadro(5, 22, 968, 137, cuadros);
	agregarCuadro(5, 968, 159, cuadros);
	agregarCuadro(5, 28, 968, 180, cuadros);
	agregarCuadro(5, 968, 201, cuadros);
	
	agregarCuadro(6, 1147, 10, cuadros);
	agregarCuadro(6, 1169, 11, cuadros);
	agregarCuadro(6, 1191, 11, cuadros);
	agregarCuadro(6, 1213, 11, cuadros);
	
	agregarCuadro(7, 677, 32, cuadros);
	agregarCuadro(7, 677, 54, cuadros);
	agregarCuadro(7, 13, 677, 75, cuadros);
	agregarCuadro(7, 678, 96, cuadros);
	agregarCuadro(7, 19, 678, 116, cuadros);
	agregarCuadro(7, 678, 137, cuadros);
	agregarCuadro(7, 678, 158, cuadros);
	agregarCuadro(7, 27, 678, 179, cuadros);
	agregarCuadro(7, 678, 201, cuadros);
	agregarCuadro(7, 678, 221, cuadros);
	agregarCuadro(7, 35, 678, 243, cuadros);
	
	agregarCuadro(8, 812, 32, cuadros);
	agregarCuadro(8, 834, 32, cuadros);
	agregarCuadro(8, 856, 32, cuadros);
	
	agregarCuadro(9, 1101, 32, cuadros);
	agregarCuadro(9, 1123, 32, cuadros);
	agregarCuadro(9, 1125, 30, cuadros);
	agregarCuadro(9, 1147, 32, cuadros);
	
	agregarCuadro(10, 901, 54, cuadros);
	agregarCuadro(10, 922, 54, cuadros);
	agregarCuadro(10, 945, 54, cuadros);
	agregarCuadro(10, 991, 54, cuadros);
	agregarCuadro(10, 1012, 54, cuadros);
	agregarCuadro(10, 1034, 54, cuadros);
	agregarCuadro(10, 1057, 54, cuadros);
	agregarCuadro(10, 1080, 54, cuadros);
	
	agregarCuadro(11, 1191, 53, cuadros);
	agregarCuadro(11, 12, 1213, 53, cuadros);
	agregarCuadro(11, 1235, 53, cuadros);
	
	agregarCuadro(12, 1213, 74, cuadros);
	agregarCuadro(12, 18, 1213, 95, cuadros);
	agregarCuadro(12, 21, 1213, 116, cuadros);
	agregarCuadro(12, 1213, 137, cuadros);
	agregarCuadro(12, 26, 1213, 159, cuadros);
	agregarCuadro(12, 1213, 180, cuadros);
	agregarCuadro(12, 1213, 200, cuadros);
	
	agregarCuadro(13, 700, 75, cuadros);
	agregarCuadro(13, 723, 75, cuadros);
	agregarCuadro(13, 768, 75, cuadros);
	agregarCuadro(13, 14, 788, 75, cuadros);
	agregarCuadro(13, 812, 75, cuadros);
	agregarCuadro(13, 834, 75, cuadros);
	agregarCuadro(13, 857, 75, cuadros);
	agregarCuadro(13, 878, 75, cuadros);
	
	agregarCuadro(14, 789, 95, cuadros);
	agregarCuadro(14, 20, 789, 117, cuadros);
	agregarCuadro(14, 789, 137, cuadros);
	agregarCuadro(14, 24, 789, 159, cuadros);
	agregarCuadro(14, 27, 789, 179, cuadros);
	
	agregarCuadro(15, 1102, 75, cuadros);
	agregarCuadro(15, 1124, 75, cuadros);
	agregarCuadro(15, 1146, 75, cuadros);
	agregarCuadro(15, 1168, 75, cuadros);
	
	agregarCuadro(16, 924, 95, cuadros);
	agregarCuadro(16, 924, 117, cuadros);
	agregarCuadro(16, 22, 924, 137, cuadros);
	
	agregarCuadro(17, 990, 95, cuadros);
	agregarCuadro(17, 1012, 95, cuadros);
	agregarCuadro(17, 1036, 95, cuadros);
	agregarCuadro(17, 1058, 95, cuadros);
	
	agregarCuadro(18, 1236, 95, cuadros);
	agregarCuadro(18, 1259, 95, cuadros);
	
	agregarCuadro(19, 633, 117, cuadros);
	agregarCuadro(19, 655, 117, cuadros);
	agregarCuadro(19, 701, 117, cuadros);
	
	agregarCuadro(20, 813, 117, cuadros);
	agregarCuadro(20, 835, 117, cuadros);
	agregarCuadro(20, 857, 117, cuadros);
	
	agregarCuadro(21, 1147, 116, cuadros);
	agregarCuadro(21, 1169, 116, cuadros);
	agregarCuadro(21, 1191, 116, cuadros);
	
	agregarCuadro(22, 901, 138, cuadros);
	agregarCuadro(22, 945, 138, cuadros);
	agregarCuadro(22, 989, 138, cuadros);
	agregarCuadro(22, 1013, 138, cuadros);
	agregarCuadro(22, 1035, 138, cuadros);
	
	agregarCuadro(23, 633, 158, cuadros);
	agregarCuadro(23, 27, 633, 180, cuadros);
	agregarCuadro(23, 633, 200, cuadros);
	agregarCuadro(23, 633, 222, cuadros);
	agregarCuadro(23, 35, 633, 244, cuadros);
	agregarCuadro(23, 633, 264, cuadros);
	agregarCuadro(23, 40, 633, 286, cuadros);
	agregarCuadro(23, 633, 306, cuadros);
	agregarCuadro(23, 633, 328, cuadros);
	agregarCuadro(23, 633, 348, cuadros);
	
	agregarCuadro(24, 812, 159, cuadros);
	agregarCuadro(24, 834, 159, cuadros);
	agregarCuadro(24, 855, 159, cuadros);
	agregarCuadro(24, 879, 159, cuadros);
	
	agregarCuadro(25, 1080, 159, cuadros);
	agregarCuadro(25, 28, 1080, 180, cuadros);
	agregarCuadro(25, 1080, 201, cuadros);
	agregarCuadro(25, 1080, 222, cuadros);
	agregarCuadro(25, 36, 1080, 243, cuadros);
	
	agregarCuadro(26, 1145, 159, cuadros);
	agregarCuadro(26, 1169, 159, cuadros);
	agregarCuadro(26, 1193, 159, cuadros);
	agregarCuadro(26, 1237, 159, cuadros);
	
	agregarCuadro(27, 610, 179, cuadros);
	agregarCuadro(27, 658, 179, cuadros);
	agregarCuadro(27, 700, 179, cuadros);
	agregarCuadro(27, 721, 179, cuadros);
	agregarCuadro(27, 745, 179, cuadros);
	agregarCuadro(27, 769, 179, cuadros);
	
	agregarCuadro(28, 901, 179, cuadros);
	agregarCuadro(28, 922, 179, cuadros);
	agregarCuadro(28, 946, 179, cuadros);
	agregarCuadro(28, 991, 179, cuadros);
	agregarCuadro(28, 1012, 179, cuadros);
	agregarCuadro(28, 1036, 179, cuadros);
	agregarCuadro(28, 1057, 179, cuadros);
	agregarCuadro(28, 1102, 179, cuadros);
	agregarCuadro(28, 1123, 179, cuadros);
	agregarCuadro(28, 1147, 179, cuadros);
	
	agregarCuadro(29, 1258, 200, cuadros);
	agregarCuadro(29, 1258, 221, cuadros);
	agregarCuadro(29, 1258, 242, cuadros);
	agregarCuadro(29, 38, 1258, 263, cuadros);
	agregarCuadro(29, 1258, 284, cuadros);
	
	agregarCuadro(30, 744, 223, cuadros);
	agregarCuadro(30, 744, 243, cuadros);
	agregarCuadro(30, 37, 744, 265, cuadros);
	agregarCuadro(30, 744, 285, cuadros);
	agregarCuadro(30, 744, 307, cuadros);
	agregarCuadro(30, 46, 744, 327, cuadros);
	
	agregarCuadro(31, 789, 222, cuadros);
	agregarCuadro(31, 813, 222, cuadros);
	agregarCuadro(31, 833, 222, cuadros);
	agregarCuadro(31, 32, 857, 222, cuadros);
	agregarCuadro(31, 879, 222, cuadros);
	
	agregarCuadro(32, 856, 244, cuadros);
	agregarCuadro(32, 856, 264, cuadros);
	agregarCuadro(32, 41, 856, 284, cuadros);
	agregarCuadro(32, 856, 306, cuadros);
	agregarCuadro(32, 46, 856, 328, cuadros);
	agregarCuadro(32, 53, 856, 348, cuadros);
	agregarCuadro(32, 54, 856, 370, cuadros);
	agregarCuadro(32, 856, 390, cuadros);
	agregarCuadro(32, 60, 856, 412, cuadros);
	
	agregarCuadro(33, 1034, 223, cuadros);
	agregarCuadro(33, 36, 1034, 243, cuadros);
	agregarCuadro(33, 1034, 264, cuadros);
	agregarCuadro(33, 42, 1034, 286, cuadros);
	agregarCuadro(33, 1034, 306, cuadros);
	agregarCuadro(33, 48, 1034, 327, cuadros);
	agregarCuadro(33, 1034, 349, cuadros);
	agregarCuadro(33, 1034, 370, cuadros);
	agregarCuadro(33, 59, 1034, 391, cuadros);
	agregarCuadro(33, 60, 1034, 411, cuadros);
	
	agregarCuadro(34, 1170, 222, cuadros);
	agregarCuadro(34, 1170, 244, cuadros);
	agregarCuadro(34, 38, 1170, 264, cuadros);
	
	agregarCuadro(35, 611, 244, cuadros);
	agregarCuadro(35, 656, 244, cuadros);
	
	agregarCuadro(36, 946, 244, cuadros);
	agregarCuadro(36, 968, 244, cuadros);
	agregarCuadro(36, 990, 244, cuadros);
	agregarCuadro(36, 1012, 244, cuadros);
	agregarCuadro(36, 1056, 244, cuadros);
	agregarCuadro(36, 1100, 244, cuadros);
	agregarCuadro(36, 1124, 244, cuadros);
	
	agregarCuadro(37, 722, 265, cuadros);
	agregarCuadro(37, 767, 265, cuadros);
	agregarCuadro(37, 789, 265, cuadros);
	agregarCuadro(37, 812, 265, cuadros);
	
	agregarCuadro(38, 1147, 265, cuadros);
	agregarCuadro(38, 39, 1191, 265, cuadros);
	agregarCuadro(38, 1213, 265, cuadros);
	agregarCuadro(38, 1236, 265, cuadros);
	
	agregarCuadro(39, 1191, 285, cuadros);
	agregarCuadro(39, 1191, 307, cuadros);
	agregarCuadro(39, 1191, 327, cuadros);
	agregarCuadro(39, 1191, 348, cuadros);
	agregarCuadro(39, 56, 1191, 369, cuadros);
	
	agregarCuadro(40, 612, 286, cuadros);
	agregarCuadro(40, 656, 286, cuadros);
	
	agregarCuadro(41, 833, 285, cuadros);
	agregarCuadro(41, 879, 285, cuadros);
	agregarCuadro(41, 901, 285, cuadros);
	agregarCuadro(41, 923, 285, cuadros);
	agregarCuadro(41, 947, 285, cuadros);
	
	agregarCuadro(42, 1057, 285, cuadros);
	agregarCuadro(42, 1079, 285, cuadros);
	agregarCuadro(42, 1101, 285, cuadros);
	agregarCuadro(42, 1125, 285, cuadros);
	
	agregarCuadro(43, 701, 307, cuadros);
	agregarCuadro(43, 701, 328, cuadros);
	agregarCuadro(43, 701, 348, cuadros);
	agregarCuadro(43, 701, 369, cuadros);
	agregarCuadro(43, 57, 701, 391, cuadros);
	agregarCuadro(43, 701, 412, cuadros);
	agregarCuadro(43, 701, 433, cuadros);
	agregarCuadro(43, 63, 701, 455, cuadros);
	agregarCuadro(43, 701, 475, cuadros);
	
	agregarCuadro(44, 990, 307, cuadros);
	agregarCuadro(44, 48, 990, 327, cuadros);
	agregarCuadro(44, 990, 348, cuadros);
	
	agregarCuadro(45, 1235, 307, cuadros);
	agregarCuadro(45, 1235, 328, cuadros);
	agregarCuadro(45, 1235, 348, cuadros);
	agregarCuadro(45, 56, 1235, 369, cuadros);
	agregarCuadro(45, 1235, 390, cuadros);
	agregarCuadro(45, 1235, 411, cuadros);
	agregarCuadro(45, 62, 1235, 432, cuadros);
	agregarCuadro(45, 1235, 454, cuadros);
	agregarCuadro(45, 68, 1235, 475, cuadros);
	agregarCuadro(45, 1235, 496, cuadros);
	
	agregarCuadro(46, 47, 768, 328, cuadros);
	agregarCuadro(46, 789, 328, cuadros);
	agregarCuadro(46, 811, 328, cuadros);
	agregarCuadro(46, 834, 328, cuadros);
	
	agregarCuadro(47, 767, 348, cuadros);
	agregarCuadro(47, 767, 370, cuadros);
	agregarCuadro(47, 57, 767, 390, cuadros);
	agregarCuadro(47, 767, 412, cuadros);
	
	agregarCuadro(48, 946, 328, cuadros);
	agregarCuadro(48, 968, 328, cuadros);
	agregarCuadro(48, 1013, 328, cuadros);
	agregarCuadro(48, 1058, 328, cuadros);
	
	agregarCuadro(49, 1102, 328, cuadros);
	agregarCuadro(49, 1102, 348, cuadros);
	agregarCuadro(49, 1102, 369, cuadros);
	agregarCuadro(49, 59, 1102, 391, cuadros);
	agregarCuadro(49, 1102, 411, cuadros);
	agregarCuadro(49, 1102, 433, cuadros);
	agregarCuadro(49, 64, 1102, 454, cuadros);
	agregarCuadro(49, 1102, 475, cuadros);
	
	agregarCuadro(50, 1147, 328, cuadros);
	agregarCuadro(50, 1147, 349, cuadros);
	agregarCuadro(50, 56, 1147, 370, cuadros);
	agregarCuadro(50, 59, 1147, 391, cuadros);
	agregarCuadro(50, 1147, 411, cuadros);
	agregarCuadro(50, 1147, 432, cuadros);
	agregarCuadro(50, 1147, 454, cuadros);
	agregarCuadro(50, 1147, 475, cuadros);
	
	agregarCuadro(51, 611, 349, cuadros);
	agregarCuadro(51, 611, 369, cuadros);
	agregarCuadro(51, 611, 391, cuadros);
	agregarCuadro(51, 611, 411, cuadros);
	
	agregarCuadro(52, 51, 656, 349, cuadros);
	agregarCuadro(52, 656, 370, cuadros);
	agregarCuadro(52, 656, 390, cuadros);
	
	agregarCuadro(53, 878, 349, cuadros);
	agregarCuadro(53, 902, 349, cuadros);
	agregarCuadro(53, 923, 349, cuadros);
	
	agregarCuadro(54, 834, 370, cuadros);
	
	agregarCuadro(55, 968, 370, cuadros);
	agregarCuadro(55, 968, 390, cuadros);
	agregarCuadro(55, 60, 968, 412, cuadros);
	agregarCuadro(55, 968, 432, cuadros);
	agregarCuadro(55, 968, 454, cuadros);
	agregarCuadro(55, 968, 475, cuadros);
	agregarCuadro(55, 968, 496, cuadros);
	
	agregarCuadro(56, 1168, 369, cuadros);
	agregarCuadro(56, 1214, 369, cuadros);
	agregarCuadro(56, 1258, 369, cuadros);
	
	agregarCuadro(57, 722, 390, cuadros);
	agregarCuadro(57, 746, 390, cuadros);
	agregarCuadro(57, 789, 390, cuadros);
	
	agregarCuadro(58, 902, 390, cuadros);
	agregarCuadro(58, 60, 902, 412, cuadros);
	agregarCuadro(58, 902, 433, cuadros);
	agregarCuadro(58, 902, 454, cuadros);
	agregarCuadro(58, 65, 902, 475, cuadros);
	
	agregarCuadro(59, 1057, 391, cuadros);
	agregarCuadro(59, 1079, 391, cuadros);
	agregarCuadro(59, 1125, 391, cuadros);
	
	agregarCuadro(60, 833, 412, cuadros);
	agregarCuadro(60, 879, 412, cuadros);
	agregarCuadro(60, 924, 412, cuadros);
	agregarCuadro(60, 946, 412, cuadros);
	agregarCuadro(60, 991, 412, cuadros);
	agregarCuadro(60, 1012, 412, cuadros);
	
	agregarCuadro(61, 633, 433, cuadros);
	agregarCuadro(61, 63, 633, 455, cuadros);
	agregarCuadro(61, 633, 474, cuadros);
	agregarCuadro(61, 634, 496, cuadros);
	agregarCuadro(61, 70, 633, 518, cuadros);
	agregarCuadro(61, 633, 538, cuadros);
	agregarCuadro(61, 78, 633, 559, cuadros);
	agregarCuadro(61, 634, 580, cuadros);
	agregarCuadro(61, 82, 634, 601, cuadros);
	
	agregarCuadro(62, 1192, 432, cuadros);
	agregarCuadro(62, 1212, 432, cuadros);
	agregarCuadro(62, 1256, 432, cuadros);
	
	agregarCuadro(63, 612, 454, cuadros);
	agregarCuadro(63, 656, 454, cuadros);
	agregarCuadro(63, 677, 454, cuadros);
	agregarCuadro(63, 723, 454, cuadros);
	
	agregarCuadro(64, 1013, 454, cuadros);
	agregarCuadro(64, 1035, 454, cuadros);
	agregarCuadro(64, 1058, 454, cuadros);
	agregarCuadro(64, 1079, 454, cuadros);
	
	agregarCuadro(65, 745, 475, cuadros);
	agregarCuadro(65, 768, 475, cuadros);
	agregarCuadro(65, 66, 789, 475, cuadros);
	agregarCuadro(65, 812, 475, cuadros);
	agregarCuadro(65, 834, 475, cuadros);
	agregarCuadro(65, 857, 475, cuadros);
	agregarCuadro(65, 878, 475, cuadros);
	agregarCuadro(65, 67, 922, 475, cuadros);
	
	agregarCuadro(66, 790, 496, cuadros);
	agregarCuadro(66, 790, 517, cuadros);
	agregarCuadro(66, 75, 790, 538, cuadros);
	agregarCuadro(66, 790, 559, cuadros);
	agregarCuadro(66, 80, 790, 580, cuadros);
	
	agregarCuadro(67, 924, 496, cuadros);
	agregarCuadro(67, 924, 517, cuadros);
	agregarCuadro(67, 76, 924, 539, cuadros);
	agregarCuadro(67, 924, 560, cuadros);
	agregarCuadro(67, 81, 924, 581, cuadros);
	agregarCuadro(67, 924, 601, cuadros);
	agregarCuadro(67, 85, 924, 622, cuadros);
	
	agregarCuadro(68, 1192, 476, cuadros);
	agregarCuadro(68, 1213, 476, cuadros);
	agregarCuadro(68, 1257, 476, cuadros);
	
	agregarCuadro(69, 677, 497, cuadros);
	agregarCuadro(69, 70, 677, 517, cuadros);
	agregarCuadro(69, 677, 538, cuadros);
	agregarCuadro(69, 78, 677, 560, cuadros);
	agregarCuadro(69, 677, 580, cuadros);
	agregarCuadro(69, 82, 677, 602, cuadros);
	agregarCuadro(69, 84, 677, 623, cuadros);
	
	agregarCuadro(70, 611, 518, cuadros);
	agregarCuadro(70, 656, 518, cuadros);
	agregarCuadro(70, 700, 518, cuadros);
	
	agregarCuadro(71, 745, 518, cuadros);
	agregarCuadro(71, 75, 745, 539, cuadros);
	agregarCuadro(71, 745, 559, cuadros);
	agregarCuadro(71, 80, 745, 581, cuadros);
	
	agregarCuadro(72, 1012, 518, cuadros);
	agregarCuadro(72, 76, 1012, 538, cuadros);
	agregarCuadro(72, 1012, 560, cuadros);
	agregarCuadro(72, 81, 1012, 580, cuadros);
	agregarCuadro(72, 1012, 601, cuadros);
	
	agregarCuadro(73, 1192, 518, cuadros);
	agregarCuadro(73, 1192, 539, cuadros);
	agregarCuadro(73, 79, 1192, 560, cuadros);
	agregarCuadro(73, 1192, 581, cuadros);
	agregarCuadro(73, 1192, 602, cuadros);
	agregarCuadro(73, 86, 1192, 623, cuadros);
	
	agregarCuadro(74, 1258, 518, cuadros);
	agregarCuadro(74, 1258, 539, cuadros);
	agregarCuadro(74, 1258, 559, cuadros);
	agregarCuadro(74, 1258, 581, cuadros);
	agregarCuadro(74, 1258, 602, cuadros);
	agregarCuadro(74, 86, 1258, 622, cuadros);
	
	agregarCuadro(75, 723, 539, cuadros);
	agregarCuadro(75, 767, 539, cuadros);
	agregarCuadro(75, 813, 539, cuadros);
	agregarCuadro(75, 834, 539, cuadros);
	agregarCuadro(75, 856, 539, cuadros);
	
	agregarCuadro(76, 945, 538, cuadros);
	agregarCuadro(76, 966, 538, cuadros);
	agregarCuadro(76, 990, 538, cuadros);
	agregarCuadro(76, 1035, 538, cuadros);
	agregarCuadro(76, 1056, 538, cuadros);
	agregarCuadro(76, 1079, 538, cuadros);
	agregarCuadro(76, 77, 1102, 538, cuadros);
	
	agregarCuadro(77, 79, 1102, 560, cuadros);
	agregarCuadro(77, 1102, 581, cuadros);
	agregarCuadro(77, 83, 1102, 601, cuadros);
	
	agregarCuadro(78, 656, 560, cuadros);
	agregarCuadro(78, 700, 560, cuadros);
	
	agregarCuadro(79, 1123, 560, cuadros);
	agregarCuadro(79, 1145, 560, cuadros);
	agregarCuadro(79, 1169, 560, cuadros);
	
	agregarCuadro(80, 723, 581, cuadros);
	agregarCuadro(80, 766, 581, cuadros);
	
	agregarCuadro(81, 858, 580, cuadros);
	agregarCuadro(81, 880, 580, cuadros);
	agregarCuadro(81, 900, 580, cuadros);
	agregarCuadro(81, 945, 580, cuadros);
	agregarCuadro(81, 967, 580, cuadros);
	agregarCuadro(81, 990, 580, cuadros);
	agregarCuadro(81, 1036, 580, cuadros);
	agregarCuadro(81, 1057, 580, cuadros);
	
	agregarCuadro(82, 611, 602, cuadros);
	agregarCuadro(82, 656, 602, cuadros);
	
	agregarCuadro(83, 1078, 602, cuadros);
	agregarCuadro(83, 1123, 602, cuadros);
	agregarCuadro(83, 1147, 602, cuadros);
	
	agregarCuadro(84, 700, 623, cuadros);
	agregarCuadro(84, 722, 623, cuadros);
	agregarCuadro(84, 745, 623, cuadros);
	agregarCuadro(84, 767, 623, cuadros);
	agregarCuadro(84, 790, 623, cuadros);
	agregarCuadro(84, 812, 623, cuadros);
	
	agregarCuadro(85, 879, 623, cuadros);
	agregarCuadro(85, 901, 623, cuadros);
	agregarCuadro(85, 946, 623, cuadros);
	agregarCuadro(85, 968, 623, cuadros);
	
	agregarCuadro(86, 1169, 623, cuadros);
	agregarCuadro(86, 1214, 623, cuadros);
	agregarCuadro(86, 1235, 623, cuadros);
}

//Llenar la lista de preguntas con todo lo que tiene el crucigrama.
void llenarPreguntas(list<Pregunta> &preguntas){
	agregarPregunta(1, "agregar", preguntas);
	agregarPregunta(2, "hexa", preguntas);
	agregarPregunta(3, "hepsilon", preguntas);
	agregarPregunta(4, "abeliano", preguntas);
	agregarPregunta(5, "altimetria", preguntas);
	agregarPregunta(6, "cero", preguntas);
	agregarPregunta(7, "micrometria", preguntas);
	agregarPregunta(8, "par", preguntas);
	agregarPregunta(9, "año", preguntas);
	agregarPregunta(10, "centroide", preguntas);
	agregarPregunta(11, "ker", preguntas);
	agregarPregunta(12, "ecuacion", preguntas);
	agregarPregunta(13, "cubicacion", preguntas);
	agregarPregunta(14, "angulo", preguntas);
	agregarPregunta(15, "cien", preguntas);
	agregarPregunta(16, "exa", preguntas);
	agregarPregunta(17, "metro", preguntas);
	agregarPregunta(18, "uno", preguntas);
	agregarPregunta(19, "arbol", preguntas);
	agregarPregunta(20, "giro", preguntas);
	agregarPregunta(21, "alfa", preguntas);
	agregarPregunta(22, "mantisa", preguntas);
	agregarPregunta(23, "hectolitro", preguntas);
	agregarPregunta(24, "largo", preguntas);
	agregarPregunta(25, "marco", preguntas);
	agregarPregunta(26, "tesis", preguntas);
	agregarPregunta(27, "heptaedro", preguntas);
	agregarPregunta(28, "equidistante", preguntas);
	agregarPregunta(29, "cifra", preguntas);
	agregarPregunta(30, "arista", preguntas);
	agregarPregunta(31, "axial", preguntas);
	agregarPregunta(32, "aplicacion", preguntas);
	agregarPregunta(33, "matematica", preguntas);
	agregarPregunta(34, "eje", preguntas);
	agregarPregunta(35, "hora", preguntas);
	agregarPregunta(36, "pentagono", preguntas);
	agregarPregunta(37, "signo", preguntas);
	agregarPregunta(38, "vector", preguntas);
	agregarPregunta(39, "conica", preguntas);
	agregarPregunta(40, "mil", preguntas);
	agregarPregunta(41, "minuto", preguntas);
	agregarPregunta(42, "equis", preguntas);
	agregarPregunta(43, "perimetro", preguntas);
	agregarPregunta(44, "ley", preguntas);
	agregarPregunta(45, "rectangulo", preguntas);
	agregarPregunta(46, "altura", preguntas);
	agregarPregunta(47, "litro", preguntas);
	agregarPregunta(48, "operar", preguntas);
	agregarPregunta(49, "division", preguntas);
	agregarPregunta(50, "eneagono", preguntas);
	agregarPregunta(51, "cos", preguntas);
	agregarPregunta(52, "ssi", preguntas);
	agregarPregunta(53, "cara", preguntas);
	agregarPregunta(54, "ni", preguntas);
	agregarPregunta(55, "binomio", preguntas);
	agregarPregunta(56, "exacto", preguntas);
	agregarPregunta(57, "micra", preguntas);
	agregarPregunta(58, "censo", preguntas);
	agregarPregunta(59, "cubica", preguntas);
	agregarPregunta(60, "inferencia", preguntas);
	agregarPregunta(61, "tetraedro", preguntas);
	agregarPregunta(62, "giga", preguntas);
	agregarPregunta(63, "centro", preguntas);
	agregarPregunta(64, "duplo", preguntas);
	agregarPregunta(65, "reduccion", preguntas);
	agregarPregunta(66, "digito", preguntas);
	agregarPregunta(67, "notacion", preguntas);
	agregarPregunta(68, "kilo", preguntas);
	agregarPregunta(69, "adicion", preguntas);
	agregarPregunta(70, "yarda", preguntas);
	agregarPregunta(71, "seno", preguntas);
	agregarPregunta(72, "legua", preguntas);
	agregarPregunta(73, "tensor", preguntas);
	agregarPregunta(74, "decena", preguntas);
	agregarPregunta(75, "ceviana", preguntas);
	agregarPregunta(76, "alineados", preguntas);
	agregarPregunta(77, "suma", preguntas);
	agregarPregunta(78, "deca", preguntas);
	agregarPregunta(79, "union", preguntas);
	agregarPregunta(80, "cono", preguntas);
	agregarPregunta(81, "equiangulo", preguntas);
	agregarPregunta(82, "nodo", preguntas);
	agregarPregunta(83, "masa", preguntas);
	agregarPregunta(84, "numerar", preguntas);
	agregarPregunta(85, "menos", preguntas);
	agregarPregunta(86, "bruja", preguntas);
}
