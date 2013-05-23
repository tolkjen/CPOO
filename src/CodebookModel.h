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
    typedef struct codebook_element {
        struct codebook_element* next;
        int t_last_update;
        int stale;
        uchar boxMin[3];
        uchar boxMax[3];
        uchar learnMin[3];
        uchar learnMax[3];
    } codebook_element;
public:
	CodebookModel();
	virtual ~CodebookModel();

	virtual void insert(IplImage *rawFrame);
	virtual Mat& resultingFrame();
	virtual IplImage* resultingCap();
	virtual bool useCapture();

private:
    codebook_element * new_element();
    void remove_element(codebook_element *elem);

    void codebookUpdate();
    void codebookClearStale(int staleThresh);
    int codebookDiff();
    
    IplImage *ImaskCodeBook, *ImaskCodeBookCC;
    IplImage* yuvImage;
    int c, nframes;

    int nframes_to_learn;

    /* parametry do modelu*/
    int t;
    CvSize size;
    uchar cbBounds[3];
    uchar modMin[3];
    uchar modMax[3];
    /* do przechowywania elementow zaalokowanych wprzod */
    codebook_element *tmp_elem;
    int tmp_elems_free;
    codebook_element** cbmap;
};

#endif /* HISTOGRAMMODEL_H_ */
