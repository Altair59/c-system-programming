#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    char phone[11];
    int length;
    int ERROR = 0;
    scanf("%s",phone);
    while (scanf("%d",&length) == 1){
        if (length == -1){
    	   printf("%s\n", phone);
        } else if (length>=0 && length<=9){
    	   printf("%c\n", phone[length]);
        } else{
    	   printf("%s\n", "ERROR");
           ERROR = 1;
        }
    }
    return ERROR;
}