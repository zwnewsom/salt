#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

unsigned long int hash(char *str)
{
    int i, j, n; unsigned long int left, right; char temp_left[9], temp_right[8];
    
    // avoid segfaults
    if (str == NULL)
        return INT_MIN;
    
    // break the string up into left and right sections
    n = strlen(str);
    for (i = 0, j = n - 7; j < n; i++, j++)
        temp_right[i] = str[j];
    strcat(temp_right, "\0");
    
    for (i = 0; i <= n - 8; i++)
        temp_left[i] = str[i];
    strcat(temp_left, "\0");
     
    // convert strings to long integers
    left = atoi(temp_left);
    right = atoi(temp_right);
     
    return ((243 * left) + right) % 85767489;    
}

void generateDictionaryFile(char *str, FILE *fPtr)
{
    char *ASCIIval = malloc(sizeof(char) * 3), *unsalted_pw = malloc(sizeof(char) * 13), *salted_pw = malloc(sizeof(char) * 16); 
    int i, j, temp; long unsigned int hash_value;
    
    // convert characters of plaintext password to their corresponding ASCII values
    for (i = 0; i < 6; i++)
    {
        temp = str[i];
        sprintf(ASCIIval, "%d", temp);
        strcat(unsalted_pw, ASCIIval);
    }
    
    // format the output correctly using a combination of strings, format specifiers
    for (j = 0; j <= 999; j++)
    {
        sprintf(salted_pw, "%d", j);
        strcat(salted_pw, unsalted_pw);
        hash_value = hash(salted_pw);
        fprintf(fPtr, "%s ", str);
        fprintf(fPtr, "%03d ", j);
        fprintf(fPtr, "%lu\n", hash_value);
    }
    
    return;
}

int main(void)
{
    int count; char pw_file[32], hash_target[9], buffer[7], password[7], salt[4], hash_value[9];
    FILE *fPtr1, *fPtr2;
    
    printf("Enter the name of your password file.\n");
    scanf("%s", pw_file);
    
    fPtr1 = fopen(pw_file, "r");
    fPtr2 = fopen("dictionary.txt", "w+");

    while (fscanf(fPtr1, "%s", buffer)  != EOF)
        generateDictionaryFile(buffer, fPtr2);
    
    rewind(fPtr2);
    
    printf("Enter the target hash value.\n");
    scanf("%s", hash_target);
    
    // scan dictionary.txt for targeted hash value
    count = 0;
    while (!feof(fPtr2))
    {
        count++;
        fscanf(fPtr2, "%s", password);
        fscanf(fPtr2, "%s", salt);
        fscanf(fPtr2, "%s", hash_value);
        
        // case 1: password was recovered
        if (strcmp(hash_target, hash_value) == 0)
        {
            printf("Password Recovered!\n");
            printf("Password: %s\n", password);
            printf("Salt Value: %s\n", salt);
            printf("Entries Searched: %d\n", count);
            break;
        }
        
        // case 2: sorry Charlie :(
        if (count == 100000)
            printf("Password was not found.\nEntries searched: %d\n", count);
    }
    
    fclose(fPtr1);
    fclose(fPtr2);
    return 0;
}
