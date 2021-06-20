#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

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
bool cycle(int position, int start);

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
    int nameLen = strlen(name);
    
    // loop through all the candidates
    for (int i = 0; i < candidate_count; i++)
    {
        int candLen = strlen(candidates[i]);
        
        // check to see if the current candidate matchs the name length given
        if (nameLen == candLen) {
            
            // checks to see if the name matchs the name given
            for (int j = 0; j < candLen; j++)
            {
                if (candidates[i][j] != name[j])
                {
                    return false;
                }
            }
            
            // passes the candidates index value to its rank in the ranks array
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    // loops through the ranks one candidate at a time
    for (int i = 0; i < candidate_count - 1; i++) 
    {
        // loops through all the ranks below the current canadidate and adds 1
        for (int j = i + 1; j < candidate_count; j++)
        {
            preferences[ranks[i]][ranks[j]]++;
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    // loop through the rows of preferences
    for (int i = 0; i < candidate_count; i++)
    {
        // loops through the collums of preferences and matches the number of
        // wins for each candidates in each pair and passes wins to pairs array
        for (int j = i + 1; j < candidate_count; j++)
        {
            int count1 = preferences[i][j];
            int count2 = preferences[j][i];
            
            pair counts;
            
            if (count1 > count2)
            {
                counts.winner = i;
                counts.loser = j;
                
                pairs[pair_count] = counts;
                pair_count++;
            }
            else if (count2 > count1)
            {
                counts.winner = j;
                counts.loser = i;
                
                pairs[pair_count] = counts;
                pair_count++;
            }
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    // temperary variables
    pair swap;
    int lastvalue = 0;
    int corrections = 1;
    
    // continue loop until no corrections need to be made
    while (corrections > 0)
    {
        corrections = 0;
        
        // loop through the array
        for (int i = 0; i < pair_count; i++)
        {
            // subtract winners value from losers value to get the difference
            pair currentTestPair = pairs[i];
            int winnerValue = preferences[currentTestPair.winner][currentTestPair.loser];
            int loserValue = preferences[currentTestPair.loser][currentTestPair.winner];
            int testvalue = winnerValue - loserValue;
            
            // if this is the first value in the array then record its value and continue
            if (i == 0)
            {
                lastvalue = testvalue;
            }
            else
            {
                // if this value is greater then the last value then swap them
                if (testvalue > lastvalue)
                {
                swap = pairs[i - 1];
                pairs[i - 1] = pairs[i];
                pairs[i] = swap;
                
                corrections++;
                }
                else
                {
                    lastvalue = testvalue;
                }
            }
        }
    }
    
    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for (int i = 0; i < pair_count; i++)
    {
        if (!cycle(pairs[i].winner, pairs[i].loser))
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
        else 
        {
            locked[pairs[i].winner][pairs[i].loser] = false;
        }
    }
    return;
}

bool cycle(int winner, int loser)
{
    // checks if the loser has previously beat the winner
    if (locked[loser][winner])
    {
        return true;
    }
    
    //checks if the loser has won previously
    for (int i = 0; i < candidate_count; i++)
    {
        if (locked[loser][i] && i != winner)
        {
            cycle(winner, i);
        }
    }
}

// Print the winner of the election
void print_winner(void)
{
    int winner;
    bool loseFound = false;
    
    // loop over all candidates and search for one with no loses
    for (int i = 0; i < pair_count; i++)
    {
        for (int j = 0; j < pair_count; j++)
        {
            if (locked[i][j] == true)
            {
                loseFound = true;
            }
        }
        if (loseFound == false)
        {
            winner = i;
        }
    }
    
    printf("%s\n", candidates[winner]);
    return;
}
