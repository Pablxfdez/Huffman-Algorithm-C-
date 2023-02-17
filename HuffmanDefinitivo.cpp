#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <limits.h>
#include <queue>

using namespace std;

#include "abb.cpp"

// 1.- CONSTRUCCIÓN DE LA TABLA DE FRECUENCIAS
//La tabla de frecuencias es un árbol donde en cada nodo se guarda una clave y el valor (frecuencia) de esa clave.

template <typename clave, typename valor>
using Tabla = Abb<clave, valor>;

template <typename clave, typename valor>
Tabla<clave, valor> tabla_vacia(){                     //Creamos la tabla vacía, que en realidad es un árbol vacío
    Tabla<clave,valor> t = abb_vacio<clave, valor>();
    return t;
};


// 2.- CONSTRUCCIÓN ÁRBOL DE HUFFMAN
//El árbol de Huffman es un árbol con un valor y nodos de dos tipos: nodos internos y hojas. En los nodos internos no hay información, y en las hojas se guardan claves.

template<typename clave, typename valor>
struct Nodo_h{
    valor v;
    virtual bool es_hoja(){
        return true;
    }
};

template<typename clave, typename valor>
struct Hoja: Nodo_h<clave, valor>{
    clave cl;
    bool es_hoja(){
        return true;
    }
};

template <typename clave, typename valor>
struct Nodo_interior: Nodo_h<clave,valor>{
    Nodo_h<clave,valor>* izqdo = NULL, *drcho = NULL;
    bool es_hoja(){
        return false;
    }
};

template <typename clave, typename valor>
using Arbol_h = Nodo_h<clave,valor>*;

//Relación de orden de árboles de Huffman. Un árbol de Huffman h1 tiene mayor prioridad que otro h2 si su valor es menor.
template <typename clave, typename valor> 
struct comp_Arbol{
    bool operator() (Arbol_h<clave,valor> h1, Arbol_h<clave,valor> h2){
        return (h1->v) > (h2->v);
    }
};


//Método que introduce en una cola con prioridad tantos árboles de Huffman con una única hoja con una clave como claves haya en la tabla de frecuencias dada. El valor de cada árbol viene dado también por la tabla de frecuencias. Tiene además un parámetro pasado por referencia que indica la suma total de los valores de la tabla de frecuencias. 
template <typename clave, typename valor>
void meteHojas(Tabla<clave, valor> t, priority_queue <Arbol_h<clave,valor>,vector<Arbol_h<clave,valor>>,comp_Arbol<clave,valor>> & q, valor & numcaracteres){
    if (!es_abb_vacio(t)){
        Hoja<clave, valor>* a = new Hoja<clave,valor>;
        a->cl = t->cl;
        a->v = t->v;
        numcaracteres += a->v;
        q.push(a);
        meteHojas (t->izqdo, q, numcaracteres);
        meteHojas (t->drcho, q, numcaracteres);
    }
}

//Función que crea un árbol de Huffman a partir de una tabla de frecuencias dada. Utiliza el método auxiliar meteHojas anterior para crear la cola de prioridad, y luego va creando el árbol a través del algoritmo que va juntando los árboles de Huffman de menor frecuencia. 
template <typename clave, typename valor>
Arbol_h<clave,valor> arbol_h_desde_tabla_frecs(Tabla<clave,valor> t){
    priority_queue <Arbol_h<clave,valor>,vector<Arbol_h<clave,valor>>,comp_Arbol<clave,valor>> q;
    valor numcaracteres = 0;
    meteHojas (t, q, numcaracteres);
    while (q.top()->v != numcaracteres){ 
        Arbol_h<clave,valor> primero = q.top();
        q.pop();
        Arbol_h<clave,valor> segundo = q.top();
        q.pop();
        Nodo_interior<clave,valor>* a = new Nodo_interior<clave,valor>;
        a->izqdo = primero;
        a->drcho = segundo;
        a->v = primero->v + segundo->v;
        q.push(a);
    }
    return q.top();
};

//Operador para mostrar por pantalla los árboles de Huffman.
template <typename clave, typename valor>
ostream & operator << (ostream & o, Arbol_h<clave, valor> a){
    if(a!=NULL){
        if(a->es_hoja()){
            clave c = ((Hoja<clave,valor> *) a)-> cl;
            o << "("<< c << ","<<a->v << ")";
        }
        else{ //es nodo interior
            Arbol_h<clave,valor> iz = ((Nodo_interior<clave,valor> *) a)->izqdo;
            Arbol_h<clave,valor> dr = ((Nodo_interior<clave,valor> *) a)->drcho;
            o  << "(" << a->v << " hijo izqdo: " << iz <<" hijo drcho: "<< dr << ")";
        }
    }
    return o;
}


// 3.- CODIFICACIÓN A PARTIR DEL ÁRBOL DE HUFFMANN
typedef vector<bool> cod_huffman; //Un código de Huffman es un vector de booleanos, como por ejemplo 010110101001, donde  0=false, 1=true.

//Método para insertar en una tabla de códigos
template <typename clave, typename tipocod> 
void nuevoinsertar(Tabla<clave, tipocod> & a,clave c, cod_huffman & v){
    if(es_abb_vacio(a)){
        a = new Nodo_arbol<clave,tipocod>;
        a->cl = c;
        a->v = v;
    }
    else if (c <= a->cl){
        nuevoinsertar(a->izqdo, c, v);
    }
    else{
        nuevoinsertar(a->drcho, c, v);
    }
}

//Método para insertar en una tabla de códigos de forma que el árbol quede equilibrado
template <typename clave, typename tipocod>
void nuevoinsertar_eq(Tabla<clave, tipocod> & a, clave c, cod_huffman & v){
	nuevoinsertar(a,c,v);
	equilibrar(a);
}

//Método que añade a la tabla de códigos t las entradas determinadas por el árbol de Huffman a. La idea es recursiva, vamos recorriendo el árbol de Huffman, si vamos por la izquierda el prefijo del código es un 0, y si vamos por la derecha es un 1. 
template <typename clave, typename valor, typename tipocod>
void tabla_codigos (Arbol_h<clave,valor> a, Tabla<clave,tipocod> & t, cod_huffman & codigo){ // para hacerlo funcionar, en el int main crea una tabla vacia, y un codigo de huffman vacio. por qué la referencia?
    if (a!=NULL){ 
        if (a->es_hoja()){                            //Caso base
            clave c = ((Hoja<clave,valor> *) a)-> cl;
            nuevoinsertar_eq (t, c, codigo);
        }
        else{                                         //Nodo interior
            codigo.push_back(false);
            Arbol_h<clave,valor> iz = ((Nodo_interior<clave,valor> *) a)->izqdo;
            tabla_codigos(iz, t,codigo);
            codigo.pop_back(); // que pasa aqui
            Arbol_h<clave,valor> dr = ((Nodo_interior<clave,valor> *) a)->drcho;
            codigo.push_back(true);
            tabla_codigos(dr, t,codigo);
            codigo.pop_back();
        }
    }
}


// 4.- MOSTRAR POR PANTALLA Y GUARDAR EN UN ARCHIVO LA CODIFICACIÓN DEL MENSAJE ORIGINAL
//Operador para mostrar un código de Huffman
ostream & operator << (ostream & o, cod_huffman cod){
    for (int i = 0; i< cod.size(); i++){
        o<<cod[i];
    }
    return o;
}

//Operador para combinar dos códigos de Huffman (concatenación)
cod_huffman operator + (cod_huffman v1, cod_huffman v2){
    cod_huffman result = vector<bool>();
    int long1 = v1.size();
    int long2 = v2.size();
    for (int i = 0; i< long1; i++){
        result.push_back(v1[i]);
    }
    for (int i = 0; i<long2; i++){
        result.push_back(v2[i]);
    }
    return result;
}

//Método para escribir un vector de booleanos (un código de Huffman) en un archivo de texto.
void vector_bool_to_file(vector<bool> v, string file_name){
    ofstream f = ofstream(file_name);
    long num_bits = v.size();
    unsigned char c;
    long inic;
    f << num_bits<<endl;
    for(int i = 0;i< num_bits/CHAR_BIT; i++){
        c = 0;
        inic = i*CHAR_BIT;
        for(int j = CHAR_BIT-1;j>=0;j--){ 
            c = c*2+v[inic+j];
        }
        f<<c;
    }
    c = 0;
    inic = (num_bits/CHAR_BIT)*CHAR_BIT;
    for(int j = num_bits%CHAR_BIT-1;j>=0;j--){
        c = c *2 + v[inic + j];
    }
    f << c;
    f.close();
}


// 5.- GUARDAR EN UN ARCHIVO EL ÁRBOL DE HUFFMAN
//Método para escribir un árbol de Huffman en un archivo.
template<typename clave, typename valor>
void arbol_to_file (Arbol_h<clave,valor> a, ofstream & f){
    if(a!=NULL){
        if(a->es_hoja()){
            clave c = ((Hoja<clave,valor>*) a)->cl;
            f<<c;
        }
        else{
            f<<'+';
            Arbol_h<clave,valor> iz = ((Nodo_interior<clave,valor> *) a)->izqdo;
            arbol_to_file(iz, f);
            Arbol_h<clave,valor> dr = ((Nodo_interior<clave,valor> *) a)->drcho;
            arbol_to_file(dr,f);            
        }
    }
}


// 6.- OBTENER EL ÁRBOL DE HUFFMAN A PARTIR DEL ARCHIVO
//Método auxiliar
template <typename clave, typename valor>
void f_to_a_aux(Nodo_h<clave, valor>*& a, ifstream& f){
	if(!f.eof()){
		char c;
		f.get(c);
		if(c=='+'){
			Nodo_interior<clave, valor>* n= new Nodo_interior<clave, valor>;
			f_to_a_aux(n->izqdo, f);
			f_to_a_aux(n->drcho, f);
			a=n;
		}
		else{
			Hoja<clave,valor>* n= new Hoja<clave, valor>;
			n->cl=c;
			a=n;
		}
	}
}

//Función que dado un archivo de texto, devuelve el árbol de Huffman que contiene.
template <typename clave, typename valor>
Arbol_h<clave,valor> file_to_arbol (string file_name){ 
    ifstream archivo;
    archivo.open(file_name);
    char c;
    Arbol_h<clave, valor> a;
    f_to_a_aux(a,archivo);
    return a;
}


// 7.- OBTENER LA CODIFICACIÓN A PARTIR DEL ARCHIVO
//Función que dado un archivo de texto, devuelve el vector de booleanos (código de Huffman) que contiene.
vector<bool> file_to_vector_bool(string file_name){
    long num_bits;
    vector<bool> result;
    ifstream f = ifstream(file_name);
    f >> num_bits;
    unsigned char c;
    string s;
    getline(f,s); 
    long pos = 0;
    for(long i =0; i<num_bits/CHAR_BIT; i++){
        c = f.get();
        for(long j = 0 ; j<CHAR_BIT;j++){
            result.push_back(c % 2);
            c = c/2;
            pos++;
        }
    }
    c = f.get();
    for(long j = num_bits%CHAR_BIT-1;j>=0;j--){
        result.push_back(c % 2);
        c = c/2;
    }
    f.close();
    return result; 
}


// 8.-  DECODIFICACIÓN DEL MENSAJE A PARTIR DE LA CODIFICACIÓN Y EL ÁRBOL DE HUFFMAN
//Método que decodifica un mensaje y lo muestra por pantalla
template <typename clave, typename valor>
void decodificacion (Arbol_h<clave,valor> a, cod_huffman codigo){
    Arbol_h<clave,valor> copia = a;
    for (int i = 0; i< codigo.size();i++){
        if(codigo[i]){   //True == 1 (derecha)
            a = ((Nodo_interior<clave,valor>*)a)->drcho;
        }
        else{
            a = ((Nodo_interior<clave,valor>*)a)->izqdo;
        }
        if(a->es_hoja()){
            clave c = ((Hoja<clave,valor>*)a)->cl;
            cout<< c;
            a = copia;  //resetear
        }
    }
}

// PROGRAMA PRINCIPAL
int main(){
    cout << "Dime el nombre del fichero: ";
    string nombrefichero;
    cin>>nombrefichero;
    ifstream f;
    f.open(nombrefichero);
    Tabla <char, int> tablafrecs = tabla_vacia <char, int>();
    char ch;
    while(f.get(ch)){
    	insertar_eq(tablafrecs,ch,1);  //Insertamos en una tabla de frecuencias cada carácter (clave) con valor asociado 1
    }
    f.close();
    cout<< "Tabla de frecuencias: "<<endl<<tablafrecs<<endl;
    Arbol_h<char, int> a = arbol_h_desde_tabla_frecs(tablafrecs);
    cout<<"Árbol de Huffman: "<<endl<< a<<endl;
    Tabla <char,cod_huffman> tablacodigos = tabla_vacia <char, cod_huffman> ();
    cod_huffman codigo = vector<bool>();
    tabla_codigos<char,int,cod_huffman> (a, tablacodigos, codigo);
    cout<<"Tabla de códigos: "<<endl<<in_orden (tablacodigos)<<endl;
    cod_huffman codificacion;
    f.open(nombrefichero);
    while(f.get(ch)){
        codificacion = codificacion + consultar(tablacodigos, ch);
    }
    f.close();
    cout<<"Mensaje codificado: "<<endl<<codificacion<<endl;
    vector_bool_to_file(codificacion,"mensaje.cod");
    ofstream g = ofstream ("arbol.txt");
    arbol_to_file(a, g);
    g.close();
    cod_huffman coddesdearchivo = file_to_vector_bool ("mensaje.cod");
    Arbol_h<char,int> arboldesdearchivo = file_to_arbol<char, int>("arbol.txt");
    cout<< "Decodificación del mensaje: "<<endl;
    decodificacion(arboldesdearchivo, coddesdearchivo);
    cout<<endl;
    return 0;
}
