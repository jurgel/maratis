/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MTextureRef.h
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


#ifndef _M_TEXTURE_REF_H
#define _M_TEXTURE_REF_H


// Texture ref
class M_ENGINE_EXPORT MTextureRef : public MDataRef
{
private:

	// data
	bool m_mipmap;
	unsigned int m_components;
	unsigned int m_textureId;

public:

	// constructor / destructor
	MTextureRef(unsigned int textureId, const char * filename, bool mipmap);
	~MTextureRef(void);

	// clear / destroy
	void clear(void);
	void destroy(void);

	// get new
	static MTextureRef * getNew(unsigned int textureId, const char * filename, bool mipmap);

public:

	// update
	void update(void);

	// type
	int getType(void){ return M_REF_TEXTURE; }

	// data
	inline bool isMipmapEnabled(void){ return m_mipmap; }
	inline unsigned int getComponents(void){ return m_components; }
	inline void setTextureId(unsigned int textureId){ m_textureId = textureId; }
	inline unsigned int getTextureId(void){ return m_textureId; }
};

#endif