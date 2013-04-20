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
