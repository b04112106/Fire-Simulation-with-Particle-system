#include <stdio.h>
#include <stdlib.h>
#include<iostream>
#include <vector>
#include <algorithm>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <omp.h>
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

const int MaxParticles = 100000;
Particle ParticlesContainer[MaxParticles];
bool ManuallyAdjustLowPressPoint = false;
bool UseLowPress = true;
bool EnableWindEffect = false;
bool UseBigPattern = true;
vec3 OriLowPress[3][3] = {
	{vec3(2.0f, 3, -20.0f),vec3(0, 3, -20.0f),vec3(-2.0f, 3, -20.0f)},
	{vec3(2.0f, 6, -20.0f), vec3(0, 6, -20.0f), vec3(-2.0f, 6, -20.0f)},
	{vec3(0, 8, -20.0f),vec3(0, 8, -20.0f),vec3(0, 8, -20.0f)}
};
vec3 LowPress[3][3] = {
	{vec3(2.0f, 3, -20.0f),vec3(0, 3, -20.0f),vec3(-2.0f, 3, -20.0f)},
	{vec3(2.0f, 6, -20.0f), vec3(0, 6, -20.0f), vec3(-2.0f, 6, -20.0f)},
	{vec3(0, 8, -20.0f),vec3(0, 8, -20.0f),vec3(0, 8, -20.0f)}
};
float heightOfLowPress[3] = { 1.5f,3,4 };
float level1_radius = 0.25f;
float level2_radius = 0.125f;
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
	return -1;
}
void SortParticles() {
	std::sort(&ParticlesContainer[0], &ParticlesContainer[MaxParticles]);
}
float getLength(vec3 l){
	return sqrt(l.x * l.x + l.y * l.y + l.z * l.z);
}
vec3 getDirection(vec3 l){
	float len = getLength(l);
	return vec3(l.x/len,l.y/len,l.z/len);
}
class emitter {
private:
	int id;
	vec3 InitialPosition;
	float InitialPositionRadius;
	vec3 InitialVelocity;
	float randV_Xmax, randV_Xmin;
	float randV_Ymax, randV_Ymin;
	float randV_Zmax, randV_Zmin;
	int ParticlesPerSecond;
	unsigned char InitialAlpha;
	Colour InitialColor;
	float InitialSize;
	float InitLife;
	float forceOfPress;
	float BurstRate;
public:
	emitter(int _id, vec3 pos, float r, vec3 initvel, float Mx, float mx, float My, float my, float Mz, float mz, 
			int pps,unsigned char initAlpha, Colour initC, float size, float Life, float force, float BRate) :
		id(_id), InitialPosition(pos), InitialPositionRadius(r), InitialVelocity(initvel), randV_Xmax(Mx), randV_Xmin(mx), randV_Ymax(My), randV_Ymin(my), randV_Zmax(Mz), randV_Zmin(mz),
		ParticlesPerSecond(pps), InitialAlpha(initAlpha),InitialColor(initC), InitialSize(size), InitLife(Life), forceOfPress(force), BurstRate(BRate){}
	void showInformation()
	{
		std::cout << "size/Life" << InitialSize << "/" << InitLife << std::endl;
		std::cout << "Colour:" << (int)InitialColor.r << " | " << (int)InitialColor.g << " | " << (int)InitialColor.b << " | " << (int)InitialColor.a << std::endl;
		std::cout << "Alpha:" << (int)InitialAlpha<< std::endl;
	}
	void AddNewParticles(double delta) {
		int newparticles = (int)(delta * ParticlesPerSecond * BurstRate);
		if (newparticles > (int)(0.050f * ParticlesPerSecond * BurstRate))
			newparticles = (int)(0.050f * ParticlesPerSecond * BurstRate);
		for (int i = 0; i < newparticles; i++) {
			// might be bug
			int particleIndex = LastUsedParticle = FindUnusedParticle();
			if (particleIndex < 0)return;
			ParticlesContainer[particleIndex].id = id;
			ParticlesContainer[particleIndex].life = InitLife - (float)((rand() % 100) / 300.0);

			float now_life = ParticlesContainer[particleIndex].life;
			float randT = (float)(rand() % 360);
			float r = (float)(rand() % 100 / 100.0) * InitialPositionRadius;
			ParticlesContainer[particleIndex].pos = glm::vec3(r * cos(randT * PI / 180.0) + InitialPosition.x, r * sin(randT * PI / 180.0) / 1.2f + InitialPosition.y, InitialPosition.z);
			glm::vec3 maindir = InitialVelocity;
			glm::vec3 randomDir = glm::vec3(
				(rand() % (int)((randV_Xmax - randV_Xmin) * 100.0f) + 100.0f * randV_Xmin) / 100.0f,
				(rand() % (int)((randV_Ymax - randV_Ymin) * 100.0f) + 100.0f * randV_Ymin) / 100.0f,
				(rand() % (int)((randV_Zmax - randV_Zmin) * 100.0f) + 100.0f * randV_Zmin) / 100.0f
			);
			ParticlesContainer[particleIndex].target = rand() % 3;

			ParticlesContainer[particleIndex].speed = (maindir + randomDir) * BurstRate;

			ParticlesContainer[particleIndex].color = InitialColor;

			ParticlesContainer[particleIndex].size = InitialSize * now_life / InitLife;
		}
	}
	void Simulate(double delta, Particle &p,vec3 CameraPosition) {
		if (p.life > 0.0f) {
			// Decrease life
			p.life -= delta;
			if (p.life > 0.0f) {
				// Update Colour
				p.color.a = (p.life) / this->InitLife * this->InitialAlpha;
				p.size = (p.life) / this->InitLife * this->InitialSize;
				p.color.g = this->InitialColor.g * (p.life) / this->InitLife;
				p.color.g *= ((256 - pow(2, 8 * (1 - (p.life) / InitLife))) / 256.0);

				// Update Position
				float last_y = p.pos.y;
				p.pos += p.speed * (float)delta;
				p.pos += vec3((rand() % 100 - 50) / 1000.0f, (rand() % 100 - 50) / 1000.0f, (rand() % 100 - 50) / 1000.0f);

				// Update Velocity					
				float l = getLength(p.speed);
				float temp = p.speed.y;
				if (UseLowPress) {
					if (p.pos.y < LowPress[0][0].y) {
						p.speed += getDirection(LowPress[0][p.target] - p.pos) * (float)delta * forceOfPress * BurstRate;
					}
					else if (p.pos.y < LowPress[1][0].y) {
						if (last_y < LowPress[0][0].y)
						{
							p.target = rand() % 3;
						}
						p.speed += getDirection(LowPress[1][p.target] - p.pos) * (float)delta * forceOfPress * BurstRate;
					}
					else
						p.speed += getDirection(LowPress[2][0] - p.pos) * (float)delta * forceOfPress * BurstRate;
				}

				p.speed.y = temp  + (1 - (p.life) / this->InitLife * (p.life) / this->InitLife) * delta;  // y velocity remains the same
				float ll = getLength(p.speed);
				p.speed.x *= (l / ll);
				p.speed.z *= (l / ll);

				p.cameradistance = glm::length2(p.pos - CameraPosition);
			}else {
				// Particles that just died will be put at the end of the buffer in SortParticles();
				p.cameradistance = -1.0f;
			}			
		}
	}
};