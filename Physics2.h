

#include "Physics1.h"									//Physics1.h jest wymagane dla Physics2.h aby wykonac symulacje, naglowek zawiera klase wektor do ktorej odwolujemy sie tutaj

class Spring											//objekt reprezentujacy sprezyne z wewnetrznym tarciem wiazacym dwie masy. 
{														//sprezyna posiada prostopadla dlugosc czyli taka na ktora sprezyna nie wywiera zadnej sily
public:
	Mass* mass1;										//pierwsza masa na jednym koncu sprezyny
	Mass* mass2;										//druga masa na drugin koncu sprezyny

	float springConstant;								//stala odpowiadajaca za sztywnosc sprezyny
	float springLength;									//dlugosc na ktora sprezyna nie wywiera zadnych sil
	float frictionConstant;								//stala odpowiadajaca za tarcie wewnetrzne sprezyny

	Spring(Mass* mass1, Mass* mass2, 
		float springConstant, float springLength, float frictionConstant)		//konstruktor
	{
		this->springConstant = springConstant;									//ustawienie sztywnosci 
		this->springLength = springLength;										//ustawienie dlugosci
		this->frictionConstant = frictionConstant;								//ustawienie wspolczynnika tarcia wew

		this->mass1 = mass1;													//ustawienie masy na jednym koncu
		this->mass2 = mass2;													//ustawienie masy na drugim koncu
	}

	void solve()																 // metoda aplikujaca sily zew
	{
		Vector3D springVector = mass1->pos - mass2->pos;							//wektor pomiedzy masami
		
		float r = springVector.length();											//dligosc pomiedzy masami

		Vector3D force;																//wartosc poczatkowa sily rowna sie zero

		if (r != 0)																	//sprawdzenie czy promien nie jest rowny zero
			force += (springVector / r) * (r - springLength) * (-springConstant);	//sila sprezystosci jest dodawana do sily calkowitej 

		force += -(mass1->vel - mass2->vel) * frictionConstant;						//sila tarcia rozwniez jest dodawana
																					// uzyskujemy przy tym sile naprezen sprezyny 

		mass1->applyForce(force);													//fsila ta przykladana jest do masy pierwszej zas do masy drugiej sila rozwna co do wartosci lecz przeciwna
		mass2->applyForce(-force);													
	}

};

/*
  klasa RopeSimulation pochodzi z klasy Simulation (physics1.h). symuluje zachowanie liny zbudowanej z punktow polaczonych sprezynkami
  sprezynki maja swoje tarcie wew i normalna dlugosc. jeden koniec liny jest stabilny i zawieszony w przestrzeni "Vector3D ropeConnectionPos"
  punkt moze byc poryszany po calej przestrzeni przez wykorzystanie metody "void setRopeConnectionVel(Vector3D ropeConnectionVel)".
  RopeSimulation tworzy opor powietrza i podloze prostopadle do liny. RopeSimulation implementuje sile wykonana przez podloze. w kodzie podloze jest nazwane jako "ground"
*/
class RopeSimulation : public Simulation				//klasa stworzona do zasymulowania oddzialywania miedzy lina a podlozem i powierzem
{
public:
	Spring** springs;									//sprezynki wiazace masy, powinno byc ich o jeden mniej od calkowitej liczby mas
	Vector3D gravitation;								//sila grawitacji, dotychy wszystkich mas

	Vector3D ropeConnectionPos;							//punkt w przestrzeni aby ustawic pozycje pierwszej masy o indeksie 0 
														

	Vector3D ropeConnectionVel;							//zmienna odpowiadajaca za ruch liny

	float groundRepulsionConstant;						//stala mowiaca ile o podloze bedzie hamowac mase
	
	float groundFrictionConstant;						//tarcie miedzy masa a podlozem
														
	
	float groundAbsorptionConstant;						//zaabsorbowane tarcie przez masy
	
	float groundHeight;									//pozycja na podlozu
														

	float airFrictionConstant;							//stala oporu powietrza

	RopeSimulation(									
		int numOfMasses,								
		float m,										
		float springConstant,							
		float springLength,								
		float springFrictionConstant,					
		Vector3D gravitation,							
		float airFrictionConstant,						
		float groundRepulsionConstant,					
		float groundFrictionConstant,					
		float groundAbsorptionConstant,					
		float groundHeight							
		) : Simulation(numOfMasses, m)					
	{
		this->gravitation = gravitation;
		
		this->airFrictionConstant = airFrictionConstant;

		this->groundFrictionConstant = groundFrictionConstant;
		this->groundRepulsionConstant = groundRepulsionConstant;
		this->groundAbsorptionConstant = groundAbsorptionConstant;
		this->groundHeight = groundHeight;

		for (int index = 0; index < numOfMasses; ++index)			//pozycja poczatwkowa mas
		{
			masses[index]->pos.x = index * springLength;		
			masses[index]->pos.y = 0;						
			masses[index]->pos.z = 0;						
		}

		springs = new Spring*[numOfMasses - 1];			
														

		for (int index = 0; index < numOfMasses - 1; ++index)			
		{
			
			//tworzenie sprezynek i mas o indeksach "a" i kolejnej masy o indeksie a+1
			springs[index] = new Spring(masses[index], masses[index + 1],
				springConstant, springLength, springFrictionConstant);
		}
	}

	void release()										//release() is overriden because we have springs to delete
	{
		Simulation::release();							//Have the super class release itself

		for (int index = 0; index < numOfMasses - 1; ++index)		//to delete all springs, start a loop
		{
			delete(springs[index]);
			springs[index] = NULL;
		}

		delete(springs);
		springs = NULL;
	}

	void solve()										 //przeciążenie w celu zaaplikowania sił
	{
		for (int index = 0; index < numOfMasses - 1; ++index)		
		{
			springs[index]->solve();						//aplikowanie sił indeksowanych
		}

		for (int index = 0; index < numOfMasses; ++index)				//pętla aplikująca wspólne siły do mas
		{
			masses[index]->applyForce(gravitation * masses[index]->m);				

			masses[index]->applyForce(-masses[index]->vel * airFrictionConstant);	

			if (masses[index]->pos.y < groundHeight)		//opór podłoża jest tez dodawany jezeli zachodzi z nim kontakt
			{
				Vector3D v;								

				v = masses[index]->vel;						
				v.y = 0;								//pomijanie predkosci  w kierunku y

				
				masses[index]->applyForce(-v * groundFrictionConstant);		

				v = masses[index]->vel;						
				v.x = 0;								
				v.z = 0;								

				

				if (v.y < 0)							//apsorbcja energii masy w kontakcie z podlozem
					masses[index]->applyForce(-v * groundAbsorptionConstant);		//aplikowanie

				
				Vector3D force = Vector3D(0, groundRepulsionConstant, 0) *
					(groundHeight - masses[index]->pos.y);

				masses[index]->applyForce(force);			
			}

		}


	}

	void simulate(float dt)								
														
	{													//symulacja ruchu
		Simulation::simulate(dt);						

		ropeConnectionPos += ropeConnectionVel * dt;	//iteracje pozycji

		if (ropeConnectionPos.y < groundHeight)			
		{
			ropeConnectionPos.y = groundHeight;
			ropeConnectionVel.y = 0;
		}

		masses[0]->pos = ropeConnectionPos;				
		masses[0]->vel = ropeConnectionVel;				
	}

	void setRopeConnectionVel(Vector3D ropeConnectionVel)	
	{
		this->ropeConnectionVel = ropeConnectionVel;
	}

};

