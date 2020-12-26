#include"Particle.h"
void Particle::setColor(Colour a, Colour b, float alpha) {
	color = Colour(	a.r * alpha + b.r * (1 - alpha),
					a.g * alpha + b.g * (1 - alpha),
					a.b * alpha + b.b * (1 - alpha),
					a.a * alpha + b.a * (1 - alpha));
}
