#include <stdio.h>
#include <stdlib.h>
#include <stack>
#include <vector>

// Matriz de char representnado o labirinto
std::vector<std::vector<char>> maze; // Voce também pode representar o labirinto como um vetor de vetores de char (vector<vector<char>>)

// Numero de linhas e colunas do labirinto
int num_rows;
int num_cols;

// Representação de uma posição
struct pos_t {
	int i;
	int j;
	pos_t(){};
	pos_t(int i , int j);
};

pos_t::pos_t(int i , int j){
	this->i = i;
	this->j = j;
};

// Estrutura de dados contendo as próximas
// posicões a serem exploradas no labirinto
std::stack<pos_t> valid_positions;

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

	//maze = (char**)malloc(num_rows * (sizeof(char*)));

	maze.resize(num_rows);

	// Aloca a matriz maze (malloc)
	for (int i = 0; i < num_rows; ++i){
		// Aloca cada linha da matriz
		//maze[i] = (char*)malloc(num_cols);

		maze[i].resize(num_cols);

	}

	for (int i = 0; i < num_rows; ++i) {
		for (int j = 0; j < num_cols; ++j) {
			// Le o valor da linha i+1,j do arquivo e salva na posição maze[i][j]
			// Se o valor for 'e' salvar o valor em initial_pos
			char maze_char = (char)fgetc(maze_file);

			// DEBUG
			//fprintf(stdout , "num_col : %d /// num_row : %d /// char : %c\n" , j , i , maze_char);

			if(maze_char == '\n'){
				--j;
				continue;
			}

			maze[i][j] = maze_char;
			//fprintf(stdout , "crashou");
			
			
			if(maze_char == 'e'){
				initial_pos.i = i;
				initial_pos.j = j;
			}
		}
	}

	return initial_pos;
}

// Função que imprime o labirinto
void print_maze() {
	for (int i = 0; i < num_rows; ++i) {
		for (int j = 0; j < num_cols; ++j) {
			printf("%c", maze[i][j]);
		}
		printf("\n");
	}
	//printf("\n\n\n\n\n");
}


// Função responsável pela navegação.
// Recebe como entrada a posição initial e retorna um booleano indicando se a saída foi encontrada
bool walk(pos_t pos) {

	bool found_exit = false;
	
	// Repita até que a saída seja encontrada ou não existam mais posições não exploradas
		// Marcar a posição atual com o símbolo '.'
		// Limpa a tela
		// Imprime o labirinto
		system("cls");
		maze[pos.i][pos.j] = '.';
		print_maze();
		
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

		/*if((maze[(pos.i) - 1][pos.j] == 's') || (maze[(pos.i) + 1][pos.j] == 's')
		|| (maze[pos.i][(pos.j) + 1] == 's') || (maze[pos.i][(pos.j) - 1] == 's')){
			
			return true;

		}*/

		if(pos.i > 0){
			switch(maze[(pos.i) - 1][pos.j]){
				case 'x' : {
					pos_t temp_pos((pos.i - 1) , pos.j);
					valid_positions.push(temp_pos);
					break;
				}

				case 's' : {
					return true;
				}

				default :
					break;
			}
		}

		if(pos.i < (num_rows - 1)){
			switch(maze[(pos.i) + 1][pos.j]){
				case 'x' : {
					pos_t temp_pos((pos.i + 1) , pos.j);
					valid_positions.push(temp_pos);
					break;
				}

				case 's' :
					return true;

				default :
					break;
			}
		}

		if(pos.j < (num_cols - 1)){
			switch(maze[pos.i][(pos.j) + 1]){
				case 'x' : {
					pos_t temp_pos(pos.i , (pos.j + 1));
					valid_positions.push(temp_pos);
					break;
				}

				case 's' :
					return true;

				default :
					break;
			}
		}

		if(pos.j > 0){
			switch(maze[pos.i][(pos.j) - 1]){
				case 'x' : {
					pos_t temp_pos(pos.i , (pos.j - 1));
					valid_positions.push(temp_pos);
					break;
				}

				case 's' : 
					return true;

				default :
					break;
			}
		}

		// Verifica se a pilha de posições nao esta vazia 
		// Caso não esteja, pegar o primeiro valor de  valid_positions, remove-lo e chamar a funçao walk com esse valor
		// Caso contrario, retornar falso
		if (!valid_positions.empty()) {
			pos_t next_position = valid_positions.top();
			valid_positions.pop();
			found_exit = walk(next_position);
		}
		
		return found_exit;
}

int main(int argc, char* argv[]) {
	// carregar o labirinto com o nome do arquivo recebido como argumento
	pos_t initial_pos = load_maze(argv[1]);
	// chamar a função de navegação
	bool exit_found = walk(initial_pos);
	
	// Tratar o retorno (imprimir mensagem)
	if(exit_found)
		fprintf(stdout , "Exit Found!!!\n");
	else
		fprintf(stdout , "Exit NOT Found...\n");
	
	return 0;
}
