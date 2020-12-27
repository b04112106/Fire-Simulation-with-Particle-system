#include <stdio.h>
#include <stdlib.h>
#include<iostream>
#include <vector>
#include <algorithm>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
using namespace glm;

#include "Particle.h"
#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>

#include <math.h> 

#define PI 3.1415926
/// <summary>
/// Class Particle system hasn't created yet
/// </summary>
/// 
const int MaxParticles = 100000;
Particle ParticlesContainer[MaxParticles];
int LastUsedParticle = 0;
int FindUnusedParticle() {

	for (int i = LastUsedParticle; i < MaxParticles; i++) {
		if (ParticlesContainer[i].life < 0) {
			LastUsedParticle = i;
			return i;
		}
	}
	for (int i = 0; i < LastUsedParticle; i++) {
		if (ParticlesContainer[i].life < 0) {
			LastUsedParticle = i;
			return i;
		}
	}
	return 0;
}

void SortParticles() {
	std::sort(&ParticlesContainer[0], &ParticlesContainer[MaxParticles]);
}

float getLength(vec3 l)
{
	return sqrt(l.x * l.x + l.y * l.y + l.z * l.z);
}
vec3 getDirection(vec3 l)
{
	float len = getLength(l);
	return vec3(l.x/len,l.y/len,l.z/len);
}