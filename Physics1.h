/* Pilk zawierajacy klase wektorow, mas oraz symulacji mas z predkoscia oraz pod wplywem grawitacji. Zawiera takze polaczenie ze sprezyna*/
#include <math.h>

// Klasa wektorów	---> Obiekt reprezentujacy wektor albo punkt w 3-wymiarowej przestrzeni
class Vector3D
{
public:
	float x;									// wartosci przestrzenne wektorow 
	float y;									  
	float z;									

	Vector3D()									// wartosci poczatkowe
	{
		x = 0;
		y = 0;
		z = 0;
	}

	Vector3D(float x, float y, float z)			// konstruktor inicjalizujacy wybrane wartosci wekotra
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Vector3D& operator= (Vector3D v)			// przeciazaony operator = oznacza, ze wektor v1=v2
	{
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	Vector3D operator+ (Vector3D v)				// przeciazony operator + dodajacy wektory
	{
		return Vector3D(x + v.x, y + v.y, z + v.z);
	}

	Vector3D operator- (Vector3D v)				// przeciazony operator- odejmujacy wektroy
	{
		return Vector3D(x - v.x, y - v.y, z - v.z);
	}

	Vector3D operator* (float value)			// przeciazony operator * mnozacy wektor razy podana wartosc.
	{
		return Vector3D(x * value, y * value, z * value);
	}

	Vector3D operator/ (float value)			// dzielenie wektora przez podana wartosc.
	{
		return Vector3D(x / value, y / value, z / value);
	}

	Vector3D& operator+= (Vector3D v)			
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	Vector3D& operator-= (Vector3D v)			
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	Vector3D& operator*= (float value)			
	{
		x *= value;
		y *= value;
		z *= value;
		return *this;
	}

	Vector3D& operator/= (float value)			
	{
		x /= value;
		y /= value;
		z /= value;
		return *this;
	}


	Vector3D operator- ()						// zmiana zwrotu wektora
	{
		return Vector3D(-x, -y, -z);
	}

	float length()								// funkcja zwracajaca dlugosc wektora
	{
		return sqrtf(x*x + y*y + z*z);
	};

	void unitize()								// normalizacja wektora do 1.
	{
		float length = this->length();

		if (length == 0)
			return;

		x /= length;
		y /= length;
		z /= length;
	}

	Vector3D unit()								// funkcja zwraca nowy wektor. zwrocona warosc jest znormalizowana wersja wektora
	{
		float length = this->length();

		if (length == 0)
			return *this;
		
		return Vector3D(x / length, y / length, z / length);
	}

};

// Klasa odpowiedzialna za nadawanie mas punktom			 Objekt reprezentujacy mase
class Mass
{
public:
	float m;									// Wartosc masy
	Vector3D pos;								// pozycja masy w przestrzeni
	Vector3D vel;								// predkosc
	Vector3D force;								// sila przylozona do masy w danym przypadku

	Mass(float m)								
	{
		this->m = m;
	}

	/*
	Metoda uzywana do dodania zewnetrzej sily masie. 
	Na mase moze dzialac wiele sil z roznych kierunkow. Metoda je dodaje i zwraca wektorowa sume zadanych sil.
	*/
	void applyForce(Vector3D force)
	{
		this->force += force;					// Dodawanie zewnetrznej sily dzialajacej na mase
	}

	/*
	  Metoda ustawiajaca wektor sily na 0.
	*/
	void init()
	{
		force.x = 0;
		force.y = 0;
		force.z = 0;
	}

	/*
	Metoda liczaca nowa predkosc i pozycje masy w zaleznosci od zmiany czasu dt. 
	Do symulacji zostala wykorzystana metoda Eulera
	*/
	void simulate(float dt)
	{
		vel += (force / m) * dt;				// Zmiana predkosci jest dodawana do predkosci i jest proporcjonalna do przyspieszenia oraz zmienna w czasie

		pos += vel * dt;						// Zmiana pozycji jest funkcja predkosci zmienna w czasie											
	}

};

// Klasa do symulacji
class Simulation
{
public:
	int numOfMasses;								// ilosc mass
	Mass** masses;									// masy sa przechowywane przez wskazniki.
	
	Simulation(int numOfMasses, float m)			// konstruktor towrzacy ciala o masie m
	{
		this->numOfMasses = numOfMasses;
		
		masses = new Mass*[numOfMasses];			// Tablica wskaznikow

		for (int count = 0; count < numOfMasses; ++count)		
			masses[count] = new Mass(m);				// Tworzenie obiektow cial
	}

	virtual void release()							// usuwanie obiektow
	{
		for (int count = 0; count < numOfMasses; ++count)		
		{
			delete(masses[count]);
			masses[count] = NULL;
		}

		delete(masses);
		masses = NULL;
	}

	Mass* getMass(int index)
	{
		if (index < 0 || index >= numOfMasses)		// zabezpieczenie przed naruszeniem pamieci
			return NULL;							

		return masses[index];						// pobierz mase o danym indeksie
	}

	virtual void init()								
	{
		for (int count = 0; count < numOfMasses; ++count)		// wywolanie wszystkich mas
			masses[count]->init();						
	}

virtual void solve()				//przeciazana metoda liczenia wszystkich przylozonych  sil									
	{

	}

	virtual void simulate(float dt)					// przeliczanie mas ze zmiana w czasie
	{
		for (int count = 0; count < numOfMasses; ++count)		// przeliczanie wszystkich mas
			masses[count]->simulate(dt);				// przeliczanie wszystkich mas i nadawanie im nowych pozycji i predkosci
	}

	virtual void operate(float dt)					// kompletna symulacja
	{
		init();										// Wyzerowac sily
		solve();									// Zadac sily
		simulate(dt);								// przeliczenie mas ze zmiana w czasie
	}

};

/*
Klasa ConstantVelocity jest pochodna klasy Simulation. Tworzy jednokilogramowa mase z predkoscia wzdluz 1.0 wzdluz x.
*/
class ConstantVelocity : public Simulation
{
public:
	ConstantVelocity() : Simulation(1, 1.0f)				//Konstruktor jednokilkogramowej masy
	{
		masses[0]->pos = Vector3D(0.0f, 0.0f, 0.0f);		//Stworzenie masy i ustawienie jej na poczatku
		masses[0]->vel = Vector3D(1.0f, 0.0f, 0.0f);		//nadanie predkosci masie
	}

};

/*
Klasa MotionUnderGravitation jest pochodna klasy Simulation
 Tworzy jednokilogramowa mase i nadaje jej predkosc o wartosciach(10.,15.0.0) i pozycje (-10.0.0,0.0).
 Te zalozenia dobrze symuluja spadanie ciala pod wplywem grawitacji z g=9.81 m/s 
*/
class MotionUnderGravitation : public Simulation
{
public:
	Vector3D gravitation;													//przyspieszenie ziemskie

	MotionUnderGravitation(Vector3D gravitation) : Simulation(1, 1.0f)		//Konstruktor
	{																		//Wektor przyspieszenia zmieskiego
		this->gravitation = gravitation;									
		masses[0]->pos = Vector3D(-10.0f, 0.0f, 0.0f);						//ustawienia polozenia i masy
		masses[0]->vel = Vector3D(10.0f, 15.0f, 0.0f);						
	}

	virtual void solve()													//Sila grawistacji bedzie przylozona do ciala dlatego potrzeba metody solve
	{
		for (int a = 0; a < numOfMasses; ++a)								//przylozymy sily do wszystkich mas
			masses[a]->applyForce(gravitation * masses[a]->m);				//przyspieszenie zmiemskie rowna sie F = m * g. 
	}
	
};

/*
Klasa tworzy jednokilogramowa mase i wiaze ja ze stalym punktem sprezyny.
  Punkt ten jest reprezentowany przez connectionPos i sprezyna posiadada stala reprezentujaca jej sztywnosc.
*/
class MassConnectedWithSpring : public Simulation
{
public:
	float springConstant;													//stala sprezystosci
	Vector3D connectionPos;													//punkt polaczenia masy ze sprezyna

	MassConnectedWithSpring(float springConstant) : Simulation(1, 1.0f)		
	{
		this->springConstant = springConstant;								//ustawienie stalej sprezyny

		connectionPos = Vector3D(0.0f, -5.0f, 0.0f);						//ustawienie punktu styku

		masses[0]->pos = connectionPos + Vector3D(10.0f, 0.0f, 0.0f);		//ustawienie pozycji masy
		masses[0]->vel = Vector3D(0.0f, 0.0f, 0.0f);						//ustawienie wektora predkosci masy na 0
	}

	virtual void solve()													//Dodawanie sily sprezystosci
	{
		for (int count = 0; count < numOfMasses; ++count)								//dodawanie sily do wszystkich mas
		{
			Vector3D springVector = masses[count]->pos - connectionPos;			//znajdz wektor z pozycji masy do punktu styku
			masses[count]->applyForce(-springVector * springConstant);			//przyloz sile zgodna ze znana sila sprezystosci
		}
	}

};
