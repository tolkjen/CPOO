/*
 * MedianModel.cpp
 *
 *  Created on: 03-05-2013
 *      Author: tolkjen
 */

#include <cmath>
#include <cstdio>
#include <vector>
#include <algorithm>
#include "MedianModel.h"

MedianModel::MedianModel() {
	frameBufferSize = 5;
	queuePtr = 0;
	queueFull = false;
	treshold = 20;
	outputFrame = 0;
	data = 0;
	_name = "Median based model";
}

MedianModel::MedianModel(int frames, int tresh) {
	frameBufferSize = frames;
	queuePtr = 0;
	queueFull = false;
	treshold = tresh;
	outputFrame = 0;
	data = 0;
	_name = "Median based model";
}

MedianModel::~MedianModel() {
	if (outputFrame) {
		delete outputFrame;
	}
	if (data) {
		delete data;
	}
}

void MedianModel::insert(Mat &m) {
	// First we need to fill the buffer with 'frameBufferSize' frames. If it
	// is filled, we may proceed to the algorithm.
	if (queueFull) {
		for (int y = 0; y < m.rows; y++) {
			Vec3b *newFramePtr = m.ptr<Vec3b>(y);
			uchar *outputPtr = outputFrame->ptr<uchar>(y);
			for (int x = 0; x < m.cols; x++) {
				int newGrey = (newFramePtr[x][0]+newFramePtr[x][1]+newFramePtr[x][2]) / 3;
				data[(y*m.cols + x)*frameBufferSize + queuePtr] = newGrey;

				uchar *bufferPtr = data + (y*m.cols + x)*frameBufferSize;
				vector<uchar> v(bufferPtr, bufferPtr + frameBufferSize);
				nth_element(v.begin(), v.begin() + frameBufferSize / 2, v.end());

				int diff = abs(v[frameBufferSize / 2] - newGrey);
				outputPtr[x] = diff > treshold ? 255 : 0;
			}
		}
	// If the buffer is not filled yet ...
	} else {
		// If the buffer is not allocated, allocate the memory for it.
		if (!data) {
			data = new uchar[m.rows * m.cols * frameBufferSize];
		}
		// Copy a frame into a buffer
		for (int y = 0; y < m.rows; y++) {
			Vec3b *newFramePtr = m.ptr<Vec3b>(y);
			for (int x = 0; x < m.cols; x++) {
				int newGrey = (newFramePtr[x][0]+newFramePtr[x][1]+newFramePtr[x][2]) / 3;
				data[(y*m.cols + x)*frameBufferSize + queuePtr] = newGrey;
			}
		}
		if (!queueFull) {
			if (queuePtr == frameBufferSize - 1) {
				queueFull = true;
			}
		}
		// If there is not output frame yet, create it and give it dimensions of
		// a captured frame.
		if (!outputFrame) {
			outputFrame = new Mat(m.size(), CV_8U);
		}
	}
	queuePtr = (queuePtr + 1) % frameBufferSize;
}

Mat& MedianModel::resultingFrame() {
	return *outputFrame;
}

