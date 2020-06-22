#include <stdio.h>
#include <cs50.h>
#include <string.h>
#include <ctype.h>

bool iskey(string);
bool isalpha_s(string);
bool contains26(string);
string encrypt(string, string);

int main(int argc, string argv[])
{
    if(argc != 2)
    {
        printf("ERROR: Accepts ONE command line argument, KEY\nUsage: ./substitution KEY\n");
        return 1;
    }

    string key = argv[1];
    if(!iskey(key))
    {
        printf("Keys must contain all 26 alphabetic characters\n");
        return 1;
    }

    string plaintext = get_string("plaintext: ");
    printf("ciphertext: %s\n", encrypt(plaintext, key));

    return 0;
}

string encrypt(string s, string key)
{
    for (int i = 0, n = strlen(s); i < n; i++)
    {
        if(isalpha(s[i]))
        {
            if(isupper(s[i]))
            {
                s[i] = toupper(key[s[i] - 'A']);
            }
            else
            {
                s[i] = tolower(key[s[i] - 'a']);
            }
        }
    }
    return s;
}

bool iskey(string key)
{
    bool check[26] = {0};

    if(strlen(key) != 26 || !isalpha_s(key))
    {
        return false;
    }

    for(int i = 0; i < 26; i++)
    {
        if(isupper(key[i]))
        {
            check[key[i] - 'A']++;
        }
        else
        {
            check[key[i] - 'a']++;
        }
    }

    // Check if any letter is missing and return false. (Some letter is not accouted for)
    for(int i = 0; i < 26; i++)
    {
        if(check[i])
        {
            return false;
        }
    }

    // Passed all the tests, key checks out.
    return true;
}

bool isalpha_s(string s)
{
    for (int i = 0, n = strlen(s); i < n; i++)
    {
        if(!isalpha(s[i]))
            return false;
    }
    return true;
}