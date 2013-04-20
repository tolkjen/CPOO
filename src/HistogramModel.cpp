/*
 * HistogramModel.cpp
 *
 *  Created on: 14-04-2013
 *      Author: tolkjen
 */

#include <cmath>
#include <cstdio>
#include "HistogramModel.h"

HistogramModel::HistogramModel() {
	frameBufferSize = 10;
	treshold = 50;
	H = 2.0;
	outputFrame = 0;
	modelFrame = 0;
	_name = "Histogram based model";
	preparePrecomp();
}

HistogramModel::HistogramModel(int frames, int tresh, int h) {
	frameBufferSize = frames;
	treshold = tresh;
	H = h;
	outputFrame = 0;
	modelFrame = 0;
	_name = "Histogram based model";
	preparePrecomp();
}

HistogramModel::~HistogramModel() {
	if (outputFrame) {
		delete outputFrame;
	}
	if (modelFrame) {
		delete modelFrame;
	}
}

void HistogramModel::insert(Mat &m) {
	if (frames.size() == (unsigned int) frameBufferSize) {
		list<Mat>::iterator it;
		for (int y = 0; y < m.rows; y++) {
			Vec3b *newFramePtr = m.ptr<Vec3b>(y);
			uchar *outputPtr = outputFrame->ptr<uchar>(y);
			uchar *modelPtr = modelFrame->ptr<uchar>(y);

			for (int x = 0; x < m.cols; x++) {
				int newGrey = (newFramePtr[x][0]+newFramePtr[x][1]+newFramePtr[x][2]) / 3;
				double sum0 = 0, sum1 = 0;

				for (it=frames.begin(); it!=frames.end(); ++it) {
					Vec3b *buffFramePtr = it->ptr<Vec3b>(y);
					int buffValue = (buffFramePtr[x][0]+buffFramePtr[x][1]+buffFramePtr[x][2]) / 3;
					double weight = KfuncPrecomp(modelPtr[x] - buffValue / H);
					sum0 += weight * buffValue;
					sum1 += weight;
				}

				modelPtr[x] = (double) sum0 / sum1;
				int diff = abs(modelPtr[x] - newGrey);
				outputPtr[x] = diff > treshold ? 255 : 0;
			}
		}
		frames.push_front(m.clone());
		frames.pop_back();
	} else {
		frames.push_front(m.clone());
		if (!outputFrame) {
			outputFrame = new Mat(m.size(), CV_8U);
			modelFrame = new Mat(m.size(), CV_8U);
		}
	}
}

Mat& HistogramModel::resultingFrame() {
	return *outputFrame;
}

double HistogramModel::Kfunc(int x) {
	double xNorm = x / 32;
	return (double) exp(-xNorm*xNorm);
}

double HistogramModel::KfuncPrecomp(int x) {
	if (x < 0) x = -x;
	return precomp[x];
}

void HistogramModel::preparePrecomp() {
	for (int i = 0; i < 256; i++)
		precomp[i] = Kfunc(i);
}

