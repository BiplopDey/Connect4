#include <stdio.h>
#include <stdlib.h>

#include "connect4.h"
#include "minimax.h"


double Max_Value(Nodo *p,double alfa, double beta,int level){
	if(p->n_hijos==0) 
  		return(funcionHeur(p->tablero));
  			p->valor=-Inf;

  	double v1;
	for(int i=0;i<p->n_hijos;i++){

		p->hijos[i]=creaNodo(p,i,level+1);
    	v1=Min_Value(p->hijos[i],alfa,beta,level+1);

    	if(v1>p->valor) 
			p->valor=v1;

    	if(v1>=beta){
      		double v=p->valor;
      		free(p->hijos[i]->hijos);
      		free(p->hijos[i]);
      		return v;
    	}

    	if(v1>alfa)  
			alfa=v1;

    	if(level!=0){// no matar a los hijos de la raiz, porque se necesita luego
      		free(p->hijos[i]->hijos);
      		free(p->hijos[i]);
    	}
  	}

  	return p->valor;    

}

double Min_Value(Nodo *p,double alfa, double beta,int level){

	if(p->n_hijos==0) 
		return(funcionHeur(p->tablero));

  	p->valor=+Inf;
  	double v1;
	
  	for(int i=0;i<p->n_hijos;i++){
    	p->hijos[i]=creaNodo(p,i,level+1);
    	v1=Max_Value(p->hijos[i],alfa,beta,level+1);
    	
		if(v1< p->valor) 
			p->valor=v1;
    	
		if(v1<= alfa){
      		double v=p->valor;
      		free(p->hijos[i]->hijos);
      		free(p->hijos[i]);
      		return v;
      }

    if(v1< beta)  
		beta=v1;

    free(p->hijos[i]->hijos);
    free(p->hijos[i]);

  }

  return p->valor;

}

Nodo *creaNodo(Nodo *padre, int numHijo, int level){

  Nodo *p=malloc(sizeof(Nodo));
  capiarTablero(p->tablero,padre->tablero);
  aplicarTirada(p->tablero,numHijo,level);//numHijo es el numero del hijo, que va de 0 a ...

  if(level<K){
    p->n_hijos=numHijos(p->tablero);
    p->hijos=malloc(p->n_hijos*sizeof(Nodo*));
  }
  else{
    p->n_hijos=0;
    p->hijos= NULL;//malloc(0);
  }

  return p;

}

Nodo *creaRaiz(char table[N][N]){

  Nodo *p=malloc(sizeof(Nodo));
  capiarTablero(p->tablero,table);
  p->n_hijos=numHijos(p->tablero);
  p->hijos=malloc(p->n_hijos*sizeof(Nodo*));
  
  return p;

}

void mostraValor(Nodo *p,int level) {

    for(int i=0;i<level;i++)
        printf("\t");

   printf("%f\n",p->valor);

}

//inicion funcion heur
int compruebaLineaHeur(int a1,int a2, int a3, int a4,int jugador,int raya){
    
	switch (raya) {
    	case 2: // escribir mas cosas para penalizar 2 en raya y un vacio al lado
            return (a1==jugador && a2==jugador && a3==0) 
				+ (a2==jugador && a3==jugador && a4==0)
				+ (a1==0 && a2==jugador && a3==jugador) 
				+ (a2==0 && a3==jugador && a4==jugador);
        case 3:
            return (a1==jugador && a2==jugador && a3==jugador && a4==0) 
				+ (a1==0 && a2==jugador && a3==jugador && a4==jugador);
        case 4:
            return (a1==jugador && a2==jugador && a3==jugador && a4==jugador);
        }

    return 0;//pos si a caso

}


int resultadoTableHeur(char table[N][N], int jugador, int raya){// player 1 1, player 2 2, empate 3, nada 4
  int i, j;
  int p=0; 

  for(j=0;j<N;j++)
    for(i=0;i<=N-4;i++){
        p+=compruebaLineaHeur(// por columns
			table[i][j],
			table[i+1][j],
			table[i+2][j],
			table[i+3][j],
			jugador,
			raya);
        p+=compruebaLineaHeur(// por filas
			table[j][i],
			table[j][i+1],
			table[j][i+2],
			table[j][i+3],
			jugador,
			raya);
      }

  	for( i=0;i<=N-4;i++)// por diagonal inclinado abajo
    	for( j=0;j<=N-4;j++)
        	p+=compruebaLineaHeur(
				table[i][j],
				table[i+1][j+1], 
				table[i+2][j+2], 
				table[i+3][j+3],
				jugador,
				raya);

  	for(i=3;i<N;i++) // por diagonal inclinado arriba
   		for( j=0;j<=N-4;j++)
       		p+=compruebaLineaHeur(
				   table[i][j],
				   table[i-1][j+1], 
				   table[i-2][j+2], 
				   table[i-3][j+3],
				   jugador,
				   raya);

 	return p;

}

int funcionHeur(char table[N][N]){
  
	if(K==2){
    	if(resultadoTableHeur(table,2,4)) 
			return 100000;
    	else if(resultadoTableHeur(table,1,4)) 
			return -100000*resultadoTableHeur(table,1,4);
  	}
  
  	if(resultadoTableHeur(table,1,4)) //si el humano hace 4 en raya
  		return -100000*resultadoTableHeur(table,1,4);
  	else if(resultadoTableHeur(table,2,4)) // si el ordenador hace 4 en raya
  		return 100000;

  	int p=0;
    p+=resultadoTableHeur(table,2,3)*10;
    //penalizar si el humano tien 3 en raya
    p+=resultadoTableHeur(table,1,3)*(-10);
    p+=resultadoTableHeur(table,2,2)*2;
    //penalizar si el humano tien 2 en raya
    p+=resultadoTableHeur(table,1,2)*(-2);
 
 	return p;

}
//fin funcion heur

int tiradaRaiz(Nodo *p){// que column tirar despues del minimax
  
  	double m;
  	int j=0;
  	m=p->hijos[0]->valor;

  	for(int i=1;i<p->n_hijos;i++){
		if(p->hijos[i]->valor>m){
      		m=p->hijos[i]->valor;
      		j=i;
    	}
  	}
  	// el hijo numero j que column corresponde de p->tablero?
  	return numHijoAColumn(p->tablero, j);

}
