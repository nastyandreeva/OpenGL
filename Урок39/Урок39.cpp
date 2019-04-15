#include <windows.h>											
#include <stdio.h>												
#include <gl.h>												
#include <glu.h>												
#include "NeHeGL.h"												
#include "Physics1.h"											

#pragma comment( lib, "opengl32.lib" )							
#pragma comment( lib, "glu32.lib" )								

#ifndef CDS_FULLSCREEN											
#define CDS_FULLSCREEN 4										
#endif															

GL_Window*	g_window;
Keys*		g_keys;

 

ConstantVelocity* constantVelocity = new ConstantVelocity();

 

MotionUnderGravitation* motionUnderGravitation = 
	new MotionUnderGravitation(Vector3D(0.0f, -9.81f, 0.0f));

 

MassConnectedWithSpring* massConnectedWithSpring = 
	new MassConnectedWithSpring(2.0f);

float slowMotionRatio = 10.0f;									
float timeElapsed = 0;											

GLuint	base;													

GLYPHMETRICSFLOAT gmf[256];										

GLvoid BuildFont(GL_Window* window)								
{
	HFONT	font;												

	base = glGenLists(256);										

	font = CreateFont(	-12,									
						0,										
						0,										
						0,										
						FW_BOLD,								
						FALSE,									
						FALSE,									
						FALSE,									
						ANSI_CHARSET,							
						OUT_TT_PRECIS,							
						CLIP_DEFAULT_PRECIS,					
						ANTIALIASED_QUALITY,					
						FF_DONTCARE|DEFAULT_PITCH,				
						NULL);									
	
	HDC hDC = window->hDC;
	SelectObject(hDC, font);									

	wglUseFontOutlines(	hDC,									
						0,										
						255,									
						base,									
						0.0f,									
						0.0f,									
						WGL_FONT_POLYGONS,						
						gmf);									
}

GLvoid KillFont(GLvoid)											
{
	glDeleteLists(base, 256);									
}

GLvoid glPrint(float x, float y, float z, const char *fmt, ...)	
{
	float		length=0;										
	char		text[256];										
	va_list		ap;												

	if (fmt == NULL)											
		return;													

	va_start(ap, fmt);											
	    vsprintf(text, fmt, ap);								
	va_end(ap);													

	for (unsigned int loop=0;loop<(strlen(text));loop++)		
	{
		length+=gmf[text[loop]].gmfCellIncX;					
	}

	glTranslatef(x - length, y, z);								

	glPushAttrib(GL_LIST_BIT);									
	glListBase(base);											
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);			
	glPopAttrib();												

	glTranslatef(-x, -y, -z);									
}

BOOL Initialize (GL_Window* window, Keys* keys)					
{
	g_window	= window;
	g_keys		= keys;

	glClearColor (0.0f, 0.0f, 0.0f, 0.5f);						
	glShadeModel (GL_SMOOTH);									
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			

	BuildFont(window);											

	return TRUE;												
}

void Deinitialize (void)										
{
	KillFont();
	
	constantVelocity->release();
	delete(constantVelocity);
	constantVelocity = NULL;

	motionUnderGravitation->release();
	delete(motionUnderGravitation);
	motionUnderGravitation = NULL;

	massConnectedWithSpring->release();
	delete(massConnectedWithSpring);
	massConnectedWithSpring = NULL;
}

void Update (DWORD milliseconds)								
{
	if (g_keys->keyDown [VK_ESCAPE] == TRUE)					
		TerminateApplication (g_window);						

	if (g_keys->keyDown [VK_F1] == TRUE)						
		ToggleFullscreen (g_window);							

	if (g_keys->keyDown [VK_F2] == TRUE)						
		slowMotionRatio = 1.0f;									

	if (g_keys->keyDown [VK_F3] == TRUE)						
		slowMotionRatio = 10.0f;								

	
	

	float dt = milliseconds / 1000.0f;							

	dt /= slowMotionRatio;										

	timeElapsed += dt;											

	float maxPossible_dt = 0.1f;								
																

  	int numOfIterations = (int)(dt / maxPossible_dt) + 1;		
	if (numOfIterations != 0)									
		dt = dt / numOfIterations;								

	for (int a = 0; a < numOfIterations; ++a)					
	{
		constantVelocity->operate(dt);							
		motionUnderGravitation->operate(dt);					
		massConnectedWithSpring->operate(dt);					
	}

}

void Draw (void)
{	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity ();											
	
	
	
	gluLookAt(0, 0, 40, 0, 0, 0, 0, 1, 0);						

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		


	
	
	glColor3ub(0, 0, 255);										
	glBegin(GL_LINES);
	
	
	for (float x = -20; x <= 20; x += 1.0f)						
	{
		glVertex3f(x, 20, 0);
		glVertex3f(x,-20, 0);
	}

	
	for (float y = -20; y <= 20; y += 1.0f)						
	{
		glVertex3f( 20, y, 0);
		glVertex3f(-20, y, 0);
	}

	glEnd();
	

	
	glColor3ub(255, 0, 0);										
	int a;
	for (a = 0; a < constantVelocity->numOfMasses; ++a)
	{
		Mass* mass = constantVelocity->getMass(a);
		Vector3D* pos = &mass->pos;

		glPrint(pos->x, pos->y + 1, pos->z, "Mass with constant vel");

		glPointSize(4);
		glBegin(GL_POINTS);
			glVertex3f(pos->x, pos->y, pos->z);
		glEnd();
	}
	

	
	glColor3ub(255, 255, 0);									
	for (a = 0; a < motionUnderGravitation->numOfMasses; ++a)
	{
		Mass* mass = motionUnderGravitation->getMass(a);
		Vector3D* pos = &mass->pos;

		glPrint(pos->x, pos->y + 1, pos->z, "Motion under gravitation");

		glPointSize(4);
		glBegin(GL_POINTS);
			glVertex3f(pos->x, pos->y, pos->z);
		glEnd();
	}
	

	
	glColor3ub(0, 255, 0);										
	for (a = 0; a < massConnectedWithSpring->numOfMasses; ++a)
	{
		Mass* mass = massConnectedWithSpring->getMass(a);
		Vector3D* pos = &mass->pos;

		glPrint(pos->x, pos->y + 1, pos->z, "Mass connected with spring");

		glPointSize(8);
		glBegin(GL_POINTS);
			glVertex3f(pos->x, pos->y, pos->z);
		glEnd();

		
		glBegin(GL_LINES);
			glVertex3f(pos->x, pos->y, pos->z);
			pos = &massConnectedWithSpring->connectionPos;
			glVertex3f(pos->x, pos->y, pos->z);
		glEnd();
	}
	


	glColor3ub(255, 255, 255);									
	glPrint(-5.0f, 14, 0, "Time elapsed (seconds): %.2f", timeElapsed);	
	glPrint(-5.0f, 13, 0, "Slow motion ratio: %.2f", slowMotionRatio);	
	glPrint(-5.0f, 12, 0, "Press F2 for normal motion");
	glPrint(-5.0f, 11, 0, "Press F3 for slow motion");

	glFlush ();													
}
