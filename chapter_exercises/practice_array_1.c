#include <stdio.h>

int main(void){ 
    int scores[5] = {88, 61, 90, 75, 93};
    int sumScores = 0; 
    int maxScores = scores[0];
    int minScores = scores[0];

    for (int i=0 ; i < 5 ; i++){
        sumScores += scores[i];
        if (scores[i] > maxScores){
            maxScores = scores[i];
        }
        if (scores[i] < minScores){
            minScores = scores[i];
        } 
    }
    printf("max score:%d, min score:%d, average score:%.2f", maxScores, minScores, sumScores/5.00);
    return 0;
} 