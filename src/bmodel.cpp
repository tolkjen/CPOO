/*
 * bmodel.cpp
 *
 *  Created on: 24-03-2013
 *      Author: Piotr T¹kiel
 */

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <memory>

#include "PreviewModel.h"
#include "HistogramModel.h"
#include "MedianModel.h"

using namespace cv;
using namespace std;

// ----------------------------------------------------------------------------
// printHelp
//
// Prints a message on how to use program
void printHelp();

// ----------------------------------------------------------------------------
// createModels
//
// Returns a vector containing pointers to objects, which perform background
// subtraction operations. These objects are created in this function and freed
// by freeModels() function.
vector<BackgroundModel*> createModels();

// ----------------------------------------------------------------------------
// freeModels
//
// Frees memory occupied by BackgroundModels stored in a vector object
void freeModels(vector<BackgroundModel*> &v);

// ----------------------------------------------------------------------------
// The first thing which happens inside main() is opening the video stream.
// Next, instances of classes derived from BackgroundModel class are created
// and stored in models vector. Lastly inside a loop each background model
// object is fed with video frames and the resulting video output is presented
// in a window.
int main( int argc, char** argv ) {
	// arguments
	if (argc != 2) {
		printHelp();
		return 1;
	}

	// input filename
	const string inputFilename = argv[1];

	// open video stream
	VideoCapture videoStream(inputFilename);
	if (!videoStream.isOpened()) {
		cerr << "Can't open file (" << inputFilename << ")" << endl;
		return 1;
	}

	// get frame rate
	int fps = (int) videoStream.get(CV_CAP_PROP_FPS);

	// vector containing objects determining background model
	vector<BackgroundModel*> models = createModels();

	// process the video stream using objects derived from BackgroundModel
	char keyPressed;
	Mat videoFrame;
	while (true) {
		videoStream >> videoFrame;
		if (videoFrame.empty()) {
			break;
		}

		for (unsigned int i = 0; i < models.size(); i++) {
			models[i]->insert(videoFrame);
			imshow(models[i]->name().c_str(), models[i]->resultingFrame());
		}

		// Look for Esc key
		keyPressed = (char) cvWaitKey(fps);
		if (keyPressed == 27) {
			break;
		}
	}

	// free background models' memory
	freeModels(models);
	return 0;
}

void printHelp() {
	cout << "Usage: bmodel <filename>" << endl;
}

vector<BackgroundModel*> createModels() {
	vector<BackgroundModel*> models;

	models.push_back( new PreviewModel() );
	models.push_back( new HistogramModel(20, 30, 2) );
	models.push_back( new MedianModel(20, 30) );

	for (unsigned int i = 0; i < models.size(); i++) {
		namedWindow(models[i]->name().c_str(), CV_WINDOW_AUTOSIZE);
	}
	return models;
}

void freeModels(vector<BackgroundModel*> &v) {
	for (unsigned int i = 0; i < v.size(); i++) {
		delete v[i];
	}
}
