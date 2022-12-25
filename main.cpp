#include <iostream>
#include <windows.h>
#include <string>
#include <ctime>
#include <fstream>

using namespace std;
const int WHITE = 1, BLACK = 0;// size = 8;


void SetColor(int tcl, int bcl)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (tcl + (bcl * 16)));
}

void getRowColbyLeftClick(int& rpos, int& cpos)
{
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	DWORD Events;
	INPUT_RECORD InputRecord;
	SetConsoleMode(hInput, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);
	do
	{
		ReadConsoleInput(hInput, &InputRecord, 1, &Events);
		if (InputRecord.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
		{
			cpos = InputRecord.Event.MouseEvent.dwMousePosition.X;
			rpos = InputRecord.Event.MouseEvent.dwMousePosition.Y;
			break;
		}
	} while (true);
}

void cursorPosition(int rpos, int cpos)
{
	COORD scrn;
	HANDLE hOuput = GetStdHandle(STD_OUTPUT_HANDLE);
	scrn.X = cpos;
	scrn.Y = rpos;
	SetConsoleCursorPosition(hOuput, scrn);
}


void Load(string fName, char B[][8])
{
	ifstream reader(fName);

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			reader >> B[i][j];
		}
	}
}

void Save(string fName, char B[][8])
{
	ofstream writer(fName);

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			writer << B[i][j];
		}
		writer << endl;
	}
}

void initialize(string N[], bool& turn)
{
	cout << "Player1 name: ";
	cin >> N[1];

	cout << "Player2 name: ";
	cin >> N[0];

	cout << "\n\nThe Game is SAVED on every move automatically as I have yet to make a Button for it.\nAlso, messages like--'Invalid Input' or 'Invalid Selection' are printed at wrong positions. Everything will be fixed when I include Graphics.";
	system("pause");

	//RANDOM TURN:-
	srand(time(0));
	turn = rand() % 2;
}

void displayBoard(char b[][8])
{
	cout << endl;
	for (char i = 'A'; i <= 'H'; i++)
	{
		cout << '\t' << i;
	}
	cout << "\n\n";

	for (int i = 0; i < 8; i++)
	{
		cout << "  " << i + 1;
		for (int j = 0; j < 8; j++)
		{
			cout << '\t' << b[i][j];
		}
		cout << "\n\n";
	}
}

void turnMsg(string n)
{
	cursorPosition(18, 23);
	cout << "\n\n" << n << "'s TURN.";
}


void peiceSelect(int& row, int& col)
{
	getRowColbyLeftClick(row, col);

	row = row / 2 - 1;
	col = col / 8 - 1;
}

bool myPeice(char B[][8], int row, int col, int turn)
{
	return (((B[row][col] >= 'A' && B[row][col] <= 'Z') && turn == WHITE)
		|| (turn == BLACK && (B[row][col] >= 'a' && B[row][col] <= 'z')));
}

bool validSelect(char B[][8], int row, int col, int turn)
{
	return ((row >= 0 && row <= 7 && col >= 0 && col <= 7) && myPeice(B, row, col, turn));
}
bool validDest(char B[][8], int row, int col, int turn)
{
	return ((row >= 0 && row <= 7 && col >= 0 && col <= 7) && !myPeice(B, row, col, turn));
}

void boardMovement(char b[][8], int sr, int sc, int dr, int dc, int turn)
{
	b[dr][dc] = b[sr][sc];
	b[sr][sc] = '-';
}

void turnUpdate(bool& turn)
{
	turn = !turn;
}


bool horzMovement(int rowA, int rowB)
{
	return rowA == rowB;
}
bool horzPathClear(char B[][8], int colA, int colB, int row)	//-->"row" can be any of "sourceRow" OR "destRow".
{
	if (colB > colA)	// Rightward movement.
	{
		for (int c = colA + 1; c < colB; c++)
		{
			if (B[row][c] != '-')
				return 0;
		}
		return 1;
	}
	else		// Leftward movement.
	{
		for (int c = colA - 1; c > colB; c--)
		{
			if (B[row][c] != '-')
				return 0;
		}
		return 1;
	}
}

bool vertMovement(int colA, int colB)
{
	return colA == colB;
}
bool vertPathClear(char B[][8], int rowA, int rowB, int col)	//-->"col" can be any of "sourceCol" OR "destCol".
{
	if (rowB > rowA)	// Downward movement.
	{
		for (int r = rowA + 1; r < rowB; r++)
		{
			if (B[r][col] != '-')
				return 0;
		}
		return 1;
	}
	else		// Upward movement.
	{
		for (int r = rowA - 1; r > rowB; r--)
		{
			if (B[r][col] != '-')
				return 0;
		}
		return 1;
	}
}

bool diagonalMovement(int rowA, int colA, int rowB, int colB)
{
	return abs(rowA - rowB) == abs(colA - colB);
}
bool diagonalPathClear(char c)
{
	return (c == '-');
}
bool diagonalPathClear(char B[][8], int rowA, int colA, int rowB, int colB)
{
	int reps = abs(rowB - rowA); //~OR reps = abs(colA - colB).
	bool result = 1;

	for (int i = 1; i < reps; i++)
	{
		if (rowB > rowA && colB > colA)
		{
			result = diagonalPathClear(B[rowA + i][colA + i]);
		}
		else if (rowB < rowA && colB > colA)
		{
			result = diagonalPathClear(B[rowA - i][colA + i]);
		}
		else if (rowB > rowA && colB < colA)
		{
			result = diagonalPathClear(B[rowA + i][colA - i]);
		}
		else
		{
			result = diagonalPathClear(B[rowA - i][colA - i]);
		}

		if (!result)
			break;
	}
	return result;
}


bool legalRook(char B[][8], int rowA, int colA, int rowB, int colB)
{
	return ((horzMovement(rowA, rowB) && horzPathClear(B, colA, colB, rowA)) || (vertMovement(colA, colB) && vertPathClear(B, rowA, rowB, colA)));
}

bool legalBishop(char B[][8], int rowA, int colA, int rowB, int colB)
{
	return (diagonalMovement(rowA, colA, rowB, colB) && diagonalPathClear(B, rowA, colA, rowB, colB));
}

bool legalQueen(char B[][8], int rowA, int colA, int rowB, int colB)
{
	return (legalRook(B, rowA, colA, rowB, colB) || legalBishop(B, rowA, colA, rowB, colB));
}

bool legalKing(int rowA, int colA, int rowB, int colB)
{
	int deltaRow = abs(rowB - rowA),
		deltaCol = abs(colB - colA);

	return ((deltaRow == 0 || deltaRow == 1) && (deltaCol == 0 || deltaCol == 1));
}

bool legalKnight(int rowA, int colA, int rowB, int colB)
{
	int deltaRow = abs(rowB - rowA),
		deltaCol = abs(colB - colA);

	return ((deltaRow == 2 && deltaCol == 1) || (deltaRow == 1 && deltaCol == 2));
}

bool legalBlackPawn(char B[][8], int rowA, int colA, int rowB, int colB)
{
	int deltaRow = rowB - rowA,
		deltaCol = colB - colA;

	return ((vertMovement(colA, colB) && B[rowB][colB] == '-' && deltaRow == 1)
		|| (diagonalMovement(rowA, colA, rowB, colB) && !diagonalPathClear(B, rowA, colA, rowB + 1, colB) && deltaRow == 1)
		|| (rowA == 1 && vertPathClear(B, rowA, rowB, colA) && deltaRow == 2 && vertMovement(colA, colB)));
}

bool legalWhitePawn(char B[][8], int rowA, int colA, int rowB, int colB)
{
	int deltaRow = rowB - rowA,
		deltaCol = colA - colB;

	return ((vertMovement(colA, colB) && B[rowB][colB] == '-' && deltaRow == -1)
		|| (diagonalMovement(rowA, colA, rowB, colB) && !diagonalPathClear(B, rowA, colA, rowB - 1, colB) && deltaRow == -1)
		|| (rowA == 6 && vertPathClear(B, rowA, rowB, colA) && deltaRow == -2 && vertMovement(colA, colB)));
}

bool legalMove(char B[][8], int rowA, int colA, int rowB, int colB)
{
	switch (B[rowA][colA])
	{
	case 'r':
	case 'R':
		return legalRook(B, rowA, colA, rowB, colB);

	case 'b':
	case 'B':
		return legalBishop(B, rowA, colA, rowB, colB);

	case 'h':
	case 'H':
		return legalKnight(rowA, colA, rowB, colB);

	case 'q':
	case 'Q':
		return legalQueen(B, rowA, colA, rowB, colB);

	case 'k':
	case 'K':
		return legalKing(rowA, colA, rowB, colB);

	case 'o':
		return legalBlackPawn(B, rowA, colA, rowB, colB);

	case 'P':
		return legalWhitePawn(B, rowA, colA, rowB, colB);
	}
}


void copy(char A[][8], char B[][8])
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			A[i][j] = B[i][j];
		}
	}
}

void findKing(char B[][8], int& row, int& col, int turn)
{
	char king = turn ? 'K' : 'k';

	for (int r = 0; r < 8; r++)
	{
		for (int c = 0; c < 8; c++)
		{
			if (B[r][c] == king)
			{
				row = r;
				col = c;
				return;
			}
		}
	}
}

bool check(char B[][8], int rowA, int colA, int rowB, int colB, bool turn)
{
	turnUpdate(turn);

	int kingRow = 9, kingCol = 9;
	findKing(B, kingRow, kingCol, turn);

	turnUpdate(turn);
	for (int r = 0; r < 8; r++)
	{
		for (int c = 0; c < 8; c++)
		{
			if (myPeice(B, r, c, turn) && legalMove(B, r, c, kingRow, kingCol))
				return 1;
		}
	}
	return 0;
}

bool selfCheck(char B[][8], int rowA, int colA, int rowB, int colB, bool turn)
{
	char tempB[8][8] = {};

	copy(tempB, B);

	boardMovement(tempB, rowA, colA, rowB, colB, turn);

	turnUpdate(turn);

	return check(tempB, rowA, colA, rowB, colB, turn);
}

bool IcanMove(char B[][8], char H[][8], int turn)
{
	for (int sr = 0; sr < 8; sr++)
	{
		for (int sc = 0; sc < 8; sc++)
		{
			if (myPeice(B, sr, sc, turn))
			{
				for (int dr = 0; dr < 8; dr++)
				{
					for (int dc = 0; dc < 8; dc++)
					{
						if (legalMove(B, sr, sc, dr, dc) && validDest(B, dr, dc, turn) && !selfCheck(B, sr, sc, dr, dc, turn))
							return 1;
					}
				}
			}
		}
	}
	return 0;
}

bool CheckMate(char B[][8], char H[][8], int rowA, int colA, int rowB, int colB, bool turn)
{
	return (!IcanMove(B, H, !turn) && check(B, rowA, colA, rowB, colB, turn));
}

bool StaleMate(char B[][8], char H[][8], int rowA, int colA, int rowB, int colB, bool turn)
{
	return (!IcanMove(B, H, !turn) && !check(B, rowA, colA, rowB, colB, turn));
}

void highlight(char B[][8], char H[][8], int row, int col, bool turn)
{
	for (int r = 0; r < 8; r++)
	{
		for (int c = 0; c < 8; c++)
		{
			if (!myPeice(B, r, c, turn) && legalMove(B, row, col, r, c) && !selfCheck(B, row, col, r, c, turn))
			{
				H[r][c] = 'x';
				cursorPosition((r * 2) + 3, (c + 1) * 8);
				cout << 'x';
			}
			else
			{
				H[r][c] = '-';
			}
		}
	}
}

void unHighlight(char B[][8], char H[][8], int row, int col, bool turn)
{
	for (int r = 0; r < 8; r++)
	{
		for (int c = 0; c < 8; c++)
		{
			if (H[r][c] == 'x')
			{
				H[r][c] = B[r][c];
				cursorPosition((r * 2) + 3, (c + 1) * 8);
				cout << B[r][c];
			}
		}
	}
}



int main()
{
	char board[8][8], hLight[8][8];
	string names[2];

	int sourceRow, sourceCol, destRow, destCol;
	bool repeat, turn;

	ofstream writer("Replay.txt");

	Load("Text.txt", board);

	initialize(names, turn);

	system("cls");

	displayBoard(board);

	turnMsg(names[turn]);

	do
	{
		/*if (UndoPressed())
		{
			Load("Previous.txt", board);
		}*/

		do
		{
			do
			{
				peiceSelect(sourceRow, sourceCol);

				repeat = !validSelect(board, sourceRow, sourceCol, turn);

				if (repeat)
				{
					cursorPosition(20, 22);
					cout << "Invalid Selection...Enter Again";
				}
			} while (repeat);

			cursorPosition(20, 22);
			cout << "                                             ";

			highlight(board, hLight, sourceRow, sourceCol, turn);

			do
			{
				peiceSelect(destRow, destCol);
				repeat = !validDest(board, destRow, destCol, turn);

				if (repeat)
				{
					cursorPosition(20, 22);
					cout << "Invalid Destination...Enter Again";
				}
			} while (repeat);

			unHighlight(board, hLight, sourceRow, sourceCol, turn);

			cursorPosition(20, 22);
			cout << "                                             ";

			repeat = !legalMove(board, sourceRow, sourceCol, destRow, destCol);

			if (repeat)
			{
				cout << "												";
				cursorPosition(20, 22);
				cout << "Invalid Move...Try Again!";
			}
		} while (repeat or selfCheck(board, sourceRow, sourceCol, destRow, destCol, turn));

		cursorPosition(20, 22);
		cout << "                                             ";

		Save("Previous.txt", board);
		//Save(writer, board);

		boardMovement(board, sourceRow, sourceCol, destRow, destCol, turn);

		if (check(board, sourceRow, sourceCol, destRow, destCol, turn))
		{
			cursorPosition(22, 24);
			cout << "CHECK BOI!";
		}
		else
		{
			cursorPosition(22, 24);
			cout << "           ";
		}

		turnUpdate(turn);

		cursorPosition(0, 0);
		displayBoard(board);

		turnMsg(names[turn]);

	} while (IcanMove(board, hLight, turn));

	turnUpdate(turn);

	cursorPosition(22, 24);

	if (CheckMate(board, hLight, sourceRow, sourceCol, destRow, destCol, turn))
	{
		cout << names[turn] << " WINS!\n" << names[!turn] << " noob.";
	}
	else if (StaleMate(board, hLight, sourceRow, sourceCol, destRow, destCol, turn))
	{
		cout << "DRAAAAWW!!";
	}


	return 0;
}