#include "Annotation_Data.h"

#include <assert.h>
#include <iostream>
#include <fstream>
#include <sstream>


#pragma warning ( disable : 4996 )


namespace Annotation
{


Data::Data (
	const std::string &ImageName,
	int ImageSizeX, int ImageSizeY, int ImageSizeC,
	const std::string &DatabaseName) :
mImageName(ImageName),
mImageSizeX(ImageSizeX), mImageSizeY(ImageSizeY), mImageSizeC(ImageSizeC),
mDatabaseName(DatabaseName),
mCoordinatesX(0), mCoordinatesY(0)
{
	;
}


Data::Data (const std::string &filename) :
mImageName("N/A"),
mImageSizeX(0), mImageSizeY(0), mImageSizeC(0),
mDatabaseName("N/A"),
mCoordinatesX(0), mCoordinatesY(0)
{
	read (filename);
}


Data::~Data (void)
{
	;
}


bool Data::read (const std::string &filename)
{
	std::ifstream in(filename.c_str());
	
	if (!in)
	{
		std::cout << "Could not open " << filename << " for reading.\n" << std::endl;
		return false;
	}
	else
	{
		std::string line;
		std::getline(in, line);

		if (line != "# PASCAL Annotation Version 1.00")
		{
			in.close();
			return false;
		}
		else
		{
			while (!in.eof())
			{
				std::getline(in, line);

				if (line == "") continue;

				if (line == "# This file was created by libAnnotation.") continue;
				if (line == "# https://github.com/hattorih/libAnnotation") continue;
				
				if (line == "# Note that there might be other objects in the image") continue;
				if (line == "# for which ground truth data has not been provided.") continue;

				{
					const std::string keyword = "Image filename : ";
					if (line.substr(0, keyword.size()) == keyword)
					{
						const std::string sub = line.substr(keyword.size());
						mImageName = sub.substr(1, sub.size()-2);

						continue;
					}
				}

				{
					const std::string keyword = "Database : ";
					if (line.substr(0, keyword.size()) == keyword)
					{
						const std::string sub = line.substr(keyword.size());
						mDatabaseName = sub.substr(1, sub.size()-2);

						continue;
					}
				}

				{
					const std::string keyword = "Image size (X x Y x C) : ";
					if (line.substr(0, keyword.size()) == keyword)
					{
						std::istringstream stream (line.substr(keyword.size()));

						std::string delim1, delim2;
						stream >> mImageSizeX >> delim1 >> mImageSizeY >> delim2 >> mImageSizeC;

						continue;
					}
				}

				{
					const std::string keyword = "# Top left pixel co-ordinates : ";
					if (line.substr(0, keyword.size()) == keyword)
					{
						std::istringstream stream (line.substr(keyword.size()));

						char bracket_b, comma, bracket_e;
						stream >> bracket_b >> mCoordinatesX >> comma >> mCoordinatesY >> bracket_e;

						continue;
					}
				}

				{
					const std::string keyword = "# Details for object ";
					if (line.substr(0, keyword.size()) == keyword)
					{
						std::istringstream stream (line.substr(keyword.size()));

						unsigned int index;
						stream >> index;

						if (0<index && mElements.size() < index)
						{
							mElements.resize(index);
						}

						continue;
					}
				}

				{
					const std::string keyword = "Original label for object ";
					if (line.substr(0, keyword.size()) == keyword)
					{
						std::istringstream stream (line.substr(keyword.size()));

						unsigned int index;
						stream >> index;

						if (0<index && index <= mElements.size())
						{
							Element &e = mElements[index-1];

							std::string label, delim, labeloriginal;
							stream >> label >> delim >> labeloriginal;

							e.getLabel() = label.substr(1, label.size()-2);
							e.getLabelOriginal() = labeloriginal.substr(1, labeloriginal.size()-2);
						}

						continue;
					}
				}

				{
					const std::string keyword = "Bounding box for object ";
					if (line.substr(0, keyword.size()) == keyword)
					{
						std::istringstream stream (line.substr(keyword.size()));

						unsigned int index;
						stream >> index;

						if (0<index && index <= mElements.size())
						{
							Element &e = mElements[index-1];

							std::string label, xmin, ymin, delim1, xmax, ymax, delim2;
							stream >> label >> xmin >> ymin >> delim1 >> xmax >> ymax >> delim2;

							char bracket_b, comma, bracket_e, delim;
							stream >> bracket_b >> e.getXmin() >> comma >> e.getYmin() >> bracket_e >> delim;
							stream >> bracket_b >> e.getXmax() >> comma >> e.getYmax() >> bracket_e;
						}

						continue;
					}
				}

				{
					const std::string keyword = "# ";
					if (line.substr(0, keyword.size()) == keyword)
					{
						mComments.push_back(line.substr(keyword.size()));

						continue;
					}
				}
			}
			
			in.close();
			return true;
		}
	}
}


bool Data::write (const std::string &filename)
{
	std::ofstream out(filename.c_str(), std::ios::binary);
	
	if (!out)
	{
		std::cout << "Could not open " << filename << " for writing.\n" << std::endl;
		return false;
	}
	else
	{
		out << "# PASCAL Annotation Version 1.00" << std::endl;
		out << std::endl;

		out << "Image filename : \"" << mImageName << "\"" << std::endl;
		out << "Image size (X x Y x C) : ";
		out << mImageSizeX << " x " << mImageSizeY << " x " << mImageSizeC << std::endl,
		out << "Database : \"" << mDatabaseName << "\"" << std::endl;
		out << "Objects with ground truth : " << mElements.size() << " { ";

		for (unsigned int i=0; i<mElements.size(); i++)
			out << "\"" << (*this)[i].getLabel() << "\" ";

		out << "}" << std::endl;

		out << std::endl;
		out << "# This file was created by libAnnotation." << std::endl;
		out << "# https://github.com/hattorih/libAnnotation" << std::endl;
		out << std::endl;

		for (unsigned int i=0; i<mComments.size(); i++)
		{
			out << "# " << mComments[i] << std::endl;
			if (i == mComments.size() - 1) out << std::endl;
		}

		out << "# Note that there might be other objects in the image" << std::endl;
		out << "# for which ground truth data has not been provided." << std::endl;
		out << std::endl;

		out << "# Top left pixel co-ordinates : ";
		out << "(" << mCoordinatesX << ", " << mCoordinatesY << ")" << std::endl;

		for (unsigned int i=0; i<mElements.size(); i++)
		{
			out << std::endl;

			out << "# Details for object " << i+1 << " (\"" << (*this)[i].getLabel() << "\")" << std::endl;

			out << "Original label for object " << i+1 << " \"" << (*this)[i].getLabel() << "\" : \"";
			out << (*this)[i].getLabelOriginal() << "\"" << std::endl;

			out << "Bounding box for object " << i+1 << " \"" << (*this)[i].getLabel() << "\" ";
			out << "(Xmin, Ymin) - (Xmax, Ymax) : ";
			out << "(" << (*this)[i].getXmin() << ", " << (*this)[i].getYmin() << ") - ";
			out << "(" << (*this)[i].getXmax() << ", " << (*this)[i].getYmax() << ")" << std::endl;

			// Polygon... not implemented!

			// Pixel Mask... not implemented!
		}

		out.close();
		return true;
	}
}


bool Data::resume (const std::string &filename)
{
	// Binary File Format
	std::ifstream in(filename.c_str(), std::ios::binary);

	if (!in)
	{
		std::cout << "Could not open " << filename << " for resuming.\n" << std::endl;
		return false;
	}
	else
	{
		// TODO

		in.close();
		return false;
	}
}


bool Data::dump (const std::string &filename)
{
	// Binary File Format
	std::ofstream out(filename.c_str(), std::ios::binary);
	
	if (!out)
	{
		std::cout << "Could not open " << filename << " for dumping.\n" << std::endl;
		return false;
	}
	else
	{
		// TODO

		out.close();
		return false;
	}
}


const Element &Data::operator[] (unsigned int index) const
{
	assert (index < mElements.size());
	return mElements[index];
}


Element &Data::operator[] (unsigned int index)
{
	assert (index < mElements.size());
	return mElements[index];
}


} // namespace Annotation

