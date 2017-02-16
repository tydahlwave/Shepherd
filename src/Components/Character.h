#ifndef Character_h
 #define Character_h


#include <iostream>
 #include <GLFW/glfw3.h>

#include "Component.h"

#include "../CameraController.h"
 #include "Camera.h"
 #include "RigidBody.h"

class Character : public Component {
	public:
		Character() { name = "Character"; };
		virtual ~Character() {};
		
		glm::vec3 lookAt = glm::vec3(0, 0, -1);
		glm::vec3 up = glm::vec3(0, 1, 0);
		glm::vec3 vel = glm::vec3(0, 0, 0);
		float dist = 10;
		//angle around player, radians
		float aap = 0;
		//turn speed, in degrees
		float currentTurnSpeed = 0;
		float currentMoveSpeed = 0;
};

#endif