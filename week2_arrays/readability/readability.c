#include <stdio.h>
#include <cs50.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

void analyze(string, int[]);
bool isend(char);

int main(void)
{
    int stats[3] = {0, 0, 0};
    string in = get_string("Text: ");
    analyze(in, stats);
    // printf("%i letter(s)\n", stats[0]);
    // printf("%i word(s)\n", stats[1]);
    // printf("%i sentences(s)\n", stats[2]);
    float L = (float) stats[0] / stats[1] * 100;
    float S = (float) stats[2] / stats[1] * 100;
    // printf("L: %f, S: %f\n", L, S);
    int index = roundf(0.0588 * L - 0.296 * S - 15.8);

    if (index >= 16)
    {
        printf("Grade 16+");
    }
    else if (index < 1)
    {
        printf("Before Grade 1");
    }
    else
    {
        printf("Grade %i", index);
    }
    printf("\n");
}

void analyze(string s, int results[])
{
    results[1] = 1;
    for(int i = 0, n = strlen(s); i < n; i++)
    {
        if (isalpha(s[i]))
            results[0]++;
        if (isspace(s[i]))
            results[1]++;
        if (isend(s[i]))
            results[2]++;
    }
    return;
}

bool isend(char c)
{
    return (c == '.' || c == '!' || c == '?');
}
