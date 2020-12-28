#include "Colour.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
using namespace glm;

class Particle {
public:
	Particle() {};
	glm::vec3 pos, speed;
	Colour color;
	float size;
	float life;				// Remaining life of the particle. if <0 : dead and unused.
	float cameradistance;	// *Squared* distance to the camera. if dead : -1.0f
	int target;
	int id;

	bool operator<(const Particle& that) const {
		// Sort in reverse order : far particles drawn first.
		return this->cameradistance > that.cameradistance;
	}
};
