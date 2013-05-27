/*
 * MedianModel.h
 *
 *  Created on: 03-05-2013
 *      Author: tolkjen
 */

#ifndef MEDIANMODEL_H_
#define MEDIANMODEL_H_

#include <list>
#include "BackgroundModel.h"

// ----------------------------------------------------------------------------
// MedianModel class
//
// This class uses median-based model to estimate the background of the video
// sequence. It holds 'frameBufferSize' last frames in memory. When a new
// frame is captured it calculates for every pixel a median of its value using
// older frames.
//
// Once a median is found, if a difference between a captured pixel and a
// median is less than a 'treshold' value, the pixel is considered to belong to
// the background. If not, it is a foreground.
class MedianModel: public BackgroundModel {
public:
	MedianModel();
	MedianModel(int frames, int tresh);
	virtual ~MedianModel();

	virtual void insert(Mat &m);
	virtual Mat& resultingFrame();

private:
	uchar *data;
	uchar medianBuffer[256];
	Mat *outputFrame;
	int frameBufferSize, treshold, queuePtr;
	bool queueFull;
};

#endif /* MEDIANMODEL_H_ */
