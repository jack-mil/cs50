// Implements a dictionary's functionality using a hash table technique

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>

#include "dictionary.h"

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
}
node;

// Number of nodes in hash table
// 143,091 words in large dictionary. Using 2^20
const unsigned int N = 1048576;

// Hash table; array of pointers to nodes
node *table[N];

unsigned int wordcount = 0;

// Returns true if word is in dictionary else false
bool check(const char *word)
{
    // Hash word and find lookup table entry
    unsigned int n = hash(word);

    // Traverse linked list at hashed index
    for (node *i = table[n]; i != NULL; i = i->next)
    {
        // Word is in hash table
        if (strcasecmp((i->word), word) == 0)
        {
            return true;
        }
    }

    // Word not found
    return false;
}

unsigned int hash(const char *word)
{

    /* djb2 by Dan Bernstein
    http://www.cse.yorku.ca/~oz/hash.html

    32,700 collisions with hash size 2^18
    load time: 0.02814
    */
    unsigned long hash = 5381UL;
    int c;

    while ((c = *word++))
    {
        hash = ((hash << 5) + hash) + tolower(c); // hash * 33 + c
    }
    return hash % N;

    /* Murmurs one-byte-at-a-time hash for 32 bit integers
    https://stackoverflow.com/questions/7666509/hash-function-for-string#answer-7666799

    32,779 with hash size 2^18
    load time: 0.03079
    */
    // const char *owrd = word;
    // uint32_t h = 3323198485UL;
    // for (;*word;++word)
    // {
    //     h ^= tolower(*word);
    //     h *= 0x5bd1e995;
    //     h ^= h >> 15;
    // }
    // // printf("hash for '%s' is '%i'\n",owrd,h % N);
    // return h % N;
}

// Loads dictionary into memory, returning true if successful else false
bool load(const char *dictionary)
{
    FILE *dict = fopen(dictionary, "rb");
    if (dict == NULL)
    {
        return false;
    }

    // Create placeholder word with space for '/0'
    char wrd[LENGTH + 1];

    node *n = NULL;

    int col = 0;
    int secol = 0;
    int tercol = 0;

    // Grab words until the end of file
    while (fscanf(dict, "%s", wrd) != EOF)
    {
        // Create a new node
        n = malloc(sizeof(node));
        if (n == NULL)
        {
            unload();
            return false;
        }

        // Copy word from file into node
        strcpy(n->word, wrd);
        n->next = NULL;

        wordcount++;

        // Hash word and insert into table
        int i = hash(wrd);

        // Insert node at beginning of linked list
        n->next = table[i];
        table[i] = n;
    }

    fclose(dict);
    return true;
}

// Returns number of words in dictionary if loaded else 0 if not yet loaded
unsigned int size(void)
{
    return wordcount;
}

void free_node(node *root)
{
    if (root == NULL)
    {
        return;
    }

    free_node(root->next);

    free(root);
}

// Unloads dictionary from memory, returning true if successful else false
bool unload(void)
{
    // Iterate through enire hash table
    // Recursivly unload
    for (int n = 0; n < N; n++)
    {
        free_node(table[n]);
    }
    return true;
}


