#Kernel-Keylogger

Acest modul este un driver de tastatură Linux care poate captura caracterele tastate și le poate expune prin intermediul unui fișier special (myDevice in directorul dev) în sistemul de fișiere. Modulul definește o structură numită "kbd" pentru a stoca informațiile despre caracterele tastate și un spinlock pentru a proteja accesul la această structură în timpul manipulării. Modulul definește, de asemenea, constantele pentru numele modulului, major-ul și minor-ul numărului de dispozitiv, numărul de dispozitive și adresele de înregistrare pentru tastatură. Acesta include si o funcție numită "is_key_press" care verifică dacă un cod de scanare corespunde unei apăsări sau eliberări a tastei și o funcție numită "get_ascii" care returnează caracterul corespunzător codului de scanare. Funcțiile kbd_init și kbd_exit sunt utilizate pentru a iniția și a elimina modulul din kernel. Funcția kbd_open initializeaza structura kbd.

Au fost incarcate si variantele de lucru pentru partea care trebuia sa includa timpul apasarii fiecarei taste si interfata in python unde sa se afiseze acestea. Interfata se bazeaza pe preluarea datelor de la nivelul unor atribute construite in partea de modul, care fac posibila citirea informatiilor de la nivelul kernel space in user space.

Modulul ar fi trebuit sa captureze codul scanat al tastei apăsate și îl mapează la un caracter, apoi stochează caracterul și timestamp-ul într-un buffer. Acest buffer poate fi citit de aplicații spațiu de utilizator. Driver-ul folosește funcția ktime_get_real_ns() pentru a obține timestamp-ul curent în nanosecunde. Partea cu timestamp ul nu am dus-o la capăt, dar toate celelalte funcționalități le-am implementat cu succes.(capturearea caracterelor apăsate, stocarea acestora într-un buffer, crearea unui fisier in /dev, scrierea acestora în fișier).

Rezultatele se pot vedea în urma inserării modulului de kernel (make keylogger.c; sudo insmod kbd.ko), scriind comanda sudo cat /dev/myDevice. Modulul se dezactivează prin comanda sudo rmmod keylogger.ko.
Problemele întâmpinate

    crearea fisierului din /dev
    gasirea functiei potrivite pentru a obtine timpul apasarii unei taste (unele functii nu erau compatibile cu versiunea de kernel)
    gasirea scancode-urilor pentru 95% din taste care sa fie compatbilie cu versiunea de kernel
    dificultati la debug
