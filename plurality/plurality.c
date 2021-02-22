#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// Candidates have name and vote count
typedef struct
{
    string name;
    int votes;
}
candidate;

// Array of candidates
candidate candidates[MAX];

// Number of candidates
int candidate_count;

// Function prototypes
bool vote(string name);
void print_winner(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: plurality [candidate ...]\n");
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
        candidates[i].name = argv[i + 1];
        candidates[i].votes = 0;
    }

    int voter_count = get_int("Number of voters: ");

    // Loop over all voters
    for (int i = 0; i < voter_count; i++)
    {
        string name = get_string("Vote: ");

        // Check for invalid vote
        if (!vote(name))
        {
            printf("Invalid vote.\n");
        }
    }

    // Display winner of election
    print_winner();
}

// Update vote totals given a new vote
bool vote(string name)
{
    // loops through all the candidates
    for(int i = 0; i < candidate_count; i++)
    {
        int canlen = strlen(candidates[i].name);
        int namelen = strlen(name);
        
        // checks to see if the current candidate name has the same length as vote
        if(canlen == namelen)
        {
            bool nameFound = true;
            
            // checks the name to make sure its a perfect match to the one given
            for(int j = 0; j < canlen; j++)
            {
                if(candidates[i].name[j] != name[j])
                {
                    nameFound = false;
                }
            }
            
            // if the name matches up perfectly then a vote is added to their count
            // then the function returns true
            if(nameFound)
            {
                candidates[i].votes++;
                return true;
            }
        }
    }
    return false;
}

// Print the winner (or winners) of the election
void print_winner(void)
{
    int highestVote = 0;
    
    // loops through the candidates and finds the highest vote count
    for(int i = 0; i < candidate_count; i++)
    {
        if(candidates[i].votes > highestVote)
        {
            highestVote = candidates[i].votes;
        }
    }
    
    // loops through the candidates again and returns all candidates with
    // the same vote count equal to the highest vote count found
    for(int i = 0; i < candidate_count; i++)
    {
        if(candidates[i].votes == highestVote)
        {
            int namelen = strlen(candidates[i].name);
            
            for(int j = 0; j < namelen; j++)
            {
                printf("%c", candidates[i].name[j]);
            }
            
            printf("\n");
        }
    }
    return;
}

