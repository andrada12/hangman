hangman: hangman.c ascuns.c desene.c
	gcc hangman.c desene.c ascuns.c -lncurses -o hangman -Wall


build:  hangman



clean: 
	rm -f hangman