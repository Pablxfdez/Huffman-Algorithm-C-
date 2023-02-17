#include <iostream>
#include <string>

using namespace std;

#include "secuencias.cpp"

template <typename clave, typename valor>
struct Nodo_arbol{
    clave cl;
    valor v;
    Nodo_arbol<clave, valor> *izqdo = NULL, *drcho = NULL;
};

template <typename clave, typename valor>
using Abb =  Nodo_arbol<clave, valor>*;

template <typename clave, typename valor>
Abb<clave, valor> abb_vacio(){
    return NULL;
}

template <typename clave, typename valor>
bool es_abb_vacio(Abb<clave, valor> a){
    return a == NULL;
}

int maximo(int a, int b){
	if (a>b){
		return a;
	}
	else{
		return b;
	}
}

template<typename clave, typename valor>
int altura(Abb<clave, valor> a){
	if (es_abb_vacio(a)){
		return 0;
	}
	else{
		return(1+maximo(altura(a->izqdo),altura(a->drcho)));
	}
}

template<typename clave, typename valor>
bool es_equilibrado(Abb<clave, valor> a){
	if(es_abb_vacio(a)){
		return true;
	}
	else{
		return (es_equilibrado(a->izqdo) && es_equilibrado(a->drcho) && (abs(altura(a->izqdo) - altura(a->drcho)) <= 1));
	}
}

template <typename clave, typename valor>
void insertar(Abb<clave, valor> & a,clave c, valor val){
    if(es_abb_vacio(a)){
        a = new Nodo_arbol<clave,valor>;
        a->cl = c;
        a->v = val;
    }
    else if (c == a->cl){
        a->v = a->v + val;
    }
    else if (c <= a->cl){
        insertar(a->izqdo, c, val);
    }
    else{
        insertar(a->drcho, c, val);
    }
}

template<typename clave, typename valor>
void rotarR(Abb<clave, valor> & a){//rotación a la derecha (hijo izquierdo del izquierdo)
	Nodo_arbol<clave, valor>* aux = a->izqdo->drcho;
	a->izqdo->drcho = a;
	a = a->izqdo;
	a->drcho->izqdo = aux;
}

template<typename clave, typename valor>
void rotarL(Abb<clave, valor> & a){//rotación a la izquierda (hijo derecho del derecho)
	Nodo_arbol<clave, valor>* aux = a->drcho->izqdo;
	a->drcho->izqdo = a;
	a = a->drcho;
	a->izqdo->drcho = aux;
}

template<typename clave, typename valor>
void rotarLR(Abb<clave, valor> & a){//hijo izquierdo del derecho
	rotarR(a->drcho);
	rotarL(a);
}

template<typename clave, typename valor>
void rotarRL(Abb<clave, valor> & a){//hijo derecho del izquierdo
	rotarL(a->izqdo);
	rotarR(a);
}


template<typename clave, typename valor>
void equilibrar(Abb<clave, valor> & a){
	if (!es_equilibrado(a)){
		if(!es_equilibrado(a->izqdo)){
			equilibrar(a->izqdo);
		}
		else if(!es_equilibrado(a->drcho)){
			equilibrar(a->drcho);
		}
		else if(altura(a->izqdo) - altura(a->drcho) == 2){
			if(altura(a->izqdo->izqdo) - altura(a->izqdo->drcho)==1){
				rotarR(a);
			}
			else{
				rotarRL(a);
			}
		}
		else if(altura(a->izqdo) - altura(a->drcho) == -2){
			if(altura(a->drcho->izqdo) - altura(a->drcho->drcho)==1){
				rotarLR(a);
			}
			else{
				rotarL(a);
			}
		}
	}
}

template<typename clave, typename valor>
void insertar_eq(Abb<clave, valor> & a, clave c, valor val){
	insertar(a,c,val);
	equilibrar(a);
}

template <typename clave, typename tipocod>
tipocod consultar(Abb<clave, tipocod> & a,clave c){
    tipocod cod;
    if (!es_abb_vacio(a)){
        if (a->cl == c){
            cod = a-> v;
        }
        else if (a->cl > c){
            cod = consultar (a->izqdo, c);
        }
        else{
            cod = consultar(a->drcho, c);
        }
    }
    return cod;
}



template <typename clave, typename valor>
void in_orden_aux(const Abb<clave, valor> & a, Secuencia<clave, valor> & s){  //INORDEN
    if(!es_abb_vacio(a)){
        in_orden_aux(a->izqdo, s);
        while(s.anterior->sig != NULL){
        	avanzar(s);
        }
        insertar(s,a->cl,a->v);
        while(s.anterior->sig != NULL){
            avanzar(s);
        }
        in_orden_aux(a->drcho, s);
    }
}


template <typename clave, typename valor>
Secuencia<clave, valor> in_orden(const Abb<clave, valor> & a){
    Secuencia<clave, valor> result = crear<clave, valor>();
    in_orden_aux(a,result);
    return result;
}

template <typename clave, typename valor>
Secuencia<clave, valor> ordenar_abb(Secuencia<clave, valor> s){
    Abb<clave, valor> a = NULL;
    reiniciar(s);
    while(!fin(s)){
        insertar(a,claveactual(s),valoractual(s));
        avanzar(s);
    }
    Secuencia<clave, valor> result = in_orden(a);
    liberar(a);
    return  result;
}


template <typename clave, typename valor>
void liberar(Abb<clave, valor> & a){
    if(a!=NULL){
        liberar(a->izqdo);
        liberar(a->drcho);
        delete a;
    }
}

template <typename clave, typename valor>
ostream & operator << (ostream & o, Abb<clave, valor> a){
    if(es_abb_vacio(a)){
        o << "" ;
    }
    else{
        o  << a->cl << ":" << a-> v<<endl << a->izqdo << a->drcho;
    }
    return o;
}
