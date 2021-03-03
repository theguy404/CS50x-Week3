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
    for(int i = 0; i < candidate_count; i++)
    {
        int candLen = strlen(candidates[i]);
        
        // check to see if the current candidate matchs the name length given
        if(nameLen == candLen) {
            
            // checks to see if the name matchs the name given
            for(int j = 0; j < candLen; j++)
            {
                if(candidates[i][j] != name[j])
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
    for(int i = 0; i < candidate_count - 1; i++) 
    {
        // loops through all the ranks below the current canadidate and adds 1
        for(int j = i + 1; j < candidate_count; j++)
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
    for(int i = 0; i < candidate_count; i++)
    {
        // loops through the collums of preferences and matches the number of
        // wins for each candidates in each pair and passes wins to pairs array
        for(int j = i + 1; j < candidate_count; j++)
        {
            int count1 = preferences[i][j];
            int count2 = preferences[j][i];
            
            pair counts;
            
            if(count1 > count2)
            {
                counts.winner = i;
                counts.loser = j;
            }
            else if(count2 > count1)
            {
                counts.winner = j;
                counts.loser = i;
            }
            
            pairs[pair_count] = counts;
            
            pair_count++;
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    // temperary variables
    int highestValue = 0;
    int highestIndex = 0;
    int correctedIndex = -1;
    pair tempValue;
    
    // loop through the array and find the highest value
    // returns highest value to the lowest index and then repeats till
    // all are "corrected"
    while(correctedIndex < pair_count)
    {
        for(int i = correctedIndex +1; i < pair_count; i++)
        {
            pair currentTestPair = pairs[i];
            int winnerValue = preferences[currentTestPair.winner][currentTestPair.loser];
            int loserValue = preferences[currentTestPair.loser][currentTestPair.winner];
            int testvalue = winnerValue - loserValue;
            
            if(testvalue > highestValue)
            {
                highestValue = testvalue;
                highestIndex = i;
            }
        }
        // add highest value to corrected section of the array
        tempValue = pairs[correctedIndex + 1];
        pairs[correctedIndex + 1] = pairs[highestIndex];
        pairs[highestIndex] = tempValue;
        
        // increase the corrected index value and resets the highest values
        correctedIndex++;
        highestValue = 0;
        highestIndex = 0;
    }
    
    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    int winners[pair_count];
    int winCount = 0;
    bool record = true;
    
    // loop through sorted pairs
    for(int i = 0; i < pair_count; i++)
    {
        // checks the winners array to see if the loser has previously won
        if(winCount)
        {
            for(int j = 0; j < winCount; j++)
            {
                // if they have won against someone else then this would create a cycle
                // and pair is thrown out
                if(winners[j] == pairs[i].loser)
                {
                    record = false;
                }
            }
        }
        
        // if no cycle then mark the winner and loser in locked array
        if(record)
        {
            winners[winCount] = pairs[i].winner;
            locked[pairs[i].winner][pairs[i].loser] = true;
            locked[pairs[i].loser][pairs[i].winner] = false;
            winCount++;
        }
        else 
        {
            locked[pairs[i].winner][pairs[i].loser] = false;
            locked[pairs[i].loser][pairs[i].winner] = false;
        }
    }
    return;
}

// Print the winner of the election
void print_winner(void)
{
    int winner;
    bool loseFound = false;
    
    // loop over all candidates and search for one with no loses
    for(int i = 0; i < pair_count; i++)
    {
        for(int j = 0; j < pair_count; j++)
        {
            if(locked[i][j] == true)
            {
                loseFound = true;
            }
        }
        if(loseFound == false)
        {
            winner = i;
        }
    }
    
    printf("%s\n", candidates[winner]);
    return;
}

