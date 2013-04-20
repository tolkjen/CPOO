/*
 * PreviewModel.cpp
 *
 *  Created on: 24-03-2013
 *      Author: tolkjen
 */

#include "PreviewModel.h"

PreviewModel::PreviewModel() {
	_name = "Video preview";
}

void PreviewModel::insert(Mat &m) {
	_frame = m;
}

Mat& PreviewModel::resultingFrame() {
	return _frame;
}
