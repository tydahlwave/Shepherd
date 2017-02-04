//
//  Noise.h
//  Shepherd
//
//  Created by Tyler Dahl on 2/3/17.
//
//

#ifndef Noise_h
#define Noise_h

#include <vector>

#include "OpenSimplexNoise.hh"
#include "glm/glm.hpp"

class Noise {
public:

/// Generate fractal noise using the Diamond-Square algorithm.
/// For best results, use sizes in the form of (2^pow)+1.
static std::vector<std::vector<float>> GenerateDiamondSquare(int size) {
    int maxStepSize = 256;
    std::vector<std::vector<float>> map;

    // Initialize map to all 0s
    for (int row = 0; row < size; row++) {
        std::vector<float> rowVector;
        for (int col = 0; col < size; col++) {
            rowVector.push_back(0);
        }
        map.push_back(rowVector);
    }
    
    // Clear map
//    for (int row = 0; row < size; row++) {
//        for (int col = 0; col < size; col++) {
//            map[row][col] = 0;
//        }
//    }
    
    // Randomize corners
    for (int row = 0; row < size; row += maxStepSize) { // 256
        for (int col = 0; col < size; col += maxStepSize) { // 256
            map[row][col] = (float)(rand() % maxStepSize);
        }
    }
    
    // Perform diamond-square
    //float max = 0;
    for (int stepSize = maxStepSize; stepSize > 1; stepSize /= 2) {
        int halfStepSize = stepSize/2;
        
        // Squares
        for (int row = stepSize; row < size; row += stepSize) { // 256
            for (int col = stepSize; col < size; col += stepSize) { // 256
                float h1 = map[row-stepSize][col-stepSize]; // 0, 0
                float h2 = map[row-stepSize][col]; // 0, 256
                float h3 = map[row][col-stepSize]; // 256, 0
                float h4 = map[row][col]; // 256, 256
                map[row-stepSize/2][col-stepSize/2] = (h1+h2+h3+h4)/4 + (rand() % halfStepSize)-halfStepSize/2;
            }
        }
        
        // Diamonds - top and left of every cell
        for (int row = stepSize; row < size; row += stepSize) { // 256
            for (int col = stepSize; col < size; col += stepSize) { // 256
                float h1 = map[row-stepSize][col-stepSize]; // 0, 0
                float h2 = map[row-stepSize][col]; // 0, 256
                float h3 = map[row][col-stepSize]; // 256, 0
                float h4 = map[row][col]; // 256, 256
                float middle = map[row-stepSize/2][col-stepSize/2];
                
                float topNeighbor = 0;
                int topCount = 3;
                if (col-3*stepSize/2 >= 0) {
                    topNeighbor = map[row-stepSize/2][col-3*stepSize/2];
                    topCount++;
                }
                float leftNeighbor = 0;
                int leftCount = 3;
                if (row-3*stepSize/2 >= 0) {
                    leftNeighbor = map[row-3*stepSize/2][col-stepSize/2];
                    leftCount++;
                }
                
                // top
                map[row-stepSize/2][col-stepSize] = (h1+middle+h3+topNeighbor)/topCount + (rand() % halfStepSize)-halfStepSize/2;
                // left
                map[row-stepSize][col-stepSize/2] = (h1+middle+h2+leftNeighbor)/leftCount + (rand() % halfStepSize)-halfStepSize/2;
            }
        }
        
        // Diamonds - bottom and right edges of map
        int lastRow = size-1;
        for (int col = stepSize; col < size; col += stepSize) {
            float h3 = map[lastRow][col-stepSize]; // 256, 0
            float h4 = map[lastRow][col]; // 256, 256
            float middle = map[lastRow-stepSize/2][col-stepSize/2];
            map[lastRow][col-stepSize/2] = (h3+middle+h4)/3 + (rand() % halfStepSize)-halfStepSize/2;
        }
        int lastCol = size-1;
        for (int row = stepSize; row < size; row += stepSize) {
            float h2 = map[row-stepSize][lastCol]; // 0, 256
            float h4 = map[row][lastCol]; // 256, 256
            float middle = map[row-stepSize/2][lastCol-stepSize/2];
            map[row-stepSize/2][lastCol] = (h2+middle+h4)/3 + (rand() % halfStepSize)-halfStepSize/2;
        }
    }
    
    // Re-scale
    float min = FLT_MAX;
    float max = FLT_MIN;
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            if (map[row][col] < min) min = map[row][col];
            if (map[row][col] > max) max = map[row][col];
        }
    }
    float range = max - min;
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            map[row][col] = (map[row][col] - min) / range * 255;
        }
    }
    
    return map;
}

//glm::mat2 Noise::generatePerlin(int size) {
//    
//}

static std::vector<std::vector<float>> GenerateSimplex(int size) {
    OSN::Noise<2> noise;
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
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            map[row][col] = noise.eval((float)row, (float)col);
        }
    }
    
    return map;
}
};

#endif /* Noise_h */
