#include <stdlib.h>
#include <stdio.h>

int main() {
    int *ptr = malloc(10 * sizeof(int));
    
    if (ptr == NULL) return 1;

    printf("Mémoire allouée... mais je ne vais pas la libérer !\n");
    
    return 0;
}
