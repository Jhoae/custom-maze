/*

	ofxWinMenu basic example - ofApp.h

	Copyright (C) 2016-2017 Lynn Jarvis.

	http://www.spout.zeal.co

	=========================================================================
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
	=========================================================================
*/

#pragma once

#include "ofMain.h"
#include "MacMenu.h" // macOS adapter for the original Windows menu
#include <iostream>
#include <string>
#include <fstream>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


// for create maze
#include "ofxGui.h"

extern ofxPanel gui;
extern ofxIntSlider widthSlider;
extern ofxIntSlider heightSlider;

typedef struct
{
	int idx1, idx2;
} Wall;

int find(int* parent, int x);
void uunion(int* parent, int* size, int x, int y);
void deleteWalls(char** maze, int N, int M, int idx1, int idx2);
void createPerfectWall(Wall* walls, int* parent, int* size, char** maze, int N, int M);
void initMaze(char** maze, int N, int M);
void fprintMaze(char** maze, int N, int M);


class ofApp : public ofBaseApp {

public:

	void setup();
	void update();
	void draw();

	void keyPressed(int key); // Traps escape key if exit disabled
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	bool readFile();
	void create_maze();
	void create_obstacle(int obstacle);
	void create_random_obstacle();
	void remove_obstacle();
	void remove_all_obstacle();
	void change_start_position();
	void change_end_position();
	void freeMemory();
	bool DFS();
	bool BFS();
	void linedraw();
	void draw_positions();
	int HEIGHT; // Height of the maze
	int WIDTH; // Width of the maze
	char** input;// A 2D array : all the information from the text file.
	int maze_col; // column index of the maze
	int maze_row; // row index of the maze
	int k;
	int is_open; // 0 means file is not opened, and 1 means opened.
	int is_dFS; // 0 means DFS function has been not executed, and 1 means executed
	int is_bFS;// 0 means BFS function has been not executed, and 1 means executed
	// Menu
	MacMenu* menu; // Menu object
	void appMenuFunction(std::string title, bool bChecked); // Menu return function

	// Used by example app
	ofImage myImage;
	float windowWidth, windowHeight;

	// Example menu variables
	bool bShowInfo;
	bool bFullscreen;
	bool bTopmost;
	bool isdfs;
	bool isbfs;
	// Example functions
	void doFullScreen(bool bFull);
	void doTopmost(bool bTop);

};
