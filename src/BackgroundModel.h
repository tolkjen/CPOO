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

	// ------------------------------------------------------------------------
	// insert
	//
	// With this method you add new frames to the object.
	virtual void insert(Mat &m) = 0;

	// ------------------------------------------------------------------------
	// name
	//
	// Returns a name description of the class object (eg. "Codebook")
	string name() const;

	// ------------------------------------------------------------------------
	// resultingFrame
	//
	// Returns a frame which is the result of algorithms performed by this
	// class (eg. black-white frame showing black background and white
	// foreground).
	virtual Mat resultingFrame() = 0;

protected:
	string _name;
};

#endif /* BACKGROUNDMODEL_H_ */
