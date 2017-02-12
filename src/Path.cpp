#include "Path.h"



Path::Path()
{	
}


Path::~Path()
{
}

void Path::SetSize(int num) {
	size = num;
}

void Path::AddNode(glm::vec3 node) {
	nodes.push_back(node);
}

std::vector<glm::vec3> Path::GetNodes() {
	return nodes;
}
