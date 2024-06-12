// include glad
#include "glad/glad.h"
// include glfw
#include "GLFW/glfw3.h"
// include stdlibs
#include<iostream>
#include <cstdlib>
#include <cmath>
#include <vector>
//Inclue ParticleClass
#include "ParticleClass/ParticleClass.hpp"

/*

Notice: Small parts of the main file are still from a template I used to skip a lot of the loading of librarys. Most of it was rewritten by myself.
This was also written in another ide and then I moved it to my current one so some things may be inconsistant.

*/


//Declare C random number generator
//Also throws a warning but we don't talk about that!
int rand(void);

//For OpenGL debugging
void CheckError (int Line) {
	GLenum Error = glGetError();
	if(Error != 0) {
		std::cerr << "OpenGl error caught on line " << Line << " with code " << Error << std::endl;
	}
}

// Vertex shader source code
const char* VertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec2 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, 1.0, 1.0);\n"
"}\0";

//Fragment shader source code
const char* ParticleShaderSource = 
"#version 330 core \n"
"uniform vec2 WindowSize;\n"
"uniform vec4 ParticleColor;\n"
"in vec4 gl_FragCoord;\n"
"out vec4 FragColor;\n"
"vec2 UV = (vec2(gl_FragCoord.x/WindowSize.x-0.5, gl_FragCoord.y/WindowSize.y-0.5)*2);\n"
"void main() {\n"
"   FragColor = ParticleColor;\n"
"}\n";

//Some universal constants
const float PI = 3.1415926f;

// Some settings you can change if you want
const int WindowSizeX = 512; //Window Size
const int WindowSizeY = 512;
const float ParticlesPerFrame = 1; // Number of particles to generate each frame (60 fps)
const float ParticleSpread = 0.06f; // Range of starting velocity for particles

int main()
{
	//Seed the C random number generator
	srand((unsigned int)time(0));

	//Work out some values ahead of time
	const int SpreadFull = int(ParticleSpread * 10000.0f);
  
	// Initialize GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object and naming it "OpenGLParticleSystem"
	GLFWwindow* window = glfwCreateWindow(WindowSizeX, WindowSizeY, "OpenGLParticleSystem", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	glViewport(0, 0, WindowSizeX, WindowSizeY);



	// Create Vertex Shader Object and get its reference
	GLuint VertexShader = glCreateShader(GL_VERTEX_SHADER);
	// Attach Vertex Shader source to the Vertex Shader Object
	glShaderSource(VertexShader, 1, &VertexShaderSource, NULL);
	// Compile the Vertex Shader into machine code
	glCompileShader(VertexShader);

	// Create Fragment Shader Object and get its reference
	GLuint ParticleShader = glCreateShader(GL_FRAGMENT_SHADER);
	// Attach Fragment Shader source to the Fragment Shader Object
	glShaderSource(ParticleShader, 1, &ParticleShaderSource, NULL);
	// Compile the Fragment Shader into machine code
	glCompileShader(ParticleShader);

	GLuint ParticleProgram = glCreateProgram();
	// Attach the Vertex and Fragment Shaders to the Shader Program
	glAttachShader(ParticleProgram, VertexShader);
	glAttachShader(ParticleProgram, ParticleShader);
	// Wrap-up/Link all the shaders together into the Shader Program
	glLinkProgram(ParticleProgram);

	//Prepare uniforms
	int ParticleWindowSize = glGetUniformLocation(ParticleProgram,"WindowSize");
	int ParticleColorUniform = glGetUniformLocation(ParticleProgram,"ParticleColor");

  
	// Delete the now useless Vertex and Fragment Shader objects
	glDeleteShader(VertexShader);
	glDeleteShader(ParticleShader);


	// Create reference containers for the Vertex Array Object, the Vertex Buffer Object, and the Element Buffer Object
	GLuint VAO, PVBO;

	// Generate the VAO, VBO, and EBO with only 1 object each
	glGenVertexArrays(1, &VAO); 
	glGenBuffers(1, &PVBO); 
   
	// Make the VAO the current Vertex Array Object by binding it
	glBindVertexArray(VAO);

	// Configure the Vertex Attribute so that OpenGL knows how to read the VBO
	// Only needs two floats for the position.
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	// Enable the Vertex Attribute so that OpenGL knows to use it
	glEnableVertexAttribArray(0);

	//Update uniform
	glUseProgram(ParticleProgram);
	glUniform2f(ParticleWindowSize, WindowSizeX, WindowSizeY);

	double lasttime = glfwGetTime();

	//Create Particle System
	ParticleSystem MainParticleSystem;

	while (!glfwWindowShouldClose(window)) {
		//I know there is a built-in GLFW frame rate limiter but I have no clue how it works and this makes more sense to me.
		//From GLFW Github Forum
		while (glfwGetTime() < lasttime + 1.0/60.0) {
			// Nothing happens here
		}
		lasttime += 1.0 / 60.0;

		// Specify the color of the background
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT);

		//Update Uniform
		glUseProgram(ParticleProgram);
		glUniform4f(ParticleColorUniform, float(sin(glfwGetTime()+PI))*0.5f+0.5f,
		float(cos(glfwGetTime()))*0.5f+0.5f,
		float(sin(glfwGetTime()))*0.5f+0.5f,1.0f);

		// Bind the VAO so OpenGL knows to use it
		glBindVertexArray(VAO);

		//Draw the current particles
		MainParticleSystem.DrawVertices(ParticleProgram);
		//Delete invalid particles
		MainParticleSystem.DeleteOldParticles ();
		//Run physics
		MainParticleSystem.UpdateParticles();
		//Handle collisions
		MainParticleSystem.CollideParticles ();

		//Generate more particles
		for (int i = 0; i < ParticlesPerFrame; i++) {
		  MainParticleSystem.NewParticle(float((rand()%2000)-1000)*0.001f,0.5f,
		    float((rand()%SpreadFull)-SpreadFull/2)*0.0001f,
		    float((rand()% SpreadFull)-SpreadFull/2)*0.0001f);
		}
		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

	// Delete all the objects
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &PVBO);
	glDeleteProgram(ParticleProgram);

	// Delete window before ending the program
	glfwDestroyWindow(window);

	// Terminate GLFW before ending the program
	// If this does not happen you need to force end it through terminal as administrator
	glfwTerminate();  
	// Particle system leaks some memory (Index and vertex buffer) but the program exits so it's no big deal
	return 0;
}





