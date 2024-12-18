// TNM084 Lab 1
// Procedural textures
// Draws concentric circles on CPU and GPU
// Make something more interesting!

// For the CPU part, you should primarily work in maketexture() below.

// New C++ version!

//uses framework Cocoa
//uses framework OpenGL
#define MAIN
#include "MicroGlut.h"
#include "GL_utilities.h"
#include <math.h>
#include <stdio.h>
#include "VectorUtils4.h"
#include "LittleOBJLoader.h"

// Lab 1 specific
// Code for you to access to make interesting patterns.
// This is for CPU. For GPU, I provide separate files
// with functions that you can add to your fragment shaders.
#include "noise1234.h"
#include "simplexnoise1234.h"
#include "cellular.h"

#define kTextureSize 800
GLubyte ptex[kTextureSize][kTextureSize][3];
const float ringDensity = 15.0;

double fract(double x){
    return(x-floor(x));
}

// Example: Radial pattern.
void maketexture()
{
// Variables
	int x, y;
    int width = 80;
    int height = 40;
    int gap = 5;
    int row = 1;
    float tempY = 1.;
    float tempX = 1.;
    float fx;
    float fy;
    float no;
    float squareSize;
    float checkNum = 8.0;
    float xx;
    float yy;
    float xn;
    float xr;


    // Background color
   /* for (x = 0; x < kTextureSize; x++)
    for (y = 0; y < kTextureSize; y++)
    {
        ptex[x][y][0] = 80;
        ptex[x][y][1] = 80;
        ptex[x][y][2] = 80;
    }*/


    // Bricks
    for (x = 0; x < kTextureSize; x++)
    for (y = 0; y < kTextureSize; y++)
    {

        // Calculations for a brick pattern
            squareSize = round(kTextureSize / checkNum); // How large is one square in pixels, used like density in _RADIAL texture

            // Coordinates inside a brick
            yy = fract((float)(y)/2/squareSize + trunc(x / squareSize)/2);
            xx = fract((float)(x)/2/squareSize + trunc(x / squareSize)/2);

            // Deriviate approximation, i think
            fx = (float)(x-kTextureSize/2.0)/kTextureSize*2.0;
            fy = (float)(y-kTextureSize/2.0)/kTextureSize*2.0;

            xr = noise2(fx*78, fy*78); // Brick noise
            xn = noise2(fx*50, fy*50); // Mortar noise

            if((yy >= 0) && (yy < 0.1)) // Vertical mortar width
                {
                ptex[x][y][0] = xn*10 + 45; // Red
                ptex[x][y][1] = xn*10 + 45; // Green
                ptex[x][y][2] = xn*10 + 45; // Blue
                }
            else if((yy >= 0.1)  &&  ((xx >= 0) && (xx < 0.06*xn*4+0.2))) // Horizontal mortar height, affected by yy
                {
                ptex[x][y][0] = xn*5 + 45; // Red
                ptex[x][y][1] = xn*5 + 45; // Green
                ptex[x][y][2] = xn*5 + 45; // Blue
                }
            else
                {
                // Bricks
                ptex[x][y][0] =  xr * 40 + 150;  // Red
                ptex[x][y][1] =  65; // Green
                ptex[x][y][2] = 65;  // Blue
                }
    }



        /*fx = (float)(x-kTextureSize/2.0)/kTextureSize*2.0;
        fy = (float)(y-kTextureSize/2.0)/kTextureSize*2.0;

        no = noise2(fy*17, fx*17);

        //width += no;
        //height += no;


        // Print current pixel orange
        ptex[x][y][0] = no * 50 + 200;
        ptex[x][y][1] = 90;
        ptex[x][y][2] = 0;

        // Print every other row with full bricks and every other row with half bricks
        if(row % 2 == 0 && y % (width/2) == 0)
        {
            y += gap;
        }
        else if(y % width == 0)
        {
            y += gap;
        }

        if(x % height == 0)
        {
            x += gap;
            row++;
        }

        width = 80;
        height = 40;
    }*/





/*
	//Circles
	int x, y;
	float fx, fy, fxo, fyo;
//	char val;

	for (x = 0; x < kTextureSize; x++)
	for (y = 0; y < kTextureSize; y++)
	{
        // Stretch in x or y axis
		fx = (float)(x-kTextureSize/2.)/kTextureSize;
		fy = (float)(y-kTextureSize/2.)/kTextureSize * 2;

		fxo = sin(noise1(fx*7));
		fyo = noise1(fy*5);
		fxo *= snoise2(fxo, fyo);

		fxo = fxo * exp(fyo) * 37;
		fyo = fyo * snoise2(fyo,fxo) * 16;

		fxo = sin(fxo) * 37;

		if (fxo > 1.0) fxo = 1;
		if (fxo < -1.0) fxo = -1.0;
		if (fyo > 1.0) fyo = 1.0;
		if (fyo < -1.0) fyo = -1.0;

		// Color rgb
		ptex[x][y][0] = fyo * 27 + 181;
		ptex[x][y][1] = fxo * 37 * exp(fyo) + 35;
		ptex[x][y][2] = 188;


	}*/
}

// Globals
// Data would normally be read from files
vec3 vertices[] = { vec3(-1.0f,-1.0f,0.0f),
					vec3(1.0f,-1.0f,0.0f),
					vec3(1.0f,1.0f,0.0f),
					vec3(-1.0f,1.0f,0.0f),
					vec3(1.0f,1.0f,0.0f),
					vec3(1.0f,-1.0f,0.0f) };
vec2 texCoords[] = {vec2(0.0f,0.0f),
					vec2(1.0f,0.0f),
					vec2(1.0f,1.0f),
					vec2(0.0f,1.0f),
					vec2(1.0f,1.0f),
					vec2(1.0f,0.0f) };
GLuint indices[] =
{
	0, 1, 2,
	0, 2, 3
};

// vertex array object
//unsigned int vertexArrayObjID;
// Texture reference
GLuint texid;
// Switch between CPU and shader generation
int displayGPUversion = 0;
// Reference to shader program
GLuint program;
// The quad
Model* quad;

void init(void)
{
	// two vertex buffer objects, used for uploading the
//	unsigned int vertexBufferObjID;
//	unsigned int texBufferObjID;

	// GL inits
	glClearColor(0.2,0.2,0.5,0);
	glEnable(GL_DEPTH_TEST);
	printError("GL inits");

	// Load and compile shader
	program = loadShaders("lab1.vert", "lab1.frag");
	glUseProgram(program);
	printError("init shader");

	// Upload geometry to the GPU:

	quad = LoadDataToModel(vertices,NULL,texCoords,NULL,indices,4,6);

	// Texture unit
	glUniform1i(glGetUniformLocation(program, "tex"), 0); // Texture unit 0

// Constants common to CPU and GPU
	glUniform1i(glGetUniformLocation(program, "displayGPUversion"), 0); // shader generation off
	glUniform1f(glGetUniformLocation(program, "ringDensity"), ringDensity);

	maketexture();

// Upload texture
	glGenTextures(1, &texid); // texture id
	glBindTexture(GL_TEXTURE_2D, texid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, kTextureSize, kTextureSize, 0, GL_RGB, GL_UNSIGNED_BYTE, ptex);

	// End of upload of geometry
	printError("init arrays");
}

// Switch on any key
void key(unsigned char key, int x, int y)
{
	displayGPUversion = !displayGPUversion;
	glUniform1i(glGetUniformLocation(program, "displayGPUversion"), displayGPUversion); // shader generation off
	printf("Changed flag to %d\n", displayGPUversion);
	glutPostRedisplay();
}

void display(void)
{
	printError("pre display");

	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DrawModel(quad, program, "in_Position", NULL, "in_TexCoord");

	printError("display");

	glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 2);
	glutInitWindowSize(kTextureSize, kTextureSize);
	glutCreateWindow ("Wooo :3");
	glutDisplayFunc(display);
	glutKeyboardFunc(key);
	init ();
	glutMainLoop();
}
