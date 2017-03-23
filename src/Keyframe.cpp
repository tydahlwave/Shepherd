
#include <queue>
#include <cstring>
#include <iostream>
#include "Keyframe.h"
#include "Components/Camera.h"

void Keyframes::add(Keyframe k) {
	if (state == 0)
		last = k;
	else 
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
		c->pos = last.pos;
		c->aap = last.aap;
		c->pitch = last.pitch;
	}
	c->pos = last.pos + (kfs.front().pos - last.pos) * (time / kf.time);
	c->aap = last.aap + (kfs.front().aap - last.aap) * (time / kf.time);
	c->pitch = last.pitch + (kfs.front().pitch - last.pitch) * (time / kf.time);

	return ret;
}

//returns true if empy
bool Keyframes::next() {
	printf("NEXT KEYFRAME!\n");
	if (kfs.empty()) {
		state = 0;
		return true;
	}
	last = kfs.front();
	kfs.pop();
	return false;
}