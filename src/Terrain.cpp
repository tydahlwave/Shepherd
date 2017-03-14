
#include "Terrain.h"
#include <iostream>

#include "GLSL.h"
#include "Program.h"
#include "Noise/Noise.h"
#include "ImageLoader.h"
#include "Texture.h"
#include "Logger.h"

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

Terrain::Terrain(int size, NoiseProperties properties) :heightmapTex("Heightmap") {
    generate(size, properties);
}

Terrain::Terrain(std::string imagePath) :heightmapTex("Heightmap") {
    // Load the image data for the terrain
    ImageProperties imageProps = LoadImage(imagePath, &data, &width, &height);
    
    std::cout << "Terrain size: " << width << "x" << height << std::endl;
    
    // Load the heightmap into a texture
    heightmapTex.Load(data, imageProps);
}

Terrain::~Terrain() {}

void Terrain::generate(int size, NoiseProperties properties) {
    width = height = size;
    seed = properties.seed;
    
    // Generate the heighmap using a noise algorithm
    std::vector<std::vector<float>> heightMap;
    if (type == SIMPLEX_TERRAIN) {
        heightMap = Noise::GenerateSimplex(properties, size);
    } else if (type == DIAMOND_SQUARE_TERRAIN) {
        srand(properties.seed);
        heightMap = Noise::GenerateDiamondSquare(size);
    } else {
        Log(ERROR, "User attempted to generate terrain of unknown type.", __FILE__, __LINE__);
        return;
    }
    
    std::cout << "Terrain size: " << heightMap.size() << "x" << heightMap[0].size() << std::endl;
    
    // TODO: refactor the noise methods to return a 1D array
    std::vector<unsigned short> heightMapFlat;
    for (int i = 0; i < heightMap.size(); i++) {
        for (int j = 0; j < heightMap[i].size(); j++) {
            heightMapFlat.push_back((unsigned short)heightMap[j][i]);
        }
    }
    data = heightMapFlat.data();
    
    // Load the heightmap into a texture
    ImageProperties imageProps(width, height, 0, GL_R16F, GL_RED, GL_FLOAT);
    heightmapTex.Load(data, imageProps);
}

void Terrain::smooth(int iterations, int kernelSize) {
    std::vector<unsigned short> newData(width*height);
    
    // Set map values
    for (int steps = 0; steps < iterations; steps++) {
//        std::vector<float> newData(width*height);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int index = (y * width + x);
                newData[index + 0] = 0;
                newData[index + 1] = 0;
                newData[index + 2] = 0;
                int count = 0;
                for (int ky = -kernelSize/2; ky < kernelSize/2; ky++) {
                    for (int kx = -kernelSize/2; kx < kernelSize/2; kx++) {
                        if (y-ky >= 0 && y-ky < height && x-kx >= 0 && x-kx < width) {
                            count++;
                            newData[index + 0] += data[((y-ky)*width + (x-kx)) + 0];
                            newData[index + 1] += data[((y-ky)*width + (x-kx)) + 1];
                            newData[index + 2] += data[((y-ky)*width + (x-kx)) + 2];
                        }
                    }
                }
                newData[index + 0] /= count;
                newData[index + 1] /= count;
                newData[index + 2] /= count;
            }
        }
        data = newData.data();
    }
}

void Terrain::createMesh() {
    
//    smooth(3, 3);
    
    // Record max/min
    max = INT_MIN;
    min = INT_MAX;
    for (int col = 0; col < width; col++) {
        for (int row = 0; row < height; row++) {
            int index = row * width + col;
            if (data[index] > max) max = data[index];
            if (data[index] < min) min = data[index];
        }
    }
    
    // Reset buffers
    if (!vertices.empty()) vertices.clear();
    vertices.resize(width * height);
    for (int i = 0; i < width*height; i++) {
        vertices.push_back(TerrainVertex());
    }
    if (!indices.empty()) indices.clear();
    int totalIndices = 2*(width-1)*width + 2*(width-2); // total vertices + degenerate vertices
    indices.resize(totalIndices);
    for (int i = 0; i < totalIndices; i++) {
        indices.push_back(0);
    }
    
    // Calculate triangle vertex positions
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int index = row * width + col;
            float x = col - width/2.0f;
            float y = (float)data[index]/SHRT_MAX*64.0f;
            float z = row - height/2.0f;
            vertices[index].pos = glm::vec3(x, y, z);
        }
    }
    
    // Calculate triangle indices
    int ndx = 0;
    for (int row = 1; row < height; row++) {
        for (int col = 0; col < width; col++) {
            // Add the first vertex
            indices[ndx++] = row * width + col;
            // Add the second vertex
            indices[ndx++] = (row-1) * width + col;
        }
        
        // Add degenerate vertices when wrapping around that will not get rendered
        if (row < height - 1) {
            int currentIndex = ndx;
            indices[ndx++] = indices[currentIndex-1];
            indices[ndx++] = indices[currentIndex-2] + 1;
        } else {
            int currentIndex = ndx;
            indices[ndx++] = indices[currentIndex-1];
            indices[ndx++] = indices[currentIndex-1];
        }
    }
    
    ComputeNormals();
}

void Terrain::ComputeNormals() {
    // printf("eleBuf size: %lu\n", eleBuf[chunkIndex].size());
    for (size_t v = 0; v < indices.size() - 2; v++) {
        int idx1 = indices[v+0];
        int idx2 = indices[v+1];
        int idx3 = indices[v+2];
        
        // If any of the 3 indices are the same, skip this triangle because it is degenerate
        if (idx1 == idx2 || idx1 == idx3 || idx2 == idx3) continue;
        // printf("indices: %d %d %d\n", idx1, idx2, idx3);
        
        glm::vec3 p1 = vertices[idx1].pos;
        glm::vec3 p2 = vertices[idx2].pos;
        glm::vec3 p3 = vertices[idx3].pos;
        
        glm::vec3 vec1 = p2 - p1;
        glm::vec3 vec2 = p3 - p1;
        
        // Because I use GL_TRAINGLE_STRIP instead of GL_TRIANGLE,
        // every other triangle is rendered in the opposite direction,
        // which means I have to do the cross product backwards
        glm::vec3 normal;
        if (v % 2 == 0) { // If the triangle is even, do 2 cross 1
            normal = glm::cross(vec2, vec1);
        } else {          // If the triangle is odd,  do 1 cross 2
            normal = glm::cross(vec1, vec2);
        }
        // printf("Normal: %.3f %.3f %.3f\n", normal[0], normal[1], normal[2]);
        
        // Add this normal to all of the vertices
        vertices[idx1].nor = normal;
        vertices[idx2].nor = normal;
        vertices[idx3].nor = normal;
    }
    // Normalize the normal vectors
    for (size_t v = 0; v < indices.size() - 2; v++) {
        int idx1 = indices[v+0];
        int idx2 = indices[v+1];
        int idx3 = indices[v+2];
        
        glm::vec3 n1 = vertices[idx1].nor;
        glm::vec3 n2 = vertices[idx2].nor;
        glm::vec3 n3 = vertices[idx3].nor;
        
        glm::vec3 normalized1 = glm::normalize(n1);
        glm::vec3 normalized2 = glm::normalize(n2);
        glm::vec3 normalized3 = glm::normalize(n3);
        
        vertices[idx1].nor = normalized1;
        vertices[idx2].nor = normalized2;
        vertices[idx3].nor = normalized3;
    }
}

void Terrain::init() {
    
    // Initialize the vertex array object
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    // Send the vertex buffer to the GPU
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(TerrainVertex), &vertices[0], GL_STATIC_DRAW);
    
    // Send the element buffer to the GPU
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    
    // Enable vertex attributes for pos, nor, tex
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    
    // Set the vertex attributes for pos, nor, tex
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (GLvoid*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (GLvoid*)offsetof(TerrainVertex, nor));
    
    // Unbind the vertex array
    glBindVertexArray(0);
    
    // Log GL Error if it occurs
    LogGLError(__FILE__, __LINE__);
}

void Terrain::update() {
    
    // Send the vertex buffer to the GPU
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(TerrainVertex), &vertices[0], GL_STATIC_DRAW);
    
    // Log GL Error if it occurs
    LogGLError(__FILE__, __LINE__);
}

void Terrain::draw() {
    
    // Draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    // Log GL Error if it occurs
    LogGLError(__FILE__, __LINE__);
}

float Terrain::getHeight(int x, int y) {
//    Log(ERROR, "Unimplemented", __FILE__, __LINE__);
    return data[y * width + x];
}

void Terrain::setHeight(int x, int y, float height) {
    Log(ERROR, "Unimplemented", __FILE__, __LINE__);
}
