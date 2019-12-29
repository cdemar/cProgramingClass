//
//  main.c
//  FirstLabInC
//
//  Created by Cory DeMar on 1/30/19.
//  Copyright © 2019 Cory DeMar. All rights reserved.
//


#include <stdio.h>
int main() {
    FILE *fp;
    //fopen() function is used to open a file named example.txt in read mode
    fp= fopen ("helloWorld.txt", "w");
    
    fprintf(fp, "Hello World!");
    
    //fclose() is used to close the file
    fclose(fp);
    return 0;
}

