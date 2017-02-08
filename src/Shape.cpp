#include "Shape.h"
#include <iostream>
#include <cfloat>
#include <vector>

#include "GLSL.h"
#include "Program.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include "TextureLoader.h"

Shape::Shape() :
	eleBufID(0),
	posBufID(0),
	norBufID(0),
	texBufID(0), 
    vaoID(0)
{}

Shape::~Shape() {}

void MeshEntry::Init(const std::vector<Vertex> &Vertices, const std::vector<unsigned int> &Indices) {
    // Initialize the vertex array object
    unsigned vaoID;
    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);
    
    // Send the vertex array to the GPU
    glGenBuffers(1, &VB);
    glBindBuffer(GL_ARRAY_BUFFER, VB);
    glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);
    
    // Send the element array to the GPU
    glGenBuffers(1, &IB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size()*sizeof(unsigned int), &Indices[0], GL_STATIC_DRAW);
    
    // Unbind the arrays
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    NumIndices = Indices.size();
    
    assert(glGetError() == GL_NO_ERROR);
}

void Shape::loadMesh(const std::string &filePath)
{
//	// Load geometry
//	// Some obj files contain material information.
//	// We'll ignore them for this assignment.
//	std::vector<tinyobj::shape_t> shapes;
//	std::vector<tinyobj::material_t> objMaterials;
//	std::string errStr;
//	bool rc = tinyobj::LoadObj(shapes, objMaterials, errStr, filePath.c_str());
//	if(!rc) {
//        std::cerr << errStr << std::endl;
//	} else {
//		posBuf = shapes[0].mesh.positions;
//		// norBuf = shapes[0].mesh.normals;
//		texBuf = shapes[0].mesh.texcoords;
//		eleBuf = shapes[0].mesh.indices;
//
//        // Initialize normal array to 0s
//        for (size_t v = 0; v < posBuf.size(); v++) {
//            norBuf.push_back(0);
//        }
//
//        computeNormals();
//	}
    
    bounds = new Bounds(glm::vec3(-1, -1, -1), glm::vec3(1, 1, 1));
    
    // Create an instance of the Importer class
    Assimp::Importer importer;
    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // propably to request more postprocessing than we do in this example.
    const aiScene* scene = importer.ReadFile( filePath,
                                             aiProcess_CalcTangentSpace       |
                                             aiProcess_Triangulate            |
                                             aiProcess_JoinIdenticalVertices  |
                                             aiProcess_GenSmoothNormals       |
                                             aiProcess_SortByPType);
    if (!scene) {
        printf("Error parsing '%s': '%s'\n", filePath.c_str(), importer.GetErrorString());
    } else {
//        std::vector<MeshEntry> m_Entries;
//        std::vector<TextureLoader*> m_Textures;
        
//        InitFromScene(scene, filePath);
        m_Entries.resize(scene->mNumMeshes);
        m_Textures.resize(scene->mNumMaterials);
        
        // Initialize the meshes in the scene one by one
        for (unsigned int i = 0 ; i < m_Entries.size() ; i++) {
            const aiMesh* paiMesh = scene->mMeshes[i];
//            InitMesh(i, paiMesh);
            m_Entries[i].MaterialIndex = paiMesh->mMaterialIndex;
            
            std::vector<Vertex> Vertices;
            std::vector<unsigned int> Indices;
            const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
            
            for (unsigned int i = 0 ; i < paiMesh->mNumVertices ; i++) {
                const aiVector3D* pPos = &(paiMesh->mVertices[i]);
                const aiVector3D* pNormal = paiMesh->HasNormals() ? &(paiMesh->mNormals[i]) : &Zero3D;
                const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;
                
                Vertex v(glm::vec3(pPos->x, pPos->y, pPos->z),
                         glm::vec3(pNormal->x, pNormal->y, pNormal->z),
                         glm::vec2(pTexCoord->x, pTexCoord->y));
                
                Vertices.push_back(v);
            }
            for (unsigned int i = 0 ; i < paiMesh->mNumFaces ; i++) {
                const aiFace& Face = paiMesh->mFaces[i];
                assert(Face.mNumIndices == 3);
                Indices.push_back(Face.mIndices[0]);
                Indices.push_back(Face.mIndices[1]);
                Indices.push_back(Face.mIndices[2]);
            }
            m_Entries[i].Init(Vertices, Indices);
        }
        
//        return InitMaterials(scene, filePath);
        for (unsigned int i = 0 ; i < scene->mNumMaterials ; i++) {
            const aiMaterial* pMaterial = scene->mMaterials[i];
            m_Textures[i] = NULL;
            if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
                aiString Path;
                
                if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
//                    std::string FullPath = Dir + "/" + Path.data;
//                    m_Textures[i] = new Texture(GL_TEXTURE_2D, filePath.c_str());
                    TextureLoader *textureLoader = new TextureLoader();
                    textureLoader->setFilename(filePath);
                    m_Textures[i] = textureLoader;
                    textureLoader->init();
                    
//                    if (!m_Textures[i]->Load()) {
//                        printf("Error loading texture '%s'\n", filePath.c_str());
//                        delete m_Textures[i];
//                        m_Textures[i] = NULL;
////                        Ret = false;
//                    }
                }
            }
            if (!m_Textures[i]) {
//                m_Textures[i] = new Texture(GL_TEXTURE_2D, "../Content/white.png");
//                Ret = m_Textures[i]->Load();
                
//                TextureLoader *textureLoader = new TextureLoader();
//                textureLoader->setFilename(filePath);
//                textureLoader->init();
            }
        }
        
//        return Ret;
    }
}

void Shape::resize() {
    float minX, minY, minZ;
    float maxX, maxY, maxZ;
    float scaleX, scaleY, scaleZ;
    float shiftX, shiftY, shiftZ;
    float epsilon = 0.001;

    minX = minY = minZ = 1.1754E+38F;
    maxX = maxY = maxZ = -1.1754E+38F;

    //Go through all vertices to determine min and max of each dimension
    for (size_t v = 0; v < posBuf.size() / 3; v++) {
		if(posBuf[3*v+0] < minX) minX = posBuf[3*v+0];
		if(posBuf[3*v+0] > maxX) maxX = posBuf[3*v+0];

		if(posBuf[3*v+1] < minY) minY = posBuf[3*v+1];
		if(posBuf[3*v+1] > maxY) maxY = posBuf[3*v+1];

		if(posBuf[3*v+2] < minZ) minZ = posBuf[3*v+2];
		if(posBuf[3*v+2] > maxZ) maxZ = posBuf[3*v+2];
	}

	//From min and max compute necessary scale and shift for each dimension
    float maxExtent, xExtent, yExtent, zExtent;
    xExtent = maxX-minX;
    yExtent = maxY-minY;
    zExtent = maxZ-minZ;
    if (xExtent >= yExtent && xExtent >= zExtent) {
        maxExtent = xExtent;
    }
    if (yExtent >= xExtent && yExtent >= zExtent) {
        maxExtent = yExtent;
    }
    if (zExtent >= xExtent && zExtent >= yExtent) {
        maxExtent = zExtent;
    }
    scaleX = 2.0 /maxExtent;
    shiftX = minX + (xExtent/ 2.0);
    scaleY = 2.0 / maxExtent;
    shiftY = minY + (yExtent / 2.0);
    scaleZ = 2.0/ maxExtent;
    shiftZ = minZ + (zExtent)/2.0;

    //Go through all verticies shift and scale them
	for (size_t v = 0; v < posBuf.size() / 3; v++) {
		posBuf[3*v+0] = (posBuf[3*v+0] - shiftX) * scaleX;
		assert(posBuf[3*v+0] >= -1.0 - epsilon);
		assert(posBuf[3*v+0] <= 1.0 + epsilon);
		posBuf[3*v+1] = (posBuf[3*v+1] - shiftY) * scaleY;
		assert(posBuf[3*v+1] >= -1.0 - epsilon);
		assert(posBuf[3*v+1] <= 1.0 + epsilon);
		posBuf[3*v+2] = (posBuf[3*v+2] - shiftZ) * scaleZ;
		assert(posBuf[3*v+2] >= -1.0 - epsilon);
		assert(posBuf[3*v+2] <= 1.0 + epsilon);
	}
}

/* Note this is fairly dorky - */
void Shape::ComputeTex() {
    float u, v;
    
    for (size_t n = 0; n < norBuf.size()/3; n++) {
        u = norBuf[n*3+0]/2.0 + 0.5;
        v = norBuf[n*3+1]/2.0 + 0.5;
        texBuf[n*3+0] = u;
        texBuf[n*3+1] = v;
    }
}

void Shape::init()
{
//    // Compute bounds
//    computeBounds();
//    
//    // Initialize the vertex array object
//    glGenVertexArrays(1, &vaoID);
//    glBindVertexArray(vaoID);
//
//	// Send the position array to the GPU
//	glGenBuffers(1, &posBufID);
//	glBindBuffer(GL_ARRAY_BUFFER, posBufID);
//	glBufferData(GL_ARRAY_BUFFER, posBuf.size()*sizeof(float), &posBuf[0], GL_STATIC_DRAW);
//	
//	// Send the normal array to the GPU
//	if(norBuf.empty()) {
//		norBufID = 0;
//	} else {
//		glGenBuffers(1, &norBufID);
//		glBindBuffer(GL_ARRAY_BUFFER, norBufID);
//		glBufferData(GL_ARRAY_BUFFER, norBuf.size()*sizeof(float), &norBuf[0], GL_STATIC_DRAW);
//	}
//	
//    // Send the texture array to the GPU
//    if(texBuf.empty()) {
//        //texBufID = 0;
//        //send in spherical constructed
//        for (size_t v = 0; v < posBuf.size(); v++) {
//            texBuf.push_back(0);
//        }
//        ComputeTex();
//	} else {
//		glGenBuffers(1, &texBufID);
//		glBindBuffer(GL_ARRAY_BUFFER, texBufID);
//		glBufferData(GL_ARRAY_BUFFER, texBuf.size()*sizeof(float), &texBuf[0], GL_STATIC_DRAW);
//	}
//	
//	// Send the element array to the GPU
//	glGenBuffers(1, &eleBufID);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, eleBuf.size()*sizeof(unsigned int), &eleBuf[0], GL_STATIC_DRAW);
//	
//	// Unbind the arrays
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//	
//	assert(glGetError() == GL_NO_ERROR);
}
 
void Shape::draw(Program *prog) const
{
//	int h_pos, h_nor, h_tex;
//	h_pos = h_nor = h_tex = -1;
//
//    glBindVertexArray(vaoID);
//	// Bind position buffer
//	h_pos = prog->getAttribute("vertPos");
//	GLSL::enableVertexAttribArray(h_pos);
//	glBindBuffer(GL_ARRAY_BUFFER, posBufID);
//	glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
//	
//	// Bind normal buffer
//	h_nor = prog->getAttribute("vertNor");
//	if (h_nor != -1 && norBufID != 0) {
//		GLSL::enableVertexAttribArray(h_nor);
//		glBindBuffer(GL_ARRAY_BUFFER, norBufID);
//		glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
//	}
//
//	if (texBufID != 0) {	
//		// Bind texcoords buffer
//		h_tex = prog->getAttribute("vertTex");
//		if (h_tex != -1 && texBufID != 0) {
//			GLSL::enableVertexAttribArray(h_tex);
//			glBindBuffer(GL_ARRAY_BUFFER, texBufID);
//			glVertexAttribPointer(h_tex, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0);
//		}
//	}
//
//	// Bind element buffer
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);
//	
//	// Draw
//	glDrawElements(GL_TRIANGLES, (int)eleBuf.size(), GL_UNSIGNED_INT, (const void *)0);
//	
//	// Disable and unbind
//	if (h_tex != -1) {
//		GLSL::disableVertexAttribArray(h_tex);
//	}
//	if (h_nor != -1) {
//		GLSL::disableVertexAttribArray(h_nor);
//	}
//	GLSL::disableVertexAttribArray(h_pos);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    
    for (unsigned int i = 0 ; i < m_Entries.size() ; i++) {
        glBindBuffer(GL_ARRAY_BUFFER, m_Entries[i].VB);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);                 // Positions
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12); // Normals
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)24); // Textures
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Entries[i].IB);
        
        const unsigned int MaterialIndex = m_Entries[i].MaterialIndex;
        
        if (MaterialIndex < m_Textures.size() && m_Textures[MaterialIndex]) {
//            m_Textures[MaterialIndex]->Bind(GL_TEXTURE0);
            m_Textures[MaterialIndex]->bind();
        }
        
        glDrawElements(GL_TRIANGLES, m_Entries[i].NumIndices, GL_UNSIGNED_INT, 0);
    }
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void Shape::computeNormals() {
    for (size_t v = 0; v < eleBuf.size()/3; v++) {
        int idx1 = eleBuf[3*v+0];
        int idx2 = eleBuf[3*v+1];
        int idx3 = eleBuf[3*v+2];
        
        glm::vec3 p1(posBuf[3*idx1+0], posBuf[3*idx1+1], posBuf[3*idx1+2]);
        glm::vec3 p2(posBuf[3*idx2+0], posBuf[3*idx2+1], posBuf[3*idx2+2]);
        glm::vec3 p3(posBuf[3*idx3+0], posBuf[3*idx3+1], posBuf[3*idx3+2]);
        
        glm::vec3 vec1 = p2 - p1;
        glm::vec3 vec2 = p3 - p1;
        
        glm::vec3 normal = cross(vec1, vec2);
        
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
    for (size_t v = 0; v < eleBuf.size()/3; v++) {
        int idx1 = eleBuf[3*v+0];
        int idx2 = eleBuf[3*v+1];
        int idx3 = eleBuf[3*v+2];
        
        glm::vec3 n1(norBuf[3*idx1+0], norBuf[3*idx1+1], norBuf[3*idx1+2]);
        glm::vec3 n2(norBuf[3*idx2+0], norBuf[3*idx2+1], norBuf[3*idx2+2]);
        glm::vec3 n3(norBuf[3*idx3+0], norBuf[3*idx3+1], norBuf[3*idx3+2]);
        
        glm::vec3 normalized1 = normalize(n1);
        glm::vec3 normalized2 = normalize(n2);
        glm::vec3 normalized3 = normalize(n3);
        
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

void Shape::computeBounds() {
    glm::vec3 min = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
    glm::vec3 max = glm::vec3(FLT_MIN, FLT_MIN, FLT_MIN);

    // Go through all vertices to determine min and max of each dimension
    for (size_t v = 0; v < posBuf.size() / 3; v++) {
        if (posBuf[3 * v + 0] < min.x) min.x = posBuf[3 * v + 0];
        if (posBuf[3 * v + 0] > max.x) max.x = posBuf[3 * v + 0];

        if (posBuf[3 * v + 1] < min.y) min.y = posBuf[3 * v + 1];
        if (posBuf[3 * v + 1] > max.y) max.y = posBuf[3 * v + 1];

        if (posBuf[3 * v + 2] < min.z) min.z = posBuf[3 * v + 2];
        if (posBuf[3 * v + 2] > max.z) max.z = posBuf[3 * v + 2];
    }
    
    bounds = new Bounds(min, max);
}
