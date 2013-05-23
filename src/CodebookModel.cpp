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

    nframes_to_learn = 100;

    c, n, nframes = 0;
    yuvImage = 0;
    ImaskCodeBook = 0;
	ImaskCodeBookCC = 0;
    
    //Set color thresholds to default values
    modMin[0] = modMin[1] = modMin[2] = 3;
    modMax[0] = modMax[1] = modMax[2] = 10;
    cbBounds[0] = cbBounds[1] = cbBounds[2] = 10;
    t=0;

    tmp_elem = 0;
    tmp_elems_free = 0;
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
        if( nframes-1 < nframes_to_learn  ) {
            cout << "t: " << t << endl;
            codebookUpdate();
        }

        if( nframes-1 == nframes_to_learn  ) {
            cout << "t: " << t << endl;
            codebookClearStale(t/2 );
        }


        if( nframes-1 >= nframes_to_learn ) {
               codebookDiff();
             cvCopy(ImaskCodeBook,ImaskCodeBookCC);
             cvSegmentFGMask( ImaskCodeBookCC,0.1,10 );
		}

		return;
}

Mat& CodebookModel::resultingFrame() {
}

IplImage* CodebookModel::resultingCap() {
  //  return ImaskCodeBook;
	return ImaskCodeBookCC;
}

CodebookModel::codebook_element *CodebookModel::new_element() {
    int nblocks = 1024;

    if( !tmp_elem )
    {
        //printf("free_elems: %d\n", tmp_elems_free);
        /* alokowanie wprzod: lepiej wiecej na raz niz osobno, bo szybciej */
        tmp_elem = (codebook_element*)malloc(sizeof(*tmp_elem) * nblocks);
        tmp_elems_free=nblocks;
        for(int i = 0; i < nblocks-1; i++ )
            tmp_elem[i].next = &tmp_elem[i+1];
        tmp_elem[nblocks-1].next = 0;
    }
    tmp_elems_free--;
    codebook_element* ret = tmp_elem;
    tmp_elem = tmp_elem->next;
    return ret;
}

void CodebookModel::remove_element(codebook_element* elem) {
    //printf("remove element \n");
    elem->next = tmp_elem;
    tmp_elem = elem;

    tmp_elems_free++;
    if(tmp_elems_free % 1024 == 0) printf("freed elements: %d\n", tmp_elems_free);
}

void CodebookModel::codebookUpdate()
{
    CvMat stub, *image = cvGetMat( yuvImage, &stub );
    int i, x, y, T;


    if( image->cols != size.width || image->rows != size.height )
    {
        //cvFree( &cbmap );
        int bufSz = image->cols*image->rows*sizeof(cbmap[0]);
        cbmap = (codebook_element**)cvAlloc(bufSz);
        memset( cbmap, 0, bufSz );
        size = cvSize(image->cols, image->rows);
    }

    T = ++t;

    for( y = 0; y < image->rows; y++ )
    {
        const uchar* p = image->data.ptr + image->step*y;
        codebook_element** cb = cbmap + image->cols*y;

        for( x = 0; x < image->cols; x++, p += 3, cb++ )
        {
            codebook_element *elem, *found = 0;
            int high[3], low[3];
            int negRun;

            //p0 = p[0]; p1 = p[1]; p2 = p[2];
            for (int i=0; i<3; i++) {
                low[i] = p[i] - cbBounds[i]; if (low[i] < 0) low[i] = 0;
                high[i] = p[i] + cbBounds[i]; if (high[i] < 0) high[i] = 0;
            }

            for( elem = *cb; elem != 0; elem = elem->next )
            {
                if( elem->learnMin[0] <= p[0] && p[0] <= elem->learnMax[0] &&
                    elem->learnMin[1] <= p[1] && p[1] <= elem->learnMax[1] &&
                    elem->learnMin[2] <= p[2] && p[2] <= elem->learnMax[2] )
                {
                    elem->tLastUpdate = T;
                    for (int i = 0; i<3; i++) {
                        elem->boxMin[i] = MIN(elem->boxMin[i], p[i]);
                        elem->boxMax[i] = MAX(elem->boxMax[i], p[i]);
                    }

                    // no need to use SAT_8U for updated learnMin[i] & learnMax[i] here,
                    // as the bounding li & hi are already within 0..255.
                    for (int i = 0; i<3; i++) {
                        if( elem->learnMin[i] > low[i] ) elem->learnMin[i]--;
                        if( elem->learnMax[i] < high[i] ) elem->learnMax[i]++;
                    }

                    found = elem;
                    break;
                }
                negRun = T - elem->tLastUpdate;
                elem->stale = MAX( elem->stale, negRun );
            }

            for( ; elem != 0; elem = elem->next )
            {
                negRun = T - elem->tLastUpdate;
                elem->stale = MAX( elem->stale, negRun );
            }

            if( !found )
            {
                elem = new_element();

                for (int i = 0; i<3; i++) {
                    elem->learnMin[i] = low[i]; elem->learnMax[i] = high[i];
                    elem->boxMin[i] = elem->boxMax[i] = p[i];
                }
                elem->tLastUpdate = T;
                elem->stale = 0;
                elem->next = *cb;
                *cb = elem;
            }
        }
    }
}


void CodebookModel::codebookClearStale(int staleThresh)
{
    int x, y, T;
    T = t;
    cout <<"Codebook clear stale" <<endl;
    for( y = 0; y < size.height; y++ )
    {
        codebook_element** cb = cbmap + size.width*y;

        for( x = 0; x < size.width; x++, cb++ )
        {
            codebook_element *elem, first, *prev = &first;

            for( first.next = elem = *cb; elem != 0; elem = prev->next )
            {
                if( elem->stale > staleThresh )
                {
                    /* nie kasujemy, tylko wrzucamy na liste wolnych */
                    prev->next = elem->next;
                    remove_element(elem);

                }
                else
                {
                    //cout << "stale = 0; stale: " << elem->stale << "staleThresh" << staleThresh << " X: "<<x << " Y: " << y << endl;
                    elem->stale = 0;
                    elem->tLastUpdate = T;
                    prev = elem;
                }
            }

            *cb = first.next;
        }
    }
    cout << "codebook clear X: "<<x << " Y: " << y << endl;

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
        codebook_element** cb = cbmap + image->cols*y;

        for( x = 0; x < image->width; x++, p += 3, cb++ )
        {
            codebook_element *elem;
            int low[3], high[3];
            for(int i=0; i<3; i++) {
                low[i] = p[i] + modMin[i];
                high[i] = p[i] - modMax[i];
            }

            m[x] = (uchar)255;

            for( elem = *cb; elem != 0; elem = elem->next )
            {
                int i, correct =0;
                for (i=0 ; i< CHANNELS ; i++) {
                    if (elem->boxMin[i] <= low[i] && high[i] <= elem->boxMax[i] ) correct++;


                }
                if (correct == CHANNELS)
                {
                    m[x] = 0;
                    maskCount--;
                    break;
                }

                continue;
                if( elem->boxMin[0] <= low[0] && high[0] <= elem->boxMax[0] &&
                    elem->boxMin[1] <= low[1] && high[1] <= elem->boxMax[1] &&
                    elem->boxMin[2] <= low[2] && high[2] <= elem->boxMax[2] )
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
