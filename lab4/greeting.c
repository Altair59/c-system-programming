#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
    This program has two arguments: the first is a greeting message, and the
    second is a name.

    The message is an impersonal greeting, such as "Hi" or "Good morning".
    name is set to refer to a string holding a friend's name, such as
    "Emmanuel" or "Xiao".

    First copy the first argument to the array greeting. (Make sure it is
    properly null-terminated.)

    Write code to personalize the greeting string by appending a space and
    then the string pointed to by name.
    So, in the first example, greeting should be set to "Hi Emmanuel", and
    in the second it should be "Good morning Xiao".

    If there is not enough space in greeting, the resulting greeting should be
    truncated, but still needs to hold a proper string with a null terminator.
    For example, "Good morning" and "Emmanuel" should result in greeting
    having the value "Good morning Emmanu" and "Top of the morning to you" and
    "Patrick" should result in greeting having the value "Top of the morning ".

    Do not make changes to the code we have provided other than to add your 
    code where indicated.
*/

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: greeting message name\n");
        exit(1);
    }
    char greeting[20];
    char *name = argv[2];

    // Your code goes here
    char *str1 = argv[1];
    int len = strlen(str1);
    int i = 0;
    if (len <= 19){
        for (i=0;i<len;i++){
            greeting[i] = str1[i];
        }
        greeting[i] = '\0';
        if (i < 18){
            greeting[i] = ' ';
            i++;
            int k = 0;
            while (i < 19 && k < strlen(name)){
                greeting[i] = name[k];
                i++;
                k++;
            }
            greeting[i] = '\0';
        }
    } else {
        int j = 0;
        for (j=0;j<19;j++){
            greeting[j] = str1[j];
        }
        greeting[j] = '\0';
    }


    printf("%s\n", greeting);
    return 0;
}


