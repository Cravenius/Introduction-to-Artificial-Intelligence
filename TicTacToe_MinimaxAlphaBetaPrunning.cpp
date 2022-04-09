#include <iostream>
using namespace std;

#define AI 1 
#define PLAYER 2 
#define SIZE 5
#define MAXTREEDEPTH 25
#define AIMOVE 'O' 
#define PLAYERMOVE 'X' 

void PrintBoard(char board[SIZE][SIZE]) {
	string a(4 * SIZE, '-');
	for (int i = 0; i < SIZE; ++i) {
		for (int j = 0; j < SIZE; ++j)
			if (j == SIZE - 1)
				cout << ' ' << board[i][j] << ' ';
			else
				cout << ' ' << board[i][j] << " |";
		if (i != SIZE - 1)
			cout << endl << a << endl;
		else
			cout << endl;
	}
	cout << endl;
}

void WhoIsTheWinner(int turn) {
	if (turn == AI) cout << "AI win!" << endl;
	else cout << "Player win!" << endl;
}

bool RowWinCondition(char board[SIZE][SIZE]) {
	for (int i = 0; i < SIZE; ++i) {
		bool win = true;

		for (int j = 0; j < SIZE - 1; ++j) {
			if (board[j][i] == board[j + 1][i] && board[j][i] != ' ' && board[j + 1][i] != ' ') {
				continue;
			}
			else {
				win = false;
				break;
			}
		}

		if (win) {
			return true;
		}
	}

	return false;
}

bool ColumnWinCondition(char board[SIZE][SIZE]) {
	for (int i = 0; i < SIZE; ++i) {
		bool win = true;

		for (int j = 0; j < SIZE - 1; ++j) {
			if (board[i][j] == board[i][j + 1] && board[i][j] != ' ' && board[i][j + 1] != ' ') {
				continue;
			}
			else {
				win = false;
				break;
			}
		}

		if (win) {
			return true;
		}
	}

	return false;
}

bool DiagonalWinCondition(char board[SIZE][SIZE]) {
	bool win = true;
	for (int i = 0; i < SIZE - 1; ++i)
		if (board[i][i] == board[i + 1][i + 1] && board[i][i] != ' ' && board[i + 1][i + 1] != ' ') {
			continue;
		}
		else {
			win = false;
			break;
		}

	if (win)
		return true;

	win = true;
	for (int i = 0, j = SIZE - 1; i < SIZE - 1; ++i, --j)
		if (board[j][i] == board[j - 1][i + 1] && board[j][i] != ' ' && board[j - 1][i + 1] != ' ') {
			continue;
		}
		else {
			win = false;
			break;
		}

	if (win)
		return true;

	//if (board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != ' ') 
		//return true; 

	//if (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != ' ') 
		//return true; 

	return false;
}

bool IsOver(char board[SIZE][SIZE]) {
	return(RowWinCondition(board) || ColumnWinCondition(board) || DiagonalWinCondition(board));
}

int Minimax(char board[SIZE][SIZE], int depth, bool isAI, int& alpha, int& beta) {
	int score = 0;
	int bestScore = 0;
	if (IsOver(board) == true) {
		if (isAI == true) {
			cout << -(MAXTREEDEPTH - depth) << endl;
			return -(MAXTREEDEPTH - depth);
		}
		if (isAI == false) {
			cout << +(MAXTREEDEPTH - depth) << endl;
			return +(MAXTREEDEPTH - depth);
		}
	}
	else {
		if (depth < MAXTREEDEPTH) {
			if (isAI == true) {
				bestScore = -999;
				for (int i = 0; i < SIZE; ++i) {
					for (int j = 0; j < SIZE; ++j) {
						if (board[i][j] == ' ') {
							board[i][j] = AIMOVE;
							score = Minimax(board, depth + 1, false, alpha, beta);
							board[i][j] = ' ';
							if (score > bestScore) {
								bestScore = score;
							}
							alpha = max(alpha, bestScore);
							if (beta <= alpha)
								return bestScore;
						}
					}
				}
				return bestScore;
			}
			else {
				bestScore = 999;
				for (int i = 0; i < SIZE; ++i) {
					for (int j = 0; j < SIZE; ++j) {
						if (board[i][j] == ' ') {
							board[i][j] = PLAYERMOVE;
							score = Minimax(board, depth + 1, true, alpha, beta);
							board[i][j] = ' ';
							if (score < bestScore) {
								bestScore = score;
							}
							beta = min(beta, bestScore);
							if (beta <= alpha)
								return bestScore;
						}
					}
				}
				return bestScore;
			}
		}
		else {
			return 0;
		}
	}
}

int SearchBestMove(char board[SIZE][SIZE], int moveIndex, int& alpha, int& beta) {
	int x = -1, y = -1;
	int score = 0, bestScore = -999;
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; ++j) {
			if (board[i][j] == ' ') {
				board[i][j] = AIMOVE;
				score = Minimax(board, moveIndex + 1, false, alpha, beta);
				board[i][j] = ' ';
				if (score > bestScore) {
					bestScore = score;
					x = i;
					y = j;
				}

			}
		}
	}

	cout << "BestScore: " << bestScore << endl;
	cout << "Score: " << score << endl << endl;
	return x * SIZE + y;
}

void TicTacToe(int turn) {
	char board[SIZE][SIZE];
	int moveIndex = 0, x = 0, y = 0;
	int alpha = INT_MIN, beta = INT_MAX;

	memset(board, ' ', sizeof(board));

	// Instruction
	cout << "\nChoose a cell numbered from 1 to " << (SIZE * SIZE) << endl;
	cout << "Example of 3 x 3 board:\n" << endl;
	cout << " 1 | 2 | 3 \n";
	cout << "-----------\n";
	cout << " 4 | 5 | 6 \n";
	cout << "-----------\n";
	cout << " 7 | 8 | 9 \n\n";

	system("pause");

	// Game
	while (IsOver(board) == false && moveIndex != SIZE * SIZE) {
		int n;
		if (turn == AI) {
			n = SearchBestMove(board, moveIndex, alpha, beta);
			x = n / SIZE;
			y = n % SIZE;
			board[x][y] = AIMOVE;
			cout << "AI put a \'" << AIMOVE << "\' in " << n + 1 << endl << endl;
			PrintBoard(board);
			moveIndex ++;
			turn = PLAYER;
		}

		else if (turn == PLAYER) {
			cout << "You can insert in the following positions : ";
			for (int i = 0; i < SIZE; ++i)
				for (int j = 0; j < SIZE; ++j)
					if (board[i][j] == ' ')
						cout << (i * SIZE + j) + 1 << ' ';

			cout << "\n\nEnter the position = "; cin >> n;
			n--;
			x = n / SIZE;
			y = n % SIZE;
			if (board[x][y] == ' ' && n < (SIZE * SIZE) && n >= 0) {
				board[x][y] = PLAYERMOVE;
				cout << "\nPlayer put a \'" << PLAYERMOVE << "\' in " << n + 1 << endl << endl;
				PrintBoard(board);
				moveIndex ++;
				turn = AI;
			}
			else if (board[x][y] != ' ' && n < (SIZE * SIZE) && n >= 0) {
				cout << "\nOccupied, not available!\n" << endl;
			}
			else if (n < 0 || n >(MAXTREEDEPTH - 1)) {
				cout << "\nPosition is not valid!\n" << endl;
			}
		}
	}

	if (IsOver(board) == false && moveIndex == SIZE * SIZE) {
		cout << "It's a draw" << endl;
	}
	else {
		if (turn == AI)
			turn = PLAYER;
		else if (turn == PLAYER)
			turn = AI;

		WhoIsTheWinner(turn);
	}

	system("pause");
}

int main() {
	srand((unsigned)time(NULL));
	bool windowOpen = true;
	do {
		system("cls");
		cout << "\n======================\n   Tic-tac-toe Game\n======================\n" << endl;
		cout << "\nMain Menu:" << endl;
		cout << "1. Play (Player start first)" << endl;
		cout << "2. Play (AI start first)" << endl;
		cout << "3. Exit" << endl;
		cout << "Choice: "; int choice; cin >> choice;

		if (choice == 1)
			TicTacToe(PLAYER);
		else if (choice == 2)
			TicTacToe(AI);
		else
			windowOpen = false;
	} while (windowOpen);
	return 0;
}