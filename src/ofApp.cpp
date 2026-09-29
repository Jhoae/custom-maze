/*

	ofxWinMenu basic example - ofApp.cpp

	Example of using ofxWinMenu addon to create a menu for a Microsoft Windows application.

	Copyright (C) 2016-2017 Lynn Jarvis.

	https://github.com/leadedge

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

	03.11.16 - minor comment cleanup
	21.02.17 - rebuild for OF 0.9.8

*/
#include "ofApp.h"
#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <algorithm>
#include <cstring>

using namespace std;

// One definition shared by the original app and its main translation unit.
ofxPanel gui;
ofxIntSlider widthSlider;
ofxIntSlider heightSlider;

// Global variable

char** maze = NULL; // store the maze state
int input_width = 0, input_height = 0; // User input - Maze width and height (e.g., for a 5x6 maze: width = 5, height = 6)
int maze_height, maze_width; // .maz file's width and height including walls (e.g., for a 5x6 maze: width = 11, height = 13)

// for dfs
int** binary_maze = NULL; // For DFS - A 2D array consisting of 0s and 1s
int** visited = NULL;

struct location {
	int row;
	int col;
};
vector<location> path; // Vector for the selected path
vector<location> all_path; // Vector for all paths


vector<location> obstacle_locations; // Vector for obstacles

// starting point
int start_row, start_col;

// end point
int end_row, end_col;
//--------------------------------------------------------------


void ofApp::setup() {
	gui.setup();
	gui.add(widthSlider.setup("Width (2~10)", 2, 2, 10)); // Default value: 2, Minimum value: 2, Maximum value: 10
	gui.add(heightSlider.setup("Height (2~10)", 2, 2, 10));  /// Default value: 2, Minimum value: 2, Maximum value: 10
	ofSetWindowTitle("Custom Maze"); // Set the app name on the title bar
	ofSetFrameRate(15);
	ofBackground(210, 180, 140);
	// Get the window size for image loading
	windowWidth = ofGetWidth();
	windowHeight = ofGetHeight();
	isdfs = false;
	isbfs = false;
	is_open = 0;
	// Centre on the screen
	ofSetWindowPosition((ofGetScreenWidth() - windowWidth) / 2, (ofGetScreenHeight() - windowHeight) / 2);

	// Load an image for the example
	//myImage.loadImage("lighthouse.jpg");


	// Disable escape key exit so we can exit fullscreen with Escape (see keyPressed)
	ofSetEscapeQuitsApp(false);

	// Create a menu using ofxWinMenu

	// A new menu object with a pointer to this class
	menu = new MacMenu([this](const std::string& title, bool checked) {
		appMenuFunction(title, checked);
	});

	// Register an ofApp function that is called when a menu item is selected.
	// The function can be called anything but must exist. 
	// See the example "appMenuFunction".
	// MacMenu forwards native menu selections to the same callback.

	// Create a window menu
	MacMenu::Handle hMenu = menu->CreateWindowMenu();

	//
	// Create a "File" popup menu
	//
	MacMenu::Handle hPopup = menu->AddPopupMenu(hMenu, "File");

	//
	// Add popup items to the File menu
	//

	// Open an maze file
	menu->AddPopupItem(hPopup, "Open", false, false); // Not checked and not auto-checked

	// Final File popup menu item is "Exit" - add a separator before it
	menu->AddPopupSeparator(hPopup);
	menu->AddPopupItem(hPopup, "Exit", false, false);

	//
	// View popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "View");

	bShowInfo = true;  // screen info display on
	menu->AddPopupItem(hPopup, "Show DFS", false, false); // Checked
	bTopmost = false; // app is topmost
	menu->AddPopupItem(hPopup, "Show BFS", false, false); // Not checked (default)
	bFullscreen = false; // not fullscreen yet
	menu->AddPopupItem(hPopup, "Full screen", false, false); // Not checked and not auto-check

	//
	// Help popup menu
	//
	/*
	hPopup = menu->AddPopupMenu(hMenu, "Help");
	menu->AddPopupItem(hPopup, "About", false, false); // No auto check
	*/

	//
// Help popup menu
//
	hPopup = menu->AddPopupMenu(hMenu, "Maze");
	menu->AddPopupItem(hPopup, "create maze", false, false); // No auto check

	hPopup = menu->AddPopupMenu(hMenu, "Obstacle");
	menu->AddPopupItem(hPopup, "create obstacle", false, false); // No auto check
	menu->AddPopupItem(hPopup, "remove obstacle", false, false); // No auto check
	menu->AddPopupItem(hPopup, "remove all obstacle", false, false); // No auto check

	hPopup = menu->AddPopupMenu(hMenu, "Positon");
	menu->AddPopupItem(hPopup, "change start position", false, false); // No auto check
	menu->AddPopupItem(hPopup, "change end position", false, false); // No auto check

	// Set the menu to the window
	menu->SetWindowMenu();

} // end Setup


//
// Menu function
//
// This function is called by ofxWinMenu when an item is selected.
// The the title and state can be checked for required action.
// 
void ofApp::appMenuFunction(string title, bool bChecked) {

	ofFileDialogResult result;
	string filePath;
	size_t pos;

	//
	// File menu
	//
	if (title == "Open") {
		if (readFile()) {
			is_open = 1;
			isdfs = 0;
			isbfs = 0;
		}
	}
	if (title == "Exit") {
		ofExit(); // Quit the application
	}

	//
	// Window menu
	//
	if (title == "Show DFS") {
		//bShowInfo = bChecked;  // Flag is used elsewhere in Draw()
		if (is_open)
		{
			isbfs = 0;

			DFS();
			//			bShowInfo = bChecked;
		}
		else
			cout << "you must open file first" << endl;

	}

	if (title == "Show BFS") {
		// doTopmost(bChecked); // Use the checked value directly

		if (is_open)
		{
			isdfs = 0;

			BFS();
			//			bShowInfo = bChecked;
		}
		else
			cout << "you must open file first" << endl;

	}

	if (title == "Full screen") {
		bFullscreen = !bFullscreen; // Not auto-checked and also used in the keyPressed function
		doFullScreen(bFullscreen); // But als take action immediately
	}

	//
	// Help menu
	//


	if (title == "create maze") {
		create_maze();
	}

	if (title == "create obstacle") {
		create_random_obstacle();
	}

	if (title == "remove obstacle") {
		remove_obstacle();
	}

	if (title == "remove all obstacle") {
		remove_all_obstacle();
	}

	if (title == "change start position") {
		change_start_position();
	}

	if (title == "change end position") {
		change_end_position();
	}

} // end appMenuFunction


//--------------------------------------------------------------
void ofApp::update() {

}

//--------------------------------------------------------------
void ofApp::draw() {
	gui.draw();

	ofSetColor(100);
	ofSetLineWidth(5);
	int i, j;

	for (j = 0; j < input_width; j++) {
		for (i = 0; i < input_height; i++) {

			int x = j * 17;
			int y = i * 17;

			if (maze[i][j] == '+') {
				ofSetColor(200, 200, 200);
				ofDrawCircle(x + 8.5, y + 8.5, 2);
			}
			else if (maze[i][j] == '-') {
				ofSetColor(200, 200, 200);

				ofDrawLine(x - 8.3, y + 8.5, x + 25.5, y + 8.5);
			}
			else if (maze[i][j] == '|') {
				ofSetColor(200, 200, 200);

				ofDrawLine(x + 8.3, y - 10.5, x + 8.5, y + 25.5);
			}
			else if (maze[i][j] == ' ') {
				if (i % 2 == 1 && j % 2 == 1) {
					ofSetColor(210, 180, 140);
					ofDrawRectangle(x + 5, y + 5, 7, 7);
				}
				else {
					ofSetColor(210, 180, 140);
					ofDrawRectangle(x, y, 17, 17);
				}
			}

			else if (maze[i][j] == 'X') {
				ofSetColor(255, 0, 0);
				int centerX = x + 8;
				int centerY = y + 8;
				int radius = 8;
				ofDrawCircle(centerX, centerY, radius);
			}
		}
	}

	if (is_open) {
		draw_positions();
	}

	if (isdfs || isbfs)
	{
		ofSetLineWidth(5);
		if (is_open)
			linedraw();
		else
			cout << "You must open file first" << endl;
	}


} // end Draw

// oF functions - draw the starting and end points
void ofApp::draw_positions() {
	ofPath start_point;
	start_point.setFillColor(ofColor(0));
	float startX = start_col * 17 + 8.5;
	float startY = start_row * 17 + 8.5;
	for (int i = 0; i < 6; i++) {
		float angle = TWO_PI * i / 6.0;
		start_point.moveTo(startX, startY);
		start_point.lineTo(startX + cos(angle) * 10, startY + sin(angle) * 10);
		start_point.lineTo(startX + cos(angle + PI / 6) * 5, startY + sin(angle + PI / 6) * 5);
		start_point.close();
	}
	start_point.draw();

	ofPath end_point;
	end_point.setFillColor(ofColor(255, 255, 255));
	float endX = end_col * 17 + 8.5;
	float endY = end_row * 17 + 8.5;
	for (int i = 0; i < 6; i++) {
		float angle = TWO_PI * i / 6.0;
		end_point.moveTo(endX, endY);
		end_point.lineTo(endX + cos(angle) * 10, endY + sin(angle) * 10);
		end_point.lineTo(endX + cos(angle + PI / 6) * 5, endY + sin(angle + PI / 6) * 5);
		end_point.close();
	}
	end_point.draw();

}
void ofApp::doFullScreen(bool bFull)
{
	// Enter full screen
	if (bFull) {
		// Remove the menu but don't destroy it
		menu->RemoveWindowMenu();
		// hide the cursor
		ofHideCursor();
		// Set full screen
		ofSetFullscreen(true);
	}
	else {
		// return from full screen
		ofSetFullscreen(false);
		// Restore the menu
		menu->SetWindowMenu();
		// Restore the window size allowing for the menu
		ofSetWindowShape(windowWidth, windowHeight);
		// Centre on the screen
		ofSetWindowPosition((ofGetScreenWidth() - ofGetWidth()) / 2, (ofGetScreenHeight() - ofGetHeight()) / 2);
		// Show the cursor again
		ofShowCursor();
		// Restore topmost state
		if (bTopmost) doTopmost(true);
	}

} // end doFullScreen


void ofApp::doTopmost(bool bTop)
{
	menu->SetTopmost(bTop);
} // end doTopmost


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	// Escape key exit has been disabled but it can be checked here
	if (key == OF_KEY_ESC) {
		// Disable fullscreen set, otherwise quit the application as usual
		if (bFullscreen) {
			bFullscreen = false;
			doFullScreen(false);
		}
		else {
			ofExit();
		}
	}

	// Remove or show screen info
	if (key == ' ') {
		bShowInfo = !bShowInfo;
		// Update the menu check mark because the item state has been changed here
		menu->SetPopupItem("Show DFS", bShowInfo);
	}

	if (key == 'f') {
		bFullscreen = !bFullscreen;
		doFullScreen(bFullscreen);
		// Do not check this menu item
		// If there is no menu when you call the SetPopupItem function it will crash
	}

} // end keyPressed

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
bool ofApp::readFile()
{
	ofFileDialogResult openFileResult = ofSystemLoadDialog("Select .maz file", false, ofToDataPath("", true));
	string filePath;
	size_t pos;

	// Check - opened a file
	if (openFileResult.bSuccess) {
		ofLogVerbose("User selected a file");

		//We have a file, check it and process it
		string fileName = openFileResult.getName();
		// string fileName = "maze0.maz";
		printf("file name is %s\n", fileName.c_str());
		filePath = openFileResult.getPath();
		printf("Open\n");
		pos = filePath.find_last_of(".");
		if (pos != string::npos && pos != 0 && filePath.substr(pos + 1) == "maz") {

			ofFile file(filePath);

			if (!file.exists()) {
				cout << "Target file does not exists." << endl;
				return false;
			}
			else {
				cout << "We found the target file." << endl;
				is_open = 1;
			}

			ofBuffer buffer(file);


			// Idx is a variable for index of array.
			int idx = 0;

			// Read file line by line
			int cnt = 0;

			// Takes a .maz file as input and stores it appropriately in the data structure

			ofBuffer::Lines lines = buffer.getLines();

			// input_width and input_height
			input_width = 0;
			input_height = 0;

			for (ofBuffer::Line str = lines.begin(); str != lines.end(); str++) {
				if (input_height == 0) {
					input_width = str->length();
				}
				cnt++;
			}
			input_height = cnt;

			// Dynamically allocate the maze array -> Store maze data
			maze = (char**)malloc(sizeof(char*) * input_height);
			for (int i = 0; i < input_height; i++) {
				maze[i] = (char*)malloc(sizeof(char) * (input_width + 1));
			}
			// Copy maze information line by line
			for (ofBuffer::Line str = lines.begin(); str != lines.end(); str++) {
				strcpy(maze[idx], str->c_str());
				idx++;
			}

			maze_height = input_height;
			maze_width = input_width;

			start_row = 1;
			start_col = 1;
			end_row = input_height - 2;
			end_col = input_width - 2;

			printf("Success .maz load.\n");
			return true;
		}
		else {
			printf("  Needs a '.maz' extension\n");
			return false;
		}
	}
	return false; // File selection was cancelled.
}
void ofApp::freeMemory() {

	// Function to free the memory allocated by malloc
	for (int i = 0; i < input_height; i++) {
		free(maze[i]);
		free(binary_maze[i]);
		free(visited[i]);
	}
	free(maze);
	free(binary_maze);
	free(visited);
}

bool ofApp::DFS() {

	// Initialize the 2D array for DFS
	binary_maze = (int**)malloc(sizeof(int*) * input_height);
	visited = (int**)malloc(sizeof(int*) * input_height);

	for (int i = 0; i < input_height; i++) {
		binary_maze[i] = (int*)malloc(sizeof(int) * input_width);
		visited[i] = (int*)malloc(sizeof(int) * input_width);
	}

	// For binary_maze: Set the border to 0, walls to 0 (inaccessible), and the rest to 1 (accessible)
	// For visited: Set the border to 1, walls to 1 (not visitable), and the rest to 0 (visitable)
	for (int i = 0; i < input_height; i++) {
		for (int j = 0; j < input_width; j++) {
			// Border
			if (i == 0 || i == input_height - 1 || j == 0 || j == input_width - 1) {
				binary_maze[i][j] = 0;
				visited[i][j] = 1;
			}
			else if (maze[i][j] == '-' || maze[i][j] == '|' || maze[i][j] == '+' || maze[i][j] == 'X') {
				binary_maze[i][j] = 0;
				visited[i][j] = 1;
			}
			else {
				binary_maze[i][j] = 1;
				visited[i][j] = 0;
			}
		}
	}

	// Right, Down, Left, Up
	int d_row[4] = { 0, 1, 0 ,-1 };
	int d_col[4] = { 1, 0, -1 ,0 };

	// stack for DFS
	stack<location> dfs_stack;
	path.clear();
	all_path.clear();

	if (end_row == 0 && end_col == 0) {
		end_row = input_height - 2;
		end_col = input_width - 2;
	}

	// Push starting point onto the stack, mark as visited, and add to the path
	dfs_stack.push({ start_row, start_col });
	visited[start_row][start_col] = 1;
	path.push_back({ start_row, start_col });
	all_path.push_back({ start_row,start_col });

	while (!dfs_stack.empty()) {
		location now = dfs_stack.top();
		int now_row = now.row;
		int now_col = now.col;

		// If path found
		if (now_row == end_row && now_col == end_col) {
			break;
		}

		// For push back paths
		int moved = 0;

		for (int i = 0; i < 4; i++) {
			int next_row = now_row + d_row[i];
			int next_col = now_col + d_col[i];

			// Check if within range && not visited && accessible (1)
			if (next_row >= 1 && next_row < input_height - 1 &&
				next_col >= 1 && next_col < input_width - 1 &&
				visited[next_row][next_col] == 0 &&
				binary_maze[next_row][next_col] == 1) {
				dfs_stack.push({ next_row, next_col });
				visited[next_row][next_col] = 1;
				path.push_back({ next_row, next_col });
				all_path.push_back({ next_row,next_col });
				moved = 1;
				break;
			}
		}

		// Blocked path -> Remove
		if (moved == 0) {
			dfs_stack.pop();
			path.pop_back();
		}
	}

	isdfs = true;
	return 1;
}

bool ofApp::BFS() {
	// Initialize the 2D array for BFS
	binary_maze = (int**)malloc(sizeof(int*) * input_height);
	visited = (int**)malloc(sizeof(int*) * input_height);

	for (int i = 0; i < input_height; i++) {
		binary_maze[i] = (int*)malloc(sizeof(int) * input_width);
		visited[i] = (int*)malloc(sizeof(int) * input_width);
	}

	// For binary_maze: Set the border to 0, walls to 0 (inaccessible), and the rest to 1 (accessible)
	// For visited: Set the border to 1, walls to 1 (not visitable), and the rest to 0 (visitable)
	for (int i = 0; i < input_height; i++) {
		for (int j = 0; j < input_width; j++) {
			// Border
			if (i == 0 || i == input_height - 1 || j == 0 || j == input_width - 1) {
				binary_maze[i][j] = 0;
				visited[i][j] = 1;
			}
			else if (maze[i][j] == '-' || maze[i][j] == '|' || maze[i][j] == '+' || maze[i][j] == 'X') {
				binary_maze[i][j] = 0;
				visited[i][j] = 1;
			}
			else {
				binary_maze[i][j] = 1;
				visited[i][j] = 0;
			}
		}
	}

	// Right, Down, Left, Up
	int d_row[4] = { 0, 1, 0, -1 };
	int d_col[4] = { 1, 0, -1, 0 };

	// Queue for BFS
	queue<location> bfs_queue;
	path.clear();
	all_path.clear();

	// Find the path from [1][1] to [input_height - 2][input_width - 2] in binary_maze
	if (end_row == 0 && end_col == 0) {
		end_row = input_height - 2;
		end_col = input_width - 2;
	}

	// Insert the starting point into the Queue, mark as visited, and add to the path
	bfs_queue.push({ start_row, start_col });
	visited[start_row][start_col] = 1;
	all_path.push_back({ start_row, start_col });

	// malloc parent array (for tracking the shortest path) 
	location** parent_bfs = (location**)malloc(sizeof(location*) * input_height);
	for (int i = 0; i < input_height; i++) {
		parent_bfs[i] = (location*)malloc(sizeof(location) * input_width);
	}

	while (!bfs_queue.empty()) {
		location now = bfs_queue.front();
		bfs_queue.pop();

		// When the target position is reached
		if (now.row == end_row && now.col == end_col) {

			// Trace and the shortest path
			location current = now;
			while (current.row != start_row || current.col != start_col) {
				path.push_back(current);
				// Store the parent-child relationships
				current = parent_bfs[current.row][current.col];
			}
			path.push_back({ start_row, start_col });
			reverse(path.begin(), path.end()); // Reverse the path
			break;
		}

		//  up, down, left, right
		for (int i = 0; i < 4; i++) {
			int next_row = now.row + d_row[i];
			int next_col = now.col + d_col[i];

			// Within range && not visited && accessible (1)
			if (next_row >= 1 && next_row < input_height - 1 &&
				next_col >= 1 && next_col < input_width - 1 &&
				visited[next_row][next_col] == 0 &&
				binary_maze[next_row][next_col] == 1) {
				bfs_queue.push({ next_row, next_col });
				visited[next_row][next_col] = 1;
				all_path.push_back({ next_row, next_col });
				parent_bfs[next_row][next_col] = now;
			}
		}
	}

	isbfs = true;

	for (int i = 0; i < input_height; i++) {
		free(parent_bfs[i]);
	}
	free(parent_bfs);

	return 1;
}

// Function that creates obstacles sequentially from coordinate (1,1) towards the right and downward
// This function is not called for every case of obstacle creation
// It is called when a random obstacle generation fails a certain number of times
void ofApp::create_obstacle(int obstacle) {
	int suc_count = 0;


	for (int h = 1; h < maze_height; h = h += 2) {
		for (int w = 1; w < maze_width; w += 2) {
			// Not create obstacles in areas too close to the starting point
			if (h <= 3 && w <= 3) {
				continue;
			}
			if (h == end_row && w == end_col) {
				continue;
			}

			// create obstacles
			if (maze[h][w] == ' ') {
				maze[h][w] = 'X';
				suc_count += 1;
				// Store the obstacle positions
				obstacle_locations.push_back(location{ h,w });
			}
			if (suc_count == obstacle) {
				break;
			}
		}
		if (suc_count == obstacle) {
			break;
		}
	}
}

// Random obstacle generation function
// - Creates obstacles at random positions
void ofApp::create_random_obstacle() {
	string nInput = ofSystemTextBoxDialog("obstacle count");
	if (nInput.empty()) {
		return;
	}
	int obstacle_count = stoi(nInput);

	// Prompt the user to re-enter 
	// if the input exceeds the maximum number of obstacles
	if (((maze_height - 1) / 2) * ((maze_width - 1) / 2) - 4 < obstacle_count) {
		ofSystemAlertDialog("Too many obstacles!");
		return;
	}

	// Number of successful obstacle creations
	int suc_count = 0;
	// Number of failed obstacle creations
	int fail_count = 0;
	const int fail_max_count = 5000000;
	srand(time(NULL));
	while (suc_count != obstacle_count) {

		// If the number of failed obstacle creations exceeds a certain limit, create obstacles sequentially from (1,1)	
		if (fail_count == fail_max_count) 
		{
			create_obstacle(obstacle_count - suc_count);
			break;
		}

		// Random horizontal for obstacle generation
		int ran_height = rand() % maze_height;

		// Random vertical for obstacle generation
		int ran_width = rand() % maze_width;

		// If obstacles cannot be created, 
		// fail_count++  and continue;
		if (ran_height % 2 == 0 || ran_width % 2 == 0) {
			fail_count += 1;
			continue;
		}
		if (ran_height == end_row || ran_width == end_col) {
			fail_count += 1;
			continue;
		}
		if (maze[ran_height][ran_width] == 'X') {
			fail_count += 1;
			continue;
		}
		if (ran_height <= 3 && ran_width <= 3) {
			fail_count += 1;
			continue;
		}

		// create obstacles in the maze
		maze[ran_height][ran_width] = 'X';

		// Add the coordinates where obstacles are created to the vector
		obstacle_locations.push_back(location{ ran_height, ran_width });
		suc_count += 1;
	}

	if (isbfs) {
		BFS();
	}
	if (isdfs) {
		DFS();
	}
}

// Delete obstacles
void ofApp::remove_obstacle() {
	string nInput = ofSystemTextBoxDialog("remove obstacle count");
	if (nInput.empty()) {
		return;
	}
	int obstacle_count = stoi(nInput);

	// If attempting to delete more obstacles than are created, display an alert
	if (obstacle_locations.size() < obstacle_count) {
		ofSystemAlertDialog("Too many obstacles!");
		return;
	}
	int suc_count = 0;
	srand(time(NULL));

	// Remove obstacles one by one from the vector and delete them
	while (suc_count != obstacle_count) {
		int ran_num = rand() % obstacle_locations.size();
		// Randomly pick an obstacle from the vector
		location remove_location = obstacle_locations[ran_num];
		// Remove the obstacle from the maze
		maze[remove_location.row][remove_location.col] = ' ';
		// Remove the obstacle from the vector
		obstacle_locations.erase(obstacle_locations.begin() + ran_num);
		suc_count += 1;
	}

	if (isbfs) {
		BFS();
	}
	if (isdfs) {
		DFS();
	}
}

// Function to change the starting point
void ofApp::change_start_position() {
	string rowInput = ofSystemTextBoxDialog("Start Position Row");
	if (rowInput.empty()) {
		return;
	}
	string columnInput = ofSystemTextBoxDialog("Start Position Column");
	if (columnInput.empty()) {
		return;
	}

	// Convert input to an integer
	int temp_row = (stoi(rowInput) * 2) - 1;
	int temp_col = (stoi(columnInput) * 2) - 1;

	// Display a warning if the input coordinates are outside the maze
	if (temp_row <= 0 || temp_row >= maze_height || temp_col <= 0 || temp_col >= maze_width) {
		ofSystemAlertDialog("change position is out of range");
		return;
	}

	start_row = temp_row;
	start_col = temp_col;



	if (isbfs) {
		BFS();
	}
	if (isdfs) {
		DFS();
	}
}

// Function to change the destination point
void ofApp::change_end_position() {
	string rowInput = ofSystemTextBoxDialog("End Positon Row");
	if (rowInput.empty()) {
		return;
	}
	string columnInput = ofSystemTextBoxDialog("End Position Column");
	if (columnInput.empty()) {
		return;
	}

	int temp_row = (stoi(rowInput) * 2) - 1;
	int temp_col = (stoi(columnInput) * 2) - 1;

	// Display a warning if the input coordinates are outside the maze
	if (temp_row <= 0 || temp_row >= maze_height || temp_col <= 0 || temp_col >= maze_width) {
		ofSystemAlertDialog("change position is out of range");
		return;
	}

	// Convert input to an integer
	end_row = temp_row;
	end_col = temp_col;

	if (isbfs) {
		BFS();
	}
	if (isdfs) {
		DFS();
	}
}

// Function to remove all obstacles
void ofApp::remove_all_obstacle() {
	int suc_count = 0;
	int obstacle_count = obstacle_locations.size();
	// Loop until all obstacles are removed
	while (suc_count != obstacle_count) {
		location remove_location = obstacle_locations[0];
		maze[remove_location.row][remove_location.col] = ' ';
		obstacle_locations.erase(obstacle_locations.begin() + 0);
		suc_count += 1;
	}

	if (isbfs) {
		BFS();
	}
	if (isdfs) {
		DFS();
	}
}

void ofApp::create_maze() {
	// input values for N and M
	string nInput = ofSystemTextBoxDialog("Width");
	if (nInput.empty()) {
		return;
	}
	string mInput = ofSystemTextBoxDialog("Height");
	if (mInput.empty()) {
		return;
	}

	// Convert input to an integer
	int N = stoi(nInput);
	int M = stoi(mInput);

	// Add a text field to receive the filename (using GUI elements)
	string filename = ofSystemTextBoxDialog("FileName");

	if (filename.empty()) {
		ofSystemAlertDialog("Not empty.");
		return;
	}

	cout << "Craete Maze" << N << " x " << M << endl;

	int parent[10000];
	int size[10000];

	int num_walls = (N - 1) * M + N * (M - 1); // 벽의 총 개수
	Wall* walls = (Wall*)malloc(num_walls * sizeof(Wall));

	char** perfect_Maze = (char**)malloc((2 * M + 1) * sizeof(char*));
	for (int i = 0; i < 2 * M + 1; i++) {
		perfect_Maze[i] = (char*)malloc((2 * N + 1) * sizeof(char));
	}

	initMaze(perfect_Maze, N, M);
	createPerfectWall(walls, parent, size, perfect_Maze, N, M);

	// input file name
	string savePath = ofToDataPath(filename + ".maz", true);
	FILE* file = fopen(savePath.c_str(), "w");
	if (file == nullptr) {
		ofSystemAlertDialog("ERROR : fopen.");
		return;
	}

	// Save maze information to a new .maz file
	for (int i = 0; i < 2 * M + 1; i++) {
		for (int j = 0; j < 2 * N + 1; j++) {
			fprintf(file, "%c", perfect_Maze[i][j]);
		}
		fprintf(file, "\n");
	}

	fclose(file);
	ofSystemAlertDialog("Maze created and saved");
}



void ofApp::linedraw()
{
	// Draw the explored cells and selected path.

	if (isdfs || isbfs) {
		int N_p = path.size();
		int N_a = all_path.size();

		for (int i = 0; i < N_a; i++) {
			int x = all_path[i].col;
			int y = all_path[i].row;

			ofSetColor(200);
			ofDrawRectangle(x * 17, y * 17, 17, 17);
		}


		for (int i = 0; i < N_p; i++) {
			int x = path[i].col;
			int y = path[i].row;

			ofSetColor(32, 178, 170);
			ofDrawRectangle(x * 17, y * 17, 17, 17);
		}

		draw_positions();
	}
}

int find(int* parent, int x)
{
	if (parent[x] != x)
	{
		parent[x] = find(parent, parent[x]);
	}
	return parent[x];
}

void uunion(int* parent, int* size, int x, int y)
{
	int X = find(parent, x);
	int Y = find(parent, y);

	if (X != Y)
	{
		if (size[X] < size[Y])
		{
			parent[X] = Y;
			size[Y] += size[X];
		}
		else
		{
			parent[Y] = X;
			size[X] += size[Y];
		}
	}
}

void deleteWalls(char** maze, int N, int M, int idx1, int idx2)
{
	int a = idx1 / N;
	int b = idx1 % N;

	// Remove the wall between idx1 and idx2
	// Delete on the same line
	if (abs(idx1 - idx2) == 1 && idx1 / N == idx2 / N)
	{
		// In the maze, the position of idx1 is maze[2a+1][2b+1], 
		//and the position of idx2 is maze[2a+1][2b+3]
		maze[2 * a + 1][2 * b + 2] = ' ';
	}
	else if (abs(idx1 - idx2) == N && idx1 / N != idx2 / N) // 한줄차이 삭제
	{

		// In the maze, the position of idx1 is maze[2a+1][b], 
		// and the position of idx2 is maze[2a+3][b].
		maze[2 * a + 2][2 * b + 1] = ' ';
	}
}

// Create Perfect Wall Function
void createPerfectWall(Wall* walls, int* parent, int* size, char** maze, int N, int M)
{
	int num_rooms = N * M;

	for (int i = 0; i < num_rooms; i++)
	{
		parent[i] = i;
		size[i] = 1;
	}

	// horizontal walls
	int num_walls = 0;
	for (int i = 0; i < M - 1; i++)
	{
		for (int j = 0; j < N; j++)
		{
			walls[num_walls].idx1 = i * N + j;       // Upper room 
			walls[num_walls].idx2 = (i + 1) * N + j; // Lower room
			num_walls++;
		}
	}

	// vertical walls
	for (int i = 0; i < M; i++)
	{
		for (int j = 0; j < N - 1; j++)
		{
			walls[num_walls].idx1 = i * N + j;       // left room
			walls[num_walls].idx2 = i * N + (j + 1); // right room
			num_walls++;
		}
	}

	// Shuffle the array for random maze generation
	srand(time(NULL));
	for (int i = 0; i < num_walls; i++)
	{
		int ran = rand() % num_walls;
		Wall temp = walls[i];
		walls[i] = walls[ran];
		walls[ran] = temp;
	}

	// Remove all possible walls in order from the shuffled array
	for (int i = 0; i < num_walls; i++)
	{
		int room1 = walls[i].idx1;
		int room2 = walls[i].idx2;

		if (find(parent, room1) != find(parent, room2))
		{
			deleteWalls(maze, N, M, room1, room2);
			uunion(parent, size, room1, room2);
		}
	}
}

void initMaze(char** maze, int N, int M)
{
	for (int i = 0; i < 2 * M + 1; i++)
	{
		for (int j = 0; j < 2 * N + 1; j++)
		{
			if (i % 2 == 0)
			{
				maze[i][0] = '+';
				maze[i][1] = '-';
				maze[i][2] = '+';

				if (j > 2 && j % 2 == 1)
					maze[i][j] = '-';
				if (j > 2 && j % 2 == 0)
					maze[i][j] = '+';
			}
			else
			{
				maze[i][0] = '|';
				maze[i][1] = ' ';
				maze[i][2] = '|';

				if (j > 2 && j % 2 == 1)
					maze[i][j] = ' ';
				if (j > 2 && j % 2 == 0)
					maze[i][j] = '|';
			}
		}
	}
}

void fprintMaze(char** maze, int N, int M)
{
	FILE* file = fopen("maze.maz", "w");
	if (!file)
	{
		printf("File not exist : %s\n", "maze.maz");
		return;
	}

	for (int i = 0; i < 2 * M + 1; i++)
	{
		for (int j = 0; j < 2 * N + 1; j++)
		{
			fprintf(file, "%c", maze[i][j]);
		}
		fprintf(file, "\n");
	}
	fclose(file);
}
