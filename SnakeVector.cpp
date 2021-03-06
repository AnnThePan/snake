﻿/*
	Snake
	Written by: AnnThePan
*/

#include <iostream>
#include <thread>
#include <vector>
using namespace std;

#include <stdio.h>
#include <Windows.h>

int nScreenWidth = 120;			// Console Screen Size X (columns)
int nScreenHeight = 30;			// Console Screen Size Y (rows)
int nFieldWidth = 32;
int nFieldHeight = 18;
unsigned char *pField = nullptr;
vector <vector <int>> positionSnake{
	{10, 12},
	{10, 11},
	{10, 10},
	{10, 9}};





int main()
{
	// Create Screen Buffer
	wchar_t *screen = new wchar_t[nScreenWidth*nScreenHeight];
	for (int i = 0; i < nScreenWidth*nScreenHeight; i++) screen[i] = L' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	pField = new unsigned char[nFieldWidth*nFieldHeight]; // Create play field buffer
	for (int x = 0; x < nFieldWidth; x++) // Board Boundary
		for (int y = 0; y < nFieldHeight; y++)
			pField[y*nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 1 : 0;

	
	// Game Logic
	bool bKey[4];
	bool bForceDown = false;
	int nScore = 0;
	bool bGameOver = false;
	bool food = false;
	int foodX = 0;
	int foodY = 0;
	int direction = 2;
	bool bomb = false; 
	int bombX = 0; 
	int bombY = 0; 
	int bombTime = 0; 

	
	while (!bGameOver) // Main Loop
	{
		//Count up on time bomb stay in one place - then it switches
		if (bomb == true)
		{
			if (bombTime > (100 - nScore))
			{
				bomb = false;
				bombTime = 0;
			}
			else 
				bombTime++;
		}

		// Timing =======================
		//Pace increase as you get more points 
		this_thread::sleep_for(chrono::milliseconds(100 - nScore)); // Small Step = 1 Game Tick
		bForceDown = true;

		// Input ========================
		for (int k = 0; k < 4; k++)			            //R L D U
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28\x26"[k]))) != 0;

		//Handle player movement 
		if (bKey[0] && direction != 2)
			direction = 4;
		else if (bKey[1] && direction != 4)
			direction = 2;
		else if (bKey[2] && direction != 1)
			direction = 3;
		else if (bKey[3] && direction != 3)
			direction = 1;
		
		if (bForceDown)
		{
		    int lengthOfSnake = positionSnake.size();
			//int lengthOfSnake = 4; 
			// Test if piece can be moved down
			//Define where piece hit edge
			if (positionSnake[lengthOfSnake - 1][0] != 0 && positionSnake[lengthOfSnake - 1][1] != 2 && positionSnake[lengthOfSnake - 1][0] != nFieldHeight && positionSnake[lengthOfSnake - 1][1] != nFieldWidth)
			{

				for (int i = 0; i < lengthOfSnake - 1; i++) {
					for (int j = i + 1; j < lengthOfSnake - 1; j++)
					{
						//Control that part of the snake does not overlap 
						if ((positionSnake[i][0] == positionSnake[j][0]) && (positionSnake[i][1] == positionSnake[j][1])) {
							bForceDown = false;
							bGameOver = true;
						}
					}
				}
				//Controls if it hits bomb 
				if (positionSnake[lengthOfSnake - 1][0] == bombX && positionSnake[lengthOfSnake - 1][1] == bombY)
				{
					bForceDown = false;
					bGameOver = true;
				}

				//Controls if snake hits food 
				if (positionSnake[lengthOfSnake - 1][0] == foodX && positionSnake[lengthOfSnake - 1][1] == foodY)
				{

					food = false;
					nScore = nScore + 1;
					vector <int> nextPoint;
					//Control where it gets put in, in the array 
					int difX = abs(positionSnake[0][0] - positionSnake[1][0]);
					int difY = abs(positionSnake[0][1] - positionSnake[1][1]);
		
					nextPoint = { (positionSnake[0][0] - difX), (positionSnake[0][1] - difY) };
									
					positionSnake.insert(positionSnake.begin(), nextPoint);
					lengthOfSnake = positionSnake.size(); 
				}
				
				int startPos0 = positionSnake[lengthOfSnake - 1][0];
				int startPos1 = positionSnake[lengthOfSnake - 1][1];
				for (int i = 0; i < lengthOfSnake -1; i++) {
					positionSnake[i][0] = positionSnake[i + 1][0];

					positionSnake[i][1] = positionSnake[i + 1][1];
				}
				
				//insert new field depending 
				switch (direction) {
				case 1:
					positionSnake[lengthOfSnake - 1][0] = startPos0 - 1;
					positionSnake[lengthOfSnake - 1][1] = startPos1;
					break;
				case 2:
					positionSnake[lengthOfSnake - 1][0] = startPos0;
					positionSnake[lengthOfSnake - 1][1] = startPos1 - 1;
					break;
				case 3:
					positionSnake[lengthOfSnake - 1][0] = startPos0 + 1;
					positionSnake[lengthOfSnake - 1][1] = startPos1;
					break;
				case 4:
					positionSnake[lengthOfSnake - 1][0] = startPos0;
					positionSnake[lengthOfSnake - 1][1] = startPos1 + 1;
					break;

				}

			}
			else //If it hits edge 
			{
				bForceDown = false;
				bGameOver = true;

			}
		}

		if (bomb == false && nScore>10)
		{
			int lengthOfSnake = positionSnake.size(); 
			bombX = rand() % (nFieldHeight - 2) + 1;
			bombY = rand() % (nFieldWidth - 4) + 3;
			while (positionSnake[lengthOfSnake - 1][0] == bombX && positionSnake[lengthOfSnake - 1][1] == bombY)
			{
				bombX = rand() % (nFieldHeight - 2) + 1;
				bombY = rand() % (nFieldWidth - 4) + 3;
			}
			bomb = true;
		}
		
		if (food == false)
		{
			foodX = rand() % (nFieldHeight - 2) + 1;
			foodY = rand() % (nFieldWidth - 4) + 3;
			food = true;
		}


		// Display ======================

		// Draw Field

		for (int x = 0; x < nFieldWidth; x++)
			for (int y = 0; y < nFieldHeight; y++)
			{
				//Draw the top 
				screen[x + 2] = L" #"[pField[y*nFieldWidth + x]];
				//Draw the rest. If repræsentation i given field i 1 then output ' ', if 2 then output # 
				screen[(y + 1)*nScreenWidth + (x + 2)] = L" #"[pField[y*nFieldWidth + x]];
			}
		
		// Draw snake 
		int hey = positionSnake.size(); 
		for (int px = 0; px < hey; px++)
			screen[positionSnake[px][0] * nScreenWidth + positionSnake[px][1]] = 65;
		//Draw food 
		screen[foodX * nScreenWidth + foodY] = 70;
		//Draw bomb 
		if (bomb == true)
			screen[bombX * nScreenWidth + bombY] = 66;

		
		// Draw Score
		swprintf_s(&screen[2 * nScreenWidth + nFieldWidth + 6], 16, L"SCORE: %8d", nScore);
		
		// Display Frame
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
		
	}

	// Oh Dear - dead 
	CloseHandle(hConsole);
	cout << "Game Over!! Score:" << nScore << endl;
	system("pause");
	
	return 0;
}
