#include <stdio.h>
#include <cs50.h>

int digits(long number); // How many digits a number has
long pow_ten(int n); // 10 raised to the n power
int first_digits(long num, int n); // n digits from the left
int nth_digit(long num, int n); // one digit indexed from least significant digit

bool valid_checksum(long card); // Validate checksum
string type(long card); // Classify card

int main(void)
{
    long card = get_long("Number: "); //Get user input
    if (valid_checksum(card)) //Validate checksum
    {
        printf("%s\n", type(card)); //Print type of card if valid
    }
    else
    {
        printf("INVALID\n");
    }
}

bool valid_checksum(long card)
{   
    int len = digits(card);
    int sum = 0;
    for (int i = 1; i < len; i += 2) //Get odd numbered digits starting at index 0 (ones place)
    {
        int k = 2 * nth_digit(card, i); //Multiply each odd digit by 2
        sum += nth_digit(k, 0) + nth_digit(k, 1); //Sum of the digits of the products (hacky I know)
    } 
    for (int i = 0; i < len; i += 2) //Get sum of even numbered digits starting at index 0 (ones place)
    {
        sum += nth_digit(card, i); //Add that sum to previous sum
    }

    // printf("%i",sum);
    if (sum % 10 == 0) //If the checksum ends in 0, the card is valid
    {
        return true;
    }
    else
    {
        return false;
    }
}


string type(long card) //Returns the type of card as a string. "INVALID\n" if format not recognized
{
    //Cache values
    int first2 = first_digits(card, 2);
    int first = first_digits(first2, 1);
    int len = digits(card);

    if (len == 15 && (first2 == 34 || first2 == 37)) //Check card according to specifications
    {
        return "AMEX";
    }
    else if ((len == 13 || len == 16) && first == 4)
    {
        return "VISA";
    }
    else if (len == 16 && (first2 >= 51 && first2 <= 55))
    {
        return "MASTERCARD";
    }
    else
    {
        return "INVALID"; //If none match, card is invalid
    }
}




int digits(long num) //Returns how many digits a number has
{
    int count = 0;
    while (num != 0)
    {
        num /= 10;
        count++;
    }
    return count;
}

long pow_ten(int n) //Returns 10 raised to the n power
{
    long result = 1;
    for (int i = 0; i < n; i++)
    {
        result *= 10;
    }
    return result;
}

// ORIG
// int first_digits(long num, int n) //Returns the first n digits of number
// {
//     int len = digits(num);
//     long place = pow_ten(len - n);
//     return (num - (num % place)) / place;
// }

// IMPROVED
//Returns the first n digits of number
int first_digits(long num, int n)
{
    while(num >= pow_10(n))
    {
        num /= 10;
    }
    return num;
}

int nth_digit(long num, int n) //Returns the nth digit of a number starting from the end = 0
{
    return (num / pow_ten(n)) % 10;
    // 1234 / 1 = 1234 -> mod 10 = 4
    // 1234 / 10 = 123 -> mod 10 = 3
    // 1234 / 100 = 12 -> mod 10 = 2
    // 1234 / 1000 = 1 -> mod 10 = 1
}


