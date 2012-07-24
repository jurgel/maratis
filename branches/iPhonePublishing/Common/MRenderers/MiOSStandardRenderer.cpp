/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MiOSStandardRenderer.cpp
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


#include <MEngine.h>
#include "MiOSStandardShaders.h"
#include "MiOSStandardRenderer.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Init
/////////////////////////////////////////////////////////////////////////////////////////////////////////

MStandardRenderer::MStandardRenderer(void):
m_fboId(0),
m_forceNoFX(false),
m_verticesNumber(0),
m_normalsNumber(0),
m_tangentsNumber(0),
m_vertices(NULL),
m_normals(NULL),
m_tangents(NULL),
m_FXsNumber(0)
{
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();
	
	// default FXs
	addFX(vertShader0.c_str(), fragShader0.c_str());
	addFX(vertShader1.c_str(), fragShader1.c_str());
	addFX(vertShader2.c_str(), fragShader2.c_str());
	addFX(vertShader3.c_str(), fragShader3.c_str());
	addFX(vertShader4.c_str(), fragShader4.c_str());
	addFX(vertShader5.c_str(), fragShader5.c_str());
	addFX(vertShader6.c_str(), fragShader6.c_str());
	addFX(vertShader7.c_str(), fragShader7.c_str());
	addFX(vertShader8.c_str(), fragShader8.c_str());

	// rand texture
	MImage image;
	image.create(M_UBYTE, 64, 64, 4);
	unsigned char * pixel = (unsigned char *)image.getData();
	for(unsigned int i=0; i<image.getSize(); i++)
	{
		(*pixel) = (unsigned char)(rand()%256);
		pixel++;
	}

	render->createTexture(&m_randTexture);
	render->bindTexture(m_randTexture);
	render->setTextureFilterMode(M_TEX_FILTER_LINEAR_MIPMAP_LINEAR, M_TEX_FILTER_LINEAR);
	render->setTextureUWrapMode(M_WRAP_REPEAT);
	render->setTextureVWrapMode(M_WRAP_REPEAT);
	
	render->sendTextureImage(&image, 1, 1, 0);
	
	/*
	int size = 64;
	for(int lod=0; lod<7; lod++)
	{
		render->texImage(lod, size, size, M_UBYTE, M_RGBA, image.getData());
		size /= 2;
	}*/
}

MStandardRenderer::~MStandardRenderer(void)
{
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();
	
	// delete default FXs
	for(unsigned int i=0; i<m_FXsNumber; i++)
	{
		render->deleteFX(&m_FXs[i]);
		render->deleteShader(&m_fragShaders[i]);
		render->deleteShader(&m_vertShaders[i]);
	}
	
	// delete shadowLights
	map<unsigned long, MShadowLight>::iterator
	mit (m_shadowLights.begin()),
	mend(m_shadowLights.end());
	for(;mit!=mend;++mit)
	{
		render->deleteTexture(&mit->second.shadowTexture);
	}
	
	// rand texture
	render->deleteTexture(&m_randTexture);
	
	// delete FBO
	render->deleteFrameBuffer(&m_fboId);
	
	// delete skin cache
	SAFE_DELETE_ARRAY(m_vertices);
	SAFE_DELETE_ARRAY(m_normals);
}

void MStandardRenderer::destroy(void)
{
	delete this;
}

MRenderer * MStandardRenderer::getNew(void)
{
	return new MStandardRenderer();
}

void MStandardRenderer::addFX(const char * vert, const char * frag)
{
	if(m_FXsNumber < MAX_DEFAULT_FXS)
	{
		MRenderingContext * render = MEngine::getInstance()->getRenderingContext();
		
		render->createVertexShader(&m_vertShaders[m_FXsNumber]);
		render->sendShaderSource(m_vertShaders[m_FXsNumber], vert);
		render->createPixelShader(&m_fragShaders[m_FXsNumber]);
		render->sendShaderSource(m_fragShaders[m_FXsNumber], frag);
		render->createFX(&m_FXs[m_FXsNumber], m_vertShaders[m_FXsNumber], m_fragShaders[m_FXsNumber]);
		m_FXsNumber++;
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Drawing
/////////////////////////////////////////////////////////////////////////////////////////////////////////

MVector3 * MStandardRenderer::getVertices(unsigned int size)
{
	if(size == 0)
		return NULL;
	
	if(size > m_verticesNumber)
	{
		SAFE_DELETE_ARRAY(m_vertices);
		m_vertices = new MVector3[size];
		m_verticesNumber = size;
	}
	
	return m_vertices;
}

MVector3 * MStandardRenderer::getNormals(unsigned int size)
{
	if(size == 0)
		return NULL;
	
	if(size > m_normalsNumber)
	{
		SAFE_DELETE_ARRAY(m_normals);
		m_normals = new MVector3[size];
		m_normalsNumber = size;
	}
	
	return m_normals;
}

MVector3 * MStandardRenderer::getTangents(unsigned int size)
{
	if(size == 0)
		return NULL;
	
	if(size > m_tangentsNumber)
	{
		SAFE_DELETE_ARRAY(m_tangents);
		m_tangents = new MVector3[size];
		m_tangentsNumber = size;
	}
	
	return m_tangents;
}

void MStandardRenderer::updateSkinning(MMesh * mesh, MArmature * armature)
{
	unsigned int s;
	unsigned int sSize = mesh->getSubMeshsNumber();
	for(s=0; s<sSize; s++)
	{
		MSubMesh * subMesh = &mesh->getSubMeshs()[s];
		
		// data
		MVector3 * vertices = subMesh->getVertices();
		MVector3 * normals = subMesh->getNormals();
		
		if(! vertices)
			continue;
		
		MSkinData * skinData = subMesh->getSkinData();
		if(armature && skinData)
		{
			unsigned int verticesSize = subMesh->getVerticesSize();
			MVector3 * skinVertices = getVertices(verticesSize);
			
			computeSkinning(armature, skinData, vertices, NULL, NULL, skinVertices, NULL, NULL);
			subMesh->getBoundingBox()->initFromPoints(skinVertices, verticesSize);
		}
	}
	
	mesh->updateBoundingBox();
}

void MStandardRenderer::drawDisplay(MSubMesh * subMesh, MDisplay * display, MVector3 * vertices, MVector3 * normals, MVector3 * tangents, MColor * colors)
{
	MEngine * engine = MEngine::getInstance();
	MRenderingContext * render = engine->getRenderingContext();
	

	// get material
	MMaterial * material = display->getMaterial();
	{
		float opacity = material->getOpacity();
		if(opacity <= 0.0f)
			return;
		
		// data
		M_TYPES indicesType = subMesh->getIndicesType();
		void * indices = subMesh->getIndices();
		unsigned int texCoordsSize = subMesh->getTexCoordsSize();
		unsigned int verticesSize = subMesh->getVerticesSize();
		MVector2 * texCoords = subMesh->getTexCoords();

		// begin / size
		unsigned int begin = display->getBegin();
		unsigned int size = display->getSize();
		
		// get properties
		M_PRIMITIVE_TYPES primitiveType = display->getPrimitiveType();
		M_BLENDING_MODES blendMode = material->getBlendMode();
		M_CULL_MODES cullMode = display->getCullMode();
		MVector3 * diffuse = material->getDiffuse();
		MVector3 * specular = material->getSpecular();
		MVector3 * emit = material->getEmit();
		float shininess = material->getShininess();
		
		// get current fog color
		MVector3 currentFogColor;
		render->getFogColor(&currentFogColor);
		
		// set cull mode
		if(cullMode == M_CULL_NONE){
			render->disableCullFace();
		}
		else{
			render->enableCullFace();
			render->setCullMode(cullMode);
		}
		
		// set blending mode
		render->setBlendingMode(blendMode);
		
		// set fog color depending on blending
		switch(blendMode)
		{
			case M_BLENDING_ADD:
			case M_BLENDING_LIGHT:
				render->setFogColor(MVector3(0, 0, 0));
				break;
			case M_BLENDING_PRODUCT:
				render->setFogColor(MVector3(1, 1, 1));
				break;
		}
		
		// texture passes
		unsigned int texturesPassNumber = MIN(8, material->getTexturesPassNumber());
		
		// FX
		unsigned int fxId = 0;
		MFXRef * FXRef = material->getFXRef();
		MFXRef * ZFXRef = material->getZFXRef();
		
		if(FXRef)
			fxId = FXRef->getFXId();
		
		bool basicFX = false;
		
		// force NoFX
		if(m_forceNoFX)
		{
			// optimize only for standard shader (for custom shader we don't know how geometry and alpha test is done)
			if(fxId == 0)
			{
				fxId = m_FXs[0]; // basic FX
				texturesPassNumber = 0;
				
				// alpha test
				if(material->getTexturesPassNumber() > 0)
				{
					MTexture * texture = material->getTexturePass(0)->getTexture();
					if(texture)
					{
						if(texture->getTextureRef()->getComponents() > 3)
						{
							fxId = m_FXs[7]; // basic FX with texture
							texturesPassNumber = 1;
						}
					}
				}
				
				basicFX = true;
			}
			else if(ZFXRef)
			{
				fxId = ZFXRef->getFXId();
			}
		}
		
		// standard shader
		else if(fxId == 0)
		{
			if(material->getTexturesPassNumber() == 0) // simple
				fxId = m_FXs[1];
			else if(material->getTexturesPassNumber() == 1) // diffuse
				fxId = m_FXs[2];
			else if(material->getTexturesPassNumber() == 2) // diffuse+specular
				fxId = m_FXs[3];
			else if(material->getTexturesPassNumber() == 3) // diffuse+specular+normal
				fxId = m_FXs[4];
			else{
				if(material->getTexturePass(2)->getTexture()) // diffuse+specular+emit+normal
					fxId = m_FXs[6];
				else
					fxId = m_FXs[5]; // diffuse+specular+emit
			}
		}
		
		// FX pipeline
		{
			int attribIndex;
			MMatrix4x4 * cameraViewMatrix = m_currentCamera->getCurrentViewMatrix();
			MMatrix4x4 * cameraProjMatrix = m_currentCamera->getCurrentProjMatrix();
			
			// properties
			static int AlphaTest;
			static MVector3 FogColor;
			static float FogEnd;
			static float FogScale;
			
			static MVector4 LightPosition[4];
			static MVector3 LightDiffuseProduct[4];
			static MVector3 LightSpecularProduct[4];
			static MVector3 LightSpotDirection[4];
			static float LightConstantAttenuation[4];
			static float LightQuadraticAttenuation[4];
			static float LightSpotCosCutoff[4];
			static float LightSpotExponent[4];
			static int LightActive[4];
			
			static int ShadowMaps[4];
			static int Texture[8] = {0, 1, 2, 3, 4, 5, 6, 7};
			static MMatrix4x4 TextureMatrix[8];
			static MMatrix4x4 ModelViewMatrix;
			static MMatrix4x4 ProjModelViewMatrix;
			static MMatrix4x4 NormalMatrix;
			
			
			// Alpha test
			AlphaTest = (blendMode != M_BLENDING_ALPHA);
				
			// Matrix
			render->getModelViewMatrix(&ModelViewMatrix);
			ProjModelViewMatrix = (*cameraProjMatrix) * ModelViewMatrix;
			
			if(! basicFX)
			{
				// Fog
				float min, max;
				render->getFogColor(&FogColor);
				render->getFogDistance(&min, &max);
				FogEnd = max;
				FogScale = 1.0f / (max - min);

				// Lights
				for(int i=0; i<MAX_LOCAL_LIGHTS; i++)
				{
					float spotAngle;
					float linearAttenuation;
					MVector4 lightDiffuse;
					render->getLightDiffuse(i, &lightDiffuse);
					render->getLightPosition(i, &LightPosition[i]);
					render->getLightSpotDirection(i, &LightSpotDirection[i]);
					render->getLightAttenuation(i, &LightConstantAttenuation[i], &linearAttenuation, &LightQuadraticAttenuation[i]);
					render->getLightSpotAngle(i, &spotAngle);
					render->getLightSpotExponent(i, &LightSpotExponent[i]);
				
					LightActive[i] = (lightDiffuse.w > 0.0f);
					LightSpotCosCutoff[i] = cosf(spotAngle*DEG_TO_RAD);
					LightDiffuseProduct[i] = (*diffuse) * lightDiffuse;
					LightSpecularProduct[i] = (*specular) * lightDiffuse;
					LightPosition[i] = (*cameraViewMatrix) * MVector4(LightPosition[i]);
					LightSpotDirection[i] = (cameraViewMatrix->getRotatedVector3(LightSpotDirection[i])).getNormalized();
				}
				
				// Normal Matrix
				NormalMatrix = (ModelViewMatrix.getInverse()).getTranspose();
			}
			
			
			// bind FX
			render->bindFX(fxId);
			
			
			// Vertex
			render->getAttribLocation(fxId, "Vertex", &attribIndex);
			if(attribIndex != -1)
			{
				render->setAttribPointer(attribIndex, M_FLOAT, 3, vertices);
				render->enableAttribArray(attribIndex);
			}
			
			if(! basicFX)
			{
				// Normal
				if(normals)
				{
					render->getAttribLocation(fxId, "Normal", &attribIndex);
					if(attribIndex != -1)
					{
						render->setAttribPointer(attribIndex, M_FLOAT, 3, normals);
						render->enableAttribArray(attribIndex);
					}
				}
			
				// Tangent
				if(tangents)
				{
					render->getAttribLocation(fxId, "Tangent", &attribIndex);
					if(attribIndex != -1)
					{
						render->setAttribPointer(attribIndex, M_FLOAT, 3, tangents);
						render->enableAttribArray(attribIndex);
					}
				}
				
				// Color
				if(colors)
				{
					render->getAttribLocation(fxId, "Color", &attribIndex);
					if(attribIndex != -1)
					{
						render->setAttribPointer(attribIndex, M_UBYTE, 3, colors, true);
						render->enableAttribArray(attribIndex);
					}
				}
			}
			

			// Textures
			int id = texturesPassNumber;
			for(unsigned int t=0; t<texturesPassNumber; t++)
			{
				MTexturePass * texturePass = material->getTexturePass(t);
				
				MTexture * texture = texturePass->getTexture();
				if((! texture) || (! texCoords))
				{
					render->bindTexture(0, t);
					continue;
				}
				
				// texCoords
				unsigned int offset = 0;
				if(subMesh->isMapChannelExist(texturePass->getMapChannel()))
					offset = subMesh->getMapChannelOffset(texturePass->getMapChannel());
				
				// texture id
				unsigned int textureId = 0;
				MTextureRef * texRef = texture->getTextureRef();
				if(texRef)
					textureId = texRef->getTextureId();
				
				// bind texture
				render->bindTexture(textureId, t);
				render->setTextureUWrapMode(texture->getUWrapMode());
				render->setTextureVWrapMode(texture->getVWrapMode());
				
				// texture matrix TODO:
				MMatrix4x4 * texMatrix = &TextureMatrix[t];
				texMatrix->loadIdentity();
				texMatrix->translate(MVector2(0.5f, 0.5f));
				texMatrix->scale(*texture->getTexScale());
				texMatrix->rotate(MVector3(0, 0, -1), texture->getTexRotate());
				texMatrix->translate(MVector2(-0.5f, -0.5f));
				texMatrix->translate(*texture->getTexTranslate());
				
				// texture coords
				char name[16];
				sprintf(name, "TexCoord%d", t);
				render->getAttribLocation(fxId, name, &attribIndex);
				if(attribIndex != -1)
				{
					render->setAttribPointer(attribIndex, M_FLOAT, 2, texCoords + offset);
					render->enableAttribArray(attribIndex);
				}
			}
			
			if(! basicFX)
			{
				// Shadows
				for(int i=0; i<MAX_LOCAL_LIGHTS; i++)
				{
					if(m_lightShadow[i] == 1)
					{
						render->bindTexture(m_lightShadowTexture[i], id);
						ShadowMaps[i] = id;
						id++;
					}
					else {
						render->bindTexture(0, id);
						ShadowMaps[i] = id;
						id++;
					}
				}
				
				// rand texture
				int randTextureId = id;
				{
					render->bindTexture(m_randTexture, id);
					id++;
				}
				
				// uniforms
				render->sendUniformVec4(fxId, "FogColor", MVector4(FogColor));
				render->sendUniformFloat(fxId, "FogEnd", &FogEnd);
				render->sendUniformFloat(fxId, "FogScale", &FogScale);
			
				render->sendUniformVec3(fxId, "MaterialEmit", *emit);
				render->sendUniformFloat(fxId, "MaterialShininess", &shininess);
			
				render->sendUniformVec4(fxId, "LightPosition", LightPosition[0], MAX_LOCAL_LIGHTS);
				render->sendUniformVec3(fxId, "LightDiffuseProduct", LightDiffuseProduct[0], MAX_LOCAL_LIGHTS);
				render->sendUniformVec3(fxId, "LightSpecularProduct", LightSpecularProduct[0], MAX_LOCAL_LIGHTS);
				render->sendUniformVec3(fxId, "LightSpotDirection", LightSpotDirection[0], MAX_LOCAL_LIGHTS);
				render->sendUniformFloat(fxId, "LightConstantAttenuation", LightConstantAttenuation, MAX_LOCAL_LIGHTS);
				render->sendUniformFloat(fxId, "LightQuadraticAttenuation", LightQuadraticAttenuation, MAX_LOCAL_LIGHTS);
				render->sendUniformFloat(fxId, "LightSpotCosCutoff", LightSpotCosCutoff, MAX_LOCAL_LIGHTS);
				render->sendUniformFloat(fxId, "LightSpotExponent", LightSpotExponent, MAX_LOCAL_LIGHTS);
				render->sendUniformInt(fxId, "LightActive", LightActive, MAX_LOCAL_LIGHTS);
			
				render->sendUniformInt(fxId, "LightShadowMap", ShadowMaps, MAX_LOCAL_LIGHTS);
				render->sendUniformInt(fxId, "LightShadow", m_lightShadow, MAX_LOCAL_LIGHTS);
				render->sendUniformFloat(fxId, "LightShadowBias", m_lightShadowBias, MAX_LOCAL_LIGHTS);
				render->sendUniformFloat(fxId, "LightShadowBlur", m_lightShadowBlur, MAX_LOCAL_LIGHTS);
				render->sendUniformMatrix(fxId, "LightShadowMatrix", m_lightShadowMatrix, MAX_LOCAL_LIGHTS);
				
				render->sendUniformInt(fxId, "RandTexture", &randTextureId);
				
				render->sendUniformMatrix(fxId, "ModelViewMatrix", &ModelViewMatrix);
				render->sendUniformMatrix(fxId, "NormalMatrix", &NormalMatrix);
				render->sendUniformMatrix(fxId, "ProjectionMatrix", cameraProjMatrix);
			}
			
			if(texturesPassNumber > 0)
			{
				render->sendUniformInt(fxId, "AlphaTest", &AlphaTest);
				render->sendUniformInt(fxId, "Texture", Texture, texturesPassNumber);
				render->sendUniformMatrix(fxId, "TextureMatrix", TextureMatrix, texturesPassNumber);
			}
			
			render->sendUniformFloat(fxId, "MaterialOpacity", &opacity);
			render->sendUniformMatrix(fxId, "ProjModelViewMatrix", &ProjModelViewMatrix);
			
			
			// draw
			if(indices)
			{
				switch(indicesType)
				{
					case M_USHORT:
						render->drawElement(primitiveType, size, indicesType, (unsigned short*)indices + begin);
						break;
					case M_UINT:
						render->drawElement(primitiveType, size, indicesType, (unsigned int*)indices + begin);
						break;
				}
			}
			else{
				render->drawArray(primitiveType, begin, size);
			}
			
			
			// disable attribs
			for(int i=0; i<16; i++)
				render->disableAttribArray(i);
			
			// restore textures
			for(int t=(int)(id-1); t>=0; t--)
			{
				render->bindTexture(0, t);
				render->disableTexture();
			}
			
			// restore FX
			render->bindFX(0);
		}

		// restore fog and alpha test
		render->setFogColor(currentFogColor);
	}
}

void MStandardRenderer::drawOpaques(MSubMesh * subMesh, MArmature * armature)
{
	// data
	MVector3 * vertices = subMesh->getVertices();
	MVector3 * normals = subMesh->getNormals();
	MVector3 * tangents = subMesh->getTangents();
	MColor * colors = subMesh->getColors();
	
	if(! vertices)
		return;
	
	MSkinData * skinData = subMesh->getSkinData();
	if(armature && skinData)
	{
		unsigned int verticesSize = subMesh->getVerticesSize();
		unsigned int normalsSize = subMesh->getNormalsSize();
		unsigned int tangentsSize = subMesh->getTangentsSize();
		
		MVector3 * skinVertices = getVertices(verticesSize);
		MVector3 * skinNormals = getNormals(normalsSize);
		MVector3 * skinTangents = getTangents(tangentsSize);
		
		computeSkinning(armature, skinData, vertices, normals, tangents, skinVertices, skinNormals, skinTangents);
		subMesh->getBoundingBox()->initFromPoints(skinVertices, verticesSize);
		
		vertices = skinVertices;
		normals = skinNormals;
		tangents = skinTangents;
	}
	
	unsigned int i;
	unsigned int displayNumber = subMesh->getDisplaysNumber();
	for(i=0; i<displayNumber; i++)
	{
		MDisplay * display = subMesh->getDisplay(i);
		if(! display->isVisible())
			continue;
		
		MMaterial * material = display->getMaterial();
		if(material)
		{
			if(material->getBlendMode() == M_BLENDING_NONE)
				drawDisplay(subMesh, display, vertices, normals, tangents, colors);
		}
	}
}

void MStandardRenderer::drawTransparents(MSubMesh * subMesh, MArmature * armature)
{
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();
	
	// data
	MVector3 * vertices = subMesh->getVertices();
	MVector3 * normals = subMesh->getNormals();
	MVector3 * tangents = subMesh->getTangents();
	MColor * colors = subMesh->getColors();
	
	if(! vertices)
		return;

	MSkinData * skinData = subMesh->getSkinData();
	if(armature && skinData)
	{
		unsigned int verticesSize = subMesh->getVerticesSize();
		unsigned int normalsSize = subMesh->getNormalsSize();
		unsigned int tangentsSize = subMesh->getTangentsSize();
		
		MVector3 * skinVertices = getVertices(verticesSize);
		MVector3 * skinNormals = getNormals(normalsSize);
		MVector3 * skinTangents = getTangents(tangentsSize);
		
		computeSkinning(armature, skinData, vertices, normals, tangents, skinVertices, skinNormals, skinTangents);
		subMesh->getBoundingBox()->initFromPoints(skinVertices, verticesSize);
		
		vertices = skinVertices;
		normals = skinNormals;
		tangents = skinTangents;
	}
	
	render->setColorMask(0, 0, 0, 0);
	m_forceNoFX = true;
	
	unsigned int i;
	unsigned int displayNumber = subMesh->getDisplaysNumber();
	for(i=0; i<displayNumber; i++)
	{
		MDisplay * display = subMesh->getDisplay(i);
		if((! display->isVisible()) || (! display->getMaterial()))
			continue;
		
		MMaterial * material = display->getMaterial();
		if(material)
		{
			if(material->getBlendMode() != M_BLENDING_NONE)
				drawDisplay(subMesh, display, vertices, normals, tangents, colors);
		}
	}
	
	m_forceNoFX = false;
	render->setColorMask(1, 1, 1, 1);
	render->setDepthMask(0);
	render->setDepthMode(M_DEPTH_EQUAL);
	
	for(i=0; i<displayNumber; i++)
	{
		MDisplay * display = subMesh->getDisplay(i);
		if(! display->isVisible())
			continue;
		
		MMaterial * material = display->getMaterial();
		if(material)
		{
			if(material->getBlendMode() != M_BLENDING_NONE)
				drawDisplay(subMesh, display, vertices, normals, tangents, colors);
		}
	}
	
	render->setDepthMask(1);
	render->setDepthMode(M_DEPTH_LEQUAL);
}

float MStandardRenderer::getDistanceToCam(MOCamera * camera, const MVector3 & pos)
{
	if(! camera->isOrtho())
	{
		return (pos - camera->getTransformedPosition()).getSquaredLength();
	}
	
	MVector3 axis = camera->getRotatedVector(MVector3(0, 0, -1)).getNormalized();
	float dist = (pos - camera->getTransformedPosition()).dotProduct(axis);
	return dist*dist;
}

void MStandardRenderer::setShadowMatrix(MMatrix4x4 * matrix)
{
	MEngine * engine = MEngine::getInstance();
	MRenderingContext * render = engine->getRenderingContext();
	
	const MMatrix4x4 biasMatrix(
		0.5f, 0.0f, 0.0f, 0.0f, 
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f
	);
	
	MMatrix4x4 modelViewMatrix;
	MMatrix4x4 projMatrix;
	render->getModelViewMatrix(&modelViewMatrix);
	render->getProjectionMatrix(&projMatrix);
	
	(*matrix) = biasMatrix;
	(*matrix) = (*matrix) * projMatrix;
	(*matrix) = (*matrix) * modelViewMatrix;
}

void MStandardRenderer::updateVisibility(MScene * scene, MOCamera * camera)
{
	// make frustum
	camera->getFrustum()->makeVolume(camera);
	
	// compute object visibility
	unsigned int i;
	unsigned int oSize = scene->getObjectsNumber();
	for(i=0; i<oSize; i++)
	{
		MObject3d * object = scene->getObjectByIndex(i);
		if(object->isActive())
			object->updateVisibility(camera);
	}
}

void MStandardRenderer::enableFog(MOCamera * camera)
{
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();
	
	float fogMin = camera->getClippingFar()*0.9999f;
	if(camera->hasFog())
	{
		render->enableFog();
		float camFogMin = camera->getClippingFar() - camera->getFogDistance();
		if(camFogMin < fogMin)
			fogMin = camFogMin;
	}
	else {
		render->disableFog();
	}
	
	render->setFogColor(*camera->getClearColor());
	render->setFogDistance(fogMin, camera->getClippingFar());
}

MShadowLight * MStandardRenderer::createShadowLight(MOLight * light)
{
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();
	unsigned int shadowQuality = light->getShadowQuality();
	
	map<unsigned long, MShadowLight>::iterator iter = m_shadowLights.find((unsigned long)light);
	if(iter != m_shadowLights.end())
	{
		MShadowLight * shadowLight = &iter->second;
		shadowLight->score = 1;
		
		if(shadowLight->shadowQuality != shadowQuality)
		{
			shadowLight->shadowQuality = shadowQuality;
			
			render->bindTexture(shadowLight->shadowTexture);
			render->texImage(0, shadowQuality, shadowQuality, M_UBYTE, M_DEPTH, 0);
			render->bindTexture(0);
		}
		
		return shadowLight;
	}
	else
	{
		m_shadowLights[(unsigned long)(light)] = MShadowLight();
		MShadowLight * shadowLight = &m_shadowLights[(unsigned long)(light)];
		shadowLight->score = 1;
		shadowLight->shadowQuality = shadowQuality;
		
		render->createTexture(&shadowLight->shadowTexture);
		render->bindTexture(shadowLight->shadowTexture);
		render->setTextureFilterMode(M_TEX_FILTER_LINEAR, M_TEX_FILTER_LINEAR);
		render->setTextureUWrapMode(M_WRAP_CLAMP);
		render->setTextureVWrapMode(M_WRAP_CLAMP);
		render->texImage(0, shadowQuality, shadowQuality, M_UBYTE, M_DEPTH, 0);
		render->bindTexture(0);
		
		return shadowLight;
	}
}

void MStandardRenderer::destroyUnusedShadowLights(void)
{
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();
	
	// keys
	map<unsigned long, MShadowLight>::iterator
	mit (m_shadowLights.begin()),
	mend(m_shadowLights.end());
	
	for(;mit!=mend;++mit)
	{
		MShadowLight * shadowLight = &mit->second;
		if(shadowLight->score < 1)
		{
			render->deleteTexture(&shadowLight->shadowTexture);
			m_shadowLights.erase(mit);
			mit = m_shadowLights.begin();
			mend = m_shadowLights.end();
			if(mit == mend)
				return;
		}
	}
}

void MStandardRenderer::decreaseShadowLights(void)
{
	// keys
	map<unsigned long, MShadowLight>::iterator
	mit (m_shadowLights.begin()),
	mend(m_shadowLights.end());
	
	for(;mit!=mend;++mit)
	{
		MShadowLight * shadowLight = &mit->second;
		shadowLight->score--;
	}	
}

void MStandardRenderer::drawText(MOText * textObj)
{
	MRenderingContext * render = MEngine().getInstance()->getRenderingContext();
	
	
	MFont * font = textObj->getFont();
	const char * text = textObj->getText();
	vector <float> * linesOffset = textObj->getLinesOffset();
	
	if(! (strlen(text) > 0 && font))
		return;
	
	if(linesOffset->size() == 0)
		return;
	
	
	int id = 0;
	int vertAttribIndex;
	int texAttribIndex;
	unsigned int fxId;
	static MVector2 vertices[4];
	static MVector2 texCoords[4];
	static MMatrix4x4 ModelViewMatrix;
	static MMatrix4x4 ProjModelViewMatrix;
	MMatrix4x4 * cameraProjMatrix = m_currentCamera->getCurrentProjMatrix();
	
	
	// Matrix
	render->getModelViewMatrix(&ModelViewMatrix);
	ProjModelViewMatrix = (*cameraProjMatrix) * ModelViewMatrix;
	
	
	// blending
	render->enableBlending();
	render->setBlendingMode(M_BLENDING_ALPHA);
	
	
	// bind FX
	fxId = m_FXs[8];
	render->bindFX(fxId);
	

	// ProjModelViewMatrix
	render->sendUniformMatrix(fxId, "ProjModelViewMatrix", &ProjModelViewMatrix);
	
	// Texture0
	render->sendUniformInt(fxId, "Texture0", &id);
	
	// Color
	render->sendUniformVec4(fxId, "Color", *textObj->getColor());

	// Vertex
	render->getAttribLocation(fxId, "Vertex", &vertAttribIndex);
	if(vertAttribIndex != -1)
	{
		render->setAttribPointer(vertAttribIndex, M_FLOAT, 2, vertices);
		render->enableAttribArray(vertAttribIndex);
	}
	
	// TexCoord
	render->getAttribLocation(fxId, "TexCoord", &texAttribIndex);
	if(texAttribIndex != -1)
	{
		render->setAttribPointer(texAttribIndex, M_FLOAT, 2, texCoords);
		render->enableAttribArray(texAttribIndex);
	}
	
	
	// bind texture
	render->bindTexture(font->getTextureId());
	
	unsigned int lineId = 0;
	float lineOffset = (*linesOffset)[0];
	
	float size = textObj->getSize();
	float tabSize = size*2;
	float fontSize = (float)font->getFontSize();
	float widthFactor = font->getTextureWith() / fontSize;
	float heightFactor = font->getTextureHeight() / fontSize;
	float xc = 0, yc = 0;
	
	unsigned int i;
	unsigned int textLen = strlen(text);
	for(i=0; i<textLen; i++)
	{
		if(text[i] == '\n') // return
		{
			if(((i+1) < textLen))
			{
				lineId++;
				lineOffset = (*linesOffset)[lineId];
				
				yc += size;
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
		MVector2 offset = character->getOffset() * size + MVector2(lineOffset, 0);
		
		float width = scale.x * widthFactor * size;
		float height = scale.y * heightFactor * size;
		
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
		xc += character->getXAdvance() * size;
	}
	
	// disable attribs
	if(vertAttribIndex != -1)
		render->disableAttribArray(vertAttribIndex);
	if(texAttribIndex != -1)
		render->disableAttribArray(texAttribIndex);
	
	// release FX
	render->bindFX(0);
}

void MStandardRenderer::drawScene(MScene * scene, MOCamera * camera)
{
	struct MEntityLight
	{
		MBox3d lightBox;
		MOLight * light;
	};
	
	struct MSubMeshPass
	{
		unsigned int subMeshId;
		unsigned int lightsNumber;
		MObject3d * object;
		MOLight * lights[4];
	};
	
	// sub objects
	#define MAX_TRANSP_SUBOBJ 4096
	static int transpList[MAX_TRANSP_SUBOBJ];
	static float transpZList[MAX_TRANSP_SUBOBJ];
	static MSubMeshPass transpSubObjs[MAX_TRANSP_SUBOBJ];
	
	// lights list
	#define MAX_ENTITY_LIGHTS 256
	static int entityLightsList[MAX_ENTITY_LIGHTS];
	static float entityLightsZList[MAX_ENTITY_LIGHTS];
	static MEntityLight entityLights[MAX_ENTITY_LIGHTS];
	
	
	// get render
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();
	
	
	// current render buffer
	unsigned int currentFrameBuffer = 0;
	render->getCurrentFrameBuffer(&currentFrameBuffer);
	
	// init
	render->setAlphaTest(0);
	render->disableVertexArray();
	render->disableTexCoordArray();
	render->disableNormalArray();
	render->disableColorArray();
	
	
	// destroy unused shadowLights
	destroyUnusedShadowLights();
	
	// decrease shadowLights score
	decreaseShadowLights();
	
	
	// lights
	unsigned int l;
	unsigned int lSize = scene->getLightsNumber();
	
	
	// make frustum
	camera->getFrustum()->makeVolume(camera);
	
	
	// compute lights visibility
	for(l=0; l<lSize; l++)
	{
		MOLight * light = scene->getLightByIndex(l);
		if(light->isActive())
			light->updateVisibility(camera);
	}
	
	
	// create frame buffer (TODO: only if minimum one shadow light)
	if(m_fboId == 0)
	{
		render->createFrameBuffer(&m_fboId);
		render->bindFrameBuffer(m_fboId);
		render->setDrawingBuffers(NULL, 0);
		render->bindFrameBuffer(currentFrameBuffer);
	}
	

	render->disableLighting();
	render->disableBlending();
	m_forceNoFX = true;
	
	bool restoreCamera = false;
	
	for(l=0; l<lSize; l++)
	{
		MOLight * light = scene->getLightByIndex(l);
		if(! (light->isActive() && light->isVisible()))
			continue;
		
		if(light->getSpotAngle() < 90.0f && light->isCastingShadow())
		{
			unsigned int i;
			unsigned int zListSize = 0;
			unsigned int eSize = scene->getEntitiesNumber();
			unsigned int shadowQuality = light->getShadowQuality();
			MShadowLight * shadowLight = createShadowLight(light);
			
			render->bindFrameBuffer(m_fboId);
			render->attachFrameBufferTexture(M_ATTACH_DEPTH, shadowLight->shadowTexture);

		
			for(int i=0; i<MAX_LOCAL_LIGHTS; i++){
				render->setLightDiffuse(i, MVector4(0, 0, 0, 0));
				render->disableLight(i);
				m_lightShadow[i] = 0;
			}
			
			MOCamera lightCamera;
			*lightCamera.getMatrix() = *light->getMatrix();
			lightCamera.setClippingNear(light->getRadius()*0.001f);
			lightCamera.setClippingFar(light->getRadius());
			lightCamera.setFov(light->getSpotAngle()*2.0f);
			
			
			MVector3 cameraPos = lightCamera.getTransformedPosition();
			MVector3 cameraAxis = lightCamera.getRotatedVector(MVector3(0, 0, -1)).getNormalized();
			
			render->disableScissorTest();
			render->enableDepthTest();
			render->setViewport(0, 0, shadowQuality, shadowQuality);
			
			lightCamera.enable();
			
			// frustum
			MFrustum * frustum = lightCamera.getFrustum();
			frustum->makeVolume(&lightCamera);
			
			
			for(i=0; i<eSize; i++)
			{
				MOEntity * entity = scene->getEntityByIndex(i);
				if(entity->isActive())
				{
					if(entity->isInvisible()){
						entity->setVisible(false);
						continue;
					}
					
					// compute entities visibility
					MBox3d * box = entity->getBoundingBox();
					MVector3 * min = box->getMin();
					MVector3 * max = box->getMax();
					
					MVector3 points[8] = {
						entity->getTransformedVector(MVector3(min->x, min->y, min->z)),
						entity->getTransformedVector(MVector3(min->x, max->y, min->z)),
						entity->getTransformedVector(MVector3(max->x, max->y, min->z)),
						entity->getTransformedVector(MVector3(max->x, min->y, min->z)),
						entity->getTransformedVector(MVector3(min->x, min->y, max->z)),
						entity->getTransformedVector(MVector3(min->x, max->y, max->z)),
						entity->getTransformedVector(MVector3(max->x, max->y, max->z)),
						entity->getTransformedVector(MVector3(max->x, min->y, max->z))
					};
					
					entity->setVisible(frustum->isVolumePointsVisible(points, 8));

					// adapt clipping
					if(entity->isVisible())
					{
						for(int p=0; p<8; p++)
						{
							if(zListSize == MAX_TRANSP_SUBOBJ)
								break;
						
							float dist = (points[p] - cameraPos).dotProduct(cameraAxis);
						
							transpZList[zListSize] = dist;
							zListSize++;
						}
					}
				}
			}
			
			// sort Zlist and set clipping
			if(zListSize > 1)
			{
				sortFloatList(transpZList, 0, zListSize-1);
				lightCamera.setClippingFar(MIN(lightCamera.getClippingFar(), transpZList[0]));
				lightCamera.setClippingNear(MAX(lightCamera.getClippingNear(), transpZList[zListSize-1]));
				lightCamera.enable(); // need to enable the camera again
			}
			
			m_currentCamera = &lightCamera;
			
			render->clear(M_BUFFER_DEPTH);
			render->setColorMask(0, 0, 0, 0);
			
			// entities
			for(i=0; i<eSize; i++)
			{
				// get entity
				MOEntity * entity = scene->getEntityByIndex(i);
				MMesh * mesh = entity->getMesh();
				
				// draw mesh
				if(mesh && entity->isActive() && entity->isVisible())
				{
					// animate armature
					if(mesh->getArmature() && mesh->getArmatureAnim())
						animateArmature(
										mesh->getArmature(),
										mesh->getArmatureAnim(),
										entity->getCurrentFrame()
										);
					
					// animate textures
					if(mesh->getTexturesAnim())
						animateTextures(mesh, mesh->getTexturesAnim(), entity->getCurrentFrame());
					
					// animate materials
					if(mesh->getMaterialsAnim())
						animateMaterials(mesh, mesh->getMaterialsAnim(), entity->getCurrentFrame());
					
					unsigned int s;
					unsigned int sSize = mesh->getSubMeshsNumber();
					for(s=0; s<sSize; s++)
					{
						MSubMesh * subMesh = &mesh->getSubMeshs()[s];
						MBox3d * box = subMesh->getBoundingBox();
						
						// check if submesh visible
						if(sSize > 1)
						{
							MVector3 * min = box->getMin();
							MVector3 * max = box->getMax();
							
							MVector3 points[8] = {
								entity->getTransformedVector(MVector3(min->x, min->y, min->z)),
								entity->getTransformedVector(MVector3(min->x, max->y, min->z)),
								entity->getTransformedVector(MVector3(max->x, max->y, min->z)),
								entity->getTransformedVector(MVector3(max->x, min->y, min->z)),
								entity->getTransformedVector(MVector3(min->x, min->y, max->z)),
								entity->getTransformedVector(MVector3(min->x, max->y, max->z)),
								entity->getTransformedVector(MVector3(max->x, max->y, max->z)),
								entity->getTransformedVector(MVector3(max->x, min->y, max->z))
							};
							
							if(! frustum->isVolumePointsVisible(points, 8))
								continue;
						}
						
						render->pushMatrix();
						render->multMatrix(entity->getMatrix());
						
						// draw opaques
						drawOpaques(subMesh, mesh->getArmature());
						
						render->popMatrix();
					}
					
					mesh->updateBoundingBox();
					(*entity->getBoundingBox()) = (*mesh->getBoundingBox());
				}
			}
			
			setShadowMatrix(&shadowLight->shadowMatrix);
			
			// biasUnity
			{
				MVector4 pt1 = shadowLight->shadowMatrix * MVector4(cameraPos + cameraAxis);
				MVector4 pt2 = shadowLight->shadowMatrix * MVector4(cameraPos + cameraAxis*2.0f);
				
				shadowLight->biasUnity = (- (pt1.z - pt2.z*0.5f))*0.01f;
			}
			
			render->setColorMask(1, 1, 1, 1);
			render->bindFrameBuffer(currentFrameBuffer);
			render->bindTexture(0);
			restoreCamera = true;
		}
	}
	
	// restore camera after shadow pass
	if(restoreCamera)
	{
		int * viewport = camera->getCurrentViewport();
		render->setViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	
		camera->enable();
		render->clear(M_BUFFER_DEPTH);
	}
	
	
	// update visibility
	updateVisibility(scene, camera); // TODO: don't need to test light vis again
	
	// get camera frustum
	MFrustum * frustum = camera->getFrustum();
	
	// fog
	enableFog(camera);
	
	// enable blending
	render->enableBlending();
	
	// camera
	MVector3 cameraPos = camera->getTransformedPosition();
	
	// enable lightning
	render->enableLighting();
	
	// transp sub obj number
	unsigned int transpSubObsNumber = 0;
	
	m_currentCamera = camera;
	
	// entities
	unsigned int i;
	unsigned int eSize = scene->getEntitiesNumber();
	
	
	/*
	// Z pre-pass
	render->setColorMask(0, 0, 0, 0);
	
	for(i=0; i<eSize; i++)
	{
		// get entity
		MOEntity * entity = scene->getEntityByIndex(i);
		MMesh * mesh = entity->getMesh();
		
		if(! entity->isActive())
			continue;
		
		if(! entity->isVisible())
			continue;
	
		if(mesh)
		{
			// animate armature
			if(mesh->getArmature() && mesh->getArmatureAnim())
				animateArmature(
					mesh->getArmature(),
					mesh->getArmatureAnim(),
					entity->getCurrentFrame()
				);
			
			// animate textures
			if(mesh->getTexturesAnim())
				animateTextures(mesh, mesh->getTexturesAnim(), entity->getCurrentFrame());
			
			// animate materials
			if(mesh->getMaterialsAnim())
				animateMaterials(mesh, mesh->getMaterialsAnim(), entity->getCurrentFrame());
			
			unsigned int s;
			unsigned int sSize = mesh->getSubMeshsNumber();
			for(s=0; s<sSize; s++)
			{
				MSubMesh * subMesh = &mesh->getSubMeshs()[s];
				MBox3d * box = subMesh->getBoundingBox();
				
				// check if submesh visible
				if(sSize > 1)
				{
					MVector3 * min = box->getMin();
					MVector3 * max = box->getMax();
					
					MVector3 points[8] = {
						entity->getTransformedVector(MVector3(min->x, min->y, min->z)),
						entity->getTransformedVector(MVector3(min->x, max->y, min->z)),
						entity->getTransformedVector(MVector3(max->x, max->y, min->z)),
						entity->getTransformedVector(MVector3(max->x, min->y, min->z)),
						entity->getTransformedVector(MVector3(min->x, min->y, max->z)),
						entity->getTransformedVector(MVector3(min->x, max->y, max->z)),
						entity->getTransformedVector(MVector3(max->x, max->y, max->z)),
						entity->getTransformedVector(MVector3(max->x, min->y, max->z))
					};
					
					if(! frustum->isVolumePointsVisible(points, 8))
						continue;
				}
				
				render->pushMatrix();
				render->multMatrix(entity->getMatrix());
				
				// draw opaques
				drawOpaques(subMesh, mesh->getArmature());
				
				render->popMatrix();
			}
			
			mesh->updateBoundingBox();
			(*entity->getBoundingBox()) = (*mesh->getBoundingBox());
		}
	}

	 render->setDepthMode(M_DEPTH_EQUAL);
	 render->setDepthMask(0);
	*/
	
	// opaque pass
	render->setColorMask(1, 1, 1, 1);
	m_forceNoFX = false;
	
	for(i=0; i<eSize; i++)
	{
		// get entity
		MOEntity * entity = scene->getEntityByIndex(i);
		MMesh * mesh = entity->getMesh();
		
		if(! entity->isActive())
			continue;
		
		if(! entity->isVisible())
		{
			if(mesh)
			{
				MArmature * armature = mesh->getArmature();
				MArmatureAnim * armatureAnim = mesh->getArmatureAnim();
				if(armature)
				{
					// animate armature
					if(armatureAnim)
						animateArmature(
										mesh->getArmature(),
										mesh->getArmatureAnim(),
										entity->getCurrentFrame()
										);
					
					// TODO : optimize and add a tag to desactivate it
					updateSkinning(mesh, armature);
					(*entity->getBoundingBox()) = (*mesh->getBoundingBox());
				}
			}
			
			continue;
		}
		
		// draw mesh
		if(mesh)
		{
			MVector3 scale = entity->getTransformedScale();
			MBox3d * entityBox = entity->getBoundingBox();
			
			float minScale = scale.x;
			minScale = MIN(minScale, scale.y);
			minScale = MIN(minScale, scale.z);
			minScale = 1.0f / minScale;
			
			unsigned int entityLightsNumber = 0;
			for(l=0; l<lSize; l++)
			{
				// get entity
				MOLight * light = scene->getLightByIndex(l);
				
				if(! light->isActive())
					continue;
				
				if(! light->isVisible())
					continue;
				
				if(light->getRadius() <= 0.0f)
					continue;
				
				// light box
				MVector3 lightPos = light->getTransformedPosition();
				MVector3 localPos = entity->getInversePosition(lightPos);
				
				float localRadius = light->getRadius() * minScale;
				
				MBox3d lightBox(
					MVector3(localPos - localRadius),
					MVector3(localPos + localRadius)
				);
				
				if(! entityBox->isInCollisionWith(&lightBox))
					continue;
				
				MEntityLight * entityLight = &entityLights[entityLightsNumber];
				entityLight->lightBox = lightBox;
				entityLight->light = light;
				
				entityLightsNumber++;
				if(entityLightsNumber == MAX_ENTITY_LIGHTS)
					break;
			}
			
			// animate armature
			if(mesh->getArmature() && mesh->getArmatureAnim())
				animateArmature(
								mesh->getArmature(),
								mesh->getArmatureAnim(),
								entity->getCurrentFrame()
								);
			
			// animate textures
			if(mesh->getTexturesAnim())
				animateTextures(mesh, mesh->getTexturesAnim(), entity->getCurrentFrame());
			
			// animate materials
			if(mesh->getMaterialsAnim())
				animateMaterials(mesh, mesh->getMaterialsAnim(), entity->getCurrentFrame());
			
			unsigned int s;
			unsigned int sSize = mesh->getSubMeshsNumber();
			for(s=0; s<sSize; s++)
			{
				MSubMesh * subMesh = &mesh->getSubMeshs()[s];
				MBox3d * box = subMesh->getBoundingBox();
				
				// check if submesh visible
				if(sSize > 1)
				{
					MVector3 * min = box->getMin();
					MVector3 * max = box->getMax();
					
					MVector3 points[8] = {
						entity->getTransformedVector(MVector3(min->x, min->y, min->z)),
						entity->getTransformedVector(MVector3(min->x, max->y, min->z)),
						entity->getTransformedVector(MVector3(max->x, max->y, min->z)),
						entity->getTransformedVector(MVector3(max->x, min->y, min->z)),
						entity->getTransformedVector(MVector3(min->x, min->y, max->z)),
						entity->getTransformedVector(MVector3(min->x, max->y, max->z)),
						entity->getTransformedVector(MVector3(max->x, max->y, max->z)),
						entity->getTransformedVector(MVector3(max->x, min->y, max->z))
					};
					
					if(! frustum->isVolumePointsVisible(points, 8))
						continue;
				}
				
				// subMesh center
				MVector3 center = (*box->getMin()) + ((*box->getMax()) - (*box->getMin()))*0.5f;
				center = entity->getTransformedVector(center);
				
				// sort entity lights
				unsigned int lightsNumber = 0;
				for(l=0; l<entityLightsNumber; l++)
				{
					MEntityLight * entityLight = &entityLights[l];
					if(! box->isInCollisionWith(&entityLight->lightBox))
						continue;
					
					entityLightsList[lightsNumber] = l;
					
					MOLight * light = entityLight->light;
					float z = (center - light->getTransformedPosition()).getLength();
					entityLightsZList[l] = (1.0f/z)*light->getRadius();
					
					lightsNumber++;
				}
				
				if(lightsNumber > 1)
					sortFloatList(entityLightsList, entityLightsZList, 0, (int)lightsNumber-1);
				
				// local lights
				if(lightsNumber > MAX_LOCAL_LIGHTS)
					lightsNumber = MAX_LOCAL_LIGHTS;
				
				for(l=0; l<lightsNumber; l++)
				{
					MEntityLight * entityLight = &entityLights[entityLightsList[l]];
					MOLight * light = entityLight->light;
					
					// attenuation
					float quadraticAttenuation = (8.0f / light->getRadius());
					quadraticAttenuation = (quadraticAttenuation*quadraticAttenuation)*light->getIntensity();
					
					// color
					MVector3 color = light->getFinalColor();
					
					// set light
					render->enableLight(l);
					render->setLightPosition(l, light->getTransformedPosition());
					render->setLightDiffuse(l, MVector4(color));
					render->setLightSpecular(l, MVector4(color));
					render->setLightAmbient(l, MVector3(0, 0, 0));
					render->setLightAttenuation(l, 1, 0, quadraticAttenuation);
					
					// spot
					render->setLightSpotAngle(l, light->getSpotAngle());
					if(light->getSpotAngle() < 90){
						render->setLightSpotDirection(l, light->getRotatedVector(MVector3(0, 0, -1)).getNormalized());
						render->setLightSpotExponent(l, light->getSpotExponent());
					}
					else {
						render->setLightSpotExponent(l, 0.0f);
					}
					
					// shadow
					if(light->isCastingShadow())
					{
						MShadowLight * shadowLight = &m_shadowLights[(unsigned long)(light)];
						m_lightShadow[l] = 1;
						m_lightShadowBias[l] = light->getShadowBias()*shadowLight->biasUnity;
						m_lightShadowBlur[l] = light->getShadowBlur();
						m_lightShadowTexture[l] = (int)shadowLight->shadowTexture;
						m_lightShadowMatrix[l] = shadowLight->shadowMatrix * (*entity->getMatrix());
					}
					else{
						m_lightShadow[l] = 0;
					}
				}
				
				for(l=lightsNumber; l<MAX_LOCAL_LIGHTS; l++){
					render->setLightDiffuse(l, MVector4(0, 0, 0, 0));
					render->disableLight(l);
					m_lightShadow[l] = 0;
				}
				
				render->pushMatrix();
				render->multMatrix(entity->getMatrix());
				
				// draw opaques
				drawOpaques(subMesh, mesh->getArmature());
				
				if(subMesh->hasTransparency())
				{
					if(transpSubObsNumber < MAX_TRANSP_SUBOBJ)
					{
						// transparent subMesh pass
						MSubMeshPass * subMeshPass = &transpSubObjs[transpSubObsNumber];
						
						// lights
						subMeshPass->lightsNumber = lightsNumber;
						for(l=0; l<lightsNumber; l++)
							subMeshPass->lights[l] = entityLights[entityLightsList[l]].light;
						
						// z distance to camera
						float z = getDistanceToCam(camera, center);
						
						// set values
						transpList[transpSubObsNumber] = transpSubObsNumber;
						transpZList[transpSubObsNumber] = z;
						subMeshPass->subMeshId = s;
						subMeshPass->object = entity;
						
						transpSubObsNumber++;
					}
				}
				
				render->popMatrix();
			}
			
			mesh->updateBoundingBox();
			(*entity->getBoundingBox()) = (*mesh->getBoundingBox());
		}
	}
	
	render->setDepthMask(1);
	render->setDepthMode(M_DEPTH_LEQUAL);
	
	// texts
	unsigned int tSize = scene->getTextsNumber();
	for(i=0; i<tSize; i++)
	{
		MOText * text = scene->getTextByIndex(i);
		if(text->isActive() && text->isVisible())
		{
			// transparent pass
			MSubMeshPass * subMeshPass = &transpSubObjs[transpSubObsNumber];
			
			// center
			MBox3d * box = text->getBoundingBox();
			MVector3 center = (*box->getMin()) + ((*box->getMax()) - (*box->getMin()))*0.5f;
			center = text->getTransformedVector(center);
			
			// z distance to camera
			float z = getDistanceToCam(camera, center);
			
			// set values
			transpList[transpSubObsNumber] = transpSubObsNumber;
			transpZList[transpSubObsNumber] = z;
			subMeshPass->object = text;
			
			transpSubObsNumber++;
		}
	}
	
	
	// sort transparent list
	if(transpSubObsNumber > 1)
		sortFloatList(transpList, transpZList, 0, (int)transpSubObsNumber-1);
	
	// draw transparents
	for(i=0; i<transpSubObsNumber; i++)
	{
		MSubMeshPass * subMeshPass = &transpSubObjs[transpList[i]];
		MObject3d * object = subMeshPass->object;
		
		// objects
		switch(object->getType())
		{
			case M_OBJECT3D_ENTITY:
			{
				MOEntity * entity = (MOEntity *)object;
				unsigned int subMeshId = subMeshPass->subMeshId;
				MMesh * mesh = entity->getMesh();
				MSubMesh * subMesh = &mesh->getSubMeshs()[subMeshId];
				
				// animate armature
				if(mesh->getArmature() && mesh->getArmatureAnim())
					animateArmature(
									mesh->getArmature(),
									mesh->getArmatureAnim(),
									entity->getCurrentFrame()
									);
				
				// animate textures
				if(mesh->getTexturesAnim())
					animateTextures(mesh, mesh->getTexturesAnim(), entity->getCurrentFrame());
				
				// animate materials
				if(mesh->getMaterialsAnim())
					animateMaterials(mesh, mesh->getMaterialsAnim(), entity->getCurrentFrame());
				
				// lights
				for(l=0; l<subMeshPass->lightsNumber; l++)
				{
					MOLight * light = subMeshPass->lights[l];
					
					// attenuation
					float quadraticAttenuation = (8.0f / light->getRadius());
					quadraticAttenuation = (quadraticAttenuation*quadraticAttenuation)*light->getIntensity();
					
					// color
					MVector3 color = light->getFinalColor();
					
					// set light
					render->enableLight(l);
					render->setLightPosition(l, light->getTransformedPosition());
					render->setLightDiffuse(l, MVector4(color));
					render->setLightSpecular(l, MVector4(color));
					render->setLightAmbient(l, MVector3(0, 0, 0));
					render->setLightAttenuation(l, 1, 0, quadraticAttenuation);
					
					// spot
					render->setLightSpotAngle(l, light->getSpotAngle());
					if(light->getSpotAngle() < 90){
						render->setLightSpotDirection(l, light->getRotatedVector(MVector3(0, 0, -1)).getNormalized());
						render->setLightSpotExponent(l, light->getSpotExponent());
					}
					else {
						render->setLightSpotExponent(l, 0.0f);
					}
					
					// shadow
					if(light->isCastingShadow())
					{
						MShadowLight * shadowLight = &m_shadowLights[(unsigned long)(light)];
						m_lightShadow[l] = 1;
						m_lightShadowBias[l] = light->getShadowBias()*shadowLight->biasUnity;
						m_lightShadowBlur[l] = light->getShadowBlur();
						m_lightShadowTexture[l] = (int)shadowLight->shadowTexture;
						m_lightShadowMatrix[l] = shadowLight->shadowMatrix * (*entity->getMatrix());
					}
					else{
						m_lightShadow[l] = 0;
					}
				}
				
				for(l=subMeshPass->lightsNumber; l<MAX_LOCAL_LIGHTS; l++){
					render->setLightDiffuse(l, MVector4(0, 0, 0, 0));
					render->disableLight(l);
					m_lightShadow[l] = 0;
				}
				
				render->pushMatrix();
				render->multMatrix(entity->getMatrix());
				drawTransparents(subMesh, mesh->getArmature());
				render->popMatrix();
				
				mesh->updateBoundingBox();
				(*entity->getBoundingBox()) = (*mesh->getBoundingBox());
			}
				break;
				
			case M_OBJECT3D_TEXT:
			{
				MOText * text = (MOText *)object;
				
				render->pushMatrix();
				render->multMatrix(text->getMatrix());
				drawText(text);
				render->popMatrix();
			}
				break;
		}
	}
	
	render->disableLighting();
	render->disableFog();	
}