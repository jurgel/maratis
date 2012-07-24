/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCorePc
// MSndFileLoader.cpp
//
// SndFile Sound loader
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


#include <stdio.h>
#include <sndfile.h>
#include "MSndFileLoader.h"


bool M_loadSound(const char * filename, void * data)
{
	// open file
    SF_INFO infos;
    SNDFILE * file = sf_open(filename, SFM_READ, &infos);
	if(! file){
		printf("ERROR Load Sound : unable to read %s file\n", filename);
		return false;
	}

	int nbSamples  = infos.channels * (int)infos.frames;
    int sampleRate = infos.samplerate;

	M_SOUND_FORMAT format;
    switch(infos.channels)
    {
	case 1 :
		format = M_SOUND_FORMAT_MONO16;
		break;
	case 2 :
		format = M_SOUND_FORMAT_STEREO16;
		break;
	default :
		printf("ERROR Load Sound : non supported format\n");
		return false;
    }

	MSound * sound = (MSound *)data;

	unsigned int size = nbSamples*2;
	sound->create(format, size, (unsigned int)sampleRate);

    // 16 bits file reading
	if(sf_read_short(file, (short*)sound->getData(), nbSamples) < nbSamples)
	{
		printf("ERROR Load Sound : unable to read samples\n");
        return false;
	}

    sf_close(file);
	return true;
}