"Copyright [2018] Gavan Adrian-George, 334CA"

Nume, prenume: Gavan, Adrian-George

Grupa, seria: 334CA

Tema 1 APD - Antialiasing si micro renderer

Prezentarea implementarii:
==========================

Task 1:
=========

A. homework.c
==============

a. Pasi urmati + eficienta:
- Se initializeaza o variabila de tipul "image" cu datele imaginii de intrare.
- Se apeleaza functia resize, care in functie de tipul imaginii si factorul
de scalare calculeaza valorile pixelilor de iesire.
- Eficienta vine din faptul ca s-a paralelizat calcularea pixelilor imaginii 
finale. Fiecare thread calculeaza valorile tuturor pixelilor de pe anumite linii.
Acesta se foloseste de pixelii din matricea initiala, deci nu avem nici probleme 
de concurenta, fiecare thread calculand valorile pentru pixeli diferiti.
- Se apeleaza functia writeData, care in functie de tipul imaginii scrie datele
in fisierul de iesire.

b. Functii utilizate:
1. readInput:
- Retine mai intai tipul imaginii, lungimea, latimea si valoarea maxima.
- Daca este imagine alb-negru, se aloca memorie variabilei img->pixels_bw,
matrice ce va contine valorile pixelilor din matricea de intrare.
- Daca este imagine color, se aloca memorie variabilei img->pixels, matrice
ce va contine valorile pixelilor din matricea de intrare (fiecare element al
matricei este o structura cu 3 variabile, pentru a retine cele 3 culori ale
unui pixel).

2. free_memory_BW si free_memory_Color:
- Functii care elibereaza memoria in functie de tipul imaginii.

3. writeData:
- Deschide fisierul de iesire si scrie mai intai tipul imaginii, latimea,
lungimea si valoarea maxima.
- Daca imaginea este alb-negru apeleaza functia writeDataBW.
- Daca imaginea este color apeleaza functia writeDataColor.
- Dupa ce datele sunt scrise in fisier, se apeleaza una din functiile de
eliberare a memoriei (pentru imaginea de intrare).

4. writeDataBw si writeDataColor:
- Scriu fiecare linie din matricea pixelilor imaginii de iesire in fisierul de
iesire.

5. resize:
- Imaginea de iesire retine valorile pentru tip, latime, lungime, valoare maxima.
- In functie de tipul imaginii de intrare se aloca memorie pentru matricea
pixelilor a imaginii de iesire.
- In functie de resize_factor se apeleaza resizeODD (par) sau resizeEVEN (=3).
- Dupa ce s-au calculat valorile pentru imaginea de iesire, se eliereaza memoria
pentru imaginea de intrare.

6. resizeODD si resizeEVEN:
- Ambele functii deschid thread-uri care utilizeaza anumite functii pentru resize.
- Fiecare thread primeste ca parametru o structura "info", cu id-ul thread-ului,
pointer catre imaginea de intrare si catre imaginea de iesire.

7. resizeODD_BW si resizeODD_Color:
- Calculeaza indicii de start si final ai thread-ului.
- Pentru fiecare linie a matricei pixelilor imaginii de iesire din intervalul
start - end (interval diferit pentru fiecare thread) se parcurg elementele si se
calculeaza valoarea corespunzatoare fiecaruia.
- Pentru fiecare pixel al imaginii de iesire se calculeaza media aritmetica a
resize_factor^2 pixeli din poza origina. Asta inseamna ca se utilizeaza 2 for-uri,
primul ducandu-se pe resize_factor linii si al doilea pe resize_factor coloane,
ca sa se ia cei resize_factor^2 pixeli din matricea initiala.
- Se aduna valorile pixelilor initiali, iar apoi suma se imparte la resize_factor^2
si se atribuie pixelului curent din imaginea de iesire.
- Pentru resizeODD_Color se intampla acelasi lucru, doar ca pentru fiecare culoare
in parte.

8. resizeEVEN_BW si resizeEVEN_Color:
- Se intampla aceleasi lucruri ca la resizeODD_BW si resizeODD_Color, doar ca
fiecare valoare a pixelului initial este inmultita cu elementul corespunzator din
Kernel-ul Gaussian, iar rezultatul se imparte la 16.

B. homework.h
==============

- Header-ul contine 3 structuri:
1. rgb:
- 3 variabile unsigned char pentru a retine valorile unui pixel color.

2. image:
- Descrie structura unei imagini.
- Contine tipul imaginii, latimea, lungimea si valoarea maxima.
- Contine matricea cu valorile pixelilor cand imaginea este alb-negru.
- Contine matricea cu valorile pixelilor cand imaginea este color.

3. info:
- Contine informatiile necesare unui thread.
- Contine id-ul threadului si pointeri catre imaginile de intrare si iesire.

Task 2:
=========

A. homework1.c
==============

a. Pasi urmati + eficienta:
- Se initializeaza imaginea: tipul imaginii, latimea, lungimea, valoarea maxima
si se aloca memoria necesara pentru matricea pixelilor.
- Se apeleaza functia render, unde se calculeaza valoarea fiecarui pixel din
matricea pixelilor.
- Eficienta vine din faptul ca s-a paralelizat calcularea pixelilor imaginii 
finale. Fiecare thread calculeaza valorile tuturor pixelilor de pe anumite linii.
Fiecare thread calculeaza valorile pentru pixeli diferiti, deci nu avem probleme 
de concurenta.
- Se apeleaza functia writeData pentru a scrie datele in fisierul de iesire.

b. Functii utilizate:
1. initialize:
- Pune in structura imaginii valorile corespunzatoare pentru tipul imaginii,
latime, lungime si valoare maxima.
- Aloca dinamic memorie pentru matricea pixelilor.

2. render:
- Creeaza un vector cu elemente de tipul "info" si de dimensiune egala cu numarul
de thread-uri.
- Fiecare element primeste un un numar intre 0-numarul de threaduri (va fi
id-ul thread-ului) si imaginea.
- Se creeaza thread-urile, fiecare thread utilizand functia "render_thread" si
primind ca parametru un element de tipul info.
- La final apelam join pentru a fi siguri ca se incheie executia tuturor
thread-urilor.

3. render_thread:
- Din variabila de tip "info" primita ca parametru se iau informatiile necesare
(id-ul thread-ului si se initializeaza un pointer catre imagine).
- Se calculeaza indicii de inceput si final pentru fiecare thread (conform
formulelor de la laborator).
- Pentru fiecare linie a matricei pixelilor din intervalul start - end (interval
diferit pentru fiecare thread) se parcurg elementele si se calculeaza valoarea
corespunzatoare fiecaruia.
- Mai intai se calculeaza centrul pixelului (tinandu-se cont de faptul ca
coordonatele x = 0 si y = 0 sunt in coltul din stanga jos a imaginii), iar apoi
se aplica formula pentru aflarea distantei de la un punct la o dreapta. Daca
distanta este <= 3, atunci pixelul va lua valoarea 0 (negru). Altfel, va lua
valoarea 255 (alb).
- Valoare fiecarui "x" si "y" din formula este scalata cu "dimensiune spatiu /
resolution", pentru ca in functie de rezolutie pot fi mai multi pixeli pe cm^2.

4. writeData:
- Se deschide fisierul si se scriu informatiile legate de tipul, lungimea, latimea
si valoarea maxima a imaginii.
- Valorile pixelilor se scriu in fisier pe o singura linie, asa ca in fisier se
scrie pe rand fiecare linie din matricea pixelilor.
- Se elibereaza memoria si se inchide fisierul.

B. homework1.h
==============

- Header-ul contine 2 structuri:
1. image:
- Descrie structura unei imagini.
- Contine tipul imaginii, latimea, lungimea si valoarea maxima.
- De asemenea, contine si matricea cu valorile pixelilor.

2. info:
- Contine informatiile necesare unui thread.
- Contine id-ul threadului si un pointer catre imagine.

- Scalabilitate task-uri:
=========================

A. Task 1
a. Incolor (imaginea lenna_bw.pgm):
1. Factor resize 2:
- 1 thread:	0.003776
- 2 thread-uri: 0.002222
- 4 thread-uri: 0.001829
- 8 thread-uri: 0.000927

2. Factor resize 3:
- 1 thread:	0.003963
- 2 thread-uri: 0.002049
- 4 thread-uri: 0.001142
- 8 thread-uri: 0.000848

3. Factor resize 8:
- 1 thread:	0.002345
- 2 thread-uri: 0.001475
- 4 thread-uri: 0.000894
- 8 thread-uri: 0.000657

b. Color (imaginea lenna_color.pnm):
1. Factor resize 2:
- 1 thread:     0.005964
- 2 thread-uri: 0.003277
- 4 thread-uri: 0.001752
- 8 thread-uri: 0.001106

2. Factor resize 3:
- 1 thread:	0.011969
- 2 thread-uri:	0.006135
- 4 thread-uri: 0.003065
- 8 thread-uri: 0.002565

3. Factor resize 8:
- 1 thread:	0.004698
- 2 thread-uri:	0.002546
- 4 thread-uri:	0.001345
- 8 thread-uri: 0.001414

B. Task 2
1. Pentru 1000:
- 1 thread:	0.018376
- 2 thread-uri:	0.009608
- 4 thread-uri:	0.004327
- 8 thread-uri: 0.002349

2. Pentru 8000:
- 1 thread:	0.695682
- 2 thread-uri:	0.371242
- 4 thread-uri:	0.164214
- 8 thread-uri: 0.082215
