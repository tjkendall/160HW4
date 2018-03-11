#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINES 20000

// Struct to keep track of the number of tweets per user
struct tweeterCounter{
    unsigned short numTweets;
    char* name;
};

struct tweeterCounter *tweeterList[MAX_LINES];      // Array to hold list of all tweeters
struct tweeterCounter* mostTweeters[10];        // Array to hold top 10 tweeters
unsigned short lastOpen = 0;    // Variable to keep track of last open spot in tweeterCounter array
unsigned short nameIndex;       // index of the name column in the csv


int findIndex(char *string)
{
    char *token;
    token = strtok(string, ",");
    short count = 0;

    while(token != NULL)
    {
        if (strcmp(token, "\"name\"" ) == 0 || strcmp(token, "name") == 0)
        {
            nameIndex = count;
            return 0;
        }
        count++;
        token = strtok(NULL, ",");
    }
    return -1;
}

int addTweeter(char *name){
    char* tweeter = strtok(name, "\"");
    if (tweeter == NULL)
        return -1;
    for (int i = 0; i < lastOpen; i++){
        if (strcmp(tweeterList[i]->name, tweeter) == 0)
        {
            tweeterList[i]->numTweets++;
            return 0;
        }
    }

    tweeterList[lastOpen] = malloc(sizeof(struct tweeterCounter));
    tweeterList[lastOpen]->numTweets = 1;
    tweeterList[lastOpen]->name = malloc(strlen(tweeter));
    memcpy(tweeterList[lastOpen]->name, tweeter, strlen(tweeter));
    lastOpen++;
    return 0;
}

int checkInList(struct tweeterCounter* tweeter, int numInList){
    for (int i = 0; i < numInList; i++)
        if (mostTweeters[i] == tweeter)
            return 0;
    return -1;
}

int getTop10(){
    for (int topIndex = 0; topIndex < 10 && topIndex < lastOpen; topIndex++){
        unsigned short max = 0;
        int maxIndex = -1;
        for (int i = 0; i < lastOpen; i++){
            if (tweeterList[i]->numTweets > max && checkInList(tweeterList[i], topIndex) != 0){
                max = tweeterList[i]->numTweets;
                maxIndex = i;
            }
        }
        if (maxIndex == -1)
            return -1;
        mostTweeters[topIndex] = tweeterList[maxIndex];
    }
    return 0;
}

void printTop10(){
    for (int i = 0; i < 10 && i < lastOpen; i++)
        printf("%s: %u\n", mostTweeters[i]->name, mostTweeters[i]->numTweets);
}

int findTweeter(char *line){
    char *token;
    token = strtok(line, ",");
    
    unsigned short count = 0;
    while(token != NULL && count != nameIndex)
    {
        count++;
        token = strtok(NULL, ",");
    }

    if (token == NULL)
        return -1;

    if (addTweeter(token) == -1)
        return -1;

    return 0;
}

int main(int argc, const char *argv[])
{
    if (argc != 2)
    {
        printf("Invalid Input Format\n");
        return -1;
    }

    FILE* stream = fopen(argv[1], "r");
    if (stream == NULL){
        printf("Invalid Input Format\n");
        return -1;
    }
    size_t len = 0;
    ssize_t read;

    char *line = NULL;
    if (getline(&line, &len, stream) != -1)
    {
        if (findIndex(line) == -1)
        {
            if (line)
                free(line);
            printf("Invalid Input Format\n");
            return -1;
        }
    }

    while ((read = getline(&line, &len, stream)) != -1) {
        if (findTweeter(line) == -1){
            if (line)
                free(line);
            printf("Invalid Input Format\n");
            return -1;
        }
    }

    if (getTop10() != 0)
    {
        printf("Invalid Input Format\n");
        for (int i = 0; i < lastOpen; i++)
            free(tweeterList[i]);
        return -1;
    }

    printTop10();

    for (int i = 0; i < lastOpen; i++)
        free(tweeterList[i]);


    if (stream)
        fclose(stream);
    if (line)
        free(line);
    exit(EXIT_SUCCESS);
}
