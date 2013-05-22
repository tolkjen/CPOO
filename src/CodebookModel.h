/*
 * CodebookModel.h
 *
 *  Created on: 14-04-2013
 *      Author: tolkjen
 */

#ifndef CODEBOOKMODEL_H_
#define CODEBOOKMODEL_H_

#include <list>
#include <opencv2/opencv.hpp>
#include "cvaux.h"
#include "cxmisc.h"
#include "highgui.h"

#include "BackgroundModel.h"

using namespace std;
#define CHANNELS 3

class CodebookModel: public BackgroundModel {
	typedef struct ce {
		unsigned char learnHigh[CHANNELS];
		unsigned char learnLow[CHANNELS];
		unsigned char max[CHANNELS];
		unsigned char min[CHANNELS];
		int t_last_update;
		int stale;
	} s_code_element;

	typedef struct code_book {
		s_code_element **cb;
		int numEntries;
		int t;
	} s_codeBook;

	typedef struct codebook_storage {
		s_codeBook *pixel;	
        int t;
	} codeBookStorage;
public:
	CodebookModel();
	virtual ~CodebookModel();

	virtual void insert(IplImage *rawFrame);
	virtual Mat& resultingFrame();
	virtual IplImage* resultingCap();
	virtual bool useCapture();

private:
	double Kfunc(int x);
	inline double KfuncPrecomp(int x);
	void preparePrecomp();

	list<Mat> frames;
	Mat *modelFrame;
	Mat outputFrame;
	int frameBufferSize, treshold;

    void codebookUpdate();
    void codebookClearStale(int staleThresh);
    int codebookDiff();
    //CvBGCodeBookModel* model;
    
    IplImage *ImaskCodeBook, *ImaskCodeBookCC;
    IplImage* yuvImage; //yuvImage is for codebook method
    int c, n, nframes;

    int nframes_to_learn;

    /* parametry do modelu*/
    int t;
    CvSize size;
    uchar cbBounds[3];
    uchar modMin[3];
    uchar modMax[3];
    /* do przechowywania elementow zaalokowanych wprzod */
    CvBGCodeBookElem *tmp_elem;
    CvBGCodeBookElem** cbmap;
};

#endif /* HISTOGRAMMODEL_H_ */
