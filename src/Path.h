#ifndef Path_h
#define Path_h

#include "World.h"

class Path
{
public:
	Path();
	~Path();

	int size = 0;
	int radius = 10;
	void SetSize(int num);
	void AddNode(glm::vec3 node);
	std::vector<glm::vec3> GetNodes();
	
private:
	std::vector<glm::vec3> nodes;
};

#endif
