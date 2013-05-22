/*
 * CodebookModel.cpp
 *
 *  Created on: 14-04-2013
 *      Author: tolkjen
 */

#include <cmath>
#include <cstdio>
#include "CodebookModel.h"

CodebookModel::CodebookModel() {
    c = 0;
    n = 0;
    nframes = 0;
	yuvImage = 0;
	nframesToLearnBG = 100;
	ImaskCodeBook = 0;
	ImaskCodeBookCC = 0;

    model = cvCreateBGCodeBookModel();
    
    //Set color thresholds to default values
    model->modMin[0] = 3;
    model->modMin[1] = model->modMin[2] = 3;
    model->modMax[0] = 10;
    model->modMax[1] = model->modMax[2] = 10;
    model->cbBounds[0] = model->cbBounds[1] = model->cbBounds[2] = 30;

    tmp_elem = 0;

    _name = "Codebook";
}

bool CodebookModel::useCapture() {
	return true;
}

CodebookModel::~CodebookModel() {
}

void CodebookModel::insert(IplImage *rawFrame) {
      //First time:

	nframes++;

        if( nframes == 1 )
        {
            yuvImage = cvCloneImage(rawFrame);
            ImaskCodeBook = cvCreateImage( cvGetSize(yuvImage), IPL_DEPTH_8U, 1 );
            ImaskCodeBookCC = cvCreateImage( cvGetSize(yuvImage), IPL_DEPTH_8U, 1 );
            cvSet(ImaskCodeBook,cvScalar(255));
        }
	    cvCvtColor( rawFrame, yuvImage, CV_BGR2YCrCb );
        //  yuvImage = cvCloneImage(rawFrame);
		if( nframes-1 < nframesToLearnBG  )
            codebookUpdate();

        if( nframes-1 == nframesToLearnBG  )
            codebookClearStale(model->t/2 );

		if( nframes-1 >= nframesToLearnBG  ) {
             codebookDiff();
             cvCopy(ImaskCodeBook,ImaskCodeBookCC);
             cvSegmentFGMask( ImaskCodeBookCC,0,10 );
		}

		return;
}

Mat& CodebookModel::resultingFrame() {
	Mat m;
	return m;
}

IplImage* CodebookModel::resultingCap() {
    //return ImaskCodeBook;
	return ImaskCodeBookCC;
}

void CodebookModel::codebookUpdate()
{
    CvMat stub, *image = cvGetMat( yuvImage, &stub );
    int x, y, T;
    int nblocks;

    if( image->cols != model->size.width || image->rows != model->size.height )
    {
        model->freeList = 0;
        cvFree( &model->cbmap );
        int bufSz = image->cols*image->rows*sizeof(model->cbmap[0]);
        model->cbmap = (CvBGCodeBookElem**)cvAlloc(bufSz);
        memset( model->cbmap, 0, bufSz );
        model->size = cvSize(image->cols, image->rows);
    }

    T = ++model->t;
    nblocks = 1024;

    for( y = 0; y < image->rows; y++ )
    {
        const uchar* p = image->data.ptr + image->step*y;
        CvBGCodeBookElem** cb = model->cbmap + image->cols*y;

        for( x = 0; x < image->cols; x++, p += 3, cb++ )
        {
            CvBGCodeBookElem *e, *found = 0;
            int high[3], low[3];
            int negRun;

            //p0 = p[0]; p1 = p[1]; p2 = p[2];
            for (int i=0; i<3; i++) {
                low[i] = p[i] - model->cbBounds[i]; if (low[i] < 0) low[i] = 0;
                high[i] = p[i] + model->cbBounds[i]; if (high[i] < 0) high[i] = 0;
            }

            for( e = *cb; e != 0; e = e->next )
            {
                if( e->learnMin[0] <= p[0] && p[0] <= e->learnMax[0] &&
                    e->learnMin[1] <= p[1] && p[1] <= e->learnMax[1] &&
                    e->learnMin[2] <= p[2] && p[2] <= e->learnMax[2] )
                {
                    e->tLastUpdate = T;
                    for (int i = 0; i<3; i++) {
                        e->boxMin[i] = MIN(e->boxMin[i], p[i]);
                        e->boxMax[i] = MAX(e->boxMax[i], p[i]);
                    }

                    // no need to use SAT_8U for updated learnMin[i] & learnMax[i] here,
                    // as the bounding li & hi are already within 0..255.
                    for (int i = 0; i<3; i++) {
                        if( e->learnMin[i] > low[i] ) e->learnMin[i]--;
                        if( e->learnMax[i] < high[i] ) e->learnMax[i]++;
                    }

                    found = e;
                    break;
                }
                negRun = T - e->tLastUpdate;
                e->stale = MAX( e->stale, negRun );
            }

            for( ; e != 0; e = e->next )
            {
                negRun = T - e->tLastUpdate;
                e->stale = MAX( e->stale, negRun );
            }

            if( !found )
            {
                if( !tmp_elem )
                {
                    /* alokowanie wprzod: lepiej wiecej na raz niz osobno, bo szybciej */
                    tmp_elem = (CvBGCodeBookElem*)malloc(sizeof(*tmp_elem) * nblocks);
                    for(int i = 0; i < nblocks-1; i++ )
                        tmp_elem[i].next = &tmp_elem[i+1];
                    tmp_elem[nblocks-1].next = 0;
                }
                e = tmp_elem;
                tmp_elem = tmp_elem->next;

                for (int i = 0; i<3; i++) {
                    e->learnMin[i] = low[i]; e->learnMax[i] = high[i];
                    e->boxMin[i] = e->boxMax[i] = p[i];
                }
                e->tLastUpdate = T;
                e->stale = 0;
                e->next = *cb;
                *cb = e;
            }
        }
    }
}


void CodebookModel::codebookClearStale(int staleThresh)
{
    int x, y, T;
    T = model->t;

    for( y = 0; y < model->size.height; y++ )
    {
        CvBGCodeBookElem** cb = model->cbmap + model->size.width*y;

        for( x = 0; x < model->size.width; x++, cb++ )
        {
            CvBGCodeBookElem *e, first, *prev = &first;

            for( first.next = e = *cb; e != 0; e = prev->next )
            {
                if( e->stale > staleThresh )
                {
                    /* nie kasujemy, tylko wrzucamy na liste wolnych */
                    prev->next = e->next;
                    e->next = tmp_elem;
                    tmp_elem = e;
                }
                else
                {
                    e->stale = 0;
                    e->tLastUpdate = T;
                    prev = e;
                }
            }

            *cb = first.next;
        }
    }

}


int CodebookModel::codebookDiff()
{
    int maskCount = -1;

    CvMat stub, *image = cvGetMat( yuvImage, &stub );
    CvMat mstub, *mask = cvGetMat( ImaskCodeBook, &mstub );
    int x, y;

    maskCount = image->cols * image->rows;
    for( y = 0; y < image->rows; y++ )
    {
        const uchar* p = image->data.ptr + image->step*y;
        uchar* m = mask->data.ptr + mask->step*y;
        CvBGCodeBookElem** cb = model->cbmap + image->cols*y;

        for( x = 0; x < image->width; x++, p += 3, cb++ )
        {
            CvBGCodeBookElem *e;
            int low[3], high[3];
            for(int i=0; i<3; i++) {
                low[i] = p[i] + model->modMin[i];
                high[i] = p[i] - model->modMax[i];
            }

            m[x] = (uchar)255;

            for( e = *cb; e != 0; e = e->next )
            {
                if( e->boxMin[0] <= low[0] && high[0] <= e->boxMax[0] &&
                    e->boxMin[1] <= low[1] && high[1] <= e->boxMax[1] &&
                    e->boxMin[2] <= low[2] && high[2] <= e->boxMax[2] )
                {
                    m[x] = 0;
                    maskCount--;
                    break;
                }
            }
        }
    }

    return maskCount;
}
