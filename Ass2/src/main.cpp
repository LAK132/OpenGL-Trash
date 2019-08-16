/******************************************************************************/
/* The idea is to draw a teapot, that can be moved by a crystal ball          */
/* interface                                                                  */
/******************************************************************************/


// May need to replace with absolute path on some systems 
#define PATH_TO_TEAPOT_OBJ_FILE "teapot.obj"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include "shaders.h"
#include "Transform.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <grader.h>
#include "Geometry.h"
#include <GL/gl3w.h>

using namespace std ; 

// Main variables in the program.  
#define MAINPROGRAM 
#include "variables.h" 
#include "readfile.h" // prototypes for readfile.cpp  
void display(void) ;  // prototype for display function. 

Grader grader;
bool allowGrader = false;

SDL_Window* window = nullptr;
SDL_GLContext glContext;

// This function gets called when the window size gets changed
void reshape(int width,int height){
	w = width;
	h = height;

	glViewport(0, 0, w, h);

	float aspect = (float) w / (float) h, zNear = 0.1, zFar = 99.0 ;
	// Change the projection matrix to fit with the new window aspect ratio
	if (useGlu) 
		projection = glm::perspective(glm::radians(fovy),aspect,zNear,zFar); 
	else
		projection = Transform::perspective(fovy,aspect,zNear,zFar);
	// Now send the updated projection matrix to the shader
	glUniformMatrix4fv(projectionPos, 1, GL_FALSE, &projection[0][0]);
}

void saveScreenshot(string fname) {
	int pix = w * h;
	BYTE *pixels = new BYTE[3*pix];	
	glReadBuffer(GL_FRONT);
	glReadPixels(0,0,w,h,GL_RGB,GL_UNSIGNED_BYTE, pixels);

	stbi_flip_vertically_on_write(true);
	if(stbi_write_png(fname.c_str(), w, h, 3, pixels, 3*w) == 0) cout << "failed to write image" << endl;

	std::cout << "Saving screenshot: " << fname << "\n";

	delete[] pixels;
}

void printHelp() {
	std::cout << "\npress 'h' to print this message again.\n" 
		<< "press '+' or '-' to change the amount of rotation that occurs with each arrow press.\n" 
		<< "press 'i' to run image grader test cases\n"
		<< "press 'g' to switch between using glm::lookAt and glm::Perspective or your own LookAt.\n"     
		<< "press 'r' to reset the transformation (eye and up).\n"
    	<< "press 'v' 't' 's' to do view [default], translate, scale.\n"
		<< "press ESC to quit.\n";  
}

void keyboard(unsigned char key,int x,int y) {
	switch(key) {
	case '+':
		amount++;
		std::cout << "amount set to " << amount << "\n";
		break;
	case '-':
		amount--;
		std::cout << "amount set to " << amount << "\n"; 
		break;
	case 'i':
		if(useGlu) {
			std::cout << "Please disable glm::LookAt by pressing 'g'"
				  << " before running tests\n";
		}
		else if(!allowGrader) {
			std::cout << "Error: no input file specified for grader\n";
		} else {
			std::cout << "Running tests...\n";
			grader.runTests();
			std::cout << "Done! [ESC to quit]\n";
		}
		break;
	case 'g':
		useGlu = !useGlu;
		std::cout << "Using glm::LookAt and glm::Perspective set to: " 
			<< (useGlu ? " true " : " false ") << "\n"; 
		break;
	case 'h':
		printHelp();
		break;
    case 'r': // reset eye and up vectors, scale and translate. 
		eye = eyeinit ; 
		up = upinit ; 
		amount = amountinit ;
		transop = view ;
		sx = sy = 1.0 ; 
		tx = ty = 0.0 ; 
		break ;   
    case 'v': 
		transop = view ;
		std::cout << "Operation is set to View\n" ; 
		break ; 
    case 't':
		transop = translate ; 
		std::cout << "Operation is set to Translate\n" ; 
		break ; 
    case 's':
		transop = scale ; 
		std::cout << "Operation is set to Scale\n" ; 
		break ;                 
	}
	// glutPostRedisplay();
	SDL_GL_SwapWindow(window);
}

//  You will need to enter code for the arrow keys 
//  When an arrow key is pressed, it will call your transform functions

void specialKey(int key,int x,int y) {
	switch(key) {
	case 100: //left
		if (transop == view) Transform::left(amount, eye,  up);
		else if (transop == scale) sx -= amount * 0.01 ; 
		else if (transop == translate) tx -= amount * 0.01 ; 
		break;
	case 101: //up
		if (transop == view) Transform::up(amount,  eye,  up);
		else if (transop == scale) sy += amount * 0.01 ; 
		else if (transop == translate) ty += amount * 0.01 ; 
		break;
	case 102: //right
		if (transop == view) Transform::left(-amount, eye,  up);
		else if (transop == scale) sx += amount * 0.01 ; 
		else if (transop == translate) tx += amount * 0.01 ; 
		break;
	case 103: //down
		if (transop == view) Transform::up(-amount,  eye,  up);
		else if (transop == scale) sy -= amount * 0.01 ; 
		else if (transop == translate) ty -= amount * 0.01 ; 
		break;
	}
	// glutPostRedisplay();
	SDL_GL_SwapWindow(window);
}

void init() {
	// Initialize shaders
	vertexshader = initshaders(GL_VERTEX_SHADER, "shaders/light.vert.glsl") ;
	fragmentshader = initshaders(GL_FRAGMENT_SHADER, "shaders/light.frag.glsl") ;
	shaderprogram = initprogram(vertexshader, fragmentshader) ; 
	// Get locations of all uniform variables.
	enablelighting = glGetUniformLocation(shaderprogram,"enablelighting") ;
	lightpos = glGetUniformLocation(shaderprogram,"lightposn") ;       
	lightcol = glGetUniformLocation(shaderprogram,"lightcolor") ;       
	numusedcol = glGetUniformLocation(shaderprogram,"numused") ;       
	ambientcol = glGetUniformLocation(shaderprogram,"ambient") ;       
	diffusecol = glGetUniformLocation(shaderprogram,"diffuse") ;       
	specularcol = glGetUniformLocation(shaderprogram,"specular") ;       
	emissioncol = glGetUniformLocation(shaderprogram,"emission") ;       
	shininesscol = glGetUniformLocation(shaderprogram,"shininess") ;    
	projectionPos = glGetUniformLocation(shaderprogram, "projection");
	modelviewPos = glGetUniformLocation(shaderprogram, "modelview");
	// Initialize geometric shapes
	initBufferObjects();
	initTeapot(); initCube(); initSphere();
}

int main(int argc,char* argv[]) {

	if (argc < 2) {
		cerr << "Usage: transforms scenefile [grader input (optional)]\n"; 
		exit(-1); 
	}

    // Setup SDL
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // Setup Window
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    window = SDL_CreateWindow("HW1: Transformations", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    glContext = SDL_GL_CreateContext(window);
    if (SDL_GL_SetSwapInterval(-1) == -1) // adaptive vsync
    {
        SDL_GL_SetSwapInterval(1); // standard vsync
    }

    // Initialise gl3w
    gl3wInit();

	init();
  	readfile(argv[1]) ; 
	// glutDisplayFunc(display);
	// glutSpecialFunc(specialKey);
	// glutKeyboardFunc(keyboard);
	// glutReshapeFunc(reshape);
	// glutReshapeWindow(w,h);

	if (argc > 2) {
		allowGrader = true;
		stringstream tcid;
		tcid << argv[1] << "." << argv[2];
		grader.init(tcid.str());
		grader.loadCommands(argv[2]);
		grader.bindDisplayFunc(display);
		grader.bindSpecialFunc(specialKey);
		grader.bindKeyboardFunc(keyboard);
		grader.bindScreenshotFunc(saveScreenshot);
	}

	printHelp();
	flush(cout);
	bool run = true;
	reshape(w, h);
	while(run)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT: {
					run = false;
				} break;
				case SDL_APP_TERMINATING: {
					run = false;
				} break;
				case SDL_WINDOWEVENT: {
					if(event.window.event == SDL_WINDOWEVENT_RESIZED)
					{
						w = event.window.data1;
						h = event.window.data2;
						reshape(w, h);
					}
				} break;
				case SDL_KEYDOWN: {
					switch(event.key.keysym.scancode)
					{
						case SDL_SCANCODE_LEFT: {
							specialKey(100, 0, 0);
							cout << "Left\n";
						} break;
						case SDL_SCANCODE_UP: {
							specialKey(101, 0, 0);
							cout << "Up\n";
						} break;
						case SDL_SCANCODE_RIGHT: {
							specialKey(102, 0, 0);
							cout << "Right\n";
						} break;
						case SDL_SCANCODE_DOWN: {
							specialKey(103, 0, 0);
							cout << "Down\n";
						} break;
						case SDL_SCANCODE_EQUALS: 
						case SDL_SCANCODE_KP_PLUS: {
							keyboard('+', 0, 0);
						} break;
						case SDL_SCANCODE_MINUS:
						case SDL_SCANCODE_KP_MINUS: {
							keyboard('-', 0, 0);
						} break;
						case SDL_SCANCODE_I: {
							keyboard('i', 0, 0);
						} break;
						case SDL_SCANCODE_G: {
							keyboard('g', 0, 0);
						} break;
						case SDL_SCANCODE_H: {
							keyboard('h', 0, 0);
						} break;
						case SDL_SCANCODE_ESCAPE: {
							// keyboard(27, 0, 0);
							run = false;
						} break;
						case SDL_SCANCODE_R: {
							keyboard('r', 0, 0);
						} break;
						case SDL_SCANCODE_V: {
							keyboard('v', 0, 0);
						} break;
						case SDL_SCANCODE_T: {
							keyboard('t', 0, 0);
						} break;
						case SDL_SCANCODE_S: {
							keyboard('s', 0, 0);
						} break;
						default: break;
					}
				} break;
				default: break;
			}
		}
		display();
    	SDL_GL_SwapWindow(window);
	}
	destroyBufferObjects();

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

	return 0;
}
