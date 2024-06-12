// include glad
#include "../glad/glad.h"
//Include some stdlibs
#include <vector>
#include <iostream>
#include <cstdlib>
#include <cmath>
// include glfw
#include "GLFW/glfw3.h"


//Spaces indent because this was written in a different ide


class ParticleSystem {

  //Encapsulation really is not nessesary here
  public:
  //Defining members
  float ParticleSize = 0.05f;
  float AirResistance = 0.98f;
  float Gravity = -0.001f;
  float Lifetime = 5.0f;
  float WallDamp = 1.2f;
  float FloorDamp = 0.8f;
  float MinParticleVelocity = 0.001f;
  float MinHeightBeforeDelete = -0.95f;
  float MinLifeBeforeDelete = 1.0f;
  int ParticleCount = 0;

  //I suppose these *could* be private but it's sometimes useful to edit them from the outside
  std::vector<GLfloat> ParticleVerts;
  std::vector<float> ParticlePosX;
  std::vector<float> ParticlePosY;
  std::vector<float> ParticleVelX;
  std::vector<float> ParticleVelY;
  std::vector<float> ParticleLifetime;

  //Vertex and index buffer references
  GLuint VertexBuffer, IndexBuffer; 
  
  public:
  //Constructor creates the buffers
  ParticleSystem () {
	glGenBuffers(1, &VertexBuffer);
	glGenBuffers(1, &IndexBuffer);
	
  }
  //Destructor deletes them.
  //THIS SHOULD BE CALLED IN A VALID OPENGL CONTEXT OR THE BUFFERS WILL NOT BE DELETED AND LEAK MEMORY!
  ~ParticleSystem () {
	glDeleteBuffers(1, &VertexBuffer);
	glDeleteBuffers(1, &IndexBuffer);
  }
  //Creates a new particle at the position with a set velocity
  void NewParticle (float PosX,float PosY,float VelX,float VelY) {
	ParticlePosX.push_back((const float)PosX);
	ParticlePosY.push_back((const float)PosY);
	ParticleVelX.push_back((const float)VelX);
	ParticleVelY.push_back((const float)VelY);
	ParticleLifetime.push_back((const float)(glfwGetTime() + Lifetime));
	ParticleCount += 1;
  }

  //Run before DrawVertices
  void GenParticleVerts () {
	ParticleVerts.clear();
	for (int i = 0; i < ParticleCount; i++) {
	  ParticleVerts.push_back((const GLfloat) ParticlePosX[i]+(ParticleSize*0.5f));
	  ParticleVerts.push_back((const GLfloat) ParticlePosY[i]+(ParticleSize*0.5f));
	  ParticleVerts.push_back((const GLfloat) ParticlePosX[i]-(ParticleSize*0.5f));
	  ParticleVerts.push_back((const GLfloat) ParticlePosY[i]+(ParticleSize*0.5f));
	  ParticleVerts.push_back((const GLfloat) ParticlePosX[i]-(ParticleSize*0.5f));
	  ParticleVerts.push_back((const GLfloat) ParticlePosY[i]-(ParticleSize*0.5f));
	  ParticleVerts.push_back((const GLfloat) ParticlePosX[i]+(ParticleSize*0.5f));
	  ParticleVerts.push_back((const GLfloat) ParticlePosY[i]-(ParticleSize*0.5f));
	}
  }

  //Runs physics on particles
  void UpdateParticles () {
	for (int i = 0; i < ParticleCount; i++) {
	  ParticlePosX[i] = ParticlePosX[i] + ParticleVelX[i];
	  ParticlePosY[i] = ParticlePosY[i] + ParticleVelY[i];
	  ParticleVelY[i] = ParticleVelY[i] + Gravity;
	  ParticleVelX[i] = ParticleVelX[i] * AirResistance;
	  ParticleVelY[i] = ParticleVelY[i] * AirResistance;
	}
  }

  //Deletes particles when they expend their lifetime or loose their velocity.
  void DeleteOldParticles () {
	int NewParticleCount = ParticleCount;
	for (int i = 0; i < NewParticleCount; i++) {
	  if (ParticleLifetime[i] < glfwGetTime()) {
		ParticlePosX.erase(ParticlePosX.begin()+i);
		ParticlePosY.erase(ParticlePosY.begin()+i);
		ParticleVelY.erase(ParticleVelY.begin()+i);
		ParticleVelX.erase(ParticleVelX.begin()+i);
		ParticleLifetime.erase(ParticleLifetime.begin()+i);
		ParticleCount -= 1;
		NewParticleCount -= 1;
	  }
	  else if (ParticlePosY[i] < MinHeightBeforeDelete && abs(ParticleVelY[i]) < MinParticleVelocity && glfwGetTime()-(ParticleLifetime[i]-(Lifetime)) > MinLifeBeforeDelete) {
		ParticlePosX.erase(ParticlePosX.begin()+i);
		ParticlePosY.erase(ParticlePosY.begin()+i);
		ParticleVelY.erase(ParticleVelY.begin()+i);
		ParticleVelX.erase(ParticleVelX.begin()+i);
		ParticleLifetime.erase(ParticleLifetime.begin()+i);
		ParticleCount -= 1;
		NewParticleCount -= 1;
	  }
	}
  }

  //Checks collision with bounds and changes velocity accordingly
  void CollideParticles () {
	float Bounds = ParticleSize*0.5f;
	for (int i = 0; i < ParticleCount; i++) {
	  bool CollideX = (ParticlePosX[i] < -1.0f+Bounds || 
	  ParticlePosX[i] > 1.0f-Bounds);
	  bool CollideY = (ParticlePosY[i] < -1.0f+Bounds || 
	  ParticlePosY[i] > 1.0f-Bounds);
	  if (CollideX) {
		ParticlePosX[i] = (ParticlePosX[i] < -1.0f+Bounds) ?
		  -1.0f+Bounds : 1.0f-Bounds;
		ParticleVelX[i] *= -WallDamp;
	  }
	  if (CollideY) {
		  ParticlePosY[i] = (ParticlePosY[i] < -1.0f+Bounds) ?
			-1.0f+Bounds : 1.0f-Bounds;
		ParticleVelY[i] *= -FloorDamp;
	  }
	}
  }

  //Generates indices for the IndexBufffer; Run before DrawVertices
  std::vector<GLuint> GenIndices () {
	std::vector<GLuint> indices;

	for (int i = 0; i < ParticleCount; i++) {
	  indices.push_back((const int)i*4+0);
	  indices.push_back((const int)i*4+1);
	  indices.push_back((const int)i*4+2);
	  indices.push_back((const int)i*4+2);
	  indices.push_back((const int)i*4+3);
	  indices.push_back((const int)i*4+0);
	}
	return indices;
  }

  //Run to draw the particles
  void DrawVertices (GLuint ParticleProgram) {
	std::vector<GLuint> indices;
	indices = GenIndices();
	
	GenParticleVerts ();
	
	glUseProgram(ParticleProgram);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
	glBindBuffer (GL_ARRAY_BUFFER,VertexBuffer);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
	
	glBufferData (GL_ARRAY_BUFFER,ParticleVerts.size()*sizeof(GLfloat), ParticleVerts.data(), GL_DYNAMIC_DRAW);
  glDrawElements (GL_TRIANGLES, ParticleVerts.size(),GL_UNSIGNED_INT,0);
  }
};
