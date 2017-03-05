#include "Terrain.h"
#include <iostream>

#include "GLSL.h"
#include "Program.h"
#include "Noise/Noise.h"
#include "TextureLoader.h"
#include "SOIL/Soil.h"

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

Terrain::Terrain() :
    eleBufID(0),
    posBufID(0),
    norBufID(0),
    texBufID(0),
    vaoID(0)
{}

Terrain::~Terrain() {}

void Terrain::Generate() {
    seed = time(0);
    GenerateHeightmap(seed);
        
    auto width = heightMap[0].size();
    auto height = heightMap.size();
    std::cout << "Terrain size: " << width << "x" << height << std::endl;
    
    UpdateBuffers();
    init();
}

void Terrain::Regenerate() {
    seed = time(0);
    GenerateHeightmap(seed);
    
    auto width = heightMap[0].size();
    auto height = heightMap.size();
    std::cout << "Terrain size: " << width << "x" << height << std::endl;
    
    UpdateBuffers();
    update();
}

void Terrain::GenerateHeightmap(time_t seed) {
    srand(seed);
    switch (type) {
        case 0:
            heightMap = Noise::GenerateSimplex(size);
            break;
        default:
            heightMap = Noise::GenerateDiamondSquare(size);
            break;
    }
}

void Terrain::GenerateHeightmap(NoiseProperties &properties, time_t seed) {
    srand(seed);
    switch (type) {
        case 0:
            heightMap = Noise::GenerateSimplex(properties, size);
            break;
        default:
            heightMap = Noise::GenerateDiamondSquare(size);
            break;
    }
}

void Terrain::GenerateFromImage(std::string imagePath) {
    int width = 0, height = 0, channels = 0;
    unsigned char* img = SOIL_load_image(imagePath.c_str(),
                                         &width, &height, &channels,
                                         SOIL_LOAD_AUTO);
    std::cout << SOIL_last_result() << std::endl;
    
    this->size = width;
    
    std::vector<std::vector<float>> map;
    // Initialize map to all 0s
    for (int row = 0; row < size; row++) {
        std::vector<float> rowVector;
        for (int col = 0; col < size; col++) {
            rowVector.push_back(0);
        }
        map.push_back(rowVector);
    }
    // Set map values
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
//            std::cout << img[y*size + x] << " = " << (float)img[y*size + x] << std::endl;
            
            map[x][y] = (float)img[y*size*3 + x*3] - 127.0f;
        }
    }
//    heightMap = map;
    heightMap = Noise::SmoothTerrain(map, size, 3, 5);
    
//    auto width = heightMap[0].size();
//    auto height = heightMap.size();
    std::cout << "Terrain size: " << size << "x" << size << std::endl;
    std::cout << "Image Channels: " << channels << std::endl;
    
    UpdateBuffers();
    init();
}

void Terrain::UpdateBuffers() {
    auto width = size;//heightMap[0].size();
    auto height = size;//heightMap.size();
    
    // Record max/min
    max = INT_MIN;
    min = INT_MAX;
    for (int i = 0; i < heightMap.size(); i++) {
        for (int j = 0; j < heightMap[i].size(); j++) {
            if (heightMap[j][i] > max) max = heightMap[j][i];
            if (heightMap[j][i] < min) min = heightMap[j][i];
        }
    }
    
    // Reset buffers
    posBuf.clear();
    norBuf.clear();
    eleBuf.clear();
    
    // Initialize position, normal, and texture buffers to 0s
    for (size_t v = 0; v < width*height*3; v++) {
        posBuf.push_back(0);
        norBuf.push_back(0);
        // texBuf.push_back(0);
    }
    
    // Initialize the element buffer to 0s
    int totalElements = 2*(width-1)*width + 2*(width-2); // total vertices + degenerate vertices
    for (size_t v = 0; v < totalElements; v++) {
        eleBuf.push_back(0);
    }
    
    // Calculate triangle vertex positions
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * width + x) * 3;
            posBuf[index + 0] = x - size/2.0f;
            posBuf[index + 1] = heightMap[x][y];// * 20;
            posBuf[index + 2] = y - size/2.0f;
            // printf("Point %d: %d %.3f %d\n", x+width*y, x, posBuf[chunkIndex][index + 1], y);
        }
    }
    
    // Populate the element buffer with the triangle vertices
    int ndx = 0;
    for (int y = 1; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Add the first vertex
            eleBuf[ndx++] = y * width + x;
            // Add the second vertex
            eleBuf[ndx++] = (y-1) * width + x;
        }
        // Add degenerate vertices when wrapping around that will not get rendered
        if (y < width - 1) {
            int currentIndex = ndx;
            eleBuf[ndx++] = eleBuf[currentIndex-1];
            eleBuf[ndx++] = eleBuf[currentIndex-2] + 1;
        }
    }
    
    ComputeNormals();
}

void Terrain::init() {
    
    // Initialize texture
    makeTexture();
    
    // Initialize the vertex array object
    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);
    
    // Send the position array to the GPU
    glGenBuffers(1, &posBufID);
    glBindBuffer(GL_ARRAY_BUFFER, posBufID);
    glBufferData(GL_ARRAY_BUFFER, posBuf.size()*sizeof(float), &posBuf[0], GL_STATIC_DRAW);

    // Send the normal array to the GPU
    if(norBuf.empty()) {
        norBufID = 0;
    } else {
        glGenBuffers(1, &norBufID);
        glBindBuffer(GL_ARRAY_BUFFER, norBufID);
        glBufferData(GL_ARRAY_BUFFER, norBuf.size()*sizeof(float), &norBuf[0], GL_STATIC_DRAW);
    }

    // Send the texture array to the GPU
    if(texBuf.empty()) {
        texBufID = 0;
    } else {
        glGenBuffers(1, &texBufID);
        glBindBuffer(GL_ARRAY_BUFFER, texBufID);
        glBufferData(GL_ARRAY_BUFFER, texBuf.size()*sizeof(float), &texBuf[0], GL_STATIC_DRAW);
    }

    // Send the element array to the GPU
    glGenBuffers(1, &eleBufID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, eleBuf.size()*sizeof(unsigned int), &eleBuf[0], GL_STATIC_DRAW);

    // Unbind the arrays
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    assert(glGetError() == GL_NO_ERROR);
}

void Terrain::update() {
    
    // Initialize texture
    makeTexture();
    
    // Send the position array to the GPU
    glBindBuffer(GL_ARRAY_BUFFER, posBufID);
    glBufferData(GL_ARRAY_BUFFER, posBuf.size()*sizeof(float), &posBuf[0], GL_STATIC_DRAW);
    
    // Send the normal array to the GPU
    glBindBuffer(GL_ARRAY_BUFFER, norBufID);
    glBufferData(GL_ARRAY_BUFFER, norBuf.size()*sizeof(float), &norBuf[0], GL_STATIC_DRAW);
    
    // Unbind the arrays
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    assert(glGetError() == GL_NO_ERROR);
}

void Terrain::makeTexture() {
//    if (texture) delete texture;
    texture = new TextureLoader();
    texture->setUnit(123);
    char *array = (char*) malloc(sizeof(char) * size * size * 3);
    float min = FLT_MAX;
    float max = FLT_MIN;
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            if (heightMap[row][col] < min) min = heightMap[row][col];
            if (heightMap[row][col] > max) max = heightMap[row][col];
        }
    }
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            char value = (char)((heightMap[col][row] - min) / (max-min) * 255);
            array[(row * size + col)*3 + 0] = value;
            array[(row * size + col)*3 + 1] = value;
            array[(row * size + col)*3 + 2] = value;
        }
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    TextureLoader::Image image = { size, size, array };
    texture->init(image);
}
 
void Terrain::draw(Program *prog) const {
    int h_pos, h_nor, h_tex;
    h_pos = h_nor = h_tex = -1;

    glBindVertexArray(vaoID);

    // Bind position buffer
    h_pos = prog->getAttribute("vertPos");
    GLSL::enableVertexAttribArray(h_pos);
    glBindBuffer(GL_ARRAY_BUFFER, posBufID);
    glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);

    // Bind normal buffer
    h_nor = prog->getAttribute("vertNor");
    if(h_nor != -1 && norBufID != 0) {
        GLSL::enableVertexAttribArray(h_nor);
        glBindBuffer(GL_ARRAY_BUFFER, norBufID);
        glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
    }

    if (texBufID != 0) {
        // Bind texcoords buffer
        h_tex = prog->getAttribute("vertTex");
        if(h_tex != -1 && texBufID != 0) {
            GLSL::enableVertexAttribArray(h_tex);
            glBindBuffer(GL_ARRAY_BUFFER, texBufID);
            glVertexAttribPointer(h_tex, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0);
        }
    }

    // Bind element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);

    // Draw
    glDrawElements(GL_TRIANGLE_STRIP, (int)eleBuf.size(), GL_UNSIGNED_INT, (const void *)0);
    GLenum err_code = glGetError();
    if (err_code != 0) std::cout << "Terrain Draw Error: " << err_code << std::endl;
   
    // Disable and unbind
    if(h_tex != -1) {
        GLSL::disableVertexAttribArray(h_tex);
    }
    if(h_nor != -1) {
        GLSL::disableVertexAttribArray(h_nor);
    }
    GLSL::disableVertexAttribArray(h_pos);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Terrain::ComputeNormals() {
    // printf("eleBuf size: %lu\n", eleBuf[chunkIndex].size());
    for (size_t v = 0; v < eleBuf.size() - 2; v++) {
        int idx1 = eleBuf[v+0];
        int idx2 = eleBuf[v+1];
        int idx3 = eleBuf[v+2];
        
        // If any of the 3 indices are the same, skip this triangle because it is degenerate
        if (idx1 == idx2 || idx1 == idx3 || idx2 == idx3) continue;
        // printf("indices: %d %d %d\n", idx1, idx2, idx3);
        
        glm::vec3 p1(posBuf[3*idx1+0], posBuf[3*idx1+1], posBuf[3*idx1+2]);
        glm::vec3 p2(posBuf[3*idx2+0], posBuf[3*idx2+1], posBuf[3*idx2+2]);
        glm::vec3 p3(posBuf[3*idx3+0], posBuf[3*idx3+1], posBuf[3*idx3+2]);
        // printf("P1: %.3f %.3f %.3f\n", p1[0], p1[1], p1[2]);
        // printf("P2: %.3f %.3f %.3f\n", p2[0], p2[1], p2[2]);
        // printf("P3: %.3f %.3f %.3f\n", p3[0], p3[1], p3[2]);
        
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
        norBuf[3*idx1+0] += normal[0];
        norBuf[3*idx1+1] += normal[1];
        norBuf[3*idx1+2] += normal[2];
        norBuf[3*idx2+0] += normal[0];
        norBuf[3*idx2+1] += normal[1];
        norBuf[3*idx2+2] += normal[2];
        norBuf[3*idx3+0] += normal[0];
        norBuf[3*idx3+1] += normal[1];
        norBuf[3*idx3+2] += normal[2];
    }
    // Normalize the normal vectors
    for (size_t v = 0; v < eleBuf.size() - 2; v++) {
        int idx1 = eleBuf[v+0];
        int idx2 = eleBuf[v+1];
        int idx3 = eleBuf[v+2];
        
        glm::vec3 n1(norBuf[3*idx1+0], norBuf[3*idx1+1], norBuf[3*idx1+2]);
        glm::vec3 n2(norBuf[3*idx2+0], norBuf[3*idx2+1], norBuf[3*idx2+2]);
        glm::vec3 n3(norBuf[3*idx3+0], norBuf[3*idx3+1], norBuf[3*idx3+2]);
        
        glm::vec3 normalized1 = glm::normalize(n1);
        glm::vec3 normalized2 = glm::normalize(n2);
        glm::vec3 normalized3 = glm::normalize(n3);
        
        norBuf[3*idx1+0] = normalized1[0];
        norBuf[3*idx1+1] = normalized1[1];
        norBuf[3*idx1+2] = normalized1[2];
        norBuf[3*idx2+0] = normalized2[0];
        norBuf[3*idx2+1] = normalized2[1];
        norBuf[3*idx2+2] = normalized2[2];
        norBuf[3*idx3+0] = normalized3[0];
        norBuf[3*idx3+1] = normalized3[1];
        norBuf[3*idx3+2] = normalized3[2];
    }
}

//float Terrain::getHeight(int posX, int posZ) {
//    // If less than 0, add chunksize to make the point at least 1 chunk negative
//    // Otherwise we end up with chunk 0 and chunk -0, which are both treated as just 0
//    if (posX < 0) posX -= (chunkSize-1);
//    if (posZ < 0) posZ -= (chunkSize-1);
//    // Get the terrain chunk for the given position
//    int chunkX = (int)(posX / (chunkSize-1));
//    int chunkZ = (int)(posZ / (chunkSize-1));
//
//    chunkX = chunkX - referenceChunkX;
//    chunkZ = chunkZ - referenceChunkZ;
//
//    if (abs(chunkX) > 2 || abs(chunkZ) > 2) return 0;
//
//    posX = posX % (chunkSize-1);
//    posZ = posZ % (chunkSize-1);
//    float posY = posBuf[chunkX + 2 + (chunkZ + 2) * chunkRows][(posX + posZ * (chunkSize-1)) * 3 + 1];
//    // printf("Chunk [%d %d] ", chunkX + 2, chunkZ + 2);
//    // printf("pos [%d %d] ", posX, posZ);
//    // printf("posY: %.3f\n", posY);
//    return posY;
//}
