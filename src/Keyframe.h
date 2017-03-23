
#ifndef Keyframe_h
#define Keyframe_h

#include <glm/glm.hpp>
#include <queue>
class Camera;

class Keyframe {
public:
	//time in ms it holds for
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

	void add(Keyframe k);

	//returns 1 if keyframe jumps
	//returns 2 if no more kfs
	//returns 0 if else
	int update(Camera * c, float dt);

	//returns true if empy
	bool next();

	//holds last keyframe for x seconds
	void hold(float time);

};

#endif
