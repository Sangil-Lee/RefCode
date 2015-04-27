#include <stdio.h>
#include <stdlib.h>


static unsigned int data1;
static unsigned int data2;

void main(void)
{
    FILE *file;

    file = fopen("test.dat", "w");

    data1 = 0x12345678;
    fwrite((void*) &data1,sizeof(unsigned int), 1, file);
    fclose(file);


    file = fopen("test.dat", "r");
    fread((void*) &data2, sizeof(unsigned int), 1, file);
    fclose(file);


    printf("data1: 0x%12x, data2: 0x%12x\n", data1, data2);

    return;

}
