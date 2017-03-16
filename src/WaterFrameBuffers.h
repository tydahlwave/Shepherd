#ifndef WaterFrameBuffers_h
#define WaterFrameBuffers_h

#include "GLSL.h"
#include "Window.h"
#include <iostream>

class WaterFrameBuffers {
public:
	WaterFrameBuffers();
	~WaterFrameBuffers();

	void bindReflectionFrameBuffer();
	void bindRefractionFrameBuffer();
	void unbindCurrentFrameBuffer();
	GLuint getReflectionTexture();
	GLuint getRefractionTexture();
	GLuint getRefractionDepthTexture();
	void initReflectionFrameBuffer();
	void initRefractionFrameBuffer();
	void createReflectionFrameBuffer();
	void createRefractionFrameBuffer();

	void cleanUp();

private:
	GLuint reflectionFrameBuffer;
	GLuint reflectionTexture;
	GLuint reflectionDepthBuffer;
	GLuint refractionFrameBuffer;
	GLuint refractionTexture;
	GLuint refractionDepthTexture;

	const int width = 1080;
	const int height = 920;

	GLuint texture;
	GLuint depthTexture;
	GLuint depthBuffer;
};

#endif