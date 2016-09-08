/*
Title: Vertex Shaders
File Name: shape.cpp
Copyright ? 2016
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

#include "../header/shape.h"

Shape::Shape(std::vector<glm::vec2> vertices, std::vector<unsigned int> indices)
{
	m_vertices = vertices;
	m_indices = indices;
	// Create the shape by setting up buffers

	// Set up vertex buffer
	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);

	///NOTE: The data in this array will no longer change when drawing with different world matrices.
	///		 We can now use GL_STATIC_DRAW, because we won't need to write to the buffer.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Set up index buffer same as above
	glGenBuffers(1, &m_indexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_indexBuffer);
	glBufferData(GL_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Shape::~Shape()
{
	// Clear buffers for the shape object when done using them.
	glDeleteBuffers(1, &m_vertexBuffer);
	glDeleteBuffers(1, &m_indexBuffer);
}



void Shape::Draw(glm::mat3 worldMatrix, GLuint uniformLocation)
{
	// Previously, we multiplied each vertex one by one, but now we just have to send the world matrix to the gpu.


	// Bind the vertex buffer and set the Vertex Attribute.
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);



	// This function sets the uniform variable at the given LOCATION.
	// In this case it's a Matrix containing 3 vectors with 3 floats each.
	// 1 is the COUNT of matrices we are sending. (It's possible to send an entire array of matrices).
	// We are passing in FALSE into whether or not we want to transpose our matrices.
	// Last we give it a POINTER to the first float in our matrix.
	// It just interprets the data as an array of floats anyway.
	glUniformMatrix3fv(uniformLocation, 1, GL_FALSE, &(worldMatrix[0][0]));




	// Bind index buffer to GL_ELEMENT_ARRAY_BUFFER, and enable vertex attribute
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
	glEnableVertexAttribArray(0);

	// Draw all indices in the index buffer
	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, (void*)0);

	// Disable vertex attribute and unbind index buffer.
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
