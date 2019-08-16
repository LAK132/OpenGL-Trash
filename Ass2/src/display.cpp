// This file is display.cpp.  It includes the skeleton for the display routine

// Basic includes to get this file to work.  
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include <GL/gl3w.h>
#include "Transform.h"
#include "Geometry.h"

using namespace std; 
#include "variables.h"
#include "readfile.h"

// New helper transformation function to transform vector by modelview 
// May be better done using newer glm functionality.
// Provided for your convenience.  Use is optional.  
// Some of you may want to use the more modern routines in readfile.cpp 
// that can also be used.  
void transformvec (const GLfloat input[4], GLfloat output[4]) 
{
    glm::vec4 inputvec(input[0], input[1], input[2], input[3]);

    glm::vec4 outputvec = modelview * inputvec;

    output[0] = outputvec[0];

    output[1] = outputvec[1];

    output[2] = outputvec[2];

    output[3] = outputvec[3];
}

void display() 
{
    glClearColor(0,0,1,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (useGlu) 
    {
        modelview = glm::lookAt(eye, center, up);
    }
    else 
    {
        modelview = Transform::lookAt(eye, center, up);
    }

    glUniformMatrix4fv(modelviewPos, 1, GL_FALSE, &modelview[0][0]);
    // glUniformMatrix4fv(glGetUniformLocation(shaderprogram, "view"), 1, GL_FALSE, &modelview[0][0]);

    // Lights are transformed by current modelview matrix. 
    // The shader can't do this globally. 
    // So we need to do so manually.  
    if (numused) 
    {
        glUniform1i(enablelighting,true);

        // YOUR CODE FOR Assignment 2 HERE.  
        // You need to pass the light positions and colors to the shader. 
        // glUniform4fv() and similar functions will be useful. See FAQ for help with these functions.
        // The lightransf[] array in variables.h and transformvec() might also be useful here.
        // Remember that light positions must be transformed by modelview.  

    } else {
        glUniform1i(enablelighting,false); 
    }

    // Transformations for objects, involving translation and scaling 
    mat4 sc(1.0) , tr(1.0);//, transf(1.0); 
    sc = Transform::scale(sx,sy,1.0); 
    tr = Transform::translate(tx,ty,0.0); 

    // YOUR CODE FOR Assignment 2 HERE.  
    // You need to use scale, translate and modelview to 
    // set up the net transformation matrix for the objects.  
    // Account for GLM issues, matrix order (!!), etc.  


    // The object draw functions will need to further modify the top of the stack,

    // so assign whatever transformation matrix you intend to work with to modelview

    // rather than use a uniform variable for that.
    mat4 view = modelview * tr * sc;
    // modelview = transf;

    // glUniform1iv(glGetUniformLocation(shaderprogram, "activeLights"), 1, &(GLint)numused);
    glUniform1i(glGetUniformLocation(shaderprogram, "activeLights"), numused);
    glUniform1i(glGetUniformLocation(shaderprogram, "enablelighting"), numused);
        
    for (int i = 0; i < numused; i++)
    {
        char buffer[30];
        sprintf(buffer, "lightcolor[%d]", i);
        glUniform4f(glGetUniformLocation(shaderprogram, buffer), lightcolor[i*4], lightcolor[(i*4)+1], lightcolor[(i*4)+2], lightcolor[(i*4)+3]);
    }
    
    for (int i = 0 ; i < numobjects ; i++) 
    {
        object* obj = &(objects[i]); // Grabs an object struct.

        // YOUR CODE FOR Assignment 2 HERE. 
        // Set up the object transformations 
        // And pass in the appropriate material properties
        // Again glUniform() related functions will be useful

        modelview = glm::transpose(glm::inverse(obj->transform)); // world -> object space

        for (int i = 0; i < numused; i++)
        {
            char buffer[30];
            float fs[4];
            transformvec(&lightposn[i*4], fs); // world -> object space
            // float* fs = &lightposn[i*4];
            sprintf(buffer, "lightposn[%d]", i);
            glUniform4f(glGetUniformLocation(shaderprogram, buffer), fs[0], fs[1], fs[2], fs[3]);
        }

        modelview = view * obj->transform;
        // glUniformMatrix4fv(glGetUniformLocation(shaderprogram, "modelview"), 1, GL_FALSE, &mv[0][0]);
        glUniform4fv(glGetUniformLocation(shaderprogram, "ambient"), 1, &obj->ambient[0]);
        glUniform4fv(glGetUniformLocation(shaderprogram, "diffuse"), 1, &obj->diffuse[0]);
        glUniform4fv(glGetUniformLocation(shaderprogram, "specular"), 1, &obj->specular[0]);
        glUniform4fv(glGetUniformLocation(shaderprogram, "emission"), 1, &obj->emission[0]);
        glUniform1fv(glGetUniformLocation(shaderprogram, "shininess"), 1, &obj->shininess);
        // glUniformMatrix4fv(glGetUniformLocation(shaderprogram, "model"), 1, GL_FALSE, &obj->transform[0][0]);

        // Actually draw the object
        // We provide the actual drawing functions for you.  
        // Remember that obj->type is notation for accessing struct fields
        if (obj->type == cube) {
            solidCube(obj->size); 
        }
        else if (obj->type == sphere) {
            const int tessel = 20; 
            solidSphere(obj->size, tessel, tessel); 
        }
        else if (obj->type == teapot) {
            solidTeapot(obj->size); 
        }
    }

    // modelview = transf;
}