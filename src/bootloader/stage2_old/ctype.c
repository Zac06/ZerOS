#include "ctype.h"

bool isupper(char c){
    return c>='A' && c<='Z';
}

bool islower(char c){
    return c>='a' && c<='z';
}

char toupper(char c){
    if(islower(c)){
        return c-'a'+'A';
    }

    return c;
}

char tolower(char c){
    if(isupper(c)){
        return c-'A'+'a';
    }

    return c;
}
