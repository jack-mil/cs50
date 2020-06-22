// Prints a pyramid

#include <cs50.h>
#include <stdio.h>

// Helper functions
int get_int_in_range(string, int, int); 
void print_n_blocks(int); 

int main(void)
{
    int height = get_int_in_range("Height: ", 1, 8); // Get a height from the user in a certain range

    for (int row = 1; row <= height; row++) // Print the specified number of rows
    {
        for (int j = 0; j < height - row ; j++) // Print padding per row to center the pyramid
        {
            printf(" ");
        }

        print_n_blocks(row);

        printf("  ");

        print_n_blocks(row);

        printf("\n");
    }
}

void print_n_blocks(int n)
{
    for (int i = 0; i < n; i++)
    {
        printf("#");
    }
}

int get_int_in_range(string prompt, int min, int max)
{
    int n;
    do 
    {
        n = get_int("%s", prompt);
    } 
    while (n < min || n > max);
    return n;
}
