#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <iostream>
//#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <math.h>

#include <GL/glew.h>
#include<vector>
#include <GL/freeglut.h>
//#include "OFFReader.c"
#include "file_utils.h"
#include "math_utils.h"
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#define max(x, y) (((x) > (y)) ? (x) : (y))
#define min(x, y) (((x) < (y)) ? (x) : (y))
/********************************************************************/
/*   Variables */

int i, j;


const int vert = 11665408;
Vector3f Vertices[vert];
//Vector3f poly[noPolys];
const int ind = 256*256*178;


int teapotColour[vert];

Vector3f newvert[ind];

Vector3f cubeVertices[8];

char theProgramTitle[] = "Task 3c";
int theWindowWidth = 700, theWindowHeight = 700;
int theWindowPositionX = 40, theWindowPositionY = 40;
bool isFullScreen = true;
bool isAnimating = false;
float rotation = 0.0f;
float translate = 0.0f;
GLuint VBO, VAO, cubeVAO, cubeVBO;
GLuint gWorldLocation, now;
GLuint posID, colorID;
GLuint elementbuffer, cubeelementbuffer;
unsigned int indeces[ind], cubeindeces[24];

/* Constants */
const int ANIMATION_DELAY = 20; /* milliseconds between rendering */
const char* pVSFileName = "shader.vs";
const char* pFSFileName = "shader.fs";
int totalvert = 0;

/********************************************************************
  Utility functions
 */

 /* post: compute frames per second and display in window's title bar */
void computeFPS() {
	static int frameCount = 0;
	static int lastFrameTime = 0;
	static char* title = NULL;
	int currentTime;

	if (!title)
		title = (char*)malloc((strlen(theProgramTitle) + 20) * sizeof(char));
	frameCount++;
	currentTime = glutGet((GLenum)(GLUT_ELAPSED_TIME));
	if (currentTime - lastFrameTime > 1000) {
		sprintf(title, "%s [ FPS: %4.2f ]",
			theProgramTitle,
			frameCount * 1000.0 / (currentTime - lastFrameTime));
		glutSetWindowTitle(title);
		lastFrameTime = currentTime;
		frameCount = 0;
	}
}

float interpolate(float x, float y,float isolevel,float a,float b)
{
		float z;
		if (a == b) {
			cout << "1";
			z = (x + y) / 2.0f;
			
		}
		else {
			// interp == 0: vert should be at p1
			// interp == 1: vert should be at p2
			float interp = abs((isolevel - a) / (b - a));
			float oneMinusInterp = 1 - interp;

			z = x * oneMinusInterp + y * interp;
		}
		return z;
}

void tetra(int isolevel, vector<int>& col,int a,int b,int c,int d,int &p) {
	unsigned char index = 0;
	for (int i = 0; i < 4; ++i)
		if (col[i] < isolevel)
			index |= (1 << i);
	

	switch (index) {

		// we don't do anything if everyone is inside or outside
	case 0x00:
	case 0x0F:
		break;

		// only vert 0 is inside
	case 0x01:
		newvert[p] = Vector3f(interpolate(Vertices[a].x,Vertices[b].x,isolevel,col[0],col[1]), interpolate(Vertices[a].y,Vertices[b].y,isolevel,col[0],col[1]), interpolate(Vertices[a].z,Vertices[b].z,isolevel,col[0],col[1])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[a].x,Vertices[d].x,isolevel,col[0],col[3]), interpolate(Vertices[a].y,Vertices[d].y,isolevel,col[0],col[3]), interpolate(Vertices[a].z,Vertices[d].z,isolevel,col[0],col[3])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[a].x,Vertices[c].x,isolevel,col[0],col[2]), interpolate(Vertices[a].y,Vertices[c].y,isolevel,col[0],col[2]), interpolate(Vertices[a].z,Vertices[c].z,isolevel,col[0],col[2])); p++;
		break;

		// only vert 1 is inside
	case 0x02:
		newvert[p] = Vector3f(interpolate(Vertices[b].x,Vertices[a].x,isolevel,col[1],col[0]), interpolate(Vertices[b].y,Vertices[a].y,isolevel,col[1],col[0]), interpolate(Vertices[b].z,Vertices[a].z,isolevel,col[1],col[0])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[b].x,Vertices[c].x,isolevel,col[1],col[2]), interpolate(Vertices[b].y,Vertices[c].y,isolevel,col[1],col[2]), interpolate(Vertices[b].z,Vertices[c].z,isolevel,col[1],col[2])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[b].x,Vertices[d].x,isolevel,col[1],col[3]), interpolate(Vertices[b].y,Vertices[d].y,isolevel,col[1],col[3]), interpolate(Vertices[b].z,Vertices[d].z,isolevel,col[1],col[3])); p++;
		break;
		

		// only vert 2 is inside
	case 0x04:
		newvert[p] = Vector3f(interpolate(Vertices[c].x,Vertices[a].x,isolevel,col[2],col[0]), interpolate(Vertices[c].y,Vertices[a].y,isolevel,col[2],col[0]), interpolate(Vertices[c].z,Vertices[a].z,isolevel,col[2],col[0])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[c].x,Vertices[d].x,isolevel,col[2],col[3]), interpolate(Vertices[c].y,Vertices[d].y,isolevel,col[2],col[3]), interpolate(Vertices[c].z,Vertices[d].z,isolevel,col[2],col[3])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[c].x,Vertices[b].x,isolevel,col[2],col[1]), interpolate(Vertices[c].y,Vertices[b].y,isolevel,col[2],col[1]), interpolate(Vertices[c].z,Vertices[b].z,isolevel,col[2],col[1])); p++;
		break;
		

		// only vert 3 is inside
	case 0x08:
		newvert[p] = Vector3f(interpolate(Vertices[d].x,Vertices[b].x,isolevel,col[3],col[1]), interpolate(Vertices[d].y,Vertices[b].y,isolevel,col[3],col[1]), interpolate(Vertices[d].z,Vertices[b].z,isolevel,col[3],col[1])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[d].x,Vertices[c].x,isolevel,col[3],col[2]), interpolate(Vertices[d].y,Vertices[c].y,isolevel,col[3],col[2]), interpolate(Vertices[d].z,Vertices[c].z,isolevel,col[3],col[2])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[d].x,Vertices[a].x,isolevel,col[3],col[0]), interpolate(Vertices[d].y,Vertices[a].y,isolevel,col[3],col[0]), interpolate(Vertices[d].z,Vertices[a].z,isolevel,col[3],col[0])); p++;
		break;
		

		// verts 0, 1 are inside
	case 0x03:
		newvert[p] = Vector3f(interpolate(Vertices[a].x,Vertices[d].x,isolevel,col[0],col[3]), interpolate(Vertices[a].y,Vertices[d].y,isolevel,col[0],col[3]), interpolate(Vertices[a].z,Vertices[d].z,isolevel,col[0],col[3])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[a].x,Vertices[c].x,isolevel,col[0],col[2]), interpolate(Vertices[a].y,Vertices[c].y,isolevel,col[0],col[2]), interpolate(Vertices[a].z,Vertices[c].z,isolevel,col[0],col[2])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[b].x,Vertices[d].x,isolevel,col[1],col[3]), interpolate(Vertices[b].y,Vertices[d].y,isolevel,col[1],col[3]), interpolate(Vertices[b].z,Vertices[d].z,isolevel,col[1],col[3])); p++;
		
		newvert[p] = Vector3f(interpolate(Vertices[a].x,Vertices[c].x,isolevel,col[0],col[2]), interpolate(Vertices[a].y,Vertices[c].y,isolevel,col[0],col[2]), interpolate(Vertices[a].z,Vertices[c].z,isolevel,col[0],col[2])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[c].x,Vertices[b].x,isolevel,col[2],col[1]), interpolate(Vertices[c].y,Vertices[b].y,isolevel,col[2],col[1]), interpolate(Vertices[c].z,Vertices[b].z,isolevel,col[2],col[1])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[b].x,Vertices[d].x,isolevel,col[1],col[3]), interpolate(Vertices[b].y,Vertices[d].y,isolevel,col[1],col[3]), interpolate(Vertices[b].z,Vertices[d].z,isolevel,col[1],col[3])); p++;
		break;

		// verts 0, 2 are insideasd
	case 0x05:
		newvert[p] = Vector3f(interpolate(Vertices[a].x,Vertices[b].x,isolevel,col[0],col[1]), interpolate(Vertices[a].y,Vertices[b].y,isolevel,col[0],col[1]), interpolate(Vertices[a].z,Vertices[b].z,isolevel,col[0],col[1])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[a].x,Vertices[d].x,isolevel,col[0],col[3]), interpolate(Vertices[a].y,Vertices[d].y,isolevel,col[0],col[3]), interpolate(Vertices[a].z,Vertices[d].z,isolevel,col[0],col[3])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[b].x,Vertices[c].x,isolevel,col[1],col[2]), interpolate(Vertices[b].y,Vertices[c].y,isolevel,col[1],col[2]), interpolate(Vertices[b].z,Vertices[c].z,isolevel,col[1],col[2])); p++;
		
		newvert[p] = Vector3f(interpolate(Vertices[c].x,Vertices[b].x,isolevel,col[2],col[1]), interpolate(Vertices[c].y,Vertices[b].y,isolevel,col[2],col[1]), interpolate(Vertices[c].z,Vertices[b].z,isolevel,col[2],col[1])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[a].x,Vertices[d].x,isolevel,col[0],col[3]), interpolate(Vertices[a].y,Vertices[d].y,isolevel,col[0],col[3]), interpolate(Vertices[a].z,Vertices[d].z,isolevel,col[0],col[3])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[d].x,Vertices[c].x,isolevel,col[3],col[2]), interpolate(Vertices[d].y,Vertices[c].y,isolevel,col[3],col[2]), interpolate(Vertices[d].z,Vertices[c].z,isolevel,col[3],col[2])); p++;
		break;


		// verts 0, 3 are inside
	case 0x09:
		newvert[p] = Vector3f(interpolate(Vertices[a].x,Vertices[b].x,isolevel,col[0],col[1]), interpolate(Vertices[a].y,Vertices[b].y,isolevel,col[0],col[1]), interpolate(Vertices[a].z,Vertices[b].z,isolevel,col[0],col[1])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[b].x,Vertices[d].x,isolevel,col[1],col[3]), interpolate(Vertices[b].y,Vertices[d].y,isolevel,col[1],col[3]), interpolate(Vertices[b].z,Vertices[d].z,isolevel,col[1],col[3])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[a].x,Vertices[c].x,isolevel,col[0],col[2]), interpolate(Vertices[a].y,Vertices[c].y,isolevel,col[0],col[2]), interpolate(Vertices[a].z,Vertices[c].z,isolevel,col[0],col[2])); p++;
		
		newvert[p] = Vector3f(interpolate(Vertices[d].x,Vertices[b].x,isolevel,col[3],col[1]), interpolate(Vertices[d].y,Vertices[b].y,isolevel,col[3],col[1]), interpolate(Vertices[d].z,Vertices[b].z,isolevel,col[3],col[1])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[c].x,Vertices[d].x,isolevel,col[2],col[3]), interpolate(Vertices[c].y,Vertices[d].y,isolevel,col[2],col[3]), interpolate(Vertices[c].z,Vertices[d].z,isolevel,col[2],col[3])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[a].x,Vertices[c].x,isolevel,col[0],col[2]), interpolate(Vertices[a].y,Vertices[c].y,isolevel,col[0],col[2]), interpolate(Vertices[a].z,Vertices[c].z,isolevel,col[0],col[2])); p++;
		
		break;

		// verts 1, 2 are inside
	case 0x06:
		newvert[p] = Vector3f(interpolate(Vertices[a].x,Vertices[b].x,isolevel,col[0],col[1]), interpolate(Vertices[a].y,Vertices[b].y,isolevel,col[0],col[1]), interpolate(Vertices[a].z,Vertices[b].z,isolevel,col[0],col[1])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[b].x,Vertices[d].x,isolevel,col[1],col[3]), interpolate(Vertices[b].y,Vertices[d].y,isolevel,col[1],col[3]), interpolate(Vertices[b].z,Vertices[d].z,isolevel,col[1],col[3])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[a].x,Vertices[c].x,isolevel,col[0],col[2]), interpolate(Vertices[a].y,Vertices[c].y,isolevel,col[0],col[2]), interpolate(Vertices[a].z,Vertices[c].z,isolevel,col[0],col[2])); p++;
		
		newvert[p] = Vector3f(interpolate(Vertices[a].x,Vertices[c].x,isolevel,col[0],col[2]), interpolate(Vertices[a].y,Vertices[c].y,isolevel,col[0],col[2]), interpolate(Vertices[a].z,Vertices[c].z,isolevel,col[0],col[2])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[b].x,Vertices[d].x,isolevel,col[1],col[3]), interpolate(Vertices[b].y,Vertices[d].y,isolevel,col[1],col[3]), interpolate(Vertices[b].z,Vertices[d].z,isolevel,col[1],col[3])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[d].x,Vertices[c].x,isolevel,col[3],col[2]), interpolate(Vertices[d].y,Vertices[c].y,isolevel,col[3],col[2]), interpolate(Vertices[d].z,Vertices[c].z,isolevel,col[3],col[2])); p++;
		break;
		
		// verts 2, 3 are inside
	case 0x0C:
		newvert[p] = Vector3f(interpolate(Vertices[d].x,Vertices[b].x,isolevel,col[3],col[1]), interpolate(Vertices[d].y,Vertices[b].y,isolevel,col[3],col[1]), interpolate(Vertices[d].z,Vertices[b].z,isolevel,col[3],col[1])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[a].x,Vertices[d].x,isolevel,col[0],col[3]), interpolate(Vertices[a].y,Vertices[d].y,isolevel,col[0],col[3]), interpolate(Vertices[a].z,Vertices[d].z,isolevel,col[0],col[3])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[a].x,Vertices[c].x,isolevel,col[0],col[2]), interpolate(Vertices[a].y,Vertices[c].y,isolevel,col[0],col[2]), interpolate(Vertices[a].z,Vertices[c].z,isolevel,col[0],col[2])); p++;
		
		newvert[p] = Vector3f(interpolate(Vertices[a].x,Vertices[c].x,isolevel,col[0],col[2]), interpolate(Vertices[a].y,Vertices[c].y,isolevel,col[0],col[2]), interpolate(Vertices[a].z,Vertices[c].z,isolevel,col[0],col[2])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[b].x,Vertices[d].x,isolevel,col[1],col[3]), interpolate(Vertices[b].y,Vertices[d].y,isolevel,col[1],col[3]), interpolate(Vertices[b].z,Vertices[d].z,isolevel,col[1],col[3])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[b].x,Vertices[c].x,isolevel,col[1],col[2]), interpolate(Vertices[b].y,Vertices[c].y,isolevel,col[1],col[2]), interpolate(Vertices[b].z,Vertices[c].z,isolevel,col[1],col[2])); p++;
		break;

		// verts 1, 3 are inside
	case 0x0A:
		newvert[p] = Vector3f(interpolate(Vertices[a].x,Vertices[b].x,isolevel,col[0],col[1]), interpolate(Vertices[a].y,Vertices[b].y,isolevel,col[0],col[1]), interpolate(Vertices[a].z,Vertices[b].z,isolevel,col[0],col[1])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[a].x,Vertices[d].x,isolevel,col[0],col[3]), interpolate(Vertices[a].y,Vertices[d].y,isolevel,col[0],col[3]), interpolate(Vertices[a].z,Vertices[d].z,isolevel,col[0],col[3])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[b].x,Vertices[c].x,isolevel,col[1],col[2]), interpolate(Vertices[b].y,Vertices[c].y,isolevel,col[1],col[2]), interpolate(Vertices[b].z,Vertices[c].z,isolevel,col[1],col[2])); p++;
		
		newvert[p] = Vector3f(interpolate(Vertices[c].x,Vertices[b].x,isolevel,col[2],col[1]), interpolate(Vertices[c].y,Vertices[b].y,isolevel,col[2],col[1]), interpolate(Vertices[c].z,Vertices[b].z,isolevel,col[2],col[1])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[a].x,Vertices[d].x,isolevel,col[0],col[3]), interpolate(Vertices[a].y,Vertices[d].y,isolevel,col[0],col[3]), interpolate(Vertices[a].z,Vertices[d].z,isolevel,col[0],col[3])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[d].x,Vertices[c].x,isolevel,col[3],col[2]), interpolate(Vertices[d].y,Vertices[c].y,isolevel,col[3],col[2]), interpolate(Vertices[d].z,Vertices[c].z,isolevel,col[3],col[2])); p++;
		break;

		

		// verts 0, 1, 2 are inside
	case 0x07:
		newvert[p] = Vector3f(interpolate(Vertices[d].x,Vertices[b].x,isolevel,col[3],col[1]), interpolate(Vertices[d].y,Vertices[b].y,isolevel,col[3],col[1]), interpolate(Vertices[d].z,Vertices[b].z,isolevel,col[3],col[1])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[a].x,Vertices[d].x,isolevel,col[0],col[3]), interpolate(Vertices[a].y,Vertices[d].y,isolevel,col[0],col[3]), interpolate(Vertices[a].z,Vertices[d].z,isolevel,col[0],col[3])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[d].x,Vertices[c].x,isolevel,col[3],col[2]), interpolate(Vertices[d].y,Vertices[c].y,isolevel,col[3],col[2]), interpolate(Vertices[d].z,Vertices[c].z,isolevel,col[3],col[2])); p++;
		break;
		
		// verts 0, 1, 3 are inside
	case 0x0B:
		newvert[p] = Vector3f(interpolate(Vertices[c].x,Vertices[b].x,isolevel,col[2],col[1]), interpolate(Vertices[c].y,Vertices[b].y,isolevel,col[2],col[1]), interpolate(Vertices[c].z,Vertices[b].z,isolevel,col[2],col[1])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[c].x,Vertices[d].x,isolevel,col[2],col[3]), interpolate(Vertices[c].y,Vertices[d].y,isolevel,col[2],col[3]), interpolate(Vertices[c].z,Vertices[d].z,isolevel,col[2],col[3])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[a].x,Vertices[c].x,isolevel,col[0],col[2]), interpolate(Vertices[a].y,Vertices[c].y,isolevel,col[0],col[2]), interpolate(Vertices[a].z,Vertices[c].z,isolevel,col[0],col[2])); p++;
		break;
		

		// verts 0, 2, 3 are inside
	case 0x0D:
		newvert[p] = Vector3f(interpolate(Vertices[a].x,Vertices[b].x,isolevel,col[0],col[1]), interpolate(Vertices[a].y,Vertices[b].y,isolevel,col[0],col[1]), interpolate(Vertices[a].z,Vertices[b].z,isolevel,col[0],col[1])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[b].x,Vertices[d].x,isolevel,col[1],col[3]), interpolate(Vertices[b].y,Vertices[d].y,isolevel,col[1],col[3]), interpolate(Vertices[b].z,Vertices[d].z,isolevel,col[1],col[3])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[b].x,Vertices[c].x,isolevel,col[1],col[2]), interpolate(Vertices[b].y,Vertices[c].y,isolevel,col[1],col[2]), interpolate(Vertices[b].z,Vertices[c].z,isolevel,col[1],col[2])); p++;
		break;
		

		// verts 1, 2, 3 are inside
	case 0x0E:
		newvert[p] = Vector3f(interpolate(Vertices[a].x,Vertices[b].x,isolevel,col[0],col[1]), interpolate(Vertices[a].y,Vertices[b].y,isolevel,col[0],col[1]), interpolate(Vertices[a].z,Vertices[b].z,isolevel,col[0],col[1])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[a].x,Vertices[d].x,isolevel,col[0],col[3]), interpolate(Vertices[a].y,Vertices[d].y,isolevel,col[0],col[3]), interpolate(Vertices[a].z,Vertices[d].z,isolevel,col[0],col[3])); p++;
		newvert[p] = Vector3f(interpolate(Vertices[a].x,Vertices[c].x,isolevel,col[0],col[2]), interpolate(Vertices[a].y,Vertices[c].y,isolevel,col[0],col[2]), interpolate(Vertices[a].z,Vertices[c].z,isolevel,col[0],col[2])); p++;
		break;
		

		// what is this I don't even
	default:
		break;
	}

}

static void CreateVertexBuffer() {
	glGenVertexArrays(1, &VAO);
	cout << "VAO: " << VAO << endl;
	glBindVertexArray(VAO);

	//cout << q<<" ";

	FILE* inp = fopen("dest.off", "r");
	int i = 0, j = 0, k = 0, l = 0, temp = 0;
	float q = 0.0;
	for (k = 0; k < 178; k++)
	{
		for (j = 0; j < 256; j++)
		{
			for (i = 0; i < 256; i++)
			{
				Vertices[l] = Vector3f(i*0.005, j*0.005, k*0.005);
				fscanf(inp, "%d", &temp);
				
				teapotColour[l] = temp;
				
				l++;
				
			}
		}
		
	}
	fclose(inp);
	int p = 0;
	l = 0;
	int z = 85;
	int iso = 25;
	for (z = 0; z < 178; z++)
	{
		for (l = z * (256 * 256); l < ((z + 1) * (256 * 256) - 256); l++)
		{
			if ((l % 256) != 255)
			{
				int a, b, c, d, e, f, g, h;
				a = l;
				b = l + 1;
				c = l + 257;
				d = l + 256;
				e = (256 * 256) + (l);
				f = e + 1;
				g = e + 257;
				h = e + 256;

				vector<int>col(4,0);
				col[0] = (teapotColour[h]), col[1] = (teapotColour[g]), col[2] = (teapotColour[c]) ,col[3] = (teapotColour[f]);
				tetra(iso, col, h, g, c, f, p);

				col[0] = teapotColour[h], col[1] = teapotColour[c], col[2] = teapotColour[d], col[3] = teapotColour[b];
				tetra(iso, col, h, c, d, b, p);

				col[0] = teapotColour[d], col[1] = teapotColour[a], col[2] = teapotColour[e], col[3] = teapotColour[b];
				tetra(iso, col, d, a, e, b, p);

				col[0] = teapotColour[h], col[1] = teapotColour[d], col[2] = teapotColour[e], col[3] = teapotColour[b];
				tetra(iso, col, h, d, e, b, p);

				col[0] = teapotColour[h], col[1] = teapotColour[c], col[2] = teapotColour[f], col[3] = teapotColour[b];
				tetra(iso, col, h, c, f, b, p);

				col[0] = teapotColour[h], col[1] = teapotColour[e], col[2] = teapotColour[b], col[3] = teapotColour[f];
				tetra(iso, col, h, e, b, f, p);

			}
		}
	}
	totalvert = p;
	
	cout <<  " \n" << p << "\n";
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(newvert), newvert, GL_STATIC_DRAW);


	glEnableVertexAttribArray(posID);
	glVertexAttribPointer(posID, 3, GL_FLOAT, GL_FALSE, 0, 0);

	



	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	cubeVertices[0] = Vector3f(0, 0, 0);
	cubeVertices[1] = Vector3f(255*0.005, 0, 0);
	cubeVertices[2] = Vector3f(255*0.005, 255*0.005, 0);
	cubeVertices[3] = Vector3f(0, 255*0.005, 0);
	cubeVertices[4] = Vector3f(0, 0, 178*0.005);
	cubeVertices[5] = Vector3f(255*0.005, 0, 178*0.005);
	cubeVertices[6] = Vector3f(255*0.005, 255*0.005, 178*0.005);
	cubeVertices[7] = Vector3f(0, 255*0.005, 178*0.005);

	cubeindeces[0] = 0;
	cubeindeces[1] = 1;
	cubeindeces[2] = 1;
	cubeindeces[3] = 2;
	cubeindeces[4] = 2;
	cubeindeces[5] = 3;
	cubeindeces[6] = 3;
	cubeindeces[7] = 7;
	cubeindeces[8] = 7;
	cubeindeces[9] = 4;
	cubeindeces[10] = 4;
	cubeindeces[11] = 0;
	cubeindeces[12] = 4;
	cubeindeces[13] = 5;
	cubeindeces[14] = 5;
	cubeindeces[15] = 1;
	cubeindeces[16] = 5;
	cubeindeces[17] = 6;
	cubeindeces[18] = 6;
	cubeindeces[19] = 7;
	cubeindeces[20] = 2;
	cubeindeces[21] = 6;
	cubeindeces[22] = 0;
	cubeindeces[23] = 3;

	glGenVertexArrays(1, &cubeVAO);
	cout << "VAO: " << VAO << endl;
	glBindVertexArray(cubeVAO);


	glGenBuffers(1, &cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(posID);
	glVertexAttribPointer(posID, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &cubeelementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeelementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 24 * sizeof(unsigned int), cubeindeces, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType) {
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
		exit(0);
	}

	const GLchar* p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0] = strlen(pShaderText);
	glShaderSource(ShaderObj, 1, p, Lengths);
	glCompileShader(ShaderObj);
	GLint success;
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		exit(1);
	}

	glAttachShader(ShaderProgram, ShaderObj);
}

using namespace std;

static void CompileShaders() {
	GLuint ShaderProgram = glCreateProgram();

	if (ShaderProgram == 0) {
		fprintf(stderr, "Error creating shader program\n");
		exit(1);
	}

	string vs, fs;

	if (!ReadFile(pVSFileName, vs)) {
		exit(1);
	}

	if (!ReadFile(pFSFileName, fs)) {
		exit(1);
	}



	AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);
	AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	glLinkProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
		exit(1);
	}
	glBindVertexArray(VAO);
	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Invalid shader program1: '%s'\n", ErrorLog);
		exit(1);
	}

	glUseProgram(ShaderProgram);
	gWorldLocation = glGetUniformLocation(ShaderProgram, "gWorld");
	posID = glGetAttribLocation(ShaderProgram, "Position");
	colorID = glGetAttribLocation(ShaderProgram, "Colour");
	now = glGetUniformLocation(ShaderProgram, "now");
}

/********************************************************************
 Callback Functions
 These functions are registered with the glut window and called
 when certain events occur.
 */

void onInit(int argc, char* argv[])
/* pre:  glut window has been initialized
   post: model has been initialized */ {
   /* by default the back ground color is black */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	CompileShaders();
	CreateVertexBuffer();


	/* set to draw in window based on depth  */
	glEnable(GL_DEPTH_TEST);
}

static void onDisplay() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glPointSize(5);
	//cout<<"yes"<<"\n";
	Matrix4f World;

	World.m[0][0] = 1.0f; World.m[0][1] = 0.0f; World.m[0][2] = 0.1f; World.m[0][3] = 0.0f;
	World.m[1][0] = 0.0f; World.m[1][1] = 1.0f; World.m[1][2] = 0.0f;  World.m[1][3] = 0.0f;
	World.m[2][0] = 0.0f; World.m[2][1] = 0.0f; World.m[2][2] = 1.0f; World.m[2][3] = 0.0f;
	World.m[3][0] = 0.0f; World.m[3][1] = 0.0f; World.m[3][2] = 0.4f; World.m[3][3] = 1.0f;

	Matrix4f T;

	T.m[0][0] = 1.0f; T.m[0][1] = 0.0f; T.m[0][2] = 0.0f; T.m[0][3] = 0.0f;
	T.m[1][0] = 0.0f; T.m[1][1] = 1.0f; T.m[1][2] = 0.0f; T.m[1][3] = 0.0f;
	T.m[2][0] = 0.0f; T.m[2][1] = 0.0f; T.m[2][2] = 1.0f; T.m[2][3] = 0.4425f;
	T.m[3][0] = 0.0f; T.m[3][1] = 0.0f; T.m[3][2] = 0.0f; T.m[3][3] = 1.0f;

	Matrix4f R;

	R.m[0][0] = 1.0; R.m[0][1] = 0.0; R.m[0][2] = 0.0f; R.m[0][3] = -0.6375f;
	R.m[1][0] = 0.0; R.m[1][1] = 1.0;  R.m[1][2] = 0.0f; R.m[1][3] = -0.6375f;
	R.m[2][0] = 0.0f;        R.m[2][1] = 0.0f;   R.m[2][2] = 1.0f; R.m[2][3] = -0.4425f;
	R.m[3][0] = 0.0f;        R.m[3][1] = 0.0f;        R.m[3][2] = 0.0f; R.m[3][3] = 1.0f;


	
	Matrix4f mat2 = { cosf(rotation),0.0f,-sinf(rotation),0.0f,
	0.0f,1.0f,0.0f,0.0f,
	sinf(rotation),0.0f,cosf(rotation),0.0f,
	0.0f,0.0f,0.0f,1.0f };
	

	
	int curr = 1;
	World =World *T *mat2* R;

	glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &World.m[0][0]);
	glUniform1i(now, 1);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, totalvert);

	glBindVertexArray(0);

	glUniform1i(now, 2);
	glBindVertexArray(cubeVAO);
	glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &World.m[0][0]);
	glDrawElements(
		GL_LINES,      // mode
		24,    // count
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
	);

	glBindVertexArray(0);



	/* check for any errors when rendering */
	GLenum errorCode = glGetError();
	if (errorCode == GL_NO_ERROR) {
		/* double-buffering - swap the back and front buffers */
		glutSwapBuffers();
	}
	else {
		fprintf(stderr, "OpenGL rendering error %d\n", errorCode);
	}
}

/* pre:  glut window has been resized
 */
static void onReshape(int width, int height) {
	glViewport(0, 0, width, height);
	if (!isFullScreen) {
		theWindowWidth = width;
		theWindowHeight = height;
	}
	// update scene based on new aspect ratio....
}

/* pre:  glut window is not doing anything else
   post: scene is updated and re-rendered if necessary */
static void onIdle() {
	static int oldTime = 0;
	if (isAnimating) {
		int currentTime = glutGet((GLenum)(GLUT_ELAPSED_TIME));
		/* Ensures fairly constant framerate */
		if (currentTime - oldTime > ANIMATION_DELAY) {
			// do animation....
			rotation += 0.01;
			translate -= 0.001;

			oldTime = currentTime;
			/* compute the frame rate */
			computeFPS();
			/* notify window it has to be repainted */
			glutPostRedisplay();
		}
	}
}

/* pre:  mouse is dragged (i.e., moved while button is pressed) within glut window
   post: scene is updated and re-rendered  */
static void onMouseMotion(int x, int y) {
	/* notify window that it has to be re-rendered */
	glutPostRedisplay();
}

/* pre:  mouse button has been pressed while within glut window
   post: scene is updated and re-rendered */
static void onMouseButtonPress(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		// Left button pressed
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		// Left button un pressed
	}
	/* notify window that it has to be re-rendered */
	glutPostRedisplay();
}

/* pre:  key has been pressed
   post: scene is updated and re-rendered */
static void onAlphaNumericKeyPress(unsigned char key, int x, int y) {
	switch (key) {
		/* toggle animation running */
	case 'a':
		isAnimating = !isAnimating;
		break;
		/* reset */
	case '1':
		translate -= 0.01f;
		rotation += 0.1f;

		break;
		/* quit! */
	case 'Q':
	case 'q':
	case 27:
		exit(0);
	}

	/* notify window that it has to be re-rendered */
	glutPostRedisplay();
}

/* pre:  arrow or function key has been pressed
   post: scene is updated and re-rendered */
static void onSpecialKeyPress(int key, int x, int y) {
	/* please do not change function of these keys */
	switch (key) {
		/* toggle full screen mode */
	case GLUT_KEY_F1:
		isFullScreen = !isFullScreen;
		if (isFullScreen) {
			theWindowPositionX = glutGet((GLenum)(GLUT_WINDOW_X));
			theWindowPositionY = glutGet((GLenum)(GLUT_WINDOW_Y));
			glutFullScreen();
		}
		else {
			glutReshapeWindow(theWindowWidth, theWindowHeight);
			glutPositionWindow(theWindowPositionX, theWindowPositionY);
		}
		break;
	}

	/* notify window that it has to be re-rendered */
	glutPostRedisplay();
}

/* pre:  glut window has just been iconified or restored
   post: if window is visible, animate model, otherwise don't bother */
static void onVisible(int state) {
	if (state == GLUT_VISIBLE) {
		/* tell glut to show model again */
		glutIdleFunc(onIdle);
	}
	else {
		glutIdleFunc(NULL);
	}
}

static void InitializeGlutCallbacks() {
	/* tell glut how to display model */
	glutDisplayFunc(onDisplay);
	/* tell glutwhat to do when it would otherwise be idle */
	glutIdleFunc(onIdle);
	/* tell glut how to respond to changes in window size */
	glutReshapeFunc(onReshape);
	/* tell glut how to handle changes in window visibility */
	glutVisibilityFunc(onVisible);
	/* tell glut how to handle key presses */
	glutKeyboardFunc(onAlphaNumericKeyPress);
	glutSpecialFunc(onSpecialKeyPress);
	/* tell glut how to handle the mouse */
	glutMotionFunc(onMouseMotion);
	glutMouseFunc(onMouseButtonPress);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);

	/* request initial window size and position on the screen */
	glutInitWindowSize(theWindowWidth, theWindowHeight);
	glutInitWindowPosition(theWindowPositionX, theWindowPositionY);
	/* request full color with double buffering and depth-based rendering */
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
	/* create window whose title is the name of the executable */
	glutCreateWindow(theProgramTitle);

	InitializeGlutCallbacks();

	// Must be done after glut is initialized!
	GLenum res = glewInit();
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	printf("GL version: %s\n", glGetString(GL_VERSION));

	/* initialize model */
	onInit(argc, argv);

	/* give control over to glut to handle rendering and interaction  */
	glutMainLoop();

	/* program should never get here */

	return 0;
}