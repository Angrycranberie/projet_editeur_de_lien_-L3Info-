#include "fichA.h"
#include <stdio.h>

void affichage_hello(){
    printf("Hello word");
}

void suite_syracuse(int n){
    while(n != 1){
        print(n);
        if(n%2 == 0){
            n = n/2;
        }
        else{
            n = n * 3 + 1;
        }
    }
}