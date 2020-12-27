#include "Particle_System.h"
int main() {
	///////////////Control Variables/////////////////
    bool ManuallyAdjustLowPressPoint = false;
    bool UseLowPress = true;
    bool EnableWindEffect = true;
    vec3 OriLowPress[3][3] = {
        {vec3(0.5f, 3, -20.0f),vec3(0, 3, -20.0f),vec3(-0.5f, 3, -20.0f)},
        {vec3(0.25f, 6, -20.0f), vec3(0, 6, -20.0f), vec3(-0.25f, 6, -20.0f)},
        {vec3(0, 8, -20.0f),vec3(0, 8, -20.0f),vec3(0, 8, -20.0f)}
    };
    vec3 LowPress[3][3] = {
        {vec3(0.5f, 3, -20.0f),vec3(0, 3, -20.0f),vec3(-0.5f, 3, -20.0f)},
        {vec3(0.25f, 6, -20.0f), vec3(0, 6, -20.0f), vec3(-0.25f, 6, -20.0f)},
        {vec3(0, 8, -20.0f),vec3(0, 8, -20.0f),vec3(0, 8, -20.0f)}
    };
    float heightOfLowPress[3] = { 2,4,8 };

    float ParticlesPerSecond = 4500;
    float InitialPositionRadius = 1.0f;
    vec3 PoolOfInitialVelocity[] = {
        vec3(-1,5,0),
        vec3(1,5,0),
        vec3(0,5,0)
    };
    int lengthOfPool = sizeof(PoolOfInitialVelocity) / 12;
    float randV_Xmax = 2.0, randV_Xmin = -2.0;
    float randV_Ymax = 1.0, randV_Ymin = -1.0;
    float randV_Zmax = 2.0, randV_Zmin = -2.0;
    unsigned int InitialAlpha = 64;
    Colour InitialColor(255, 191, 0, InitialAlpha);
    float InitialSize = 0.5f;
    float InitLife = 3.0f;
    float forceOfPress = 20.0f;
    float BurstRate = 1.0f;
	/////////////////////////////////////////////////
    // Initialise GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

    // Open a window and create its OpenGL context
    GLFWwindow* window; // (In the accompanying source code, this variable is global for simplicity)
    window = glfwCreateWindow(1024, 768, "Particle System Implementation", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // Initialize GLEW
    glewExperimental = true; // Needed in core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024 / 2, 768 / 2);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    GLuint programID = LoadShaders(".\\shader\\SimpleVertexShader.vertexshader", ".\\shader\\SimpleFragmentShader.fragmentshader");

    // Vertex shader
    GLuint CameraRight_worldspace_ID = glGetUniformLocation(programID, "CameraRight_worldspace");
    GLuint CameraUp_worldspace_ID = glGetUniformLocation(programID, "CameraUp_worldspace");
    GLuint ViewProjMatrixID = glGetUniformLocation(programID, "VP");

    // fragment shader
    GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");

    static GLfloat* g_particule_position_size_data = new GLfloat[MaxParticles * 4];
    static GLubyte* g_particule_color_data = new GLubyte[MaxParticles * 4];

    for (int i = 0; i < MaxParticles; i++) {
        ParticlesContainer[i].life = -1.0f;
        ParticlesContainer[i].cameradistance = -1.0f;
    }

    GLuint Texture = loadDDS("particle.DDS");

    // The VBO containing the 4 vertices of the particles.
    // Thanks to instancing, they will be shared by all particles.
    static const GLfloat g_vertex_buffer_data[] = {
        0.0f,0.0f,0.0f,
        0.3535f,-0.3535f,0.0f,
        0.0f,-0.5f,0.0f,
        -0.3535f,-0.3535f,0.0f,
        -0.5f,0.0f,0.0f,
        -0.3535f,0.3535f,0.0f,
        0.0f,0.5f,0.0f,
        0.3535f,0.3535f,0.0f,
        0.5f,0.0f,0.0f,
        0.3535f,-0.3535f,0.0f/*
        -0.5f,-0.5f,0.0f,
        0.5f,-0.5f,0.0f,
        -0.5f,0.5f,0.0f,
        0.5f,0.5f,0.0f*/
    };

    GLuint billboard_vertex_buffer;
    glGenBuffers(1, &billboard_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    GLuint particles_position_buffer;
    glGenBuffers(1, &particles_position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

    GLuint particles_color_buffer;
    glGenBuffers(1, &particles_color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);

    double lastTime = glfwGetTime();

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
	
	if(!ManuallyAdjustLowPressPoint){
		LowPress[0][0].y = heightOfLowPress[0] * BurstRate;
		LowPress[0][1].y = heightOfLowPress[0] * BurstRate;
		LowPress[0][2].y = heightOfLowPress[0] * BurstRate;
		LowPress[1][0].y = heightOfLowPress[1] * BurstRate;
		LowPress[1][1].y = heightOfLowPress[1] * BurstRate;
		LowPress[1][2].y = heightOfLowPress[1] * BurstRate;
		LowPress[2][0].y = heightOfLowPress[2] * BurstRate;
	}
    do {
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        double currentTime = glfwGetTime();
        double delta = currentTime - lastTime;
        lastTime = currentTime;

        computeMatricesFromInputs(window);
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();

        // We will need the camera's position in order to sort the particles w.r.t the camera's distance.
        // There should be a getCameraPosition() function in common/controls.cpp, 
        // but this works too.
        glm::vec3 CameraPosition(glm::inverse(ViewMatrix)[3]);
       

        glm::mat4 ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

        if (EnableWindEffect)
        {
            LowPress[0][0].x = OriLowPress[0][0].x + sin(currentTime * 2.0f) + 1;
            LowPress[0][1].x = OriLowPress[0][1].x + sin(currentTime * 2.0f) + 1;
            LowPress[0][2].x = OriLowPress[0][2].x + sin(currentTime * 2.0f) + 1;
            LowPress[1][0].x = OriLowPress[1][0].x + sin(currentTime * 5.0f) + 1;
            LowPress[1][1].x = OriLowPress[1][1].x + sin(currentTime * 5.0f) + 1;
            LowPress[1][2].x = OriLowPress[1][2].x + sin(currentTime * 5.0f) + 1;
            LowPress[2][0].x = OriLowPress[2][0].x + sin(currentTime) + 1;
        }
        // Numbers of newparitcles in each frame, limited by 16 particles each frame
        int newparticles = (int)(delta * ParticlesPerSecond * BurstRate);
        if (newparticles > (int)(0.050f * ParticlesPerSecond * BurstRate))
            newparticles = (int)(0.050f * ParticlesPerSecond * BurstRate);
        
        // Add new particles and initialize
        for (int i = 0; i < newparticles; i++) {
            int particleIndex = FindUnusedParticle();
            ParticlesContainer[particleIndex].life = InitLife - (float)((rand() % 100) / 300.0);

            float randT = (float)(rand() % 360);
            float r = (float)(rand() % 100 / 100.0) * InitialPositionRadius;
            ParticlesContainer[particleIndex].pos = glm::vec3(r * cos(randT * PI / 180.0), r * sin(randT * PI / 180.0) / 1.2f, -20.0f);

            glm::vec3 maindir = PoolOfInitialVelocity[rand() % lengthOfPool];
            glm::vec3 randomDir = glm::vec3(
                (rand() % (int)((randV_Xmax - randV_Xmin) * 1000.0f) + 1000.0f * randV_Xmin) / 1000.0f,
                (rand() % (int)((randV_Ymax - randV_Ymin) * 1000.0f) + 1000.0f * randV_Ymin) / 1000.0f,
                (rand() % (int)((randV_Zmax - randV_Zmin) * 1000.0f) + 1000.0f * randV_Zmin) / 1000.0f
            );
            ParticlesContainer[particleIndex].target = rand() % 3;

            ParticlesContainer[particleIndex].speed = (maindir + randomDir) * BurstRate;

            ParticlesContainer[particleIndex].color = InitialColor;
            ParticlesContainer[particleIndex].size = InitialSize;
        }
        // Simulate all particles
        int ParticlesCount = 0;
        for (int i = 0; i < MaxParticles; i++) {

            Particle& p = ParticlesContainer[i]; // shortcut

            if (p.life > 0.0f) {
                // Decrease life
                p.life -= delta;
                if (p.life > 0.0f) {
                    // Update Colour
                    p.color.a = (p.life) / InitLife * InitialAlpha;
                    p.size = (p.life) / InitLife * InitialSize;
                    p.color.g = 191 * (p.life) / InitLife;
                    p.color.g *= ((256 - pow(2,8*(1 - (p.life) / InitLife)))/256.0);


                    // Update Position
                    float last_y = p.pos.y;
                    p.pos += p.speed * (float)delta; 

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

                    p.speed.y = temp;  // y velocity remains the same
                    float ll = getLength(p.speed);                    
                    p.speed.x *= (l / ll);
                    p.speed.z *= (l / ll);
                    
                    p.cameradistance = glm::length2(p.pos - CameraPosition);

                    // Fill the GPU buffer
                    g_particule_position_size_data[4 * ParticlesCount + 0] = p.pos.x;
                    g_particule_position_size_data[4 * ParticlesCount + 1] = p.pos.y;
                    g_particule_position_size_data[4 * ParticlesCount + 2] = p.pos.z;

                    g_particule_position_size_data[4 * ParticlesCount + 3] = p.size;

                    g_particule_color_data[4 * ParticlesCount + 0] = p.color.r;
                    g_particule_color_data[4 * ParticlesCount + 1] = p.color.g;
                    g_particule_color_data[4 * ParticlesCount + 2] = p.color.b;
                    g_particule_color_data[4 * ParticlesCount + 3] = p.color.a;
                }
                else {
                    // Particles that just died will be put at the end of the buffer in SortParticles();
                    p.cameradistance = -1.0f;
                    p.color.a = 0;
                }
                ParticlesCount++;
            }
        }

        SortParticles();

        glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
        glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning
        glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLfloat) * 4, g_particule_position_size_data);

        glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
        glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning
        glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLubyte) * 4, g_particule_color_data);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Use our shader
        glUseProgram(programID);

        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture);
        // Set our "myTextureSampler" sampler to use Texture Unit 0
        glUniform1i(TextureID, 0);

        // Same as the billboards tutorial
        glUniform3f(CameraRight_worldspace_ID, ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
        glUniform3f(CameraUp_worldspace_ID, ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);

        glUniformMatrix4fv(ViewProjMatrixID, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);

        // 1st attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
        glVertexAttribPointer(
            0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );

        // 2nd attribute buffer : positions of particles' centers
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
        glVertexAttribPointer(
            1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
            4,                                // size : x + y + z + size => 4
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
        );

        // 3rd attribute buffer : particles' colors
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
        glVertexAttribPointer(
            2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
            4,                                // size : r + g + b + a => 4
            GL_UNSIGNED_BYTE,                 // type
            GL_TRUE,                          // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
            0,                                // stride
            (void*)0                          // array buffer offset
        );

        // These functions are specific to glDrawArrays*Instanced*.
        // The first parameter is the attribute buffer we're talking about.
        // The second parameter is the "rate at which generic vertex attributes advance when rendering multiple instances"
        glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
        glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
        glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1

        // Draw the particules !
        // This draws many times a small triangle_strip (which looks like a quad).
        // This is equivalent to :
        // for(i in ParticlesCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4), 
        // but faster.
        glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 10, ParticlesCount);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

    delete[] g_particule_position_size_data;

    // Cleanup VBO and shader
    glDeleteBuffers(1, &particles_color_buffer);
    glDeleteBuffers(1, &particles_position_buffer);
    glDeleteBuffers(1, &billboard_vertex_buffer);
    glDeleteProgram(programID);
    glDeleteTextures(1, &Texture);
    glDeleteVertexArrays(1, &VertexArrayID);


    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}