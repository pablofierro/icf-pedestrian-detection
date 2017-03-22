/********************************************************************************
 Copyright (c) 2013-2014 Hironori Hattori <hattorih@cs.cmu.edu>.
 All rights reserved.
---------------------------------------------------------------------------------
 $Id: $
********************************************************************************/

// "libAnnotation - Element"

// Hironori Hattori, Carnegie Mellon University
// hattorih@cs.cmu.edu


#ifndef __LIB_ANNOTATION_ELEMENT_H__
#define __LIB_ANNOTATION_ELEMENT_H__


#include <string>


namespace Annotation
{


class Element
{
public:
	Element (
		const std::string &Label="N/A", const std::string LabelOriginal="N/A",
		int Xmin=0, int Ymin=0, int Xmax=0, int Ymax=0);

	~Element (void);

	const std::string &getLabel (void) const { return mLabel; }
	std::string &getLabel (void) { return mLabel; }
	const std::string &getLabelOriginal (void) const { return mLabelOriginal; }
	std::string &getLabelOriginal (void) { return mLabelOriginal; }

	const int &getXmin (void) const { return mXmin; }
	int &getXmin (void) { return mXmin; }
	const int &getYmin (void) const { return mYmin; }
	int &getYmin (void) { return mYmin; }

	const int &getXmax (void) const { return mXmax; }
	int &getXmax (void) { return mXmax; }
	const int &getYmax (void) const { return mYmax; }
	int &getYmax (void) { return mYmax; }

private:
	std::string mLabel;
	std::string mLabelOriginal;

	// LeftTop of Bounding Box
	int mXmin;
	int mYmin;

	// RightBottom of Bounding Box
	int mXmax;
	int mYmax;

	// Polygon... not implemented!

	// Pixel Mask... not implemented!
};


} // namespace Annotation


#endif // __LIB_ANNOTATION_ELEMENT_H__

