#include <iostream>
#include <list>
#include <chrono>
#include <ctime>
#include <windows.h>
#include <fstream>
#include <iostream>
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
Pregunta preguntar();
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
	Imagen *fondo1 = sdl.loadImg("img/tablero.png");
	
	llenarPreguntas(preguntas);
	llenarRecords(records);
	
	auto start = std::chrono::system_clock::now();//Empezar a contar el tiempo
	
	while (true){
    	if (sdl.clickeoSalir()){//Si el jugador ha precionado la X para cerrar la ventana...
    		break;//Terminar el ciclo para que no se imrpima.
		}
		
        sdl.limpiar();
        sdl.setColorFondo(0, 0, 0);
        
        sdl.dibujar(fondo1);
        
        sdl.render();
        sdl.esperar(7);
        
        Pregunta p = preguntar();
        if (existePregunta(p, preguntas)){
        	eliminar(preguntas, p);
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
