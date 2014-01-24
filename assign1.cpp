// assign1.cpp : Defines the entry point for the console application.
//

/*
CSCI 480 Computer Graphics
Assignment 1: Height Fields
Ju Young (Amy) Lee
*/

#include "stdafx.h"
#include <pic.h>
#include <windows.h>
#include <stdlib.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include <sstream>
#include <iostream>

using namespace std;

int g_iMenuId;

int g_vMousePos[2] = {0, 0};
int g_iLeftMouseButton = 0;    /* 1 if pressed, 0 if not */
int g_iMiddleMouseButton = 0;
int g_iRightMouseButton = 0;

//functions//
void renderTriangles();
void renderMesh();
void renderPoints();
void renderWireFrame();
///////////////////////


//global variables//

int imageNum =0; //counter for screenshot
bool SAVE = false; //determine when to save, set to be false when imageNum reaches 300

float width, height, pixSize, scalingZ; // for scaling the coordinates

typedef enum { ROTATE, TRANSLATE, SCALE } CONTROLSTATE;
typedef enum {DOTS, WIREFRAME, TRIANGLES, MESH} RENDERSTATE;

CONTROLSTATE g_ControlState = ROTATE;  
RENDERSTATE g_RenderState = DOTS;


/* state of the world */
float g_vLandRotate[3] = {0.0, 0.0, 0.0};
float g_vLandTranslate[3] = {0.5, 0.5, 0.0};
float g_vLandScale[3] = {1.0, 1.0, 1.0};

/* see <your pic directory>/pic.h for type Pic */
Pic * g_pHeightData;

/* Write a screenshot to the specified filename */
void saveScreenshot (char *filename)
{
	int i, j;
	Pic *in = NULL;

	if (filename == NULL)
		return;

	/* Allocate a picture buffer */
	in = pic_alloc(640, 480, 3, NULL);

	printf("File to save to: %s\n", filename);

	for (i=479; i>=0; i--) {
		glReadPixels(0, 479-i, 640, 1, GL_RGB, GL_UNSIGNED_BYTE,
			&in->pix[i*in->nx*in->bpp]);
	}

	if (jpeg_write(filename, in))
		printf("File saved Successfully\n");
	else
		printf("Error in Saving\n");

	pic_free(in);
}

void myinit()
{
	/* setup gl view here */
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glColor3f(1,1,1);


}
void rainbowScale(float z)
{
	//color scheme to get rainbow by normalizing the z values

	if(z< 0.2f) //if z value is less than a fifth then it's red  to orange
		glColor3f(1.0f , z / 0.2f , 0.0f); 
	else if(z< 0.4f) //z value between 1/5 and 2/5 then orange to yellow
	{
		z  = z - 0.2f;
		glColor3f(1.0f - (z / 0.2f), 1.0f, 0.0f);
	}
	else if(z<0.6f) //yellow to green
	{
		z = z - 0.4f;
		glColor3f(0.0f, 1.0f, z / 0.2f);
	}
	else if(z< 0.8f ) //green to blue
	{
		z = z - 0.6f;
		glColor3f(0.0f, 1.0f - (z / 0.2f), 1.0f);
	}
	else						//blue to purple
	{
		z = z - 0.8f;
		glColor3f(z/0.2f, 0.0f, 1.0f);
	}
}


void renderPoints() //function to render points
{
	float zBuffer;

	glBegin(GL_POINTS);

	for (int col = 0; col < g_pHeightData->nx; col++) 
	{
		for (int row = 0; row < g_pHeightData->ny; row++) 
		{
			zBuffer = ((float) PIC_PIXEL(g_pHeightData, col, row, 0)) / pixSize;
			rainbowScale(zBuffer); //sets the apporpriate color3f as a rainbow

			glVertex3f((((float)col) / width) - 0.5f, (((float)row) / height) - 0.5f, zBuffer/scalingZ); // so that coordinate lies within -.5 to .5
			//This is so that the rotation, scaling, and translate can be done about the object's axis
			//zBuffer is divided by scalingZ because it's too large
		}
	}

	glEnd();

}

void renderWireFrame() //function to render lines
{
	for (int row = 0; row<g_pHeightData->nx;row++) 
	{
		glBegin(GL_LINE_STRIP); //strip of line along the row
		for (int col=0; col<g_pHeightData->ny;col++)
		{
			float zBuffer = ((float) PIC_PIXEL(g_pHeightData, col, row, 0)) / pixSize;
			rainbowScale(zBuffer);

			glVertex3f((((float)col) / width) - .5f,(((float)row) / height) - .5f, zBuffer/scalingZ);// so that coordinate lies within -.5 to .5
		}

		glEnd();
	}

	for (int col = 0; col<g_pHeightData->nx;col++)
	{
		glBegin(GL_LINE_STRIP); //a strip of line along the column
		for (int row=0; row<g_pHeightData->ny;row++)
		{
			float zBuffer = ((float) PIC_PIXEL(g_pHeightData, col, row, 0)) / pixSize;
			rainbowScale(zBuffer);

			glVertex3f((((float)col)/width) - .5f,(((float)row)/height) - .5f, zBuffer/scalingZ);// so that coordinate lies within -.5 to .5
		}
		glEnd();
	}

}

void renderTriangles()
{
	for (int col = 0; col<g_pHeightData->nx-1; col++) // -1 because there are n-1 rows of triangles
	{
		glBegin(GL_TRIANGLE_STRIP); //to create a strip of triangle for eac row
		for (int row = 0; row<g_pHeightData->ny-1; row++)
		{
			float zBuffer1 = ((float) PIC_PIXEL(g_pHeightData, col, row+1, 0)) / pixSize;
			float zBuffer2 = ((float) PIC_PIXEL(g_pHeightData, col, row, 0)) / pixSize;
			float zBuffer3 = ((float) PIC_PIXEL(g_pHeightData, col+1, row+1, 0)) / pixSize;
			float zBuffer4 = ((float) PIC_PIXEL(g_pHeightData, col+1, row, 0)) / pixSize;
			//4th vertice represent the next vertice to make the two triangles and the process is repeated n-1 times for both row and col

			//setting colors for each vertices
			// subtracts the x and y by .5 so that coordinate lies within (-.5 , .5) for x and y axis.

			rainbowScale(zBuffer1);
			glVertex3f( (((float)col) / width) - .5f, (((float)(row+1)) / height) - .5f, zBuffer1 / scalingZ);

			rainbowScale(zBuffer2);
			glVertex3f( (((float)col) / width) - .5f, (((float)(row)) / height) - .5f, zBuffer2 / scalingZ);

			rainbowScale(zBuffer3);
			glVertex3f(  (((float)(col+1)) / width ) - .5f, (((float)(row+1)) / height) - .5f, zBuffer3 / scalingZ);

			rainbowScale(zBuffer4);
			glVertex3f( (((float)(col+1)) / width) - .5f, (((float)(row)) / height) - .5f, zBuffer4 / scalingZ);

		}
		glEnd();
	}

}

void renderMesh()
{

	glEnable(GL_POLYGON_OFFSET_FILL); //for offset
	glEnable (GL_BLEND); //use transparency for enhancement of the visual effect
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPolygonOffset(1,1); //offset
	renderTriangles(); //first render triangles
	
	glDisable(GL_POLYGON_OFFSET_FILL);

	//render wireframe
	for (int row = 0; row<g_pHeightData->nx;row++) 
	{
		glBegin(GL_LINE_STRIP); //strip of line along the row
		for (int col=0; col<g_pHeightData->ny;col++)
		{
		
			glColor4f(0,0,0,0.4); //black wireframe with alpha at 0.4 intensity for transparency 
			float zBuffer = ((float) PIC_PIXEL(g_pHeightData, col, row, 0)) / pixSize;
			glVertex3f( (((float)col) / width) - .5f, (((float)row) / height) - .5f, zBuffer/scalingZ);
		}
		glEnd();//end line strips

	}

	for (int col = 0; col<g_pHeightData->nx;col++)
	{
		glBegin(GL_LINE_STRIP); //a strip of line along the column
		for (int row=0; row<g_pHeightData->ny;row++)
		{
			
			glColor4f(0,0,0,0.4); //black wireframe with alpha at 0.4 intensity for transparency 
			float zBuffer = ((float) PIC_PIXEL(g_pHeightData, col, row, 0)) / pixSize;
			glVertex3f( (((float)col) / width) - .5f, (((float)row) / height) - .5f, zBuffer/scalingZ);
		}
		glEnd();
	}

		glDisable(GL_BLEND);

	}



void reshape(int x, int y)
{
	glViewport(0, 0, x, y);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, 1.0 * x / y, 0.01, 10.0);
}


void display()
{ 
	gluLookAt(1.0, -2.0, 2.0, 1.0, 2.0, 0.0, 0.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();



	glPushMatrix();

	width = (float)g_pHeightData->nx;
	height = (float)g_pHeightData->ny;
	pixSize = width; //number of pixels
	scalingZ = 2.0; //scaling Z because it was too big

	glTranslatef(g_vLandTranslate[0], g_vLandTranslate[1], g_vLandTranslate[2]);
	glRotatef(g_vLandRotate[0],1,0,0); //x axis
	glRotatef(g_vLandRotate[1],0,1,0); //y axis
	glRotatef(g_vLandRotate[2],0,0,1); //z axis
	glScalef(g_vLandScale[0],g_vLandScale[1],g_vLandScale[2]);

	switch(g_RenderState)
	{
	case DOTS: //drawing points (POINTS was a reserve word and couldn't be used)
		renderPoints();
		break;
	case WIREFRAME: //drawing lines
		renderWireFrame();
		break;
	case TRIANGLES: //drawing triangles
		renderTriangles();
		break;
	case MESH:	//drawing mesh
		renderMesh();
		break;
	}



	glFlush();
	glPopMatrix();

	glutSwapBuffers();
}



void menufunc(int value) 
{
	switch (value)
	{
	case 0:
		exit(0);
		break;
	}

}

//some of the key press driven commands such as changing type of rendering
void keyboardFunc(unsigned char button, int x, int y)
{
	float xaxis = g_vLandTranslate[0];
	float yaxis = g_vLandTranslate[1];
	float zaxis = g_vLandTranslate[2];
	switch(button)
	{
	case '1': 
		g_RenderState = DOTS;
		break;
	case '2': 
		g_RenderState = WIREFRAME;
		break;
	case '3':
		g_RenderState = TRIANGLES;
		break;
	case '4':
		g_RenderState = MESH;
		break;
	case 'p': //plus = zoom in 
	case 'P':
		g_vLandScale[0] *= 1.1;
		g_vLandScale[1] *=1.1;
		g_vLandScale[2] *=1.1;
		break;
	case 'm': //minus = zoom out
	case 'M':
		g_vLandScale[0] *= 0.9;
		g_vLandScale[1] *= 0.9;
		g_vLandScale[2] *= 0.9;
		break;
	case 'w':
	case 'W':
		g_vLandRotate[0] -=3; //rotate about x axis "upward"

		break;
	case 's':
	case 'S':
		g_vLandRotate[0] +=3; //rotate bout x axis "downward"
		break;
	case 'd':
	case 'D':
		g_vLandRotate[2] -=3; //rotate about z axis "right"
		break;
	case 'a':
	case 'A':
		g_vLandRotate[2] += 3; //rotate about -z axis "left"
		break;
	case 'i':
	case 'I':
		SAVE =true;			//determine when to start taking screnshots
		break;
	case 'o':
	case 'O':
		SAVE = false;
		break;
	}
}


void doIdle()
{
	/* screenshot saving here */
	stringstream ss;
	string imageFile;
	char *cstr;

	if (SAVE) //automatically saves 300 images
	{
		if (imageNum<10)
		{
			ss<<imageNum;
			imageFile = "00" +  ss.str() + ".jpg";
			cstr = (char*)imageFile.c_str();
			saveScreenshot(cstr);
		}
		else if(imageNum<100)
		{
			ss<<imageNum;
			imageFile = "0" + ss.str() + ".jpg";
			cstr = (char*)imageFile.c_str();
			saveScreenshot(cstr);
		}
		else if(imageNum<=300)
		{
			ss<<imageNum;
			imageFile =  ss.str() + ".jpg";
			cstr = (char*)imageFile.c_str();
			saveScreenshot(cstr);

		}
		else
		{
			SAVE = FALSE;
		}

		imageNum++;

	}



	/* make the screen update */
	glutPostRedisplay();
}

/* converts mouse drags into information about 
rotation/translation/scaling */
void mousedrag(int x, int y)
{
	int vMouseDelta[2] = {x-g_vMousePos[0], y-g_vMousePos[1]};

	switch (g_ControlState)
	{
	case TRANSLATE:  
		if (g_iLeftMouseButton)
		{
			g_vLandTranslate[0] += vMouseDelta[0]*0.01;
			g_vLandTranslate[1] -= vMouseDelta[1]*0.01;
		}
		if (g_iMiddleMouseButton)
		{
			g_vLandTranslate[2] += vMouseDelta[1]*0.01;
		}
		break;
	case ROTATE:
		if (g_iLeftMouseButton)
		{
			g_vLandRotate[0] += vMouseDelta[1];
			g_vLandRotate[1] += vMouseDelta[0];
		}
		else if (g_iMiddleMouseButton)
		{
			g_vLandRotate[2] += vMouseDelta[1];
		}
		else
		{
			g_vLandRotate[0] += vMouseDelta[1];
			g_vLandRotate[1] += vMouseDelta[0];
		}
		break;
	case SCALE:
		if (g_iLeftMouseButton)
		{
			g_vLandScale[0] *= 1.0+vMouseDelta[0]*0.01;
			g_vLandScale[1] *= 1.0+vMouseDelta[1]*0.01;
		}
		if (g_iMiddleMouseButton)
		{
			g_vLandScale[2] *= 1.0-vMouseDelta[1]*0.01;
		}
		break;
	}
	g_vMousePos[0] = x;
	g_vMousePos[1] = y;
}

void mouseidle(int x, int y)
{
	g_vMousePos[0] = x;
	g_vMousePos[1] = y;
}

void mousebutton(int button, int state, int x, int y)
{

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		g_iLeftMouseButton = (state==GLUT_DOWN);
		break;
	case GLUT_MIDDLE_BUTTON:
		g_iMiddleMouseButton = (state==GLUT_DOWN);
		break;
	case GLUT_RIGHT_BUTTON:
		g_iRightMouseButton = (state==GLUT_DOWN);
		break;
	}

	switch(glutGetModifiers())
	{
	case GLUT_ACTIVE_CTRL:
		g_ControlState = TRANSLATE;
		break;
	case GLUT_ACTIVE_SHIFT:
		g_ControlState = SCALE;
		break;
	default:
		g_ControlState = ROTATE;
		break;
	}

	g_vMousePos[0] = x;
	g_vMousePos[1] = y;
}



int main(int argc, char** argv)
{
	// I've set the argv[1] to spiral.jpg.
	// To change it, on the "Solution Explorer",
	// right click "assign1", choose "Properties",
	// go to "Configuration Properties", click "Debugging",
	// then type your texture name for the "Command Arguments"
	if (argc<2)
	{  
		printf ("usage: %s heightfield.jpg\n", argv[0]);
		exit(1);
	}

	g_pHeightData = jpeg_read(argv[1], NULL);

	if (!g_pHeightData)
	{
		printf ("error reading %s.\n", argv[1]);
		exit(1);
	}

	pixSize =g_pHeightData->nx;

	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (640, 480);
	glutCreateWindow (argv[0]);


	/* tells glut to use a particular display function to redraw */
	glutDisplayFunc(display);

	/* allow the user to quit using the right mouse button menu */
	g_iMenuId = glutCreateMenu(menufunc);
	glutSetMenu(g_iMenuId);
	glutAddMenuEntry("Quit",0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	/* replace with any animate code */
	glutIdleFunc(doIdle);

	/* reshape function */
	glutReshapeFunc(reshape);
	/* callback for mouse drags */
	glutMotionFunc(mousedrag);
	/* callback for idle mouse movement */
	glutPassiveMotionFunc(mouseidle);
	/* callback for mouse button changes */
	glutMouseFunc(mousebutton);
	/* callback for keyboard button changes */
	glutKeyboardFunc(keyboardFunc);


	/* do initialization */
	myinit();
	glEnable(GL_DEPTH_TEST);

	glutMainLoop();

	return 0;
}