/******************************************************************************/
/* The idea is to draw a teapot, that can be moved by a crystal ball          */
/* interface                                                                  */
/******************************************************************************/


// May need to replace with absolute path on some systems 
#define PATH_TO_TEAPOT_OBJ_FILE "teapot.obj"

#include <iostream>
#include "Geometry.h"
#include "shaders.h"
#include "Transform.h"
#include <string>
#include <sstream>
// #include <FreeImage.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include "../UCSD/grader.h"

SDL_Window* window = nullptr;
SDL_GLContext glContext;

int amount; // The amount of rotation for each arrow press

vec3 eye; // The (regularly updated) vector coordinates of the eye location 
vec3 up;  // The (regularly updated) vector coordinates of the up location 
const vec3 eyeinit(0.0,0.0,7.0); // Initial eye position, also for resets
const vec3 upinit(0.0,1.0,0.0); // Initial up position, also for resets
const int amountinit = 5; //Initial step amount for camera movement, also for resets

bool useGlu; // Toggle use of "official" opengl/glm transform vs user code
int w = 500, h = 500; // width and height 

Grader grader;
bool allowGrader = false;

// Constants to set up lighting on the teapot
const vec4 light_position(0,5,10,1);    // Position of light 0
const vec4 light_position1(0,5,-10,1);  // Position of light 1
const vec4 light_specular(0.6,0.3,0,1);    // Specular of light 0
const vec4 light_specular1(0,0.3,0.6,1);   // Specular of light 1
const vec4 one(1,1,1,1);                 // Specular on teapot
const vec4 medium(0.5,0.5,0.5,1);        // Diffuse on teapot
const vec4 small(0.2,0.2,0.2,1);         // Ambient on teapot
const GLfloat high = 100;                      // Shininess of teapot
vec4 light0,light1;

// Variables to set uniform params for lighting fragment shader 
GLuint islight; 
GLuint light0posn; 
GLuint light0color; 
GLuint light1posn; 
GLuint light1color; 
GLuint ambient; 
GLuint diffuse; 
GLuint specular; 
GLuint shininess;
GLuint color;

// Uniform variables used for the transformation matrices
GLuint projectionPosition;
mat4 view, projection;

// New helper transformation function to transform vector by the view matrix
void transformvec (const vec4 input, vec4& output) {
	output = view * input;
}

// Deallocate any dynamically allocated memory here
void cleanup() {
	destroyBufferObjects();

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

std::string imgNumber(int num) {
	std::stringstream ss;
	//Return 3-digit number (or more if num > 999, but this case shouldn't be encountered)
	if(num < 10) {
		ss << "00" << num;
	} else if(num < 100) {
		ss << "0" << num;
	} else {
		ss << num;
	}
	return ss.str();
}

void saveScreenshot(string fname) {
	int pix = w * h;
	BYTE *pixels = new BYTE[3*pix];	
	glReadBuffer(GL_FRONT);
	glReadPixels(0,0,w,h,GL_RGB,GL_UNSIGNED_BYTE, pixels);

	// FIBITMAP *img = FreeImage_ConvertFromRawBits(pixels, w, h, w * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);


	std::cout << "Saving screenshot: " << fname << "\n";

	// FreeImage_Save(FIF_PNG, img, fname.c_str(), 0);
	stbi_flip_vertically_on_write(true);
	if(stbi_write_png(fname.c_str(), w, h, 3, pixels, 3*w) == 0) cout << "failed to write image" << endl;

	delete[] pixels;
}


void printHelp() {
	std::cout << "\npress 'h' to print this message again.\n" 
		<< "press '+' or '-' to change the amount of rotation that\n"
		<< "occurs with each arrow press.\n" 
		<< "press 'i' to run image grader test cases\n"
		<< "press 'g' to switch between using glm::lookAt or your own LookAt.\n"     
		<< "press 'r' to reset the transformation (eye and up).\n"
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
		std::cout << "Using glm::LookAt set to: " 
			<< (useGlu ? " true " : " false ") << "\n"; 
		break;
	case 'h':
		printHelp();
		break;
	case 27:  // Escape to quit
		// exit(0);
		break;
	case 'r': // reset eye and up vectors 
		eye = eyeinit; 
		up = upinit; 
		amount = amountinit;
		std::cout << "eye and up vectors reset, amount set to " << amountinit << "\n";
		break;                 
	}
	// glutPostRedisplay();
	// SDL_GL_SwapWindow(window);
}

//  You will need to enter code for the arrow keys 
//  When an arrow key is pressed, it will call your transform functions

void specialKey(int key,int x,int y) {
	switch(key) {
	case 100: //left
		Transform::left(amount,eye,up);
		break;
	case 101: //up
		Transform::up(amount,eye,up);
		break;
	case 102: //right
		Transform::left(-amount,eye,up);
		break;
	case 103: //down
		Transform::up(-amount,eye,up);
		break;
	}
	// glutPostRedisplay();
	// SDL_GL_SwapWindow(window);
}

// This function gets called when the window size gets changed
void reshape(int width,int height){
	w = width;
	h = height;
	glViewport(0,0,w,h);
	// Set the projection matrix based on the current perspective, then pass
	// the calculated matrix onto the shader program. Remember that the field of
	// view is in radians!
	projection = glm::perspective(90.0f * glm::pi<float>() / 180.0f, (float)w / (float)h, 0.01f, 100.0f);
	glUniformMatrix4fv(projectionPosition, 1, GL_FALSE, &projection[0][0]);
}

void init() 
{
	// Set up initial position for eye,up and amount
	// As well as booleans 

	eye = eyeinit; 
	up = upinit; 
	amount = amountinit;
	useGlu = true;

	glEnable(GL_DEPTH_TEST);

	// The lighting is enabled using the same framework as in mytest 3 
	// Except that we use two point lights
	// For now, lights and materials are set in display.  Will move to init 
	// later, per update lights

	vertexshader = 		initshaders(GL_VERTEX_SHADER,"shaders/light.vert.glsl");
	fragmentshader = 	initshaders(GL_FRAGMENT_SHADER,"shaders/light.frag.glsl");
	shaderprogram = 	initprogram(vertexshader,fragmentshader); 

	islight = 			glGetUniformLocation(shaderprogram,"islight");        
	light0posn = 		glGetUniformLocation(shaderprogram,"light0posn");       
	light0color = 		glGetUniformLocation(shaderprogram,"light0color");       
	light1posn = 		glGetUniformLocation(shaderprogram,"light1posn");       
	light1color = 		glGetUniformLocation(shaderprogram,"light1color");       
	ambient = 			glGetUniformLocation(shaderprogram,"ambient");       
	diffuse = 			glGetUniformLocation(shaderprogram,"diffuse");       
	specular = 			glGetUniformLocation(shaderprogram,"specular");       
	shininess = 		glGetUniformLocation(shaderprogram,"shininess");       
	color = 			glGetUniformLocation(shaderprogram, "color");

	// Get the uniform locations of the transformation matrices
	projectionPosition = glGetUniformLocation(shaderprogram, "projection");
	modelviewPos = glGetUniformLocation(shaderprogram, "modelview");

	// Other initializations here
	initBufferObjects();
	// Load 3D model of the teapot so it can be drawn later.
	parse(PATH_TO_TEAPOT_OBJ_FILE);
}

void display() {
	glClearColor(0,0,1,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// obj.draw();

	const vec3 center(0.0,0.0,0.0); 

	if (useGlu) {
		view = glm::lookAt(eye, center, up);
	}
	else {
		view = Transform::lookAt(eye,up); 
	}

	// Set Light and Material properties for the teapot
	// Lights are transformed by current modelview matrix. 
	// The shader can't do this globally. 
	// So we need to do so manually.  
	transformvec(light_position,light0); 
	transformvec(light_position1,light1); 
	glUniform4fv(light0posn,1,&light0[0]); 
	glUniform4fv(light0color,1,&light_specular[0]); 
	glUniform4fv(light1posn,1,&light1[0]);
	glUniform4fv(light1color,1,&light_specular1[0]);
 
	glUniform4fv(ambient,1,&small[0]);
	glUniform4fv(diffuse,1,&small[0]);
	glUniform4fv(specular,1,&one[0]);
	glUniform1f(shininess, high);
	glUniform1i(islight,true);

	solidTeapot(4.5f);
	// glutSwapBuffers();
	SDL_GL_SwapWindow(window);
}

int main(int argc,char* argv[]) {

// 	//Initialize GLUT
// 	FreeImage_Initialise();
// 	glutInit(&argc,argv);
// #ifdef __APPLE__ // OSX systems require an extra flag
// 	glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
// #else
//     glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
// #endif
// 	glutCreateWindow("HW1: Transformations");

// #ifndef __APPLE__ // GLew not needed on OSX systems.
// 	GLenum err = glewInit() ; 
// 	if (GLEW_OK != err) { 
// 		std::cerr << "Error: " << glewGetString(err) << std::endl; 
// 	} 
// #endif


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
	// glutDisplayFunc(display);
	// glutSpecialFunc(specialKey);
	// glutKeyboardFunc(keyboard);
	// glutReshapeFunc(reshape);
	// glutReshapeWindow(w,h);

	if(argc > 1) {
		allowGrader = true;
		grader.init(argv[1]);
		grader.loadCommands(argv[1]);
		grader.bindDisplayFunc(display);
		grader.bindSpecialFunc(specialKey);
		grader.bindKeyboardFunc(keyboard);
		grader.bindScreenshotFunc(saveScreenshot);
	}

	printHelp();
	// glutMainLoop();	
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
							keyboard(27, 0, 0);
							run = false;
						} break;
						case SDL_SCANCODE_R: {
							keyboard('r', 0, 0);
						} break;
						default: break;
					}
				} break;
				default: break;
			}
		}
		display();
	}

	// FreeImage_DeInitialise();
	cleanup();

	return 0;
}
