//
//  main.c
//  Homework2
//
//  Created by Cory DeMar on 5/5/19.
//  Copyright © 2019 Cory DeMar. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

void function1(unsigned int n, int range);
void function2(unsigned int n, int range);
void function3(unsigned int n, int range);
void printMain(void);

int main()
{
    void(*array[])(unsigned int n, int range) =
    {
        function1, function2, function3
        
    };
    unsigned choice = 0;
    int range = 0;
    while(choice != 4)
    {
        do
        {
            printMain();
            scanf("%u", &choice);
            printf("Enter your range: ");
            scanf("%d", &range);
            
            if (choice != 4)
                array[choice](choice, range);
            
        }while(choice <0 || choice >4);
        
        return 0;
    }
}

void function1(unsigned int n, int range)
{
    int num = n + 201;
    n = range % num + 1;
    printf("%d\n", n-201);
}

void function2(unsigned int n, int range)
{
    int num = n + 201;
    printf("%d\n", num);
}

void function3(unsigned int n, int range)
{
    long fact = 1;
    int num = n + 201;
    for (int i=0; i<=num; i++)
        fact *= i;
    printf("The factorial of your numer is %ld\n", fact);
}

void printMain()
{
    printf("%s", "Enter choice\n");
    printf("0 = Random number\n");
    printf("1 = Random number in range of -201 to your number\n");
    printf("2 = Number of digits\n");
    printf("3 = The factorial of the number\n");
    printf("4 = End program\n");
}
