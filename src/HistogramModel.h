/*
 * HistogramModel.h
 *
 *  Created on: 14-04-2013
 *      Author: tolkjen
 */

#ifndef HISTOGRAMMODEL_H_
#define HISTOGRAMMODEL_H_

#include <list>
#include "BackgroundModel.h"

using namespace std;

// ----------------------------------------------------------------------------
// HistogramModel class
//
// Histogram method uses kernel density estimation functions to estimate the
// probability function of each pixel's value.
//
// The estimation is acquired by calculating a weighted sum of pixel's previous
// values. Calculating a weight for each pixel is time-consuming, so in order
// to increase performance, the weights are precalculated in object's
// constructor.
class HistogramModel: public BackgroundModel {
public:
	HistogramModel();
	HistogramModel(int frames, int tresh, int h);
	virtual ~HistogramModel();

	virtual void insert(Mat &m);
	virtual Mat& resultingFrame();

private:
	double Kfunc(int x);
	inline double KfuncPrecomp(int x);
	void preparePrecomp();

	list<Mat> frames;
	Mat *outputFrame, *modelFrame;
	int frameBufferSize, treshold;
	double H, precomp[256];
};

#endif /* HISTOGRAMMODEL_H_ */
