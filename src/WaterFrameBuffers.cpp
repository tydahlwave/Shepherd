#include "WaterFrameBuffers.h"



WaterFrameBuffers::WaterFrameBuffers() {
	initReflectionFrameBuffer();
	initRefractionFrameBuffer();
}


WaterFrameBuffers::~WaterFrameBuffers()
{
}

void WaterFrameBuffers::initReflectionFrameBuffer() {
	createReflectionFrameBuffer();
	unbindCurrentFrameBuffer();
}

void WaterFrameBuffers::initRefractionFrameBuffer() {
	createRefractionFrameBuffer();
	unbindCurrentFrameBuffer();
}

void WaterFrameBuffers::unbindCurrentFrameBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);
}

GLuint WaterFrameBuffers::getReflectionTexture() {
	return reflectionTexture;
}

GLuint WaterFrameBuffers::getRefractionTexture() {
	return refractionTexture;
}

GLuint WaterFrameBuffers::getRefractionDepthTexture() {
	return refractionDepthTexture;
}

void WaterFrameBuffers::bindReflectionFrameBuffer() {
	glBindTexture(GL_TEXTURE_2D, 0); // Makes sure the texture isn't bound
	glBindFramebuffer(GL_FRAMEBUFFER, reflectionFrameBuffer);
	glViewport(0, 0, width, height);
}

void WaterFrameBuffers::bindRefractionFrameBuffer() {
	glBindTexture(GL_TEXTURE_2D, 0); // Makes sure the texture isn't bound
	glBindFramebuffer(GL_FRAMEBUFFER, refractionFrameBuffer);
	glViewport(0, 0, width, height);
}

void WaterFrameBuffers::createReflectionFrameBuffer() {
	glGenFramebuffers(1, &reflectionFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, reflectionFrameBuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0); //specify the color buffer to be drawn into

	//Create Texture Attachment
	glGenTextures(1, &reflectionTexture);
	glBindTexture(GL_TEXTURE_2D, reflectionTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
		0, GL_RGB, GL_UNSIGNED_BYTE, nullptr); //specify a two-dimensional texture image
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTexture, 0);

	//Create Depth Buffer Attachment
	glGenRenderbuffers(1, &reflectionDepthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, reflectionDepthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height,
		0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, reflectionDepthBuffer);
}

void WaterFrameBuffers::createRefractionFrameBuffer() {
	glGenFramebuffers(1, &refractionFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, refractionFrameBuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT1); //specify the color buffer to be drawn into

	//Create Texture Attachment
	glGenTextures(1, &refractionTexture);
	glBindTexture(GL_TEXTURE_2D, refractionTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
		0, GL_RGB, GL_UNSIGNED_BYTE, nullptr); //specify a two-dimensional texture image
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, refractionTexture, 0);
	
	//Create Depth Texture Attachment
	glGenTextures(1, &refractionDepthTexture);
	glBindTexture(GL_TEXTURE_2D, refractionDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height,
		0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
		refractionDepthTexture, 0);
}

void WaterFrameBuffers::cleanUp() {
	glDeleteFramebuffers(1, &reflectionFrameBuffer);
	glDeleteTextures(1, &texture);
	glDeleteRenderbuffers(1, &depthBuffer);
	glDeleteFramebuffers(1, &refractionFrameBuffer);
	glDeleteTextures(1, &refractionTexture);
	glDeleteTextures(1, &refractionDepthTexture);
}
