#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX] = {{0}};

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    for(int i = 0; i < candidate_count; i++)
    {
        //Find the index of candidate with 'name'
        if(strcmp(name, candidates[i]) == 0)
        {
            //Record the candidate's index in rank
            ranks[rank] = i;
            return true;
        }

    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    /*  'ranks' is array of size 'candidate_count'
        it contains the index of the first choice candidate at index 0, the last choice at the last index
    */
        //first, look at each element of the ranks array:
    for(int i = 0; i < candidate_count; i ++)
    {
        // for the rest of the elements in the list
        for(int j = i+1; j < candidate_count; j++)
        {
            // candidate at ranks[i] is prefered over all the others
            preferences[ranks[i]][ranks[j]]++;

            // remeber, ranks[n] is the index of candidate rated n. EX: [1,3,2,0]
            // ranks[1] = 3
            // candidate 3 is prefered over candidate ranks[2]= 2 and ranks[3] = 0
        }
    /*  preferences[i][j] is a square matrix where element i,j represents the
        number of voters who prefer candidate i over candidate j.
        EX:               0 1 2 3
                          | | | |
            Candidate 0 - 0 0 0 0
                    " 1 - 2 0 2 1
                    " 2 - 2 0 0 0
                    " 3 - 2 1 2 0
        Row 1, Col 2 = 2. Means candidate 1 is prefered over candidate 2 by 2 voters.
    */

        for(int k = 0; k < candidate_count; k++)
        {
            printf("%i ",preferences[i][k]);
        }
        printf("\n");
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    /*  Determine who wins for each pair
        In this matrix representation,
         pairs are symmetric elements. i.e., Jane-Bob is the same pair as Bob-Jane
        It is only necessary to check one condition, as the loop looks at _every_ element.
        For the case i == j, (the matrix diagonals), n > n, so nothing is performed
        It is also neccesary to record how many pairs are found. ties are not counted
    */
     for(int i = 0; i < candidate_count; i ++)
     {
        for(int j = 0; j < candidate_count; j++)
        {
            if(preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;

                pair_count++;
            }
        }
    }

    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    // Use bubble sort
    for (int i = 0; i < pair_count-1; i++)
    {
        if(preferences[pairs[i].winner][pairs[i].loser] < preferences[pairs[i+1].winner][pairs[i+1].loser])
        {
            // Swap items
            pair t1 = pairs[i];
            pairs[i] = pairs[i+1];
            pairs[i+1] = t1;
        }
    }

    // for(int i = 0; i < pair_count; i++)
    // {
    //     printf("Pair %i\nwinner: %i\nloser: %i\n\n",i,pairs[i].winner,pairs[i].loser);

    // }

    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    if(pair_count > 0)
    {
    locked[pairs[0].winner][pairs[0].loser] = true;
    }

    for(int i = 0; i < pair_count; i++)
    {
        bool cycle = false;

        for(int j = 0; j < pair_count; j++)
        {
            if(locked[pairs[i].loser][j] == true)
            {
                cycle = true;
                break;
            }
        }

        if(!cycle)
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
            // printf("Pair %i\n%i LOCKED against %i\n\n",i,pairs[i].winner,pairs[i].loser);
        }
    }
    return;
}


// Print the winner of the election
void print_winner(void)
{
    for(int i = 0; i < candidate_count; i ++)
     {
        bool source = true;

        for(int j = 0; j < candidate_count; j++)
        {
            if(locked[j][i])
            {
                source = false;
            }
        }

        if(source)
        {
            printf("%s\n",candidates[i]);
        }
    }
    return;
}

