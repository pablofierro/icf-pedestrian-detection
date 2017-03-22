/********************************************************************************
 Copyright (c) 2013-2014 Hironori Hattori <hattorih@cs.cmu.edu>.
 All rights reserved.
---------------------------------------------------------------------------------
 $Id: $
********************************************************************************/

// "libAnnotation - Data"

// Hironori Hattori, Carnegie Mellon University
// hattorih@cs.cmu.edu


#ifndef __LIB_ANNOTATION_DATA_H__
#define __LIB_ANNOTATION_DATA_H__


#include "Annotation_Element.h"

#include <vector>


namespace Annotation
{


class Data
{
public:
	Data (
		const std::string &ImageName="N/A",
		int ImageSizeX=0, int ImageSizeY=0, int ImageSizeC=0,
		const std::string &DatabaseName="N/A");

	Data (const std::string &filename);

	~Data (void);

	bool read (const std::string &filename);
	bool write (const std::string &filename);

	bool resume (const std::string &filename);
	bool dump (const std::string &filename);

	const Element &operator[] (unsigned int index) const;
	Element &operator[] (unsigned int index);

	const std::string &getImageName (void) const { return mImageName; }
	std::string &getImageName (void) { return mImageName; }

	const int &getImageSizeX (void) const { return mImageSizeX; }
	int &getImageSizeX (void) { return mImageSizeX; }
	const int &getImageSizeY (void) const { return mImageSizeY; }
	int &getImageSizeY (void) { return mImageSizeY; }
	const int &getImageSizeC (void) const { return mImageSizeC; }
	int &getImageSizeC (void) { return mImageSizeC; }

	const std::string &getDatabaseName (void) const { return mDatabaseName; }
	std::string &getDatabaseName (void) { return mDatabaseName; }

	const int &getCoordinatesX (void) const { return mCoordinatesX; }
	int &getCoordinatesX (void) { return mCoordinatesX; }
	const int &getCoordinatesY (void) const { return mCoordinatesY; }
	int &getCoordinatesY (void) { return mCoordinatesY; }

	const std::vector<std::string> &getComments (void) const { return mComments; }
	std::vector<std::string> &getComments (void) { return mComments; }

	const std::vector<Element> &getElements (void) const { return mElements; }
	std::vector<Element> &getElements (void) { return mElements; }

private:
	std::string mImageName;

	// Image size (X x Y x C)
	int mImageSizeX;
	int mImageSizeY;
	int mImageSizeC;

	std::string mDatabaseName;

	// Top left pixel co-ordinates
	int mCoordinatesX;
	int mCoordinatesY;

	std::vector<std::string> mComments;

	// Details for object
	std::vector<Element> mElements;
};


} // namespace Annotation


#endif // __LIB_ANNOTATION_DATA_H__

