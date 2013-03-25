/*
 * bmodel.cpp
 *
 *  Created on: 24-03-2013
 *      Author: tolkjen
 */

#include <opencv2\opencv.hpp>
#include <iostream>
#include <vector>
#include <memory>

#include "PreviewModel.h"

using namespace cv;
using namespace std;

void printHelp();
vector<BackgroundModel*> createModels();
void freeModels(vector<BackgroundModel*> &v);

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
	models.push_back(new PreviewModel());

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
