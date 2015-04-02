/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis Image tools
// Color.cpp
//
// Color tools
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2013 Anael Seghezzi <www.maratis3d.com>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

#include <MCore.h>
#include "Common.h"
#include "Color.h"


bool contrast(MImage * image, float factor)
{
	if(! isImageValid(image))
		return false;

	unsigned int i, size = image->getSize();
	unsigned int components = image->getComponents();
	
	if(image->getDataType() == M_FLOAT)
	{
		float * imageData = (float *)image->getData();
		if(components == 4)
		{
			for(i=0; i<size; i+=4)
			{
				imageData[0] = imageData[0] + factor*(imageData[0] - 0.5f);
				imageData[1] = imageData[1] + factor*(imageData[1] - 0.5f);
				imageData[2] = imageData[2] + factor*(imageData[2] - 0.5f);
				imageData+=4;
			}
		}
		else
		{
			for(i=0; i<size; i++)
				imageData[i] = imageData[i] + factor*(imageData[i] - 0.5f);
		}
		return true;
	}
	
	return false;
}

bool clamp(MImage * image, float min, float max)
{
	if(! isImageValid(image))
		return false;

	unsigned int i, size = image->getSize();
	if(image->getDataType() == M_FLOAT)
	{
		float * imageData = (float *)image->getData();
        for(i=0; i<size; i++)
            imageData[i] = CLAMP(imageData[i], min, max);
		return true;
	}
	
	return false;
}

bool normalize(MImage * image)
{
	if(! isImageValid(image))
		return false;

	unsigned int i, size = image->getSize();
	if(image->getDataType() == M_FLOAT)
	{
		float min, max;
		float * imageData = (float *)image->getData();
		
		min = max = imageData[0];
		
        for(i=0; i<size; i++)
		{
            min = MIN(min, imageData[i]);
			max = MAX(max, imageData[i]);
		}
		
		for(i=0; i<size; i++)
			imageData[i] = (imageData[i]-min)/(max-min);
			
		return true;
	}
	
	return false;
}

bool product(MImage * image, float factor)
{
	if(! isImageValid(image))
		return false;

	unsigned int i, size = image->getSize();
	if(image->getDataType() == M_FLOAT)
	{
		float * imageData = (float *)image->getData();
        for(i=0; i<size; i++)
            imageData[i] *= factor;
		return true;
	}
	
	return false;
}

bool product(MImage * image, MImage * image2)
{
	if(! (isImageValid(image) && isImageValid(image)))
		return false;

	unsigned int i, size = image->getSize();
	unsigned int c1 = image->getComponents();
	unsigned int c2 = image2->getComponents();
	
	if(image->getDataType() == M_FLOAT && image2->getDataType() == M_FLOAT)
	{
		float * imageData = (float *)image->getData();
		float * image2Data = (float *)image2->getData();
		
		if(c1 == c2)
		{
			for(i=0; i<size; i++)
				imageData[i] *= image2Data[i];
			return true;
		}
		else if(c2 == 1)
		{
			for(i=0; i<image2->getSize(); i++)
			{
				for(int c=0; c<c1; c++)
					imageData[i*c1+c] *= image2Data[i];
			}
			return true;
		}
	}
	
	return false;
}

bool addition(MImage * image, float factor)
{
	if(! isImageValid(image))
		return false;

	unsigned int i, size = image->getSize();
	unsigned int components = image->getComponents();
	
	if(image->getDataType() == M_FLOAT)
	{
		float * imageData = (float *)image->getData();
		if(components == 4)
		{
			for(i=0; i<size; i+=4)
			{
				imageData[0] += factor;
				imageData[1] += factor;
				imageData[2] += factor;
				imageData+=4;
			}
		}
		else
		{
			for(i=0; i<size; i++)
				imageData[i] += factor;
		}
		return true;
	}
	
	return false;
}

bool addition(MImage * image, MImage * image2)
{
	if(! (isImageValid(image) && isImageValid(image)))
		return false;

	if(! (image->getWidth() == image2->getWidth() && image->getHeight() == image2->getHeight()))
		return false;

	unsigned int i, size = image->getSize();
	if(image->getDataType() == M_FLOAT && image->getDataType() == M_FLOAT)
	{
		float * imageData = (float *)image->getData();
		float * image2Data = (float *)image2->getData();
        for(i=0; i<size; i++)
            imageData[i] += image2Data[i];
		return true;
	}
	
	return false;
}

bool substraction(MImage * image, MImage * image2)
{
	if(! (isImageValid(image) && isImageValid(image)))
		return false;

	unsigned int i, size = image->getSize();
	if(image->getDataType() == M_FLOAT && image->getDataType() == M_FLOAT)
	{
		float * imageData = (float *)image->getData();
		float * image2Data = (float *)image2->getData();
        for(i=0; i<size; i++)
            imageData[i] -= image2Data[i];
		return true;
	}
	
	return false;
}

bool blendOver(MImage * image, MImage * image2)
{
	if(! (isImageValid(image) && isImageValid(image2)))
		return false;
	
	if(image->getDataType() == M_FLOAT && image2->getDataType() == M_FLOAT
	&& image->getComponents() == 4 && image2->getComponents() == 4
	&& image->getSize() == image2->getSize())
	{
		unsigned int i, size = image->getSize();
		
		float * imageData = (float *)image->getData();
		float * image2Data = (float *)image2->getData();
        for(i=0; i<size; i+=4)
		{
			float alpha = CLAMP(image2Data[3], 0, 1);
			float ialpha = 1.0f-alpha;
			
			imageData[0] = imageData[0]*ialpha + image2Data[0];
			imageData[1] = imageData[1]*ialpha + image2Data[1];
			imageData[2] = imageData[2]*ialpha + image2Data[2];
			imageData[3] += alpha;
			
			imageData+=4;
			image2Data+=4;
		}
		
		return true;
	}
	
	return false;
}

bool negative(MImage * image)
{
	if(! isImageValid(image))
		return false;

	unsigned int i, size = image->getSize();
	unsigned int components = image->getComponents();
	if(image->getDataType() == M_FLOAT)
	{
		float * imageData = (float *)image->getData();
		if(components == 4)
		{
			for(i=0; i<size; i+=4)
			{
				imageData[0] = 1 - imageData[0];
				imageData[1] = 1 - imageData[1];
				imageData[2] = 1 - imageData[2];
				imageData+=4;
			}
		}
		else
		{
			for(i=0; i<size; i++)
				imageData[i] = 1 - imageData[i];
		}
		
		return true;
	}
	
	return false;
}

bool premultiply(MImage * image)
{
	if(! isImageValid(image))
		return false;
		
	if(image->getComponents() != 4)
		return false;
		
	if(image->getDataType() == M_FLOAT)
	{
		float * pixel = (float *)image->getData();
		unsigned int p, pSize = image->getSize();
		for(p=0; p<pSize; p+=4)
		{
			pixel[0] *= pixel[3];
			pixel[1] *= pixel[3];
			pixel[2] *= pixel[3];
			pixel+=4;
		}
		
		return true;
	}
	
	return false;
}

bool unpremultiply(MImage * image)
{
	if(! isImageValid(image))
		return false;
		
	if(image->getComponents() != 4)
		return false;
		
	if(image->getDataType() == M_FLOAT)
	{
		float * pixel = (float *)image->getData();
		unsigned int p, pSize = image->getSize();
		for(p=0; p<pSize; p+=4)
		{
			if(pixel[3] > 0)
			{
				float div = 1.0f/pixel[3];
				pixel[0] *= div;
				pixel[1] *= div;
				pixel[2] *= div;
			}
		
			pixel+=4;
		}
		
		return true;
	}
	
	return false;
}

bool threshold(MImage * image, float threshold)
{
	if(! isImageValid(image))
		return false;

	unsigned int i, size = image->getSize();
	if(image->getDataType() == M_FLOAT)
	{
		float * imageData = (float *)image->getData();
        for(i=0; i<size; i++)
		{
			if(imageData[i] < threshold)
				imageData[i] = 0;
			else
				imageData[i] = 1;
		}
		return true;
	}
	
	return false;
}

bool gammaCorrection(MImage * image, float gamma)
{
	if(! isImageValid(image))
		return false;
		
	if(image->getDataType() != M_FLOAT)
		return false;

	float * imageData = (float *)image->getData();
	unsigned int i, size = image->getSize();
	unsigned int components = image->getComponents();

	if(components == 4)
	{
		for(i=0; i<size; i+=4)
		{
			imageData[0] = powf(imageData[0], gamma);
			imageData[1] = powf(imageData[1], gamma);
			imageData[2] = powf(imageData[2], gamma);
			imageData+=4;
		}
	}
	else
	{
		for(i=0; i<size; i++)
			imageData[i] = powf(imageData[i], gamma);
	}
	
	return true;
}
