#include <stdio.h>
#include <stdlib.h>

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