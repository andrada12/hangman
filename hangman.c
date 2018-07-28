#include <ncurses.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <ctype.h>
#include "desene.h"
#include "ascuns.h"

#define INALTIME_FEREASTRA 13
#define LATIME_FEREASTRA 20

#define LUNG_INIT 10

typedef struct cuvinte {

	char a[180];
}cuvant;

/*functia ia un string si transforma fiecare majuscula in
minuscula, caracter cu caracter*/
void litere_mici(char a[]) {
    int i;
    for(i=0;i<strlen(a);i++)
        a[i]=tolower(a[i]);

}
/*se sterge caracterul de la pozitia k prin translatarea
elemntelor de pe pozitiile urmatoare cu un spatiu inapoi*/
void stergere_caracter(char s[],int k) {
	int i;
	for(i=k;i<strlen(s); i++)
		s[i]=s[i+1];
}

/*se sterg caracterele neprintabile si liniile care
contin doar caracterul space*/
void stergere_caracter_neprintabil(char s[]) {
	int i=0;
	while(i<strlen(s)) {	
		if(!isprint(s[i])) {
			stergere_caracter(s,i);
			i=i-1;
		}
		i++;
	}	

    int ok=1;
    for(i=0;i<strlen(s);i++)
        if(s[i]!=32)
            ok=0;
    if(ok==1)
        s[0]='\0';
}
/*se alege un numar aleator pana la 255, pentru utilizarea sa ca
si cod ASCII. Aceasta functie este apelata pentru alegerea unui 
caracter aleator atunci cand utilizatorul nu apasa nicio tasta
timp de 7 sec; primeste ca si parametru caracterele deja introduse
pentru a nu retunra codull ASCII al unui caracter deja existent
in joc*/
int caracter_random(char caractere_introduse[]) {
	int k,sf=1;
	srand(time(NULL));
	while(sf){
		k = rand() % 255;
		if(k>32 && (k<65 || k>90) && !(strchr(caractere_introduse,k)) && isprint(k))
			sf=0;
	}
	return k;
}

/*returneaza un numar aleator ce corespunde numarului unei
expresii citite; primeste ca parametru numarul total de
expresii stocate pentru a alege dintre toate acestea*/
int expresie_aleasa(int n) {
	int k;
    srand(time(NULL));
	k = rand() % n;
	return k;
}
 
/* creeaza o noua fereastra, primind ca parametrii inaltimea,
lungimea si pozitiile de pe ecran la care sa se afle aceasta
Fereastra creata are chenar, facut de functia box*/
 WINDOW *fereastra_noua(int ianltime, int lungime, int y, int x) {      
    WINDOW  *win; 
    win = newwin(ianltime, lungime, y, x);
    wrefresh(win);
    refresh(); 
    box(win, 0 , 0);         
    wrefresh(win);
    refresh();
    return win;        
}

/*afiseaza cele 3 optiuni din meniu, accentuand cu highlight,
creat prin functia wattron, optiunea aleasa. Dupa ce optiunea
aleasa e afisata, acest atribut ce creeaza efectul de highlight
se opreste cu watroff pentru a nu afecta si celelalte optiuni*/
void afisare_menu(WINDOW *menu, int  indice) {
    if(indice == 1) {
        wattron(menu, A_REVERSE);
        mvwprintw(menu, 2, 2, "New Game"); 
        wrefresh(menu);

        wattroff(menu, A_REVERSE);

        mvwprintw(menu, 4, 2, "Resume Game");
        wrefresh(menu);

        mvwprintw(menu, 6, 2, "Quit");
        wrefresh(menu);
    }
    else {
        if(indice == 2) {
        mvwprintw(menu, 2, 2, "New Game"); 
        wrefresh(menu);

        wattron(menu, A_REVERSE);
        mvwprintw(menu, 4, 2, "Resume Game");
        wrefresh(menu);

        wattroff(menu, A_REVERSE);

        mvwprintw(menu, 6, 2, "Quit");
        wrefresh(menu);   
    } 
        else {
        mvwprintw(menu, 2, 2, "New Game"); 
        wrefresh(menu);

        mvwprintw(menu, 4, 2, "Resume Game");
        wrefresh(menu);

        wattron(menu, A_REVERSE);
        mvwprintw(menu, 6, 2, "Quit");
        wrefresh(menu);  

        wattroff(menu, A_REVERSE);
        }

    }
} 


/*afiseaza data si ora curenta*/
void timp_curent() {
    WINDOW *timp;
    timp = fereastra_noua(5, 25, 15, 35);
    time_t t = time(NULL);
    struct tm time = *localtime(&t);
    mvwprintw(timp,2,2,"%d-%d-%d  %d:%d:%d\n", time.tm_mday, time.tm_mon +1, time.tm_year +1900, time.tm_hour, time.tm_min, time.tm_sec);
    wrefresh(timp);
    refresh();
}

/*numara cate litere sunt afisate initial in mesajul ascuns,
parcurgandu-l si marind variabila corect, de fiecare data cand
un caracter e diferit de spatiu sau '_'*/
int nr_litere_initiale(char ascuns[]) {
	int i,corect=0;
	for(i=0;i<strlen(ascuns);i++)
		if(ascuns[i]!='_' && ascuns[i]!=' ')
			corect++;
		return corect;
}

/*numara cate spatii are mesajul ascuns, primindu-l ca parametru*/
int nr_spatii(char ascuns[]) {
	int s=0,i;
	for(i=0;i<strlen(ascuns);i++)
		if(ascuns[i]==' ')
			s++;
	return s;	
}

/*se creeaza o fereastra in care se printeaza instructiunile jocului*/
void afisare_instructiuni(int nr_expresii, int nr_total_caractere) {
	WINDOW *fereastra_instructiuni;
	fereastra_instructiuni=fereastra_noua(10,42,5,35);
	mvwprintw(fereastra_instructiuni,1,2,"Apasa Q pentru a te intoarce la meniu.");
	mvwprintw(fereastra_instructiuni,2,2,"Apasa orice litera mica sau caracter ");
	mvwprintw(fereastra_instructiuni,3,1,"pentru a gasi expresia ascunsa.");
	mvwprintw(fereastra_instructiuni,4,2,"La sfarsitul jocului, apasa N pentru");
	mvwprintw(fereastra_instructiuni,5,1,"a incepe un nou joc.");
	mvwprintw(fereastra_instructiuni,6,2,"Nr expresii citite: %d", nr_expresii);
	mvwprintw(fereastra_instructiuni,7,2, "Nr caractere printabile: %d", nr_total_caractere);
	wrefresh(fereastra_instructiuni);
}

/*in cazul in care s-a introdus un caracter ce nu faace parte din mesaj,
numarul de greseli creste, se  printeaza spanzuratoare corespunzatoare 
numarului de greseli si se verifica daca jocul trebuie terminat(greseli==6)
Daca jucatorul a pierdut, se afiseaza mesajul corespunzator, iar variabila
sfarsit devine 0*/
void actiune_gresita(WINDOW *spanzuratoare, WINDOW * litere_gresite, int tasta, int *greseli, int *sfarsit, char caractere_gresite[]) {
    wclear(spanzuratoare);
    *greseli=*greseli+1;
    caractere_gresite[strlen(caractere_gresite)]=tasta;	
    if(*greseli==1)
        desen_1(spanzuratoare);
    else 
        if(*greseli == 2)
        	desen_2(spanzuratoare);
        else
        	if(*greseli == 3)
        		desen_3(spanzuratoare);
            else
        		if(*greseli == 4)
        			desen_4(spanzuratoare);
                else
                    if(*greseli == 5)
                        desen_5(spanzuratoare);
                    else {
                        desen_6(spanzuratoare);
                        mvprintw(15,14,"GAME OVER");
                        *sfarsit=0;
        			}
    if(*greseli<=6) {
        mvwprintw(litere_gresite,1,*greseli,"%c ", tasta);
        wrefresh(litere_gresite);
    }
}

/*caracterul se insereaza in mesajul ascuns si se retine numarul de 
caractere noi inserate. Acest numar se adauga la variabila corect care trebuie
la sfarsit, in cazul in care utilizatorul a ghicit toate literele, sa contina
toate caracterele din mesaj. Astfel corect+numarul de spatii din mesaj(spatii),
ca fi egal cu strlen de s cand utilizatorul castiga. Mesajul modificat se afiseaza 
in fereastra_mesaj*/
void actiune_corecta(int tasta, char mesaj[], char ascuns[], int spatii, int *corect, WINDOW *fereastra_mesaj, int *sfarsit) {
	char ch;
	int i,l=strlen(mesaj),litere_noi=0;
	ch=tasta;
    for(i=0;i<l;i++)
        if(mesaj[i]==ch) {
        	ascuns[i]=ch;
        	litere_noi++;
        }
    wclear(fereastra_mesaj);
    fereastra_mesaj=fereastra_noua(3,COLS-2,0,1);
    mvwprintw(fereastra_mesaj,1,1,"%s",ascuns);
	wrefresh(fereastra_mesaj);
	*corect=*corect+litere_noi;
    if(*corect+spatii==strlen(ascuns)) {
        mvprintw(15,14,"AI CASTIGAT!");
        *sfarsit=0;
    }
    refresh();
}

void functia_menu1(cuvant *v, int nr_expresii, int nr_total_caractere);
void functia_menu2(char mesaj[], char caractere_introduse[], int contor, char ascuns[], int greseli, int corect, int spatii, char caractere_gresite[], cuvant *v, int nr_expresii, int nr_total_caractere);

void joc(cuvant *v, int nr_expresii, int nr_total_caractere);


/* aceasta functie se apeleaza in cazul in care utilizatorul a iesit din joc fara sa il termine;
functia primeste ca parametrii mesajul ales, caracterele care au fost deja introduse, impreuna cu
numarul lor( contor ), forma mesajului ascuns din momentul parasirii jocului, numarul de gresli si
care s-au facut pana atunci si numarul de caractere corecte introduse, plus numarul de expresii citite
si numarul de caractere printabile, pentru a putea fi afisate in functia afisare_instructiuni. 
Se creeaza ferestrele pentru spanzuratoare, mesaj_ascuns si litere gresite. Se 
printeaza spanzuratoarea corespunzatoare numrului de greseli si se printeaza mesajul ascuns
in starea in care era inainte de renuntarea la joc. */
void joc_resume(char mesaj[], char caractere_introduse[], int contor, char ascuns[], int greseli, int corect, int spatii, char caractere_gresite[], cuvant *v, int nr_expresii, int nr_total_caractere) {

	int tasta;
	int sfarsit=1;

	WINDOW *fereastra_mesaj;
	fereastra_mesaj=fereastra_noua(3,COLS-2,0,1);
	mvwprintw(fereastra_mesaj,1,1,"%s",ascuns);
	wrefresh(fereastra_mesaj);

	WINDOW *spanzuratoare;
	spanzuratoare=fereastra_noua(18,30,3,1);


	WINDOW *litere_gresite;
	litere_gresite=fereastra_noua(3,15,21,7);
	mvwprintw(litere_gresite,1,1,"%s",caractere_gresite);
	wrefresh(litere_gresite);

	struct pollfd timer = { STDIN_FILENO, POLLIN|POLLPRI };
	afisare_instructiuni(nr_expresii,nr_total_caractere);

	if(greseli==0)
		desen_initial(spanzuratoare);
	else
		if(greseli==1)
        	desen_1(spanzuratoare);
        else 
        	if(greseli == 2)
        		desen_2(spanzuratoare);
        	else
        		if(greseli == 3)
        			desen_3(spanzuratoare);
        		else
        			if(greseli == 4)
        				desen_4(spanzuratoare);
        			else
        				if(greseli == 5)
        					desen_5(spanzuratoare);

	timp_curent();

	if( poll(&timer, 1, 7000) )
        tasta=getch();
    else
        tasta=caracter_random(caractere_introduse);

    while( sfarsit!=0 && tasta != 81 ) {   

    	if( !(strchr(caractere_introduse,tasta)) && isprint(tasta) ) {
        	if(!(strchr(mesaj,tasta)) )	
        		actiune_gresita(spanzuratoare,litere_gresite, tasta, &greseli, &sfarsit,caractere_gresite);
        	else
        		if( greseli<6)
        			actiune_corecta(tasta, mesaj, ascuns, spatii, &corect, fereastra_mesaj, &sfarsit);
        
        caractere_introduse[contor]=tasta;
        contor++;

        }	

        timp_curent();
        if( poll(&timer, 1, 7000) )
        	tasta=getch();
   		else
        	tasta=caracter_random(caractere_introduse);
    }

	if(tasta == 81) {
    	if(sfarsit == 0) {
    		clear();
    		functia_menu1(v,nr_expresii,nr_total_caractere);
    	}
    	else {
    		clear();
    		functia_menu2(mesaj,caractere_introduse, contor, ascuns, greseli, corect, spatii,caractere_gresite,v,nr_expresii,nr_total_caractere);
    	}
    	
    }
    else
    	if(tasta == 78) {
    		clear();
    		joc(v,nr_expresii,nr_total_caractere);
    	}
   		else 
   			while(tasta != 81 || tasta !=78) {
    			tasta=getch();
   				if(tasta == 81) {
    				if(sfarsit == 0) {
    					clear();
    					functia_menu1(v,nr_expresii,nr_total_caractere);
    					break;
    				}
    				else {
    					clear();
    					functia_menu2(mesaj,caractere_introduse, contor, ascuns, greseli, corect,spatii,caractere_gresite,v,nr_expresii,nr_total_caractere);
    					break;
    				}	
    			}
   				else
                    if(tasta == 78) {
    				clear();
    				joc(v,nr_expresii,nr_total_caractere);
    				break;
    			}
    		}
}

/* primeste ca si variabile, vectorul in care sunt stocate toate
expresiile, numarul total de expresii si numarul de caractere printabile.
La inceput, in mesaj, este stocata o expresie aleatoare, aleasa, prin
functia expresie_aleasa, care intoare un numar aleatoriu, corespunzator cu
nnumarul expresiei alese; se formeaza mesajul ascuns prin functiile
retinere_caractere si creare_mesaj_ascuns; se afiseaa, desenul initial
al spanzuratorii si timpul curent; Se preia de la tastatura un caracter,
iar functia intra in while pana cand utilizatoru vrea sa se intoarca la 
meniu cu Q sau termina jocul. In cazul in care tasta apasata e
caracter printabil si nu se gaseste in caracterele deja introduse, atunci
se verifica, daca ea e gaseste sau nu in mesaj, pentru a sti ce actiune functie
trebuie apelata. La final, se pot alege dintre N si Q pentru un nou joc sau meniu*/
void joc(cuvant *v, int nr_expresii, int nr_total_caractere) {

	int h=expresie_aleasa(nr_expresii);
	char mesaj[380]="\0";
	strcpy(mesaj,v[h].a);

	char caractere_initiale[50]="\0";
	char ascuns[380]="\0";
	char caractere_gresite[10]="\0";
	retinere_caractere(mesaj,caractere_initiale);
	creare_mesaj_ascuns(mesaj,ascuns,caractere_initiale);

	int tasta,greseli=0,corect=nr_litere_initiale(ascuns);
	int spatii=nr_spatii(ascuns);

	char caractere_introduse[380]="\0";
	int contor;
	int sfarsit=1;

	strcpy(caractere_introduse,caractere_initiale);
	caractere_introduse[strlen(caractere_introduse)]=' ';
	contor=strlen(caractere_introduse);

	WINDOW *fereastra_mesaj;
	fereastra_mesaj=fereastra_noua(3,COLS-2,0,1);
	mvwprintw(fereastra_mesaj,1,1,"%s",ascuns);
	wrefresh(fereastra_mesaj);


	WINDOW *spanzuratoare;
	spanzuratoare=fereastra_noua(18,30,3,1);

	WINDOW *litere_gresite;
	litere_gresite=fereastra_noua(3,15,21,7);

	struct pollfd timer = { STDIN_FILENO, POLLIN|POLLPRI };
	afisare_instructiuni(nr_expresii,nr_total_caractere);
	desen_initial(spanzuratoare);
	timp_curent();

    if( poll(&timer, 1, 7000) )
        tasta=getch(); 
    else
        tasta=caracter_random(caractere_introduse);

    while( sfarsit!=0 && tasta != 81 ) {   
    	if( !(strchr(caractere_introduse,tasta)) && isprint(tasta) ) {
        	if(!(strchr(mesaj,tasta)) )
        		actiune_gresita(spanzuratoare,litere_gresite, tasta, &greseli, &sfarsit,caractere_gresite);	
        	else
        		if( greseli<6)
        			actiune_corecta(tasta, mesaj, ascuns, spatii, &corect, fereastra_mesaj, &sfarsit);
        
        caractere_introduse[contor]=tasta;
        contor++;
        }	
        timp_curent();
        if( poll(&timer, 1, 7000) )
        	tasta=getch();
   			else
        	tasta=caracter_random(caractere_introduse);

    }

	if(tasta == 81) {
    	if(sfarsit == 0) {
    		clear();
    		functia_menu1(v,nr_expresii,nr_total_caractere);
    	}
    	else {
    		clear();
    		functia_menu2(mesaj,caractere_introduse, contor, ascuns, greseli, corect, spatii,caractere_gresite,v,nr_expresii,nr_total_caractere);
    	}
    }
    else
    	if(tasta == 78) {
    		clear();
    		joc(v,nr_expresii,nr_total_caractere);
    	}
   		else 
   			while(tasta != 81 || tasta !=78) {
    			tasta=getch();
   				if(tasta == 81) {
    				if(sfarsit == 0) {
    					clear();
    					functia_menu1(v,nr_expresii,nr_total_caractere);
    					break;
    				}
    				else {
    					clear();
    					functia_menu2(mesaj,caractere_introduse, contor, ascuns, greseli, corect, spatii,caractere_gresite,v,nr_expresii, nr_total_caractere);
    					break;
    				}
    			}
   				else
                    if(tasta == 78) {
    				clear();
    				joc(v,nr_expresii,nr_total_caractere);
    				break;
    			}
    		}
}

/* aceasta functie, creaza fereastra pentru meniu. Prin variabila indice,
se arata care dintre cele 3 optiuni este aleasa, pentru a putea fi indicata
atunci cand functiai afisare_meniu printeaza cele 3 optiuni; Este o bucla 
infinita, intrerupta doar daca se apasa enter ( cod ASCII 10). Daca optiunea
din final este 1, atunci sse incepe un joc nou, daca este Quit, atunci
se apeleaza endwin(), pentru inchiderea aplicatiei. Functia primeste variabilele
v, nr_expresii si nr_total_caractere, pentru a le pasa functiei joc*/
void functia_menu1(cuvant *v, int nr_expresii, int nr_total_caractere) {
	WINDOW  *menu;       
    int mijlocx, mijlocy; 
    int indice=1, intrare;

    mijlocy = (LINES - INALTIME_FEREASTRA) / 2;
    mijlocx = (COLS - LATIME_FEREASTRA) / 2;   

             
    menu = fereastra_noua(INALTIME_FEREASTRA, LATIME_FEREASTRA, mijlocy, mijlocx); 
    refresh();
    afisare_menu(menu,indice);    
    while(1) {    
    	intrare=getch();
        if(intrare == KEY_UP) {
            if(indice==1)
                indice=3;
            else indice=1;
        }
        else {
            if(intrare == KEY_DOWN) {
                if(indice==3)
                    indice = 1;
                else indice=3;              
            }
            else
                if(intrare == 10)
                    break;
        }
         
    afisare_menu(menu, indice);
    }
    if (indice == 1) {       
    	clear();
        joc(v,nr_expresii, nr_total_caractere);
        endwin();
    }
    else
      endwin();
} 

/*Variabilele pasate sunt pentru a le trasnmite functiei joc_resume.
Functia e la fel ca functia_menu1, dar permite accesarea optiunii Resume*/
void functia_menu2(char mesaj[], char caractere_introduse[], int contor, char ascuns[], int greseli, int corect, int spatii, char caractere_gresite[], cuvant *v, int nr_expresii, int nr_total_caractere) {
	WINDOW  *menu;       
    int mijlocx, mijlocy; 
    int indice=1, intrare;

    mijlocy = (LINES - INALTIME_FEREASTRA) / 2;
    mijlocx = (COLS - LATIME_FEREASTRA) / 2;   
          
    menu = fereastra_noua(INALTIME_FEREASTRA, LATIME_FEREASTRA, mijlocy, mijlocx); 
    refresh();      
    afisare_menu(menu,indice);
    while(1) {    
    	intrare=getch();
        if(intrare == KEY_UP) {
            if(indice==1)
                indice=3;
            else indice=indice-1;
        }
        else {
            if(intrare == KEY_DOWN) {
                if(indice==3)
                    indice = 1;
                else indice=indice+1;                    
            }
            else
                if(intrare == 10)
                    break;  
        }
         
    afisare_menu(menu, indice);    
    }
    if (indice == 1) {       
    	clear();
        joc(v,nr_expresii,nr_total_caractere);
        endwin();
    }
    else
        if( indice == 2) {
            clear();
            joc_resume(mesaj,caractere_introduse, contor, ascuns, greseli, corect, spatii,caractere_gresite,v,nr_expresii,nr_total_caractere);
        }   
        else
        	endwin();      
} 

int main(int argc, char const *argv[]) {	
    cuvant *v;
	int i,j=0;
	int nr_total_caractere=0;
	int spatiu;
	spatiu=LUNG_INIT;
	int nr_curent;
    char s[180];
    v=(cuvant *)malloc(spatiu*sizeof(cuvant));
    FILE *f;
    if(argc == 1) {
    	printf("[Eroare]: Nu s-au dat argumente de comanda\n" );
    	return 1;
    }
    else
    	for(i=1;i<argc; i++) { 
 			if((f=fopen(argv[i], "r")) == NULL) {
 				printf("[Eroare]: Fisieul %s nu poate fi deschis\n", argv[i]);
 				return 1;
 			}
 			while(fgets(s,180,f) != NULL ) {
                if(nr_curent==spatiu) {
 						spatiu=spatiu*2;
 						v=(cuvant *)realloc(v, spatiu*sizeof(cuvant));
 					}
 			    litere_mici(s);
 			    stergere_caracter_neprintabil(s);
 			    nr_total_caractere=nr_total_caractere+strlen(s);
 			    if(s[0]!='\0') {
			    	strcpy(v[j].a,s);
			    	j++;
			    	nr_curent=j;
			    }
 				}
 			fclose(f);
    	}
	initscr();
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);
	functia_menu1(v,j, nr_total_caractere);
	endwin();
    free(v);
	return 0;

}