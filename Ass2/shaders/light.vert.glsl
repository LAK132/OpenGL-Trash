#version 330 core
// Do not use any version older than 330! Modern OpenGL will break!

// Inputs to the vertex shader
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

// Uniform variables
// uniform mat4 modelview;
uniform mat4 modelview;
uniform mat4 projection;
// uniform mat4 view;
// uniform mat4 model;

// Additional outputs for the vertex shader in addition to gl_Position
out vec3 mynormal;
out vec3 myvertex;
// out vec3 myeye;

void main() {
    gl_Position = projection * modelview * vec4(position, 1.0f); // object -> world -> camera -> screen space

	// Forward these vectors to the fragment shader
	// mynormal = mat3(model) * normal; // object -> world space
	// myvertex = mat3(model) * position; // object -> world space
	// myeye = vec3(inverse(projection * view) * vec4(0.0f, 0.0f, 0.0f, 1.0f)); // camera -> world space
	
	mynormal = normal; // object space
	myvertex = position; // object space
	// myeye = vec3(inverse(projection * modelview) * vec4(0.0f, 0.0f, 0.0f, 1.0f)); // camera -> world -> object space
}

