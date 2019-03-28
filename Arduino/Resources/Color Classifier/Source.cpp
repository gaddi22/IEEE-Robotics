//Take tuple of RBG colors (255, 0, 255) and classifies that as 1 of 8 colors:
//black, red, green, yellow, blue, cyan, magenta, white
#include "brichey.h"

using namespace std;

string classifyColor(int r, int g, int b);
double distanceArr(int arr[], int arr2[], int arrSize);

int main() {
	int RGB[3];
	initArray(RGB, 3, 0);

	RGB[0] = 122;
	RGB[1] = 130;
	RGB[2] = 136;
	cout << "Classifying the following color: ";
	printArr(RGB, 3);
	cout << classifyColor(RGB[0], RGB[1], RGB[2]) << endl;

	system("pause");
	return 0;
}

string classifyColor(int r, int g, int b) {
	int currentArr[] = { r, g, b };	//color passed to function
	string classes[] = { "red", "cyan", "blue", "black", "green", "white", "yellow", "magenta" };

	int red[]     = { 255,   0,   0};
	int cyan[]    = {   0, 255, 255};
	int blue[]    = {   0,   0, 255};
	int black[]   = {   0,   0,   0};
	int green[]   = {   0, 255,   0};
	int white[]   = { 255, 255, 255};
	int yellow[]  = { 255, 255,   0};
	int magenta[] = { 255,   0, 255};

	struct distance //distance from color array
	{
		int rd;
		int cd;
		int bd;
		int bkd;
		int gd;
		int wd;
		int yd;
		int md;
	};

	distance deltas = {	//distances from rgb to each vertex of RGB cube
		distanceArr(currentArr, red,     3), 
		distanceArr(currentArr, cyan,    3),
		distanceArr(currentArr, blue,    3),
		distanceArr(currentArr, black,   3),
		distanceArr(currentArr, green,   3),
		distanceArr(currentArr, white,   3),
		distanceArr(currentArr, yellow,  3),
		distanceArr(currentArr, magenta, 3),
	};


	//----------Find minimum distance----------
	double dArr[] = { deltas.rd, deltas.cd, deltas.bd, deltas.bkd, deltas.gd, deltas.wd, deltas.yd, deltas.md };

	int classIndex = 0;	//index of closest color for classifier
	double min = dArr[0];
	for (int i = 0; i < 8; i++) {
		if (dArr[i] < min) {
			min = dArr[i];
			classIndex = i;
		}
	}

	return classes[classIndex];
}
