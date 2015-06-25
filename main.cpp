


#include "main.h"															


bool isProgramLooping;														
S_AppStatus AppStatus;														



bool InitTimers(Uint32 *C)													// Wywolanie wszystkich timerow w prormaie
{
	*C = SDL_GetTicks();													// przechowywanie wartosci timera

	return true;															
}

void TerminateApplication(void)												// Wyjscie z aplikacji
{
	static SDL_Event Q;														// wyslanie zapytania wyjscia do kolejki SDL
	Q.type = SDL_QUIT;													

	if(SDL_PushEvent(&Q) == -1)												// proba wyslania zapytania
	{
		printf("SDL_QUIT zdarzenie jest wykonane: %s\n", SDL_GetError() );						// ewenttualny blad
		exit(1);															// wyjscie
	}

}

void ToggleFullscreen(void)													// Ustaw pelny ekran
{
	SDL_Surface *S;															// powierzchnia punktow ekranu

	S = SDL_GetVideoSurface();												// pobierz powierzchnie video

	if(!S || (SDL_WM_ToggleFullScreen(S)!=1))								// raport o bledzie
	{
		printf("Niemozliwe jest wejscie w tryb pelnoekranowy: %s\n", SDL_GetError() );			
	}
	
}

void ReshapeGL(int width, int height)										// przerysuj okno jesli jest ruszane badz przeskalowywane
{
	glViewport(0,0,(GLsizei)(width),(GLsizei)(height));						// Resetuj obecny obraz
	glMatrixMode(GL_PROJECTION);											// wybierz maciez projekcji
	glLoadIdentity();														// przerysuj macierz projekcji

	gluPerspective(45.0f,(GLfloat)(width)/(GLfloat)(height),1.0f,100.0f);	// Oblicz nowe wartosci okna
	glMatrixMode(GL_MODELVIEW);												// wybierz Modelview Matrix
	glLoadIdentity();														// przerysuj The Modelview Matrix

}

bool CreateWindowGL (SDL_Surface *S, int W, int H, int B, Uint32 F)			// Stworz okno OpenGL
{
	if(!(S = SDL_SetVideoMode(W, H, B, F)))									// użycie biblioteki SDL
	{
		return false;														// w przypadku bledu zwroc false
	}

	ReshapeGL(SCREEN_W, SCREEN_H);											// wywolanie funkcji reshape

	return true;															
}

int main(int argc, char **argv)												
{
	SDL_Surface *Screen;													// ekran
	SDL_Event	E;															
	Uint8		*Keys;														// tablica dla klawiatury
	Uint32		Vflags;														// flagi
	Uint32		TickCount;													// zliczenie tykniec
	Uint32		LastCount;													
	
	Screen = NULL;															// inicjalizacja zmiennych
	Keys = NULL;															
	Vflags = SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_OPENGLBLIT;					// Hardware Surface, Double Buffering Feature, Special OpenGLBlit
																			
	if(SDL_Init(SDL_INIT_VIDEO)<0)											// inicjalizacja biblioteki SDL - The VIDEO Subsystem
	{
		printf("SDL nie dziala: %s\n", SDL_GetError() );					// zabezpieczenie
		exit(1);															
	}

	atexit(SDL_Quit);														// zdarzenie wyjscia

#if defined FULLSCREEN_ASK													// zapytanie o pełny ekran

	if(MessageBox(HWND_DESKTOP, "chcesz otworzyc w trybie pelnoekranowym?",				
		"pelnoekran?", MB_YESNO|MB_ICONQUESTION) == IDYES)
	{
		Vflags|=SDL_FULLSCREEN;												
	}

#elif defined FULLSCREEN													

	Vflags|=SDL_FULLSCREEN;													

#endif																		
																			

	if(!CreateWindowGL(Screen, SCREEN_W, SCREEN_H, SCREEN_BPP, Vflags))		// flagi tworzenia okna GL
	{
		printf("Blad okna: %s\n", SDL_GetError() );		
		exit(1);															
	}

	SDL_WM_SetCaption(APP_NAME, NULL);										

	if(!InitTimers(&LastCount))												// wywowalnie timerow
	{
		printf("Blad timera: %s\n", SDL_GetError() );				
		exit(1);															
	}

	if(!InitGL(Screen))														// wywolanie GL
	{
		printf("Blad GL: %s\n", SDL_GetError() );						
		exit(1);															
	}

	if(!Initialize())														//wywoalanie aplikacji
	{
		printf("Blad aplikacji: %s\n", SDL_GetError() );						
		exit(1);															
	}

	isProgramLooping = true;												// petla programowa

	while(isProgramLooping)													
	{
		if(SDL_PollEvent(&E))												// pierwsze zdarzenie kolejki
		{
			switch(E.type)													
			{
				
			case SDL_QUIT:													// zdarzenie wyjscia
				{
					isProgramLooping = false;								
					break;													
				}

			case SDL_VIDEORESIZE:											// Zdarzenie skalowania
				{
					ReshapeGL(E.resize.w, E.resize.h);						// przeskaluj scene i okno
					break;													
				}

			case SDL_ACTIVEEVENT:											// zdarzenie aktywnosci
				{
					if(E.active.state & SDL_APPACTIVE)						
					{
						if(E.active.gain)									
						{
							AppStatus.Visible = true;						
						}
						else												
						{
							AppStatus.Visible = false;						
						}
					}
					
					if(E.active.state & SDL_APPMOUSEFOCUS)					// zdarzenie myszki w pelnym ekrania. 
					{
						if(E.active.gain)									
						{
							AppStatus.MouseFocus = true;						
						}
						else												
						{
							AppStatus.MouseFocus = false;					
						}
					}

					if(E.active.state & SDL_APPINPUTFOCUS)					// sprawdzenie obslugi klawiatury
					{
						if(E.active.gain)									
						{
							AppStatus.KeyboardFocus = true;					
						}
						else												
						{
							AppStatus.KeyboardFocus = false;				
						}
					}
					
					break;													
				}

			case SDL_KEYDOWN:												// zdarzenie guzika F1
				{
					Keys = SDL_GetKeyState(NULL);							// wcisniecie guzika i wyslanie zawiadomienia
					break;													
				}

			}
		}
		else																// brak zdarzen w kolejce
		{
			if(!AppStatus.Visible)											// jesli aplikacja nie jest widoczna
			{
				SDL_WaitEvent(NULL);										// wstrzymaj program
			}
			else															
			{
				TickCount = SDL_GetTicks();									// pobierz obecne tykniecie
				Update(TickCount-LastCount, Keys);							// uaktualnij ruch i dane
				LastCount = TickCount;										// zapisz obecne tykniecie
				Draw();														// rysuj
				SDL_GL_SwapBuffers();										// podmien bufory
			}
		}
	}

	Deinitialize();															// koniec dzialania programu
																			// usuniecie obiektow
	exit(0);																// zdarzenie wyjscia wysylanie do kolejnki

	return 0;																
}
