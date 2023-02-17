#include <iostream>
#include <string>

using namespace std;

const string FLECHA = "↓";

template <typename clave, typename tipocod>
struct Nodo_sec{
    clave cl;
    tipocod v;
    Nodo_sec<clave, tipocod>* sig = NULL;
};

template <typename clave, typename tipocod>
struct Secuencia{
    Nodo_sec<clave, tipocod>* primero;
    Nodo_sec<clave,tipocod>* anterior; //puntero al anterior al actual
};

template <typename clave, typename tipocod> //Secuencia vacía
Secuencia<clave, tipocod> crear(){
    Secuencia<clave, tipocod> s;
    s.primero = new Nodo_sec<clave, tipocod>;
    s.anterior = s.primero;
    return s;
}

template <typename clave, typename tipocod>
void insertar(Secuencia<clave, tipocod> & s, clave clav, tipocod val){
    Nodo_sec<clave, tipocod>* new_node = new Nodo_sec<clave, tipocod>;
    new_node->cl = clav;
    new_node->v = val;
    new_node->sig = s.anterior->sig;
    s.anterior->sig = new_node;
    s.anterior = new_node;
}


template <typename clave, typename tipocod>
void eliminar(Secuencia<clave, tipocod> & s){
    if(s.anterior->sig != NULL){
        Nodo_sec<clave, tipocod>* aux = s.anterior->sig;
    	s.anterior->sig = aux->sig;
    	delete aux;
    }
    else{
    	throw runtime_error(" final de la secuencia");
    }
}

template <typename clave, typename tipocod>
clave claveactual(const Secuencia<clave, tipocod>& s){
    if(s.anterior->sig != NULL){
    	return s.anterior->sig->cl;
    }
    else{
    	throw runtime_error(" final de la secuencia");
    }
}

template <typename clave, typename tipocod>
tipocod valoractual(const Secuencia<clave, tipocod>& s){
    if(s.anterior->sig != NULL){
    	return s.anterior->sig->v;
    }
    else{
    	throw runtime_error(" final de la secuencia");
    }
}

template <typename clave, typename tipocod>
void avanzar(Secuencia<clave, tipocod>& s){
    if(s.anterior->sig != NULL){
    	s.anterior = s.anterior->sig;
    }
    else{
    	throw runtime_error(" final de la secuencia");
    }
}


template <typename clave, typename tipocod>
void reiniciar(Secuencia<clave, tipocod>& s){
    s.anterior = s.primero;
}

template <typename clave, typename tipocod>
bool fin(Secuencia<clave, tipocod> s){
    return s.anterior->sig == NULL;
}

template <typename clave, typename tipocod>
void liberar(Secuencia<clave, tipocod> s){
    reiniciar(s);
    while(!fin(s)){
        eliminar(s);
    }
    delete s.primero;
}


template<typename clave, typename tipocod>
ostream & operator <<(ostream & o, Secuencia<clave, tipocod> s){
    Nodo_sec<clave,tipocod>* aux = s.primero;
    while(aux->sig != NULL){
		if(aux == s.anterior){
			o << FLECHA;
		}
		o << aux->sig->cl;
		tipocod cod = aux->sig->v;
		o <<  ":";
		for (int i = 0; i< cod.size();i++){
			o << cod[i];
		}
		o << endl;
		aux = aux->sig;
    }
    o << endl;
    return o;
}

template <typename clave, typename tipocod>
bool es_vacia(Secuencia<clave, tipocod> s){
    return s.primero->sig == NULL;
}
