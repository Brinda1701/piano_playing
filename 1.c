#include<stdio.h>
#include<windows.h>

#define C4 262
#define D4 294
#define E4 330
#define F4 349
#define G4 392
#define A4 440
#define B4 494
#define C5 523
#define REST 0
void play_note(char c); 
int main(int argc, char const *argv[])
{
    char note;
    while (1)
    {
        note=getchar();
        if (note=='Q')
        {
            break;
        }
        
        play_note(note);
    }
    
    return 0;
}
void play_note(char c)
{
    switch (c)
    {
    case 'S':
        Beep(C4,500);
        break;
    case 'D':
        Beep(D4,500);
        break;
    case 'F':
        Beep(E4,500);
        break;
    case 'G':
        Beep(F4,500);
        break;
    case 'H':
        Beep(G4,500);
        break;
    case 'J':
        Beep(A4,500);
        break;
    case 'K':
        Beep(B4,500);
        break;
    case 'L':
        Beep(C5,500);
        break;
    default:break;
    }
}