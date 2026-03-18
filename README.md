# Wordle clone in C + Raylib (Functional but WIP)   

Deps: raylib  
replace ``#include <RLWindow.h>`` with ``#include "./lib/RLWindow.h"`` if you don't have the custom raylib window header  

Uses the Linux ``/etc/dictionaries-common/words`` file for the random 5 letter word, which means it works offline, but only on linux hehe

# TODO  
- Better UI (I only focused on functionality first)  
- Filter out super weird/uncommon words (basically the ones that are invalid in the real Wordle)  
- Sound effects and all that jazz
