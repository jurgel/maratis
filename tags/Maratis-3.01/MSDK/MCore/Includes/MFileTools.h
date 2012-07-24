/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// FileTools.h
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


#ifndef _FILE_TOOLS_H
#define _FILE_TOOLS_H

M_CORE_EXPORT bool copyFile(const char * inFilename, const char * outFilename);
M_CORE_EXPORT bool createDirectory(const char * filename);
M_CORE_EXPORT bool isDirectory(const char * filename);
M_CORE_EXPORT bool isEmptyDirectory(const char * filename);
M_CORE_EXPORT bool clearDirectory(const char * filename);
M_CORE_EXPORT bool removeDirectory(const char * filename);
M_CORE_EXPORT bool isFileExist(const char * filename);
M_CORE_EXPORT bool copyDirectory(const char * inFilename, const char * outFilename);
M_CORE_EXPORT bool readDirectory(const char * filename, vector<string> * files);

#endif
