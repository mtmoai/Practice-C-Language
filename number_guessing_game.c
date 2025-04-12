#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
    printf("*** Number Guessing Game (Level 2) ***\n");
    printf("Try to guess the 3-digit number.\n");
    printf("Note: Each digit is unique (no duplicates).\n");
    srand((unsigned)time(NULL));
    int answer[3] = {};
    char key[3] = {};
    int input[3] = {};
    char tryAgain[1] = {1};


    for (int i=0 ; i < 3 ; i++){
        answer[i] = rand()%10;
    }
    while (answer[0] == answer[1]){
        answer[1] = rand()%10;
    }
    while (answer[0] == answer[2] && answer[1] == answer[2]){
        answer[2] = rand()%10;
    }

    for (int i=0 ; i < 3 ; i++){ //answer check
        printf("%d\n", answer[i]);
    }
    while (tryAgain != 0){
        int allMatch = 0;
        int match = 0;
        for (int i=1 ; i < 4 ; i++){
            printf("%d桁目の数字を入力してください:", i);
            scanf("%s", key);
            int n = atoi(key);
            input[i-1] = n;
        }
        for (int i=0 ; i < 3 ; i++){ //input check
            printf("%d\n", input[i]);
        }
        for (int i=0 ; i < 3 ; i++){
            if (answer[i] == input[i]){
                allMatch++;
            }
            for (int j=0 ; j < 3 ; j++){
                if (answer[i] == input[j] && i != j){
                    match++;
                }
            }
        }

        printf("%d HIT!, %d BLOW!\n", allMatch, match);
        if (allMatch == 3){
            printf("Clear!\n");
            break;
        } else {
            printf("Do you want to try again? (0: Finish, 1~: Continue):\n");
        }
        scanf("%s", tryAgain);
        if (tryAgain == 0){
            break;
        }
    }
    return 0;
}
