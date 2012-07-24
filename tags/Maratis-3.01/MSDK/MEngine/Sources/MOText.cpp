/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MOText.cpp
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
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


#include "../Includes/MEngine.h"


// constructor
MOText::MOText(MFontRef * fontRef):
MObject3d(),
m_align(M_ALIGN_LEFT),
m_size(16),
m_color(1, 1, 1, 1)
{
	setFontRef(fontRef);
}

// copy constructor
MOText::MOText(const MOText & text):
MObject3d(text),
m_text(text.m_text),
m_align(text.m_align),
m_size(text.m_size),
m_fontRef(text.m_fontRef),
m_color(text.m_color),
m_boundingBox(text.m_boundingBox)
{
}

// destructor
MOText::~MOText(void)
{
	MObject3d::clearObject3d();
}

void MOText::setFontRef(MFontRef * fontRef)
{
	m_fontRef = fontRef;
	prepare();
}

MFont * MOText::getFont(void)
{
	if(! m_fontRef)
		return NULL;

	return m_fontRef->getFont();
}

void MOText::setText(const char * text)
{
	m_text.set(text);
	prepare();
}

void MOText::setAlign(M_ALIGN_MODES align)
{
	m_align = align;
	prepare();
}

void MOText::setSize(float size)
{ 
	m_size = size;
	prepare();
}

void MOText::updateLinesOffset(void)
{
	MFont * font = getFont();
	const char * text = m_text.getData();

	float tabSize = m_size*2;
	float fontSize = (float)font->getFontSize();
	float widthFactor = font->getTextureWith() / fontSize;
	float xc = 0;
	float min = 0;
	float max = 0;

	// bounding box
	MVector3 * boxMin = m_boundingBox.getMin();
	MVector3 * boxMax = m_boundingBox.getMax();
	
	// clear lines
	m_linesOffset.clear();

	unsigned int i;
	unsigned int size = strlen(text);
	for(i=0; i<size; i++)
	{
		if(text[i] == '\n') // return
		{
			switch(m_align)
			{
			case M_ALIGN_LEFT:
				m_linesOffset.push_back(0);
				break;
			case M_ALIGN_RIGHT:
				m_linesOffset.push_back(-(max - min));
				break;
			case M_ALIGN_CENTER:
				m_linesOffset.push_back(-(max - min)*0.5f);
				break;
			}

			min = 0;
			max = 0;
			xc = 0;
			continue;
		}

		if(text[i] == '\t') // tab
		{
			int tab = (int)(xc / tabSize) + 1;
			xc = tab*tabSize;
			continue;
		}

		// get character
		unsigned int charCode = (unsigned int)((unsigned char)text[i]);
		MCharacter * character = font->getCharacter(charCode);
		if(! character)
			continue;

		MVector2 scale = character->getScale();
		MVector2 offset = character->getOffset() * m_size;

		float width = scale.x * widthFactor * m_size;

		float charMin = xc + offset.x;
		float charMax = charMin + width;

		if(charMin < min)
			min = charMin;

		if(charMax > max)
			max = charMax;

		//move to next character
		xc += character->getXAdvance() * m_size;

		// last char
		if((i + 1) == size)
		{
			switch(m_align)
			{
			case M_ALIGN_LEFT:
				m_linesOffset.push_back(0);
				break;
			case M_ALIGN_RIGHT:
				m_linesOffset.push_back(-(max - min));
				break;
			case M_ALIGN_CENTER:
				m_linesOffset.push_back(-(max - min)*0.5f);
				break;
			}
		}
	}

	float globalOffset = boxMax->x - boxMin->x;

	switch(m_align)
	{
	case M_ALIGN_RIGHT:
		boxMin->x -= globalOffset;
		boxMax->x -= globalOffset;
		break;
	case M_ALIGN_CENTER:
		boxMin->x -= globalOffset*0.5f;
		boxMax->x -= globalOffset*0.5f;
		break;
	}
}

void MOText::prepare(void)
{
	MFont * font = getFont();
	const char * text = m_text.getData();

	if(! (strlen(text) > 0 && font)){
		m_boundingBox = MBox3d();
		return;
	}

	MVector3 * min = m_boundingBox.getMin();
	MVector3 * max = m_boundingBox.getMax();

	(*min) = (*max) = MVector3(0, 0, 0);

	float tabSize = m_size*2;
	float fontSize = (float)font->getFontSize();
	float widthFactor = font->getTextureWith() / fontSize;
	float heightFactor = font->getTextureHeight() / fontSize;
	float xc = 0, yc = 0;

	unsigned int i;
	unsigned int size = strlen(text);
	for(i=0; i<size; i++)
	{
		if(text[i] == '\n') // return
		{
			yc += m_size;
			xc = 0;
			continue;
		}

		if(text[i] == '\t') // tab
		{
			int tab = (int)(xc / tabSize) + 1;
			xc = tab*tabSize;
			continue;
		}

		// get character
		unsigned int charCode = (unsigned int)((unsigned char)text[i]);
		MCharacter * character = font->getCharacter(charCode);
		if(! character)
			continue;

		MVector2 scale = character->getScale();
		MVector2 offset = character->getOffset() * m_size;

		float width = scale.x * widthFactor * m_size;
		float height = scale.y * heightFactor * m_size;

		MVector2 charMin = MVector2(xc, yc) + offset;
		MVector2 charMax = charMin + MVector2(width, height);

		if(charMin.x < min->x)
			min->x = charMin.x;
		if(charMin.y < min->y)
			min->y = charMin.y;

		if(charMax.x > max->x)
			max->x = charMax.x;
		if(charMax.y > max->y)
			max->y = charMax.y;

		//move to next character
		xc += character->getXAdvance() * m_size;
	}

	updateLinesOffset();
}

void MOText::draw(void)
{
	MFont * font = getFont();
	const char * text = m_text.getData();

	if(! (strlen(text) > 0 && font))
		return;

	if(m_linesOffset.size() == 0)
		return;

	MRenderingContext * render = MEngine().getInstance()->getRenderingContext();


	render->enableBlending();
	render->enableTexture();
	render->disableLighting();

	render->setColor4(m_color);
	render->setBlendingMode(M_BLENDING_ALPHA);

	MVector2 vertices[4];
	MVector2 texCoords[4];

	render->disableNormalArray();
	render->disableColorArray();
	render->enableVertexArray();
	render->enableTexCoordArray();

	render->setVertexPointer(M_FLOAT, 2, vertices);
	render->setTexCoordPointer(M_FLOAT, 2, texCoords);

	render->bindTexture(font->getTextureId());

	unsigned int lineId = 0;
	float lineOffset = m_linesOffset[0];

	float tabSize = m_size*2;
	float fontSize = (float)font->getFontSize();
	float widthFactor = font->getTextureWith() / fontSize;
	float heightFactor = font->getTextureHeight() / fontSize;
	float xc = 0, yc = 0;

	unsigned int i;
	unsigned int size = strlen(text);
	for(i=0; i<size; i++)
	{
		if(text[i] == '\n') // return
		{
			if(((i+1) < size))
			{
				lineId++;
				lineOffset = m_linesOffset[lineId];

				yc += m_size;
				xc = 0;
			}
			continue;
		}

		if(text[i] == '\t') // tab
		{
			int tab = (int)(xc / tabSize) + 1;
			xc = tab*tabSize;
			continue;
		}

		// get character
		unsigned int charCode = (unsigned int)((unsigned char)text[i]);
		MCharacter * character = font->getCharacter(charCode);
		if(! character)
			continue;

		MVector2 pos = character->getPos();
		MVector2 scale = character->getScale();
		MVector2 offset = character->getOffset() * m_size + MVector2(lineOffset, 0);

		float width = scale.x * widthFactor * m_size;
		float height = scale.y * heightFactor * m_size;

		// construct quad
		texCoords[0] = MVector2(pos.x, (pos.y + scale.y));
		vertices[0] = MVector2(xc, (yc + height)) + offset;						
		
		texCoords[1] = MVector2((pos.x + scale.x), (pos.y + scale.y));	
		vertices[1] = MVector2((xc + width), (yc + height)) + offset;							
		
		texCoords[3] = MVector2((pos.x + scale.x), pos.y);	
		vertices[3] = MVector2((xc + width), yc) + offset;							
		
		texCoords[2] = MVector2(pos.x, pos.y);				
		vertices[2] = MVector2(xc, yc) + offset;

		// draw quad
		render->drawArray(M_PRIMITIVE_TRIANGLE_STRIP, 0, 4);

		//move to next character
		xc += character->getXAdvance() * m_size;
	}
}

void MOText::updateVisibility(MOCamera * camera)
{
	MFrustum * frustum = camera->getFrustum();

	MVector3 * min = m_boundingBox.getMin();
	MVector3 * max = m_boundingBox.getMax();

	MVector3 points[8] = {
		getTransformedVector(MVector3(min->x, min->y, min->z)),
		getTransformedVector(MVector3(min->x, max->y, min->z)),
		getTransformedVector(MVector3(max->x, max->y, min->z)),
		getTransformedVector(MVector3(max->x, min->y, min->z)),
		getTransformedVector(MVector3(min->x, min->y, max->z)),
		getTransformedVector(MVector3(min->x, max->y, max->z)),
		getTransformedVector(MVector3(max->x, max->y, max->z)),
		getTransformedVector(MVector3(max->x, min->y, max->z))
	};

	// is box in frustum
	setVisible(frustum->isVolumePointsVisible(points, 8));
}