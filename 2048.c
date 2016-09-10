#include <string.h>
#include <ncurses.h>
#include <menu.h>
#include <stdlib.h>
#include <time.h>
int copiaza_matricea(int tabla[4][4], int copie[4][4]);
int muta_dr(int tabla[4][4], int *scor);
int oglindeste(int tabla[4][4]);
int transpune_matricea(int tabla[4][4]);

int arata_punctaj(int scor)
{
	//ce este aici e usor de inteles:
	//curat linia pe care se afla punctajul si nivelul
	//apoi, functie de scor, decid care este nivelul
	//si il tiparesc alaturi de punctaj pe linia
	//tocmai curatata
	move(5, 0);
	clrtoeol();
	if (scor < 300)
		mvprintw(5, 0, "Punctaj: %d-Pruna", scor);
	if (scor >= 300 && scor < 600)
		mvprintw(5, 0, "Punctaj: %d-Manelar", scor);
	if (scor >= 600 && scor < 900)
		mvprintw(5, 0, "Punctaj: %d-Intercocalar", scor);
	if (scor >= 900 && scor < 1200)
		mvprintw(5, 0, "Punctaj: %d-Asta inseamna sa fii mafiot", scor);
	if (scor >= 1200 && scor < 1500)
		mvprintw(5, 0, "Punctaj: %d-Faraon", scor);
	if (scor >= 1500 && scor < 1800)
		mvprintw(5, 0, "Punctaj: %d-Sefu' la bani", scor);
	if (scor >= 1800 && scor < 2500)
		mvprintw(5, 0, "Punctaj: %d-Dani Mocanu", scor);
	if (scor >= 2500)
		mvprintw(5, 0, "Punctaj: %d-Iesti de haur!", scor);
	return 1;
}

char best_choice(int liber[4])
{
	//vectorul liber contine numarul casutelor care ar fi libere
	//dupa o miscare la dreapta, stanga, sus sau jos
	//(in aceasta ordine)
	int copie_liber[4];
	int i, j, aux;
	for (i = 0; i < 4; i++)
		copie_liber[i] = liber[i];
	//am facut o copie vectorului iar apoi am sortat copia
	for (i = 0; i < 3; i++)
		for (j = i + 1; j < 4; j++) {
			if (copie_liber[j] > copie_liber[i]) {
				aux = copie_liber[j];
				copie_liber[j] = copie_liber[i];
				copie_liber[i] = aux;
			}
		}
	//compar cea mai mare valoare din copia 
	//(valoarea de pe prima pozitie) cu fiecare element din
	//vectorul original. cand cele 2 coincid returnez 
	//caracterul (directia de miscare) asociata valorii 
	//respective din vectorul original. 
	//Observatie: nu conteaza daca sunt mai multe valori
	//identice, iar cea de pe pozitia 0 din copie nu 
	//coincide cu caracterul returnat. 
	//(caracterul "adevarat" si cel returnat duc la aceiasi
	//eliberare de casute)
	for (i = 0; i < 4; i++)
		if (copie_liber[0] == liber[i]) {
			if (i == 0)
				return 'd';
			if (i == 1)
				return 'a';
			if (i == 2)
				return 'w';
			if (i == 3)
				return 's';
		}
	return 'p';
}

int casute_libere(int tabla[4][4])
{
	//parcurg matricea si vad cate zerouri sunt
	//returnez numarul de zerouri, adica de poz. libere
	int nr, i, j;
	nr = 0;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			if (tabla[i][j] == 0)
				nr++;
	return nr;
}

char decide_directia(int tabla[4][4])
{
	//asociez un vector de 4 elemente ce va contine
	//numarul de casute libere pentru fiecare caz:
	//dreapta, stanga, sus, jos (in aceasta ordine)
	//(vector de frecvente)
	int copie[4][4];
	int umplutura = 0;
	copiaza_matricea(tabla, copie);
	//am facut o copie pe care voi lucra cu miscarile
	//posibile
	int liber[4];
	liber[0] = liber[1] = liber[2] = liber[3] = 0;
	//fac mutarea pe copie si vad cate casute
	//raman neocupate (au valoarea 0).
	//umplutura nu are niciun rol, functioneaza in
	//functia apelata pe post de variabila scor
	//ce in mod normal tine scorul.
	muta_dr(copie, &umplutura);
	liber[0] = casute_libere(copie);
	copiaza_matricea(tabla, copie);
	oglindeste(copie);
	muta_dr(copie, &umplutura);
	oglindeste(copie);
	liber[1] = casute_libere(copie);
	copiaza_matricea(tabla, copie);
	transpune_matricea(copie);
	oglindeste(copie);
	muta_dr(copie, &umplutura);
	liber[2] = casute_libere(copie);
	copiaza_matricea(tabla, copie);
	transpune_matricea(copie);
	muta_dr(copie, &umplutura);
	liber[3] = casute_libere(copie);
	//pe alocuri au fost necesare prelucrari suplimentare
	//matricei:
	//am scris codul doar pentru mutarea la dreapta
	//ma folosesc de el transpunand matricea sau oglindind-o
	//sau ambele. 
	//observatie: transpusa(transpusa(matrice))=matrice
	//observatie: oglindit(oglindit(matrice))=matrice
	return best_choice(liber);
	//intorc mutarea cea mai profitabila, care elibereaza
	//cele mai multe casute
}

int oglindeste(int tabla[4][4])
{
	//aceasta functie oglindeste matricea primita (tabla
	//de joc). Interschimba coloanele 0-3,1-2;
	int aux, i;
	for (i = 0; i < 4; i++) {
		aux = tabla[i][0];
		tabla[i][0] = tabla[i][3];
		tabla[i][3] = aux;
		aux = tabla[i][1];
		tabla[i][1] = tabla[i][2];
		tabla[i][2] = aux;
	}
	return 1;
}

int muta_dr(int tabla[4][4], int *scor)
{
	int i, j, k;
	//voi parcurge tabla pe linii
	for (i = 0; i < 4; i++) {
		//in primul rand shiftez totul la dreapta, astfel incat
		//zerourile sa apara doar in stanga elementelor cu valori
		//mai mari
		//ordinea elementelor cu valori nenule se va pastra
		for (j = 3; j > -1; j--) {
			if (tabla[i][j] == 0) {
				for (k = 0; k < j; k++)
					if (tabla[i][k])
						//decid daca am elemente nenule
						//adica daca linia nu e plina de zerouri
						break;
				if (k == j)
					continue;
				//inseamna ca e plin de zerouri si
				// nu am ce face aici
				if (k != j) {
					//daca am elemente nenule shiftez la dreapta
					//peste zeroul de la care am plecat
					//si adaug 0 pe prima pozitie
					for (k = j; k > 0; k--) {
						tabla[i][k] = tabla[i][k - 1];
					}
					tabla[i][0] = 0;
				}
				if (tabla[i][j] == 0)
					//daca am shiftat un zero pe pozitia
					//tocmai verificata mai fac odata prelucrarea
					j++;
			}
		}		//acum am scapat de zerouri de pe linie

		for (j = 3; j > 0; j--) {
			//parcurg pe coloane iar daca doua elemente au aceiasi valoare
			//atunci cel mai din dreapta se dubleaza iar cel din stanga
			//devine 0
			//actualizez si scorul (adaug valoarea rezultata la scor)
			if (tabla[i][j] == tabla[i][j - 1]) {
				tabla[i][j] = 2 * tabla[i][j];
				(*scor) += tabla[i][j];
				tabla[i][j - 1] = 0;	
			}
		}
		for (j = 3; j > 0; j--)
			//la fel ca in primul for din parcurgea pe linii
			//scap de zerourile puse prin shiftare la dreapta
			//nu mai e nevoie sa verific ca am elemente nenule 
			//prima pozitie devine 0
			if (tabla[i][j] == 0) {
				for (k = j; k > 0; k--)
					tabla[i][k] = tabla[i][k - 1];
				tabla[i][0] = 0;
			}
	}
	return 1;
}

int mai_sunt_mutari(int tabla[4][4])
{
	//parcurg pe linii, apoi pe coloane
	//daca doua elemente alaturate sunt egale mai sunt mutari
	int i, j;
	for (i = 0; i < 4; i++)
		for (j = 1; j < 4; j++)
			if (tabla[i][j] == tabla[i][j - 1])
				return 1;
	for (j = 0; j < 4; j++)
		for (i = 1; i < 4; i++)
			if (tabla[i][j] == tabla[i - 1][j])
				return 1;
	return 0;
}

int am_loc(int tabla[4][4])
{
	//daca am pozitie cu valoare 0 in matrice mai am loc
	//sa pun random
	int i, j;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			if (tabla[i][j] == 0)
				return 1;
	return 0;
}

int transpune_matricea(int tabla[4][4])
{
	//parcurg pe linii si pe coloane
	//interschimb elementul de pe i, j cu cel de pe j,i
	//Observatie: parcurg doar pana la diagonala 
	//principala. Pe diagonala principala
	//nu s-ar schimba nimic, iar dincolo de ea
	//matricea ar deveni cea originala
	int i, j, aux;
	for (i = 0; i < 4; i++)
		for (j = 0; j < i; j++) {
			aux = tabla[i][j];
			tabla[i][j] = tabla[j][i];
			tabla[j][i] = aux;
		}
	return 1;
}

int copiaza_matricea(int tabla[4][4], int copie[4][4])
{
	//fac o copie matricei, parcurgand pe linii si col.
	int i, j;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			copie[i][j] = tabla[i][j];
	return 1;
}

int difera_copia(int tabla[4][4], int copie[4][4])
{
	//daca cel putin doua elemente corespondente
	//difera atunci matricele difera
	int i, j;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			if (copie[i][j] != tabla[i][j])
				return 1;
	return 0;
}

int plaseaza_random(int tabla[4][4])
{
	srand(time(NULL));
	//am initializat generatorul random
	int valoare = (rand() % 2 + 1) * 2;
	//am obtinut valoare din {2,4}
	int i, j;
	i = rand() % 4;
	j = rand() % 4;
	//pozitia e generata initial aleator
	while (tabla[i][j]) {
		//daca pe pozitia generata
		//aleator am element
		//mai generez odata pozitie
		i = rand() % 4;
		j = rand() % 4;
	}
	//pe acea pozitie pun valoare random
	tabla[i][j] = valoare;
	return 1;
}

int redeseneaza_tabla(int tabla[4][4], int poz[4][4])
{
	int i, j;
	//efectiv pictez tabla pe ecran
	move(10, 0);
	printw("======================================\n");
	printw("|        |        |        |         |\n");
	printw("|_______ |________|________|_________|\n");
	printw("|        |        |        |         |\n");
	printw("|________|________|________|_________|\n");
	printw("|        |        |        |         |\n");
	printw("|________|________|________|_________|\n");
	printw("|        |        |        |         |\n");
	printw("|        |        |        |         |\n");
	printw("======================================\n");
	//parcurg tabla sub forma matriceala
	//ma deplasez la pozitia dorita pe ecran
	//(ma ajut pentru acest lucru de matricea poz ce
	//contine date despre unde trebuie sa apara numerele
	//pe ecran-determinate prin incercari)
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++) {
			if (tabla[i][j]) {
				move(11 + (i * 2), poz[1][j] + 3);
				printw("%d", tabla[i][j]);
				move(20, 0);
				//ma deplasez pe o pozitie astfel incat sa nu
				//dispara nimic de pe ecran prin apasarea
				//unei taste 
			}
		}
	return 1;
}

int gameaza(int tabla[4][4], int poz[4][4], int *scor)
{
	//aici se efectueaza jocul in sine
	int i, j, copie_scor;
	//copie scor va fi folosit pentru undo
	time_t timp_secunde;
	//variabila pt timp raw
	struct tm *timp;
	//variabila pentru timp mai usor prelucrabil
	int copie[4][4];
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			copie[i][j] = 0;
	//copia folosita pentru a decide daca miscarea
	//e valida, precum si in undo, e initializata
	//cu zerouri
	move(20, 0);
	int apasat;
	apasat = 't';
	redeseneaza_tabla(tabla, poz);
	//pun tabla fara elemente pe ecran (zerourile nu sunt
	//afisate)
	int de_undoit[4][4];
	//matrice folosita la undo
	int pot_undo = 0;
	//pot sa fac undo (nu sunt la prima miscare)
	int cheat = 0;
	//contor pentru apasarea tastei d ce activeaza cheatul
	copie_scor = 0;
	//intializez cu zero copia scorului
	//mai departe "ma joc cat mai am loc pe tabla sau
	//mai am unde sa mut"
	while ((am_loc(tabla)) || mai_sunt_mutari(tabla)) {
		time(&timp_secunde);
		//aflu timpul
		timp = localtime(&timp_secunde);
		//aflu timpul in valoare mai simpla
		mvprintw(4, 11, ": %d-%d-%d | %d:%02d",
			 timp->tm_mday, timp->tm_mon + 1, timp->tm_year + 1900,
			 timp->tm_hour, timp->tm_min);
		//printez timpul. constantele adaugate au rol de a echivala 
		//valorile din structura cu cele cu care oamenii sunt
		//obisnuiti
		mvprintw(5, 9, "%d", *scor);
		//afisez scorul (pentru cazul initial, cand nu dau un nivel
		//jucatorului, pentru ca nu a facut miscari si nu e 'fair')
		move(3, 0);
		//mut la o pozitie neutra unde astept 20 de secunde pentru
		//apasarea unei taste, altfel jocul ia singur decizia cea
		//mai optima pentru ce tasta trebuia apasata
		halfdelay(200);
		apasat = getch();
		if (apasat == ERR)
			apasat = decide_directia(tabla);
		move(3, 0);
		printw(" ");
		//am acoperit tasta citita
		switch (apasat) {
			//functie de ce tasta am decis fac
			//ce tasta are asignat sa faca
		case 'u':
			//daca e u fac undo, asta daca nu e prima
			//mutare (ca nu am la ce sa fac undo)
			//matricea de undoit contine tabla anterioara
			//scorul devine scorul anterior
			//printez tabla pe ecran
			if (pot_undo) {
				copiaza_matricea(de_undoit, tabla);
				*scor = copie_scor;
				redeseneaza_tabla(tabla, poz);
			} else
				//daca nu pot sa fac undo, apasarea lui undo
				//e echivalent cu nu am facut miscari pe
				//tabla
				pot_undo--;
			break;
		case 'q':
			//daca se apasa q ma mut la o pozitie neutra
			//si ies
			move(21, 0);
			return 1;
			break;
		case 'd':
			//fac mutarea la dreapta 
			//salvez in copie , sa vad daca mutarea
			//a fost valida
			copiaza_matricea(tabla, copie);
			copie_scor = *scor;
			muta_dr(tabla, scor);
			cheat++;
			//am apasat odata d, pentru cheat
			if (cheat == 12) {
				//daca am apasat de 12 ori consecutiv
				//tasta d
				//atunci toate casutele devin 2048 (:D)
				//iar cel care a ghicit cheatul este
				//recompensat cu un mesaj amuzant
				for (i = 0; i < 4; i++)
					for (j = 0; j < 4; j++)
						tabla[i][j] = 2048;
				redeseneaza_tabla(tabla, poz);
				mvprintw(21, 0, "Esti boss, imparate!");
			}

			if (difera_copia(tabla, copie) && am_loc(tabla))
				//daca am facut o mutare valida si mai am
				//unde sa pun valoare random, pun valoare
				//random
				plaseaza_random(tabla);
			move(20, 0);
			//mut la o pozitie neutra si redesenez tabla
			redeseneaza_tabla(tabla, poz);
			break;
		case 'a':
			//analog cazului a, dar fara cheat
			//acesta doar devine 0 (am rupt lantul de
			//taste d apasate consecutiv)
			copiaza_matricea(tabla, copie);
			copie_scor = *scor;
			oglindeste(tabla);
			//in loc sa mai scriu odata algoritmul de deplasare
			//mai bine oglindesc, fac deplasare la dreapta
			//si oglindesc iar
			muta_dr(tabla, scor);
			oglindeste(tabla);
			if (difera_copia(tabla, copie) && am_loc(tabla))
				plaseaza_random(tabla);
			move(20, 0);
			cheat = 0;
			redeseneaza_tabla(tabla, poz);
			break;
		case 'w':
			//procedez ca la a doar ca inainte sa mut dreapta
			//transpun si apoi oglindesc, apoi fac procedeul
			//invers
			copiaza_matricea(tabla, copie);
			copie_scor = *scor;
			transpune_matricea(tabla);
			oglindeste(tabla);
			muta_dr(tabla, scor);
			oglindeste(tabla);
			transpune_matricea(tabla);
			if (difera_copia(tabla, copie) && am_loc(tabla))
				plaseaza_random(tabla);
			move(20, 0);
			cheat = 0;
			redeseneaza_tabla(tabla, poz);
			break;
		case 's':
			//procedez ca la w doar ca transpun
			copiaza_matricea(tabla, copie);
			copie_scor = *scor;
			transpune_matricea(tabla);
			muta_dr(tabla, scor);
			transpune_matricea(tabla);
			if (difera_copia(tabla, copie) && am_loc(tabla))
				plaseaza_random(tabla);
			move(20, 0);
			cheat = 0;
			redeseneaza_tabla(tabla, poz);
			break;
		}
		copiaza_matricea(copie, de_undoit);
		//matricea nou creeata e folosita pt undo
		arata_punctaj(*scor);
		//afisez scorul
		pot_undo++;
		//am apasat o tasta, deci pot sa fac undo acum

	}
	mvprintw(21, 0, "\nAi pierdut.Poti naviga prin meniu");
	//mesajul de pierdere a jocului
	//revin la pozitie neutra
	move(23, 0);
	return 1;
}

int goleste_tabla(int tabla[4][4])
{
	//umplu tabla de zerouri
	int i, j;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			tabla[i][j] = 0;
	return 1;
}

int start_the_game(int tabla[4][4], int pozitia[4][4], int *scor)
{
	int valoare, aleator, linia, coloana;
	int ln_rnd, cl_rnd, cl2_rnd, ln2_rnd, aleator2;
	int i;
	//am determinat, prin incercari, unde trebuie sa apara 
	//numerele pe ecran
	for (i = 0; i < 4; i++) {
		pozitia[i][0] = 0;
		pozitia[i][1] = 10;
		pozitia[i][2] = 20;
		pozitia[i][3] = 30;
	}

	goleste_tabla(tabla);	//am umplut-o de zerouri
	//la fel ca mai sus (in random) produc valori
	//random si pozitii random pentru primul numar
	//de afisat, la inceputul jocului
	srand(time(NULL));
	valoare = (rand() % 2 + 1) * 2;
	aleator = valoare;
	linia = rand() % 4;
	coloana = rand() % 4;
	tabla[linia][coloana] = aleator;
	ln_rnd = 11 + (linia * 2);
	cl_rnd = (coloana + 1 + coloana + 4 + 8 * coloana);
	cl2_rnd = ln2_rnd = 0;
	valoare = -1;
	//pentru al2-lea numar vefific, in plus, sa nu
	//fie in aceiasi pozitie cu primul
	while (cl2_rnd == 0 || ln2_rnd == 0 || cl2_rnd == cl_rnd
	       || ln2_rnd == ln_rnd) {
		valoare = rand() % 2;
		aleator2 = (valoare + 1) * 2;
		linia = rand() % 4;
		ln2_rnd = 11 + (linia * 2);
		coloana = rand() % 4;
		cl2_rnd = (coloana + 1 + coloana + 4 + 8 * coloana);
	}
	//in continuare afisez efectiv numerele generate
	//aleator pe ecran
	tabla[linia][coloana] = aleator2;
	move(ln_rnd, cl_rnd - 4);
	printw("%d", aleator);
	move(ln2_rnd, cl2_rnd - 4);
	printw("%d", aleator2);
	move(49, 0);
	//ma mut pe o pozitie neutra si incep efectiv jocul
	gameaza(tabla, pozitia, scor);
	return 1;
}

int deseneaza_tabla(int tabla[4][4], int pozitia[4][4], int *scor)
{
	//desenez tabla pe ecran si panoul de control
	move(4, 0);
	printw("Data si ora\n");
	printw("Punctaj: 0\n");
	printw("Comenzi valide\n");
	printw("w=muta sus    | s=muta jos\n");
	printw("a=muta stanga | d=muta dreapta\n");
	printw("q=revino la meniu | u=undo\n");
	move(10, 0);
	printw("======================================\n");
	printw("|        |        |        |         |\n");
	printw("|_______ |________|________|_________|\n");
	printw("|        |        |        |         |\n");
	printw("|________|________|________|_________|\n");
	printw("|        |        |        |         |\n");
	printw("|________|________|________|_________|\n");
	printw("|        |        |        |         |\n");
	printw("|        |        |        |         |\n");
	printw("======================================\n");
	start_the_game(tabla, pozitia, scor);
	return 1;
}

int main()
{
	int ch;
	int i;
	int tabla[4][4], poz[4][4], scor;
	scor = 0;
	//pornesc cu scorul 0 si matricea poz
	//ce tine minte unde trebuie printat fiecare nr
	for (i = 0; i < 4; i++) {
		poz[i][0] = 0;
		poz[i][1] = 10;
		poz[i][2] = 20;
		poz[i][3] = 30;
	}

	initscr();		//porneste ncurses
	cbreak();		//dezactiveaza bufferul
	keypad(stdscr, TRUE);	//porneste tastatura
	refresh();		//afisez pe ecranul real
	resizeterm(24, 130);	//dau dimensiunea terminalului
	keypad(stdscr, TRUE);
	//aici voi face afisare meniului
	//precum si creearea sa
	move(5, 0);
	char *optiuni[] = {
		"New Game(Cata valoare ai?)",
		"Continua(Creste-ti valoarea)",
		"Quit(Destul pentru astazi)"
	};
	ITEM **elemente;
	MENU *meniu;
	elemente = (ITEM **) calloc(4, sizeof(ITEM *));
	//am alocat memoria pt. elem. meniului
	//acum voi adauga efectiv elementele
	for (i = 0; i < 3; i++)
		elemente[i] = new_item(optiuni[i], "\0");
	elemente[3] = (ITEM *) NULL;
	move(7, 0);
	printw("2048-Pamflet,mai amuzant decat crezi!\n\n\n");
	meniu = new_menu((ITEM **) elemente);	//am facut meniu
	post_menu(meniu);	//il pun pe ecran
	refresh();		//pun totul pe ecranul fizic
	//pozitia va tine minte pe ce este highlightul in meniu
	int pozitia = 2;	//2 pentru NG, 1 pt Res, 0 pt quit
	move(23, 0);
	//la aceasta pozitie voi citi caracterul
	//voi citi oricat de mult (se iese prin q)
	while ((ch = getch())) {
		move(23, 0);
		clrtoeol();
		//sterg linia unde apare caracterul citit
		switch (ch) {
		case KEY_DOWN:
			//daca pozitia nu e minima
			//ma deplasez in meniu
			if (pozitia != 0) {
				menu_driver(meniu, REQ_DOWN_ITEM);
				pozitia--;
			}
			break;
		case KEY_UP:
			//analog KEY_DOWN
			if (pozitia != 2) {
				menu_driver(meniu, REQ_UP_ITEM);
				pozitia++;
			}
			break;
		case '\n':
			//daca apas enter decid la ce pozitie
			//eram
			move(11, 0);
			if (pozitia == 0) {
				//la Quit ies
				endwin();
				return 0;
			}
			if (pozitia == 1) {
				//la 1 continui jocul
				mvprintw(24, 0, " ");
				move(11, 0);
				gameaza(tabla, poz, &scor);
				refresh();
			}
			if (pozitia == 2) {
				//la 2 incep joc nou
				//si curat mesajul de la jocul anterior
				mvprintw(24, 0,
					 "                                                                    ");
				move(11, 0);
				scor = 0;
				//aici curat posibile ramasite
				//de la tastatari anterioare
				move(21, 0);
				clrtoeol();
				move(22, 0);
				clrtoeol();
				deseneaza_tabla(tabla, poz, &scor);
				refresh();
			}
			break;
		}
		move(21, 0);
		clrtoeol();
	}

	endwin();
	return 0;
}
