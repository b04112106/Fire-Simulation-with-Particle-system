#include "Particle_System.h"
int main() {
	///////////////Control Variables/////////////////
    // Start particle index , End particle index
    // Position, radius
    // Main Direction xyz random Direction
    // Particle per second
    // Initial color/Size/Life/Force/BurstRate
    /*emitter(int _id, int start, int end, vec3 pos, float r, vec3 initvel, float Mx, float mx, float My, float my, float Mz, float mz,
        int pps, unsigned char initAlpha, Colour initC, float size, float Life, float force, float BRate) :
        id(_id), InitialPosition(pos), InitialPositionRadius(r), InitialVelocity(initvel), randV_Xmax(Mx), randV_Xmin(mx), randV_Ymax(My), randV_Ymin(my), randV_Zmax(Mz), randV_Zmin(mz),
        ParticlesPerSecond(pps), InitialAlpha(initAlpha), InitialColor(initC), InitialSize(size), InitLife(Life), forceOfPress(force), BurstRate(BRate), LastUsedParticle(start) {}*/
    
    emitter ListOfEmitter[] = {
        emitter(0,vec3(0,0,-20.0f),1.0f,vec3(0,2,0),2.0f,-2.0f,2.0f,-2.0f,0.0f,0.01f,3000,64,Colour(255,191,0,64),0.3f,3.0f,10.0f,1.0f),
        emitter(1,vec3(0.75f,0,-20.0f),1.0f,vec3(0.5f,2,0),  2.0f,-2.0f,2.0f,-2.0f,2.0f,-2.0f,1000,32,Colour(255,64,0,32),0.2f,3.0f,5.0f,1.0f),
        emitter(2,vec3(-0.75f,0,-20.0f),1.0f,vec3(-0.5f,2,0),2.0f,-2.0f,2.0f,-2.0f,2.0f,-2.0f,1000,32,Colour(255,64,0,32),0.2f,3.0f,5.0f,1.0f)
    };

    const int NumOfEmiiters = sizeof(ListOfEmitter) / sizeof(emitter);
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
    static const GLfloat g_vertex_buffer_data[] =
        /*{
            -0.5f,-0.5f,0.0f,
            0.5f,-0.5f,0.0f,
            -0.5f,0.5f,0.0f,
            0.5f,0.5f,0.0f
        };*/
    { 0.0f,0.0f,0.0f,
        0.3f,-0.3f,0.0f,
        0.0f,-0.45f,0.0f,
        -0.3f,-0.3f,0.0f,
        -0.45f,0.0f,0.0f,
        -0.3f,0.3f,0.0f,
        0.0f,0.45f,0.0f,
        0.3f,0.3f,0.0f,
        0.45f,0.0f,0.0f,
        0.3f,-0.3f,0.0f };

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
    // For FPS display
    float acc = 0;
	if(!ManuallyAdjustLowPressPoint){
		LowPress[0][0].y = heightOfLowPress[0];
        LowPress[0][0].x = level1_radius;
        OriLowPress[0][0].x = level1_radius;
		LowPress[0][1].y = heightOfLowPress[0];
		LowPress[0][2].y = heightOfLowPress[0];
        LowPress[0][2].x = -level1_radius;
        OriLowPress[0][2].x = -level1_radius;
		LowPress[1][0].y = heightOfLowPress[1];
        LowPress[1][0].x = level2_radius;
        OriLowPress[1][0].x = level2_radius;
		LowPress[1][1].y = heightOfLowPress[1];
		LowPress[1][2].y = heightOfLowPress[1];
        LowPress[1][2].x = -level2_radius;
        OriLowPress[1][2].x = -level2_radius;
		LowPress[2][0].y = heightOfLowPress[2];
	}
    do {
        // Clear the screen
        //getchar();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        double currentTime = glfwGetTime();
        double delta = currentTime - lastTime;
        lastTime = currentTime;
        acc += delta;
        if (acc > 1) {
            std::cout << "FPS:" << 1 / delta << std::endl;
            acc = 0;
        }
        computeMatricesFromInputs(window);
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();

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
        // Add new particles and initialize
        for (int i = 0; i < NumOfEmiiters; ++i)
        {
            ListOfEmitter[i].AddNewParticles(delta);
        }

        // Simulate all particles
        int ParticlesCount = 0;

        for (int i = 0; i < MaxParticles; i++) {

            Particle& p = ParticlesContainer[i];
            ListOfEmitter[p.id].Simulate(delta, p, CameraPosition);
        }

        SortParticles();

        for (int i = 0; i < MaxParticles; i++) {
            Particle& p = ParticlesContainer[i];
            if (p.life > 0.0f) {
                // Fill the GPU buffer
                g_particule_position_size_data[4 * ParticlesCount + 0] = p.pos.x;
                g_particule_position_size_data[4 * ParticlesCount + 1] = p.pos.y;
                g_particule_position_size_data[4 * ParticlesCount + 2] = p.pos.z;

                g_particule_position_size_data[4 * ParticlesCount + 3] = p.size;

                g_particule_color_data[4 * ParticlesCount + 0] = p.color.r;
                g_particule_color_data[4 * ParticlesCount + 1] = p.color.g;
                g_particule_color_data[4 * ParticlesCount + 2] = p.color.b;
                g_particule_color_data[4 * ParticlesCount + 3] = p.color.a;
                
                ParticlesCount++;
            }
        }       

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
        if(UseBigPattern)
            glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 10, ParticlesCount);
        else
            glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, ParticlesCount);

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