// Lab 3b, C++ version
// Terrain generation

// Current contents:
// Terrain being generated on CPU (MakeTerrain). Simple terrain as example: Flat surface with a bump.
// Note the constants kTerrainSize and kPolySize for changing the number of vertices and polygon size!

// Things to do:
// Generate a terrain on CPU, with normal vectors
// Generate a terrain on GPU, in the vertex shader, again with normal vectors.
// Generate textures for the surface (fragment shader).

// If you want to use gradient noise, use the code from Lab 1.

#define MAIN
#include "MicroGlut.h"
#include "GL_utilities.h"
#include "VectorUtils4.h"
#include "LittleOBJLoader.h"
#include "LoadTGA.h"
#include <math.h>
#include "noise1234.h"
#include "simplexnoise1234.h"
#include "cellular.h"
// uses framework OpenGL
// uses framework Cocoa

mat4 projectionMatrix;
Model *floormodel;
GLuint grasstex;

// Reference to shader programs
GLuint phongShader, texShader;

#define kTerrainSize 64
#define kPolySize 0.5

// Terrain data. To be initialized in MakeTerrain or in the shader
vec3 vertices[kTerrainSize*kTerrainSize];
vec2 texCoords[kTerrainSize*kTerrainSize];
vec3 normals[kTerrainSize*kTerrainSize];
GLuint indices[(kTerrainSize-1)*(kTerrainSize-1)*3*2];
float height[kTerrainSize*kTerrainSize];

// These are considered unsafe, but with most C code, write with caution.
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

float fx = 0;
float fz = 0;
float no = 0;


void makePerlinTerrain(int x, int z){
    float i[3];
    float val = 0;
    int ix = z * kTerrainSize + x;
    int numOctaves = 5;

    for(int k = 0; k<numOctaves; k++){
        if(k == 0){
            i[0] = x / (32.0/(k+1));
            i[1] = z / (32.0/(k+1));
            i[2] = (float)k;
        }else{
            i[0] = x / (32.0/(k+1));
            i[1] = z / (32.0/(k+1));
            i[2] = (float)k + 1,0;
        }

        val =+ (noise3(i[0],i[1],i[2]) * (128.0/(k+1)))/(128.0/(k+1));
    }
    vertices[ix] = vec3(x * kPolySize, val*1.5, z * kPolySize);

}




void MakeTerrain()
{
	// TO DO: This is where your terrain generation goes if on CPU.
	for (int x = 0; x < kTerrainSize; x++)
	for (int z = 0; z < kTerrainSize; z++)
	{
		int ix = z * kTerrainSize + x;

		#define bumpHeight 10.5
		#define bumpWidth 12.0

		fx = (float)(x-kTerrainSize/2.0)/kTerrainSize*2.0;
        fz = (float)(z-kTerrainSize/2.0)/kTerrainSize*2.0;

        no = noise2(fx*3, fz*3);

		// squared distance to center
		float h = ( (x - kTerrainSize/2)/bumpWidth * (x - kTerrainSize/2)/bumpWidth +  (z - kTerrainSize/2)/bumpWidth * (z - kTerrainSize/2)/bumpWidth );
		float y = no * bumpHeight;

		makePerlinTerrain(x,z);

		//vertices[ix] = vec3(x * kPolySize, y, z * kPolySize);
		height[ix] = vertices[ix].y;
        texCoords[ix] = vec2(x, z);
        normals[ix] = vec3(0,1,0);
	}

	// Make indices
	// You don't need to change this.
	for (int x = 0; x < kTerrainSize-1; x++)
	for (int z = 0; z < kTerrainSize-1; z++)
	{
		// Quad count
		int q = (z*(kTerrainSize-1)) + (x);
		// Polyon count = q * 2
		// Indices
		indices[q*2*3] = x + z * kTerrainSize; // top left
		indices[q*2*3+1] = x+1 + z * kTerrainSize;
		indices[q*2*3+2] = x + (z+1) * kTerrainSize;
		indices[q*2*3+3] = x+1 + z * kTerrainSize;
		indices[q*2*3+4] = x+1 + (z+1) * kTerrainSize;
		indices[q*2*3+5] = x + (z+1) * kTerrainSize;
	}

	// Make normal vectors
	// TO DO: This is where you calculate normal vectors
	for (int x = 0; x < kTerrainSize; x++)
	for (int z = 0; z < kTerrainSize; z++)
	{
		//normals[z * kTerrainSize + x] = SetVec3(0,1,0);


        // vertical
        vec3 point1 = vertices[((x-1) + z*kTerrainSize)];
        vec3 point2 = vertices[((x+1) + z*kTerrainSize)];

        // horizontal
        vec3 point3 = vertices[(x + (z-1)*kTerrainSize)];
        vec3 point4 = vertices[(x + (z+1)*kTerrainSize)];


        vec3 norm1 = normalize(point1 - point2);
        vec3 norm2 = normalize(point3 - point4);

        if (x == 0 || z == 0 || z == kTerrainSize-1 || x == kTerrainSize-1)
        {
            normals[z * kTerrainSize + x] = vec3(0,abs(height[z*kTerrainSize+x]),0);
        }
        else
        {
            normals[z * kTerrainSize + x] = cross(norm2, norm1);
        }


	}
}

void init(void)
{
	// GL inits
	glClearColor(0.4,0.5,0.8,0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	printError("GL inits");

	projectionMatrix = frustum(-0.15 * 640/480, 0.1 * 640/480, -0.1, 0.1, 0.2, 300.0);

	// Load and compile shader
	phongShader = loadShaders("phong.vert", "phong.frag");
	texShader = loadShaders("textured.vert", "textured.frag");
	printError("init shader");

	// Upload geometry to the GPU:
	MakeTerrain();
	floormodel = LoadDataToModel(vertices, normals, texCoords, NULL,
			indices, kTerrainSize*kTerrainSize, (kTerrainSize-1)*(kTerrainSize-1)*2*3);

	printError("LoadDataToModel");

// Important! The shader we upload to must be active!
	glUseProgram(phongShader);
	glUniformMatrix4fv(glGetUniformLocation(phongShader, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniform1i(glGetUniformLocation(phongShader, "tex"), 0); // Texture unit 0

	glUseProgram(texShader);
	glUniformMatrix4fv(glGetUniformLocation(texShader, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniform1i(glGetUniformLocation(texShader, "tex"), 0); // Texture unit 0

	LoadTGATextureSimple("grass.tga", &grasstex);
	glBindTexture(GL_TEXTURE_2D, grasstex);
	glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_WRAP_S,	GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_WRAP_T,	GL_REPEAT);

	printError("init arrays");
}

vec3 campos = vec3(kTerrainSize*kPolySize/4, 1.5, kTerrainSize*kPolySize/4);
vec3 forward = vec3(8, 0, 8);
vec3 up = vec3(0, 1, 0);

void display(void)
{
	printError("pre display");

	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 worldToView, m;

	if (glutKeyIsDown('a'))
		forward = mat3(Ry(0.03))* forward;
	if (glutKeyIsDown('d'))
		forward = mat3(Ry(-0.03)) * forward;
	if (glutKeyIsDown('w'))
		campos = campos + forward * 0.01;
	if (glutKeyIsDown('s'))
		campos = campos - forward * 0.01;
	if (glutKeyIsDown('q'))
	{
		vec3 side = cross(forward, vec3(0,1,0));
		campos = campos - side * 0.01;
	}
	if (glutKeyIsDown('e'))
	{
		vec3 side = cross(forward, vec3(0,1,0));
		campos = campos + side * 0.01;
	}

	// Move up/down
	if (glutKeyIsDown('z'))
		campos = campos + vec3(0,1,0) * 0.01;
	if (glutKeyIsDown('c'))
		campos = campos - vec3(0,1,0) * 0.01;

	// NOTE: Looking up and down is done by making a side vector and rotation around arbitrary axis!
	if (glutKeyIsDown('+'))
	{
		vec3 side = cross(forward, vec3(0,1,0));
		mat4 m = ArbRotate(side, 0.01);
		forward = mat3(m) * forward;
	}
	if (glutKeyIsDown('-'))
	{
		vec3 side = cross(forward, vec3(0,1,0));
		mat4 m = ArbRotate(side, -0.01);
		forward = m * forward;
	}

	worldToView = lookAtv(campos, campos + forward, up);

	glBindTexture(GL_TEXTURE_2D, grasstex); // The texture is not used but provided as example
	// Floor
	GLuint shader = texShader;
	glUseProgram(shader);
	m = worldToView;
	glUniformMatrix4fv(glGetUniformLocation(shader, "modelviewMatrix"), 1, GL_TRUE, m.m);
	DrawModel(floormodel, shader, "inPosition", "inNormal", "inTexCoord");

	printError("display");

	glutSwapBuffers();
}

void keys(unsigned char key, int x, int y)
{
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 2);
	glutInitWindowSize(640,360);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutCreateWindow ("Lab 3b");
	glutRepeatingTimer(20);
	glutDisplayFunc(display);
	glutKeyboardFunc(keys);
	init ();
	glutMainLoop();
}
