#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    char phone[11];
    int length;
    scanf("%s",phone);
    scanf("%d",&length);
    if (length == -1){
    	printf("%s\n", phone);
    	return 0;
    } else if (length>=0 && length<=9){
    	printf("%c\n", phone[length]);
    	return 0;
    } else{
    	printf("%s\n", "ERROR");
    	return 1;
    }
}