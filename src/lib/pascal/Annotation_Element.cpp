#include "Annotation_Element.h"


namespace Annotation
{


Element::Element (
	const std::string &Label, const std::string LabelOriginal,
	int Xmin, int Ymin, int Xmax, int Ymax) :
mLabel(Label), mLabelOriginal(LabelOriginal),
mXmin(Xmin), mYmin(Ymin), mXmax(Xmax), mYmax(Ymax)
{
	if (Xmax < Xmin) { int temp = Xmin; Xmin = Xmax; Xmax = temp; }
	if (Ymax < Ymin) { int temp = Ymin; Ymin = Ymax; Ymax = temp; }
}


Element::~Element (void)
{
	;
}


} // namespace Annotation

