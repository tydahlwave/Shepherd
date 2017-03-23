
#ifndef Keyframe_h
#define Keyframe_h

#include <glm/glm.hpp>
#include <queue>
class Camera;

class Keyframe {
public:
	float time;
	glm::vec3 pos;
	float aap;
	float pitch;
};

class Keyframes {
public:
	std::queue<Keyframe> kfs;
	Keyframe last;
	int state = 0;
	float time = 0;

	void Keyframes::add(Keyframe k);

	//returns 1 if keyframe jumps
	//returns 2 if no more kfs
	//returns 0 if else
	int Keyframes::update(Camera * c, float dt);

	//returns true if empy
	bool Keyframes::next();

};

#endif