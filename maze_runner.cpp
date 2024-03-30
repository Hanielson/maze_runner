/* 
	EXERCÍCIO COMPUTACIONAL 2 - AUTOMAÇÃO EM TEMPO REAL
	HANIEL NUNES SCHIRMER CARDOSO - 2020068960
*/

#include <stdio.h>
#include <stdlib.h>
#include <stack>
#include <vector>
#include <unistd.h>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

// Matriz de char representnado o labirinto
std::vector<std::vector<char>> maze; // Voce também pode representar o labirinto como um vetor de vetores de char (vector<vector<char>>)

// Numero de linhas e colunas do labirinto
int num_rows;
int num_cols;

// Representação de uma posição
struct pos_t {
	int i;
	int j;
	pos_t();
	pos_t(int i , int j);
	pos_t& operator=(const pos_t& pos){
		this->i = pos.i;
		this->j = pos.j;
		return *this;
	};
};

pos_t::pos_t(){
	this->i = -1;
	this->j = -1;
};

pos_t::pos_t(int i , int j){
	this->i = i;
	this->j = j;
};

// GLOBAL VARIABLE -- FOUND EXIT
bool found_exit;

// Função que le o labirinto de um arquivo texto, carrega em 
// memória e retorna a posição inicial
pos_t load_maze(const char* file_name) {
	pos_t initial_pos;
	// Abre o arquivo para leitura (fopen)
	FILE *maze_file = fopen(file_name , "r");

	if(maze_file == nullptr){
		fprintf(stdout , "File was not found...\n");
		exit(-1);
	}

	// Le o numero de linhas e colunas (fscanf) e salva em num_rows e num_cols
	fscanf(maze_file , "%d  %d\n" , &num_rows , &num_cols);

	maze.resize(num_rows);

	// Aloca a matriz maze (malloc)
	for (int i = 0; i < num_rows; ++i){

		maze[i].resize(num_cols);

	}

	for (int i = 0; i < num_rows; ++i) {
		for (int j = 0; j < num_cols; ++j) {
			// Le o valor da linha i+1,j do arquivo e salva na posição maze[i][j]
			// Se o valor for 'e' salvar o valor em initial_pos
			char maze_char = (char)fgetc(maze_file);

			if(maze_char == '\n'){
				--j;
				continue;
			}

			maze[i][j] = maze_char;
						
			if(maze_char == 'e'){
				initial_pos.i = i;
				initial_pos.j = j;
			}
		}
	}

	return initial_pos;
}

// Função que imprime o labirinto
// Esta função roda em uma thread continuamente, até que seja morta pela thread principal
void print_maze() {
	while(true){
		system("tput clear");
		for (int i = 0; i < num_rows; ++i) {
			for (int j = 0; j < num_cols; ++j) {
				printf("%c", maze[i][j]);
			}
			printf("\n");
		}
		std::this_thread::sleep_for(100ms);
	}
}


// Função responsável pela navegação.
// Recebe como entrada a posição initial e retorna um booleano indicando se a saída foi encontrada
void walk(pos_t pos) {

	if((found_exit) || (pos.i == -1) || (pos.j == -1)){
		return;
	}

	// EVERY POSITION IS INITIALIZED TO i = -1 ; j = -1
	// THIS REPRESENTS AN INVALID POSITION (OUT OF SCREEN)
	// HOWEVER, IF A VALID POSITION IS FOUND, THE i AND j VALUES ARE UPDATED
	pos_t temp_pos[4];
	
	// Repita até que a saída seja encontrada ou não existam mais posições não exploradas
		// Marcar a posição atual com o símbolo '.'
		// Limpa a tela
		// Imprime o labirinto
		maze[pos.i][pos.j] = '.';
		std::this_thread::sleep_for(100ms);
		
		/* Dado a posição atual, verifica quais sao as próximas posições válidas
			Checar se as posições abaixo são validas (i>0, i<num_rows, j>0, j <num_cols)
		 	e se são posições ainda não visitadas (ou seja, caracter 'x') e inserir
		 	cada uma delas no vetor valid_positions
		 		- pos.i, pos.j+1
		 		- pos.i, pos.j-1
		 		- pos.i+1, pos.j
		 		- pos.i-1, pos.j
		 	Caso alguma das posiçÕes validas seja igual a 's', retornar verdadeiro
		*/

		if(pos.i > 0){
			switch(maze[(pos.i) - 1][pos.j]){
				case 'x' : {
					temp_pos[0] = pos_t((pos.i - 1) , pos.j);
					break;
				}

				case 's' : {
					found_exit = true;
					return;
				}

				default :
					break;
			}
		}

		if(pos.i < (num_rows - 1)){
			switch(maze[(pos.i) + 1][pos.j]){
				case 'x' : {
					temp_pos[1] = pos_t((pos.i + 1) , pos.j);
					break;
				}

				case 's' :
					found_exit = true;
					return;

				default :
					break;
			}
		}

		if(pos.j < (num_cols - 1)){
			switch(maze[pos.i][(pos.j) + 1]){
				case 'x' : {
					temp_pos[2] = pos_t(pos.i , (pos.j + 1));
					break;
				}

				case 's' :
					found_exit = true;
					return;

				default :
					break;
			}
		}

		if(pos.j > 0){
			switch(maze[pos.i][(pos.j) - 1]){
				case 'x' : {
					temp_pos[3] = pos_t(pos.i , (pos.j - 1));
					break;
				}

				case 's' : 
					found_exit = true;
					return;

				default :
					break;
			}
		}

		// CREATES ONE THREAD FOR EACH POSSIBLE PATH
		std::thread path1(walk , temp_pos[0]);
		std::thread path2(walk , temp_pos[1]);
		std::thread path3(walk , temp_pos[2]);
		std::thread path4(walk , temp_pos[3]);

		// WAITS FOR EACH PATH THREAD TO TERMINATE
		path1.join();
		path2.join();
		path3.join();
		path4.join();

		return;
}

int main(int argc, char* argv[]) {
	// carregar o labirinto com o nome do arquivo recebido como argumento
	pos_t initial_pos = load_maze(argv[1]);
	
	found_exit = false;

	// PRINT MAZE UNTIL MAIN THREAD TERMINATES
	std::thread print(print_maze);
	
	walk(initial_pos);

	if(found_exit){
		fprintf(stdout , "Found exit!!!\n");
	}
	else{
		fprintf(stdout , "Exit not found...\n");
	}
	
	return 0;
}
