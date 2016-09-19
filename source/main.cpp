/*
Title: Vertex Shaders
File Name: main.cpp
Copyright � 2016
Author: David Erbelding
Written under the supervision of David I. Schwartz, Ph.D., and
supported by a professional development seed grant from the B. Thomas
Golisano College of Computing & Information Sciences
(https://www.rit.edu/gccis) at the Rochester Institute of Technology.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/



#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"
#include <vector>
#include "../header/shape.h"
#include "../header/transform2d.h"
#include "../header/shader.h"
#include <iostream>

Shape* square;

// The transform being used to draw our shape
Transform2D transform;

// These shader objects wrap the functionality of loading and compiling shaders from files.
Shader vertexShader;
Shader fragmentShader;

// GL index for shader program
GLuint shaderProgram;

// This will store the index of our world matrix within the shader program.
GLuint uniformLocation;

// Window resize callback
void resizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

int main(int argc, char **argv)
{
	// Initializes the GLFW library
	glfwInit();

	// Initialize window
	GLFWwindow* window = glfwCreateWindow(800, 600, "Vertex Shaders", nullptr, nullptr);

	glfwMakeContextCurrent(window);

	//set resize callback
	glfwSetFramebufferSizeCallback(window, resizeCallback);

	// Initializes the glew library
	glewInit();


	// Indices for square (-1, -1)[2] to (1, 1)[1]
	// [0]------[1]
	//	|		 |
	//	|		 |
	//	|		 |
	// [2]------[3]

	// Create square vertex data.
	std::vector<glm::vec2> vertices;
	vertices.push_back(glm::vec2(-1, 1));
	vertices.push_back(glm::vec2(1, 1));
	vertices.push_back(glm::vec2(-1, -1));
	vertices.push_back(glm::vec2(1, -1));

	std::vector<unsigned int> indices;
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(2);
	indices.push_back(1);


	// Create shape object
	square = new Shape(vertices, indices);
	

	transform.SetScale(.25f);
	transform.SetPosition(glm::vec2(.25, .25));

	// SHADER STUFF
	
	// Previously, we had multiplied each vertex by the world matrix before sending it to the gpu.
	// Multiplying for every vertex can get extremely slow on the gpu, and that's why vertex shaders exist!
	// We can just send the world matrix with the vertices, and do the multiplication in parallel!
	std::string vertexShaderCode =
		"#version 400 core \n"

		// Our vertex attributes are "per-vertex".
		// This means that the value is unique to each vertex that gets processed.
		"layout(location = 0) in vec2 in_position;"

		// Uniforms are "per-draw-call".
		// They act as global variables within the shader that can be used by every vertex.
		// Since each vertex needs to be multiplied by the world matrix, this is a perfect use case.
		"uniform mat3 worldMatrix;"

		"void main(void)"
		"{"
			// Multiply the position by the world matrix to convert from local to world space.
			// (We have to add a 1 here to the end of our position vector to make the translation work)
			"vec3 transformed = worldMatrix * vec3(in_position, 1);"

			// output the transformed vector as a vec4.
			"gl_Position = vec4(transformed, 1);"
		"}";



	// This function runs the shader compiler and checks for errors.
	// It's just a self contained version of the code from the previous example.
	vertexShader.InitFromString(vertexShaderCode, GL_VERTEX_SHADER);
	//vertexShader.InitFromFile("../shaders/vertex.glsl", GL_VERTEX_SHADER);

	// This function loads the shader from file, and then calls InitFromString.
	// You can do this with the vertex shader too by switching the above lines.
	fragmentShader.InitFromFile("../shaders/fragment.glsl", GL_FRAGMENT_SHADER);





	// Create a shader program.
	shaderProgram = glCreateProgram();
	
	// Attach the vertex and fragment shaders to our program.
	vertexShader.AttachTo(shaderProgram);
	fragmentShader.AttachTo(shaderProgram);

	// Tells gl to set up the connections between the shaders we have attached.
	// After this we should be ready to roll.
	glLinkProgram(shaderProgram);

	// After the program has been linked, we can ask it where it put our worldMatrix.
	// (Since there's only one uniform between our two shaders, ours should always end up at index 0)
	uniformLocation = glGetUniformLocation(shaderProgram, "worldMatrix");



	// Main Loop
	while (!glfwWindowShouldClose(window))
	{
        // Calculate delta time.
        float dt = glfwGetTime();
        // Reset the timer.
        glfwSetTime(0);

		// Clear the screen.
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.0, 0.0, 0.0, 0.0);


		// rotate square
		transform.Rotate(dt);
		
		// Set the current shader program.
		glUseProgram(shaderProgram);

		
		// The drawing code in our shape class has to change significantly now.
		// Previously we had been multiplying each vertex manually, now we will let the gpu handle the dirty work.
		// We also need to give it the unifrom location for the world Matrix so it can send the matrix.
		square->Draw(transform.GetMatrix(), uniformLocation);



		// Stop using the shader program.
		glUseProgram(0);

		// Swap the backbuffer to the front.
		glfwSwapBuffers(window);

		// Poll input and window events.
		glfwPollEvents();

	}

	// Free memory from shader program and individual shaders
	glDeleteProgram(shaderProgram);


	// Free memory from shape object
	delete square;

	// Free GLFW memory.
	glfwTerminate();


	// End of Program.
	return 0;
}
