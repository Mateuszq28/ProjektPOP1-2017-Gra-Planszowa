// POP 2017-12-21 projekt 1 Miler Mateusz EIT 2 171577
// Code::Blocks 16.01 GNU GCC Compiler (MinGW (tdm-1) 4.9.2)

#include <iostream>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iomanip>
using namespace std;
const int MAX_W = 12; //Liczba wierszy planszy (pola na ktorych moga stac pionki)
const int MAX_K = 8; //Liczba kolumn planszy
const int MAX_P = 4; //Liczba pionkow

struct gra
{
	char pola[MAX_W][MAX_K];
	char indeksy[MAX_W][MAX_K] = {  { '#','#','#','#','#','#','#','#' },
									{ '4','2','2','1','4','3','5','2' },
									{ '5','1','4','5','6','2','4','3' },
									{ '2','3','6','2','4','3','1','5' },
									{ '5','4','1','3','6','5','2','1' },
									{ '6','2','3','4','2','1','5','3' },
									{ '3','5','1','2','4','3','2','6' },
									{ '1','2','5','6','3','1','4','5' },
									{ '5','1','3','4','2','6','3','2' },
									{ '3','4','2','6','5','4','1','5' },
									{ '2','5','3','4','1','2','2','4' },
									{ '#','#','#','#','#','#','#','#' } };

	int ruch_nr = 0; //Pokazuje ktora to juz kolejka (suma ruchow obu graczy)
	int gracz = 0; //Gracz 0 to gracz 1 - komputer, a gracz 1 to gracz 2, czyli czlowiek
	int rzut; //Przechowuje wyrzucona liczbe oczek w danym ruchu
	char symbol; //Symbol obcenego gracza (O/X)
	char symbol_przeciwnik; //Symbol pionkow przeciwnika (O/X)

	//Tabela przechowujaca adrsy pionkow, ktore gracz moze przesunac podczas swojej kolejki
	//Pierwsza kolumna to aresy wierszy, druga to adresy kolumn znalezionych pionkow
	int tab[MAX_P][2];

	//Ta tablice uzywa tylko gracz 0 (komputer)
	//Pierwszy wiersz przechowuje jakim pionkiem mozemy cos zbic
	//Drugi wiersz przechowuje, co mozemy zbic
	int tabZbicie[2][2];
};

//
//------------------------------FUNKCJE UZYTE W PROGRAMIE------------------------------
//

//*********OOBSLUGA PLANSZY - PROSTE FUNKCJE*********
void ZerujPola(char [MAX_W][MAX_K]); //Ustawia spacje na calej tablicy z pozycjamni pionkow
void UstawianiePionkow(gra&); //Gracze ustawiaja pozycje startowe
void PiszPlansze(gra);
void UstawGracz(gra&); //Ustawia podstawowe zmienne dla kolejnego ruchu (nr ruchu, symbole)
void Naglowek(gra); //Wyswietla naglowek przed plansza
int Kostka();
void Wygrana(gra); //Sprawdza, czy gra jest skonczona, jesli tak, przerywa gre
int IleNaMecie(gra); //Sprawdza ile pionkow stoi na mecie
void NastepnyGracz(gra, char[]); //Komunikat o tym, ze kolej nastepnego gracza
void PiszZnalezione(gra, int); //Wypisuje elementy podana liczbe elementow z tablicy plansza.tab
void Identyfikuj(int, char, int&, int&); //Zamienia dane podane z klawiatury przez uzytkownika na indeksy pol
bool CzyJestWTablicy(int, int, gra, int); //Sprawdza, czy indeksy elementu znajduja sie w tablicy plansza.tab
void Przesun(char*, char*); //Przesuwa pionek z jednej pozycji, na inna
void Oszukuj(gra&); //Do sprawdzania, czy wygrana dziala
//***************************************************


//OGOLNA MECHANIKA (WSPOLNA DLA KOMPUTERA I UZYTKOWNIKA)
void Ruch(gra&);

//Szuka pionkow, ktore mozemy ruszyc
//Jesli znajedzie, zapisuje je w tablicy tab w planszy i zwraca ile jest takich pionkow
//Jak nie znajdzie, to zwraca zero
int IlePionkowMoznaRuszyc(gra&);

//Sprawdza czy jest mozliwe poruszenie danym pionkiem
bool CzyMoznaRuszycTenPionek(int, int, gra);

//Sprawdza czy mozemy przesunac pionek z danej pozycji na inna
bool CzyMoznaZrobicTenRuch(int, int, int, int, gra);

//Szuka pionkow na przedostatniej lini (przed meta)
//Jesli znajedzie, zapisuje je w tablicy tab w planszy i zwraca ile jest takich pionkow
//Jak nie znajdzie, to zwraca zero
int IlePionkowNaPrzedostatniej(gra&);
//******************************************************


//FUNKCJE DO RUCHU Z UZYTKOWNIKA (Z WYSWIETLANIEM)
void ZwyklyRuchGracz(gra&, int);
void PrzejscieNaMeteGracz(gra&, int);
void ZbijanieGracz(gra&);
//************************************************


//FUNKCJE DO RUCHU KOMPUTERA (BEZ WYSWIETLANIA)
bool CzyMozeszZbic(gra&);
void ZbijanieKomp(gra&);
void PrzejscieNaMeteKomp(gra&, int);
void ZwyklyRuchKomp(gra&, int);
//*********************************************

//-------------------------------------------------------------------------------------

int main()
{
	gra plansza;
	srand(time(NULL));
	ZerujPola(plansza.pola);
	//Oszukuj(plansza);
	UstawianiePionkow(plansza);

	while (true)
	{
		plansza.ruch_nr++;
		UstawGracz(plansza);
		Ruch(plansza);
	}
	system("pause");
    return 0;
}

bool CzyMozeszZbic(gra &plansza)
{
	for (int i = 0; i < MAX_W; i++)
		for (int j = 0; j < MAX_K; j++)
		{
			if (plansza.pola[i][j] == plansza.symbol) //sprawdza, czy na tym polu stoi nasz pionek
			{
				for (int wiersz = 1; wiersz < MAX_W; wiersz++) //sprawdza kolumne z pionkiem
					if (plansza.pola[wiersz][j] == plansza.symbol_przeciwnik && plansza.indeksy[wiersz][j] == static_cast<char>(plansza.rzut + 48))
					{
						plansza.tabZbicie[0][0] = i;
						plansza.tabZbicie[0][1] = j;
						plansza.tabZbicie[1][0] = wiersz;
						plansza.tabZbicie[1][1] = j;
						return true;
					}

				for (int kolumna = 0; kolumna < MAX_K; kolumna++) //sprawdza wiersz z pionkiem
					if (plansza.pola[i][kolumna] == plansza.symbol_przeciwnik && plansza.indeksy[i][kolumna] == static_cast<char>(plansza.rzut + 48))
					{
						plansza.tabZbicie[0][0] = i;
						plansza.tabZbicie[0][1] = j;
						plansza.tabZbicie[1][0] = i;
						plansza.tabZbicie[1][1] = kolumna;
						return true;
					}
			}
		}

	return false;
}

void UstawianiePionkow(gra &plansza)
{
	//Ustawianie pionkow komputera
	for (int i = 0; i < MAX_P; i++)
	{
		int los;
		do los = rand() % 8;
			while (plansza.pola[0][los] != ' ');
		plansza.pola[0][los] = 'O';
	}

	//Ustawianie pionkow uzytkownika
	char kolumnaZ;
	int kolumnaIND, temp;
	for (int i = 0; i < MAX_P; i++)
	{
		system("cls");
		PiszPlansze(plansza);
		cout << endl << "USTAWIANIE PIONKOW - GRACZ 2X" << endl;
		do
		{
			cout << "Podaj kolumne (A-H), w ktorej chcesz ustawic " << i+1 << " pionek: ";
			cin >> kolumnaZ;
			getchar();
			Identyfikuj(0, kolumnaZ, temp, kolumnaIND);

			if (kolumnaIND < 0 || kolumnaIND > 7 || plansza.pola[MAX_W - 1][kolumnaIND] != ' ')
			{
				cout << "Podano niepoprawny adres. Sprobuj jeszcze raz." << endl;
				cin.clear();
			}
		} while (kolumnaIND < 0 || kolumnaIND > 7 || plansza.pola[MAX_W - 1][kolumnaIND] != ' ');
		plansza.pola[MAX_W-1][kolumnaIND] = 'X';
	}
}

void Oszukuj(gra &plansza)
{
    plansza.pola[0][7] = 'X';
	plansza.pola[0][6] = 'X';
	plansza.pola[0][5] = 'X';
}

void ZerujPola(char pola[MAX_W][MAX_K])
{
	for (int i = 0; i < MAX_W; i++)
		for (int j = 0; j < MAX_K; j++)
			pola[i][j] = ' ';
}

void Ruch(gra &plansza)
{
	system("cls");
	Naglowek(plansza);
	PiszPlansze(plansza);

	if (plansza.gracz == 1) //Sprawdza czy gracz jest osoba fizyczna
	{
		cout << endl << "Nacisnij enter, aby rzucic kostka" << endl;
		getchar();
		plansza.rzut = Kostka();
		cout << "Wyrzuciles " << plansza.rzut << "!" << endl << endl;
	}
	else plansza.rzut = Kostka();

	char meldunek[50] = "\0";
	int licznik1 = IlePionkowNaPrzedostatniej(plansza);

	if (plansza.gracz == 0 && CzyMozeszZbic(plansza))
	{
		ZbijanieKomp(plansza);
	}
	else if (plansza.rzut == 6 && licznik1 > 0)
	{
		if (plansza.gracz == 1) PrzejscieNaMeteGracz(plansza, licznik1);
		else PrzejscieNaMeteKomp(plansza, licznik1);
	}
	else
	{
		int licznik2 = IlePionkowMoznaRuszyc(plansza);
		if (licznik2 > 0)
		{
			if (plansza.gracz == 1) ZwyklyRuchGracz(plansza, licznik2);
			else ZwyklyRuchKomp(plansza, licznik2);
		}
		else strcpy(meldunek, "Nie mozesz wykonac rzadnego ruchu!");
	}

	NastepnyGracz(plansza, meldunek);
}

void PrzejscieNaMeteGracz(gra &plansza, int licznik1)
{
	int wiersz;
	char kolumna;
	int wierszIND;
	int kolumnaIND;

	cout << "Mozesz przejsc na mete jednym z tych pinkow:" << endl;
	PiszZnalezione(plansza, licznik1);

	do
	{
		cout << "Ktory pionek wybierasz?" << endl;
		cout << "Podaj adres - kolumna (A-H) i wiersz (0-11): ";
		cin >> kolumna >> wiersz;
		getchar();
		Identyfikuj(wiersz, kolumna, wierszIND, kolumnaIND);

		if (!CzyJestWTablicy(wierszIND, kolumnaIND, plansza, licznik1))
		{
			cout << "Podano niepoprawny adres! Sprobuj jeszcze raz." << endl;
			cin.clear();
		}
	} while (!CzyJestWTablicy(wierszIND, kolumnaIND, plansza, licznik1));

	bool zbicie = false;
	if (plansza.pola[wierszIND - 1][kolumnaIND] == plansza.symbol_przeciwnik) zbicie = true;
	Przesun( &(plansza.pola[wierszIND][kolumnaIND]), &(plansza.pola[wierszIND - 1][kolumnaIND]) );
	if (zbicie) ZbijanieGracz(plansza);
	Wygrana(plansza);
}

void PrzejscieNaMeteKomp(gra &plansza, int licznik1)
{
	int los = rand() % licznik1;
	int wierszIND = plansza.tab[los][0];
	int kolumnaIND = plansza.tab[los][1];

	if (plansza.pola[wierszIND + 1][kolumnaIND] == plansza.symbol_przeciwnik)
	{
		do los = rand() % 8;
			while (plansza.pola[MAX_W - 1][los] != ' ');
		plansza.pola[MAX_W - 1][los] = plansza.symbol_przeciwnik;
	}

	Przesun(&(plansza.pola[wierszIND][kolumnaIND]), &(plansza.pola[wierszIND + 1][kolumnaIND]));
	Wygrana(plansza);
}

void Przesun(char *wskElement, char *wskCel)
{
	*wskCel = *wskElement;
	*wskElement = ' ';
}

bool CzyJestWTablicy(int wierszIND, int kolumnaIND, gra plansza, int Lelementow)
{
	for (int i = 0; i < Lelementow; i++)
		if (plansza.tab[i][0] == wierszIND && plansza.tab[i][1] == kolumnaIND)
			return true;

	return false;
}

void PiszZnalezione(gra plansza, int ile)
{
	for (int i = 0; i < ile; i++)
		cout << setw(5) << static_cast<char>(plansza.tab[i][1] + 65) << 11 - plansza.tab[i][0];
	cout << endl;
}

void Identyfikuj(int wiersz, char kolumna, int &wierszIND, int &kolumnaIND)
{
	wierszIND = 11 - wiersz;
	kolumnaIND = toupper(static_cast<int>(kolumna)) - 65;
}

void Wygrana(gra plansza)
{
	if (IleNaMecie(plansza) == 4)
	{
		system("cls");
		Naglowek(plansza);
		PiszPlansze(plansza);
		cout << endl << "WYGRAL GRACZ " << plansza.gracz + 1 << '-' << plansza.symbol << '!' << endl;
		system("pause");
		exit(0);
	}
}

int IleNaMecie(gra plansza)
{
	int licznik = 0;
	int wiersz = (MAX_W - 1) - (MAX_W - 1) * plansza.gracz;
	for (int i = 0; i < MAX_K; i++)
		if (plansza.pola[wiersz][i] == plansza.symbol)
			licznik++;

	return licznik;
}

void ZwyklyRuchGracz(gra &plansza, int licznik2)
{
	int wiersz;
	char kolumna;
	int wierszIND;
	int kolumnaIND;
	int wierszCEL;
	int kolumnaCEL;

	cout << "Mozesz wykonac ruch jendym z tych pinkow:" << endl;
	PiszZnalezione(plansza, licznik2);

	do
	{
		cout << "Ktory pionek wybierasz?" << endl;
		cout << "Podaj adres - kolumna (A-H) i wiersz (0-11): ";
		cin >> kolumna >> wiersz;
		getchar();
		Identyfikuj(wiersz, kolumna, wierszIND, kolumnaIND);

		if (!CzyJestWTablicy(wierszIND, kolumnaIND, plansza, licznik2))
		{
			cout << "Podano niepoprawny adres! Sprobuj jeszcze raz." << endl;
			cin.clear();
		}
	} while (!CzyJestWTablicy(wierszIND, kolumnaIND, plansza, licznik2));

	do
	{
		cout << "Gdzie chcesz przesunac pionek?" << endl;
		cout << "Podaj adres - kolumna (A-H) i wiersz (0-11): ";
		cin >> kolumna >> wiersz;
		getchar();
		Identyfikuj(wiersz, kolumna, wierszCEL, kolumnaCEL);

		if (!CzyMoznaZrobicTenRuch(wierszIND, kolumnaIND, wierszCEL, kolumnaCEL, plansza))
		{
			cout << "Nie mozesz wykonac takiego ruchu! Sprobuj jeszcze raz." << endl;
			cin.clear();
		}
	} while (!CzyMoznaZrobicTenRuch(wierszIND, kolumnaIND, wierszCEL, kolumnaCEL, plansza));

	bool zbicie = false;
	if (plansza.pola[wierszCEL][kolumnaCEL] == plansza.symbol_przeciwnik) zbicie = true;
	Przesun(&(plansza.pola[wierszIND][kolumnaIND]), &(plansza.pola[wierszCEL][kolumnaCEL]));
	if (zbicie) ZbijanieGracz(plansza);
}

void ZwyklyRuchKomp(gra &plansza, int licznik2)
{
	int los = rand() % licznik2;
	int wierszIND = plansza.tab[los][0];
	int kolumnaIND = plansza.tab[los][1];

	bool znaleziono = false; //czy znaleziono pole, na ktore mozna przesunac pionek

	for (int i = MAX_W - 1; i >= 0 && !znaleziono; i--) //Sprawdzamy kolumne
		if (plansza.indeksy[i][kolumnaIND] == static_cast<char>(plansza.rzut + 48) && plansza.pola[i][kolumnaIND] == ' ')
		{
			Przesun(&(plansza.pola[wierszIND][kolumnaIND]), &(plansza.pola[i][kolumnaIND]));
			znaleziono = true;
		}

	for (int i = 0; i < MAX_K && !znaleziono; i++) //Sprawdzamy wiersz
		if (plansza.indeksy[wierszIND][i] == static_cast<char>(plansza.rzut + 48) && plansza.pola[wierszIND][i] == ' ')
		{
			Przesun(&(plansza.pola[wierszIND][kolumnaIND]), &(plansza.pola[wierszIND][i]));
			znaleziono = true;
		}

	//Nie trzeba rozwazac przypadku zbicia, bo odrzucilismy go w poprzednim warunku (Ruch)
}

void ZbijanieGracz(gra &plansza)
{
	int wiersz = 11 - plansza.gracz * 11;
	char kolumna;
	int kolumnaIND;
	int temp;

	system("cls");
	Naglowek(plansza);
	PiszPlansze(plansza);
	cout << endl << "Zbiles pionek przeciwnika! Ustaw go na pasku w wierszu " << plansza.gracz * (MAX_W - 1) << endl;
	do
	{
		cout << "Podaj kolumne (A-H): ";
		cin >> kolumna;
		getchar();
		Identyfikuj(0, kolumna, temp, kolumnaIND);

		if (kolumnaIND < 0 || kolumnaIND > 7 || plansza.pola[wiersz][kolumnaIND] != ' ')
		{
			cout << "Podales niepoprawny adres! Sprobuj jeszcze raz." << endl;
			cin.clear();
		}
	} while (kolumnaIND < 0 || kolumnaIND > 7 || plansza.pola[wiersz][kolumnaIND] != ' ');

	plansza.pola[wiersz][kolumnaIND] = plansza.symbol_przeciwnik;
}

void ZbijanieKomp(gra &plansza)
{
	int wierszEL = plansza.tabZbicie[0][0];
	int kolumnaEL = plansza.tabZbicie[0][1];
	int wierszCEL = plansza.tabZbicie[1][0];
	int kolumnaCEL = plansza.tabZbicie[1][1];

	Przesun(&(plansza.pola[wierszEL][kolumnaEL]), &(plansza.pola[wierszCEL][kolumnaCEL]));
	int los;
	do los = rand() % 8;
		while (plansza.pola[MAX_W - 1][los] != ' ');
	plansza.pola[MAX_W - 1][los] = plansza.symbol_przeciwnik;
}

void NastepnyGracz(gra plansza, char meldunek[])
{
	system("cls");
	if (strlen(meldunek) > 0)
		cout << meldunek << endl;
	else
		Naglowek(plansza);
	PiszPlansze(plansza);
	if (plansza.gracz == 0) cout << endl << "KOMPUTER WYKONAL SWOJ RUCH" << endl;
	else cout << endl;
	cout << "Nacisnij enter aby przejsc do kolejnego gracza..." << endl;
	getchar();
}

int IlePionkowNaPrzedostatniej(gra &plansza)
{
	int licznik = 0;
	// w - wiersz
	// k - kolumna
	int w = MAX_W - 2 - 9 * plansza.gracz;
	int meta = w + 1 - 2 * plansza.gracz; //wiersz mety dla odpowiedniego gracza

	for (int k = 0; k < MAX_K; k++)
		if (plansza.pola[w][k] == plansza.symbol && plansza.pola[meta][k] != plansza.symbol)
		{
			plansza.tab[licznik][0] = w;
			plansza.tab[licznik][1] = k;
			licznik++;
		}
	return licznik;
}

int IlePionkowMoznaRuszyc(gra &plansza)
{
	int licznik = 0;
	for (int i = plansza.gracz; i < MAX_W - 1 + plansza.gracz; i++)
		for (int j = 0; j < MAX_K; j++)
			if (plansza.pola[i][j] == plansza.symbol && CzyMoznaRuszycTenPionek(i, j, plansza))
			{
				plansza.tab[licznik][0] = i;
				plansza.tab[licznik][1] = j;
				licznik++;
			}

	return licznik;
}

bool CzyMoznaRuszycTenPionek(int w, int k, gra plansza)
{
	for (int i = 0; i < MAX_W; i++)
		for (int j = 0; j < MAX_K; j++)
			if (CzyMoznaZrobicTenRuch(w, k, i, j, plansza)) return true;
	return false;
}

//w i k to adres pinka, ktorym chcemy ruszyc
//i i j to adrs, gdzie chcemy go przesunac
bool CzyMoznaZrobicTenRuch(int w, int k, int i, int j, gra plansza)
{
	//48 to 0 w tablicy ASCII
	if ((w != i && k != j) || (w == i && k == j) ||
		(plansza.indeksy[i][j] != static_cast<char>(plansza.rzut) + 48) || (plansza.pola[i][j] == plansza.symbol))
		return false;
	return true;
}

int Kostka()
{
	return rand() % 6 + 1;
}

void UstawGracz(gra &plansza)
{
	//Gracz 1 to od tej pory gracz 0, a gracz 2 to gracz 1
	if (plansza.ruch_nr % 2 == 1)
	{
		plansza.gracz = 0;
		plansza.symbol = 'O';
		plansza.symbol_przeciwnik = 'X';
	}
	else
	{
		plansza.gracz = 1;
		plansza.symbol = 'X';
		plansza.symbol_przeciwnik = 'O';
	}
}

void Naglowek(gra plansza)
{
	cout << "(Gracz: " << plansza.gracz + 1 << '-' << plansza.symbol << "\tKolejka: " << plansza.ruch_nr << "\tRuch: " << (plansza.ruch_nr+1)/2 << ')' << endl;
}

void PiszPlansze(gra plansza)
{
	for (int i = 0; i < MAX_W; i++)
	{
		cout << setiosflags(ios::left) << setw(2) << MAX_W - 1 - i << '|';
		for (int j = 0; j < 8; j++)
			cout << plansza.pola[i][j] << '|' << plansza.indeksy[i][j] << '|';
		cout << endl;
	}
	cout << setw(3) << ' ';
	for (int i = 65; i <= 72; i++)
		cout << setw(4) << static_cast<char>(i);
	cout << resetiosflags(ios::left) << endl;
}
