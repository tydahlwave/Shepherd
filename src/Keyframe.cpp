
#include <queue>
#include <cstring>
#include <iostream>
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
	std::cout << time << "\t" << dt << "\t" << kf.time << "\t" << std::endl;
	time += dt;
	if (time > kf.time) {
		if (next())
			return 2;
		time -= kf.time;
		ret = 1;
	}
	c->pos += (kfs.front().pos - last.pos) * (time / kf.time);
	c->aap += (kfs.front().aap - last.aap) * (time / kf.time);
	c->pitch += (kfs.front().pitch - last.pitch) * (time / kf.time);

	std::cout << last.pos.x << " " << last.pos.y << " " << last.pos.z << "\t" << c->pos.x << " " << c->pos.y << " " << c->pos.z << "\t" << kfs.front().pos.x << " " << kfs.front().pos.y << " " << kfs.front().pos.z << " " << std::endl;
	std::cout << last.aap << "\t" << c->aap << "\t" << kfs.front().aap << std::endl;
	std::cout << last.pitch << "\t" << c->pitch << "\t" << kfs.front().pitch << std::endl;
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