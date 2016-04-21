#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SCRIPT		"http://127.0.0.1/cgi-bin/cartas"
#define BARALHO		"http://127.0.0.1/cards"

#define BOTAOJOGAR  "http://127.0.0.1/buttons/jogar.png"
#define BOTAOPASSAR "http://127.0.0.1/buttons/passar.png"
#define BOTAOLIMPAR "http://127.0.0.1/buttons/limpar.png"

#define NAIPES		"DCHS"
#define VALORES		"3456789TJQKA2"

typedef long long int MAO;


typedef struct estado 

{
	MAO mao [4];
	int cartas [4];
	MAO selecao;
	int selecionar;
	int jogar;
	int passar;
	int limpar;
	MAO ultimajogada;
	int ultjogador;

} ESTADO;

char* estado2str(ESTADO e){
  static char str[10240];
  sprintf(str, "%lld_%lld_%lld_%lld_%lld_%d_%d_%d_%d_%d_%d_%d_%lld_%d", e.mao[0], e.mao[1], e.mao[2], e.mao[3], e.selecao, e.cartas[0], e.cartas[1], e.cartas[2], e.cartas[3], e.jogar, e.passar, e.limpar, e.ultimajogada, e.ultjogador);
  return str;
}

ESTADO str2estado(char* str){
  ESTADO e;
  sscanf(str, "%lld_%lld_%lld_%lld_%lld_%d_%d_%d_%d_%d_%d_%d_%lld_%d", &e.mao[0], &e.mao[1], &e.mao[2], &e.mao[3], &e.selecao, &e.cartas[0], &e.cartas[1], &e.cartas[2], &e.cartas[3], &e.jogar, &e.passar, &e.limpar, &e.ultimajogada, &e.ultjogador);  

  return e;
}

/**
Estado inicial com todas as 52 cartas do baralho
Cada carta é representada por um bit que está
a 1 caso ela pertença à mão ou 0 caso contrário
*/
long long int ESTADO_INICIAL = 0xfffffffffffff;



/**
Devolve o índice da carta
@param naipe	O naipe da carta (inteiro entre 0 e 3)
@param valor	O valor da carta (inteiro entre 0 e 12)
@return		O índice correspondente à carta
*/
int indice(int naipe, int valor) {
	return naipe * 13 + valor;
}

/**
Adiciona uma carta ao estado
@param ESTADO	O estado atual
@param naipe	O naipe da carta (inteiro entre 0 e 3)
@param valor	O valor da carta (inteiro entre 0 e 12)
@return		O novo estado
*/
long long int add_carta(long long int ESTADO, int naipe, int valor) {
	int idx = indice(naipe, valor);
	return ESTADO | ((long long int) 1 << idx);
}

/**
Remove uma carta do estado
@param ESTADO	O estado atual
@param naipe	O naipe da carta (inteiro entre 0 e 3)
@param valor	O valor da carta (inteiro entre 0 e 12)
@return		O novo estado
*/
long long int rem_carta(long long int ESTADO, int naipe, int valor) {
	int idx = indice(naipe, valor);
	return ESTADO & ~((long long int) 1 << idx);
}

/**
Verifica se uma carta pertence ao estado
@param ESTADO	O estado atual
@param naipe	O naipe da carta (inteiro entre 0 e 3)
@param valor	O valor da carta (inteiro entre 0 e 12)
@return		1 se a carta existe e 0 caso contrário
*/
int carta_existe(long long int ESTADO, int naipe, int valor) {
	int idx = indice(naipe, valor);
	return (ESTADO >> idx) & 1;
}

/**
Imprime o html correspondente a uma carta
@param path	o URL correspondente à pasta que contém todas as cartas
@param x A coordenada x da carta
@param y A coordenada y da carta
@param ESTADO	O estado atual
@param naipe	O naipe da carta (inteiro entre 0 e 3)
@param valor	O valor da carta (inteiro entre 0 e 12)
*/
void imprime_carta(char *path, int x, int y, ESTADO e,int mao, int naipe, int valor) {
	char *suit = NAIPES;
	char *rank = VALORES;
	char script[10240];
	ESTADO novo = e;
	if (mao == 0) {
		if (carta_existe(novo.selecao, naipe, valor))
			novo.selecao = rem_carta(novo.selecao, naipe, valor);
		else
			novo.selecao = add_carta(novo.selecao, naipe, valor);

		novo.selecionar = 1;
		sprintf(script, "%s?%s", SCRIPT, estado2str(novo));
		printf("<a xlink:href = \"%s\"><image x = \"%d\" y = \"%d\" height = \"110\" width = \"80\" xlink:href = \"%s/%c%c.svg\" /></a>\n", script, x, y, path, rank[valor], suit[naipe]);
	}
	else {
		printf("<image x = \"%d\" y = \"%d\" height = \"110\" width = \"80\" xlink:href = \"%s/%c%c.svg\" />\n", x, y, path, rank[valor], suit[naipe]);
	}
}

void imprime_botao_jogar(char *path, int x, int y, ESTADO e) {
	char script[10240];
	ESTADO novo = e;
	novo.jogar = 1;
	sprintf(script,"%s?%s", SCRIPT,estado2str(novo));
	printf("<a xlink:href = \"%s\"><image x = \"%d\" y = \"%d\" height = \"110\" width = \"80\" xlink:href = \"%s\" /></a>\n", script, x, y, path);
}

void imprime_botao_passar(char *path,int x, int y, ESTADO e) {
	char script[10240];
	ESTADO novo = e;
	novo.passar = 1;
	sprintf(script,"%s?%s", SCRIPT,estado2str(novo));
	printf("<a xlink:href = \"%s\"><image x = \"%d\" y = \"%d\" height = \"110\" width = \"80\" xlink:href = \"%s\" /></a>\n", script, x, y, path);
}

void imprime_botao_limpar(char *path,int x, int y, ESTADO e) {
	char script[10240];
	ESTADO novo = e;
	novo.limpar = 1;
	sprintf(script,"%s?%s", SCRIPT,estado2str(novo));
	printf("<a xlink:href = \"%s\"><image x = \"%d\" y = \"%d\" height = \"110\" width = \"80\" xlink:href = \"%s\" /></a>\n", script, x, y, path);
}

ESTADO distribuir () {
	int n,v,r;
	ESTADO e = {{0},{0},0,0,0,0,0,0,0};
	for(n = 0; n < 4; n++) {
		for (v=0;v<13; v++) {
			do {
				r = random() % 4;
			} while(e.cartas[r]==13);
			e.mao[r] = add_carta(e.mao[r],n,v);
			e.cartas[r]++;
		}
	}
	return e;
}



/**
Esta função está a imprimir o estado em quatro colunas: uma para cada naipe
@param path	o URL correspondente à pasta que contém todas as cartas
@param ESTADO	O estado atual
*/
void imprime(char *path,ESTADO e) {
	int n, v, m;
	int i;
	int X[4] = {220,40,220,680};
	int Y[4] = {660,200,20,200};

	for(m = 0; m < 4;m++) {
		int x = X[m], y = Y[m];
		for(v = 0; v < 13; v++) {
			for(n = 0; n < 4; n++)
				if(carta_existe(e.mao[m], n, v)) {
					if(m % 2 == 0)
						x += 20;
					else 
						y += 20;
					if (m == 0 && carta_existe(e.selecao, n, v))
						imprime_carta(path, x, y-20, e, m, n, v);
					else
						imprime_carta(path, x, y, e, m, n, v);
				}
		}
	imprime_botao_jogar(BOTAOJOGAR, 600, 600, e);
	imprime_botao_passar(BOTAOPASSAR, 700, 600, e);
	imprime_botao_limpar(BOTAOLIMPAR, 650, 650, e);
	}
}

ESTADO limpar (ESTADO e) {
	int n,v;
	int x = 250, y = 450;
	e.limpar = 0;
	e.ultjogador = 0;
	for(n=0;n<4;n++)
		for(v=0;v<13;v++){
			e.selecao = rem_carta(e.selecao,n,v);
		}
	return e;
}

ESTADO passar (ESTADO e) {
	e.passar = 0;
	e.ultjogador = 0;
	e = limpar(e);
	return e;
}

/* TODO */
/* Verifica se a seleção é uma combinação válida de cartas */

int combinacao_valida(ESTADO e) {
	int n, v, vant = -1;
	int quant = 0;
	for(n = 0; n < 4; n++) {
		for(v = 0; v < 13; v++) {
			if(carta_existe(e.selecao, n, v)) {
				if (vant == -1) vant = v;
				if (vant != v) return 0;
				quant++;
			}
		}
	}
	if (quant > 0 && quant <= 3) return 1;
	else return 0;
}

/*
m1 > m2
*/

int combinacao_maior (MAO m1, MAO m2){
	return 1;
}


int posso_jogar(ESTADO e) {
	if(!combinacao_valida(e))
		return 0;
	if(e.ultjogador == 0)
		return 1;
	if(combinacao_maior(e.selecao, e.ultimajogada))
		return 1;
	return 0;
}

ESTADO jogar (ESTADO e) {
	int n,v;
	int x = 250,y = 450;
	e.jogar = 0;
	if (combinacao_valida(e)) {
		for(n = 0; n < 4; n++) {
			for(v = 0; v < 13; v++) {
				if(carta_existe(e.selecao, n, v)) {
					x += 20;
					e.mao[0] = rem_carta(e.mao[0],n,v);
					imprime_carta(BARALHO, x, y, e, 4, n, v);
				}
			}
		}
	}
	else return e;
	e.ultimajogada = e.selecao;
	e.ultjogador = 0;
	e.selecao = 0;
	return e;
}
/**
Esta função recebe a query que é passada à cgi-bin e trata-a.
Neste momento, a query contém o estado que é um inteiro que representa um conjunto de cartas.
Cada carta corresponde a um bit que está a 1 se essa carta está no conjunto e a 0 caso contrário.
Caso não seja passado nada à cgi-bin, ela assume que todas as cartas estão presentes.
@query A query que é passada à cgi-bin
 */
void parse(char *query) {
	ESTADO e;
	if(query != NULL && strlen(query) != 0) {
		e =str2estado(query);
		if (e.selecionar) 
			e.selecionar = 0;
		if (e.jogar)
			e = jogar(e);
		if (e.passar)
			e = passar(e);
		if (e.limpar)
			e = limpar(e);
	} else {
		e = distribuir();
	}
	imprime(BARALHO, e);
}

/**
Função principal do programa que imprime os cabeçalhos necessários e depois disso invoca
a função que vai imprimir o código html para desenhar as cartas
 */
int main() {
	ESTADO e;
	int i;
	e.jogar = 0;
	char str[10240];
	srandom(time(NULL));
	for (i=0;i<4;i++) {
		e.mao[i]=0;
		e.cartas[i]=0;
	}
/*
 * Cabeçalhos necessários numa CGI
 */ 
 	printf("Content-Type: text/html; charset=utf-8\n\n");
	printf("<header><title>Big Two</title></header>\n");
	printf("<body>\n");

	printf("<h1>Big Two</h1>\n");
	printf("<svg height = \"800\" width = \"800\">\n");
	printf("<rect x = \"0\" y = \"0\" height = \"800\" width = \"800\" style = \"fill:#007700\"/>\n");

/*
 * Ler os valores passados à cgi que estão na variável ambiente e passá-los ao programa
 */
	parse(getenv("QUERY_STRING"));
	printf("</svg>\n");
	printf("</body>\n");
	return 0;
}