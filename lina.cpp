/*symulacja liny w oknie OpenGl*/
#include "main.h"												

extern S_AppStatus AppStatus;									



RopeSimulation* ropeSimulation = new RopeSimulation(
													80,						//80 czasteczek
													0.05f,					// kazda o wadze 50 gra
													1000.0f,				// stala sprezystosci
													0.05f,					// dlugosc sprezynki
													0.2f,					// tarcie wew sprezynki
													Vector3D(0, -9.81f, 0), // grawitacja
													0.02f,					// oporpowietrza
													100.0f,					// sila odpychania podloza
													0.2f,					// tarcie z podlozem
													2.0f,					// absorpcja
													-1.5f);					// polozenie



bool InitGL(SDL_Surface *S)										// inicjacja open gl
{
	ropeSimulation->getMass(ropeSimulation->numOfMasses - 1)->vel.z = 10.0f;

	glClearColor (1.0f, 0.5f, 0.0f, 0.5f);								// zolte tlo
	glClearDepth (1.0f);													// glebokosc
	glDepthFunc (GL_LEQUAL);											
	glEnable (GL_DEPTH_TEST);											
	glShadeModel (GL_SMOOTH);											
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);					

	return true;												
}

bool Initialize(void)											
{
	AppStatus.Visible	= true;									
	AppStatus.MouseFocus	= true;								
	AppStatus.KeyboardFocus = true;								

	return true;												
}

//destrukccja obiektu liny
void Deinitialize(void)											
{
	ropeSimulation->release();									
	delete(ropeSimulation);										
	ropeSimulation = NULL;

	return;														
}

void Update(Uint32 milliseconds, Uint8 *Keys)					
{
	if(Keys)													// obsluga przyciskow
	{
		if(Keys[SDLK_ESCAPE])									// esc
		{
			TerminateApplication();								// konic
		}

		if(Keys[SDLK_F1])										// F1
		{
			ToggleFullscreen();									// fullscreen
		}

		Vector3D ropeConnectionVel;						// tymczasowy wektor 3d

		if(Keys[SDLK_UP])								//obsluga 	strzalek	
		{
			ropeConnectionVel.z -= 3.0f;					
		}

		if(Keys[SDLK_DOWN])										
		{
			ropeConnectionVel.z += 3.0f;					
		}

		if(Keys[SDLK_LEFT])										
		{
			ropeConnectionVel.x -= 3.0f;						
		}

		if(Keys[SDLK_RIGHT])										
		{
			ropeConnectionVel.x += 3.0f;						
		}

		if(Keys[SDLK_HOME])										
		{
			ropeConnectionVel.y += 3.0f;						
		}

		if(Keys[SDLK_END])										
		{
			ropeConnectionVel.y -= 3.0f;						
		}
		ropeSimulation->setRopeConnectionVel(ropeConnectionVel);		

		float dt = milliseconds / 1000.0f;										

	float maxPossible_dt = 0.002f;											
																			// interwal

  	int numOfIterations = (int)(dt / maxPossible_dt) + 1;					
	if (numOfIterations != 0)												
		dt = dt / numOfIterations;											

	for (int a = 0; a < numOfIterations; ++a)								
		ropeSimulation->operate(dt);
	}

	return;														
}

void Draw(void)													// kod rysujacy
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity ();														// Reset 
	
	gluLookAt(0, 0, 4, 0, 0, 0, 0, 1, 0);

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);					//czyszczenie

	
	glBegin(GL_QUADS);
		glColor3ub(0, 0, 255);												// kolor jasno niebieski
		glVertex3f(20, ropeSimulation->groundHeight, 20);
		glVertex3f(-20, ropeSimulation->groundHeight, 20);
		glColor3ub(0, 0, 0);												// czarny
		glVertex3f(-20, ropeSimulation->groundHeight, -20);
		glVertex3f(20, ropeSimulation->groundHeight, -20);
	glEnd();

	// Start Drawing Shadow Of The Rope
	glColor3ub(0, 0, 0);													
	for (int index = 0; index < ropeSimulation->numOfMasses - 1; ++index)
	{
		Mass* mass1 = ropeSimulation->getMass(index);
		Vector3D* pos1 = &mass1->pos;

		Mass* mass2 = ropeSimulation->getMass(index + 1);
		Vector3D* pos2 = &mass2->pos;

		glLineWidth(2);
		glBegin(GL_LINES);
			glVertex3f(pos1->x, ropeSimulation->groundHeight, pos1->z);		// cien liny
			glVertex3f(pos2->x, ropeSimulation->groundHeight, pos2->z);		
		glEnd();
	}
	

	// rysowanie liny
	glColor3ub(0, 255, 0);												// zolty
	for (int index = 0; index < ropeSimulation->numOfMasses - 1; ++index)
	{
		Mass* mass1 = ropeSimulation->getMass(index);
		Vector3D* pos1 = &mass1->pos;

		Mass* mass2 = ropeSimulation->getMass(index + 1);
		Vector3D* pos2 = &mass2->pos;

		glLineWidth(4);
		glBegin(GL_LINES);
			glVertex3f(pos1->x, pos1->y, pos1->z);
			glVertex3f(pos2->x, pos2->y, pos2->z);
		glEnd();
	}
	

	glFlush ();													

	return;														
}
