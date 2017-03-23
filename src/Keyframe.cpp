
#include <queue>

#include "Keyframe.h"
#include "Components/Camera.h"

void Keyframes::add(Keyframe k) {
	kfs.push(k);
	state = 1;
}

//returns 1 if keyframe jumps
//returns 2 if no more kfs
//returns 0 if else
int Keyframes::update(Camera * c, float dt) {
	if (state == 0)
		return 0;
	int ret = 0;
	Keyframe kf = kfs.front();
	time += dt;
	if (time > kf.time) {
		if (next())
			return 2;
		time -= kf.time;
		ret = 1;
	}
	c->pos += (kfs.front().pos - last.pos) * time;
	c->aap += (kfs.front().aap - last.aap) * time;
	c->pitch += (kfs.front().pitch - last.pitch) * time;
	return ret;
}

//returns true if empy
bool Keyframes::next() {
	if (kfs.empty()) {
		state = 0;
		return true;
	}
	last = kfs.front();
	kfs.pop();
	return false;
}