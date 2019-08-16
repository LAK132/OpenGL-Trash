# version 330 core
// Do not use any version older than 330!

/* This is the fragment shader for reading in a scene description, including 
   lighting.  Uniform lights are specified from the main program, and used in 
   the shader.  As well as the material parameters of the object.  */

// Inputs to the fragment shader are the outputs of the same name of the vertex shader.
// Note that the default output, gl_Position, is inaccessible!
in vec3 mynormal; 
in vec3 myvertex; 
// in vec3 myeye;

// You will certainly need this matrix for your lighting calculations
uniform mat4 modelview;
uniform mat4 projection;

// This first defined output of type vec4 will be the fragment color
out vec4 fragColor;

uniform vec3 color;

const int numLights = 10; 
uniform int activeLights;               // number of lights used
uniform bool enablelighting; // are we lighting at all (global).
uniform vec4 lightposn[numLights]; // positions of lights 
uniform vec4 lightcolor[numLights]; // colors of lights

// Now, set the material parameters.
// I use ambient, diffuse, specular, shininess. 
// But, the ambient is just additive and doesn't multiply the lights.  

uniform vec4 ambient; 
uniform vec4 diffuse; 
uniform vec4 specular; 
uniform vec4 emission; 
uniform float shininess; 

void main (void) 
{       
    if (enablelighting) {
	    vec3 myeye = vec3(inverse(projection * modelview) * vec4(0.0f, 0.0f, 0.0f, 1.0f)); // world -> object space
	    // vec3 myeye = vec3(0.0f, 0.0f, 0.0f);
        vec3 viewDir = normalize(myeye - myvertex);
        vec4 baseColor = ambient + emission;

        for(int i = 0; i < activeLights && i < numLights; i++)
        {
            vec3 lightDir = normalize(vec3(lightposn[i]));
            if(lightposn[i].w > 0.0f) 
                lightDir = normalize((lightposn[i].xyz / lightposn[i].w) - myvertex);
            float dNL = max(dot(mynormal, lightDir), 0.0f);
            vec4 lambert = diffuse * lightcolor[i] * dNL;

            vec3 halfVec = normalize(lightDir + viewDir);
            float dNH = max(dot(mynormal, halfVec), 0.0f);
            vec4 phong = specular * lightcolor[i] * pow(dNH, shininess);
            baseColor += lambert + phong;
        }

        // YOUR CODE FOR HW 2 HERE
        // A key part is implementation of the fragment shader

        // Color all pixels black for now, remove this in your implementation!
        // baseColor = diffuse * lightcolor[0];//vec4(0.0f, 0.0f, 0.0f, 1.0f); 

        fragColor = baseColor; 
    } else {
        fragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f); //vec4(color, 1.0f); 
    }
}
