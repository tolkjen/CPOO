/*
 * BackgroundModel.h
 *
 *  Created on: 24-03-2013
 *      Author: tolkjen
 */

#ifndef BACKGROUNDMODEL_H_
#define BACKGROUNDMODEL_H_

#include <opencv2\opencv.hpp>
#include <string>

using namespace cv;
using namespace std;

class BackgroundModel {
public:
	virtual ~BackgroundModel();

	virtual void insert(Mat &m) = 0;
	string name() const;
	virtual Mat resultingFrame() = 0;

protected:
	string _name;
};

#endif /* BACKGROUNDMODEL_H_ */
