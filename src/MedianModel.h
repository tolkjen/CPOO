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
