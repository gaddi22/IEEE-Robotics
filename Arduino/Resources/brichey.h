#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <math.h>

int total(int x, int y) {
	return x + y;
}

//adds all indexes of source arr to target arr
void addArr(int source[], int target[], int qty) {
	for (int index = 0; index < qty; index++) {
		target[index] += source[index];
	}
}

void clearConsole() {
	system("CLS");
}

std::string classifyColor(int r, int g, int b) {
	int currentArr[] = { r, g, b };	//color passed to function
	std::string classes[] = { 
    "red", 
    "cyan", 
    "blue",
    "grey",
    "black", 
    "green", 
    "white",
    "brown",
    "orange", 
    "yellow", 
    "magenta" 
  };

	int red[]     = { 255,   0,   0};
	int cyan[]    = {   0, 255, 255};
	int blue[]    = {   0,   0, 255};
	int grey[]    = { 128, 128, 128};
	int black[]   = {   0,   0,   0};
	int green[]   = {   0, 255,   0};
	int white[]   = { 255, 255, 255};
	int brown[]   = { 101,  67,  33};
	int orange[]  = { 255, 165,   0};
	int yellow[]  = { 255, 255,   0};
	int magenta[] = { 255,   0, 255};


	struct distance //distance from color array
	{
		int rd;
		int cd;
		int bd;
		int grd;
		int bkd;
		int gd;
		int wd;
		int brd;
		int od;
		int yd;
		int md;
	};

	distance deltas = {	//distances from rgb to each vertex of RGB cube
		distanceArr(currentArr, red,     3), 
		distanceArr(currentArr, cyan,    3),
		distanceArr(currentArr, blue,    3),
		distanceArr(currentArr, grey,    3),
		distanceArr(currentArr, black,   3),
		distanceArr(currentArr, green,   3),
		distanceArr(currentArr, white,   3),
		distanceArr(currentArr, brown,   3),
		distanceArr(currentArr, orange,  3),
		distanceArr(currentArr, yellow,  3),
		distanceArr(currentArr, magenta, 3),
	};


	//----------Find minimum distance----------
	double dArr[] = { 
    deltas.rd, 
    deltas.cd, 
    deltas.bd,
    deltas.grd, 
    deltas.bkd, 
    deltas.gd, 
    deltas.wd,
    deltas.brd,
    deltas.od, 
    deltas.yd, 
    deltas.md 
  };

  // cout << "Display Distance Array" << endl;
  // for(int index = 0; index < 11; index++){
  //   cout << dArr[index] << " ";
  // }
  // cout << endl;

	int classIndex = 0;	//index of closest color for classifier
	double min = dArr[0];
	for (int i = 0; i < 11; i++) {
		if (dArr[i] < min) {
			min = dArr[i];
			classIndex = i;
		}
	}

	return classes[classIndex];
}

//replaces all values in target array with values in source
void copyArr(int source[], int target[], int qty) {
	for (int index = 0; index < qty; index++) {
		target[index] = source[index];
	}
}

//returns euclidean distance between 2 arrays given capacity of arrSize
double distanceArr(int arr[], int arr2[], int arrSize) {
	int index = 0;
	double distance = 0;
	for (index; index < arrSize; index++) {
		distance += pow((arr[index] - arr2[index]), 2);
	}
	distance = pow(distance, 1.0 / arrSize);
	return distance;
}

//reads value from console and verifies it is an int, then returns that value
//msg:	msg to display to console
int getInt(std::string msg = "Enter value: ") {
	std::string num = "";
	int rv = -32767;
	std::cout << msg;
	getline(std::cin, num);	 //get int value from console
	try
	{
		int rv = std::stoi(num); //return value
		return rv;
	}
	catch (const std::exception&)
	{
		std::cout << std::endl << "Invalid number. Try again" << std::endl;
		return getInt(msg);
	}
}

//takes val address and changes its value to whatever is input to the console
std::string & Input(std::string &val) {
	val.clear();		//reinitialize val
	getline(std::cin, val);
	return val;
}

//initializes values of an array
//size: array length
void initArray(int arr[], int size, int val) {
	for (int x = 0; x < size; x++) {
		arr[x] = val;
	}
}

//returns if first qty values in array are == 0
bool isEmpty(int arr[], int qty) {
	for (int index = 0; index < qty; index++) {
		if (arr[index] != 0) { return false; }
	}
	return true;
}

void printArr(int arr[], int qty) {
	for (int index = 0; index < qty; index++) {
		std::cout << arr[index] << "  ";
	}
	std::cout << std::endl;
}

//displays qty number of elements from an array
void spit(int array[], int qty) {
	for (int index = 0; index < qty; index++) {
		std::cout << array[index] << std::endl;
	}
}

//displays qty number of elements from an array and displays msg at the front of each one
void spit(int array[], int qty, std::string msg) {
	for (int index = 0; index < qty; index++) {
		std::cout << msg << array[index] << std::endl;
	}
}

//displays qty number of elements from an array and displays msg at the front of each one including index
void spit(int array[], int qty, std::string msg, bool log) {
	for (int index = 0; index < qty; index++) {
		std::cout << msg << " " << index << ": " << array[index] << std::endl;
	}
}

//arr: array of values to find std dev of
//qty: size of array/number of indexes to include in result
double stDev(double arr[], int qty) {
	double sum = 0;
	for (int index = 0; index < qty; index++) {
		sum += arr[index];
	}

	double meanSample = sum / double(qty);

	double sqDevSum = 0;

	for (int index = 0; index < qty; index++) {
		sqDevSum += pow((meanSample - arr[index]), 2);
	}

	double stDev = sqrt(sqDevSum / double(qty));
	return stDev;
}

//converts int, val, to a string
std::string intToString(int val) {
	std::stringstream ss;
	ss << val << std::endl;
	std::string valAsString = ss.str();
	return valAsString;
}

//returns char c as an integer between 0-9
int to_int(char c) {
	return c % 48;
}