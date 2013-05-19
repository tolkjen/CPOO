/*
 * BackgroundModel.cpp
 *
 *  Created on: 24-03-2013
 *      Author: tolkjen
 */

#include "BackgroundModel.h"

BackgroundModel::~BackgroundModel() {
}

string BackgroundModel::name() const {
	return _name;
}

void BackgroundModel::insert(IplImage *rawImage) {
}

void BackgroundModel::insert(Mat & m) {
}

IplImage* BackgroundModel::resultingCap() {
	return 0;
}
bool BackgroundModel::useCapture() {
	return false;
}
