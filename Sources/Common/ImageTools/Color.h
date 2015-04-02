/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis Image tools
// Color.h
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

#ifndef _M_IT_COLOR_H
#define _M_IT_COLOR_H

// color filters (float images only)
bool contrast(MImage * image, float factor);
bool product(MImage * image, float factor);
bool product(MImage * image, MImage * image2);
bool addition(MImage * image, float factor);
bool addition(MImage * image, MImage * image2);
bool substraction(MImage * image, MImage * image2);
bool blendOver(MImage * image, MImage * image2); // blend image2 over image1 using premultiplied alpha
bool negative(MImage * image);
bool premultiply(MImage * image);
bool unpremultiply(MImage * image);
bool threshold(MImage * image, float threshold);
bool clamp(MImage * image, float min, float max);
bool normalize(MImage * image);
bool gammaCorrection(MImage * image, float gamma);

#endif
