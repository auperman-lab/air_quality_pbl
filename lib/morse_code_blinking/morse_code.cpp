#include "morse_code.h"

Morse_Code::Morse_Code(Led led) : led(led) {}

void Morse_Code::printMorseSymbol(const char *symbol) {
    while (*symbol) {
        led.on();
        if (*symbol == '.') {
            delay(dotDuration);
        } else if (*symbol == '-') {
            delay(dashDuration);
        }
        led.off();
        delay(pauseInLetter);
        symbol++;
    }
}

void Morse_Code::printLetter(char letter) {
    switch (tolower(letter)) {
        case 'a': printMorseSymbol(".-"); break;
        case 'b': printMorseSymbol("-..."); break;
        case 'c': printMorseSymbol("-.-."); break;
        case 'd': printMorseSymbol("-.."); break;
        case 'e': printMorseSymbol("."); break;
        case 'f': printMorseSymbol("..-."); break;
        case 'g': printMorseSymbol("--."); break;
        case 'h': printMorseSymbol("...."); break;
        case 'i': printMorseSymbol(".."); break;
        case 'j': printMorseSymbol(".---"); break;
        case 'k': printMorseSymbol("-.-"); break;
        case 'l': printMorseSymbol(".-.."); break;
        case 'm': printMorseSymbol("--"); break;
        case 'n': printMorseSymbol("-."); break;
        case 'o': printMorseSymbol("---"); break;
        case 'p': printMorseSymbol(".--."); break;
        case 'q': printMorseSymbol("--.-"); break;
        case 'r': printMorseSymbol(".-."); break;
        case 's': printMorseSymbol("..."); break;
        case 't': printMorseSymbol("-"); break;
        case 'u': printMorseSymbol("..-"); break;
        case 'v': printMorseSymbol("...-"); break;
        case 'w': printMorseSymbol(".--"); break;
        case 'x': printMorseSymbol("-..-"); break;
        case 'y': printMorseSymbol("-.--"); break;
        case 'z': printMorseSymbol("--.."); break;
        case ' ': delay(pauseInterWords); return;
        default: return; 
    }
    delay(pauseInterLetter); 
}

void Morse_Code::print(char *text) {
    while (*text) {
        printLetter(*text);
        text++;
    }
}
