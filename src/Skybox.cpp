#include "Skybox.h"

#include "ImageLoader.h"

Skybox::Skybox(std::string resourceDir)
{
	rd = resourceDir;
	faces.push_back("left.jpg");
	faces.push_back("right.jpg");
	faces.push_back("top.jpg");
	faces.push_back("bottom.jpg");
	faces.push_back("front.jpg");
	faces.push_back("back.jpg");
	loadCubeMap();
}


Skybox::~Skybox()
{
}

void Skybox::loadCubeMap() {
	glGenTextures(1, &cubeMapTexture);
	glActiveTexture(GL_TEXTURE2);

	int width, height;
	unsigned char *image;

	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
	
	for (GLuint i = 0; i < faces.size(); i++) {
        LoadImage((rd + faces[i]), &image, &width, &height, 0, 0);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        FreeImage(image);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
