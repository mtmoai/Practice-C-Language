# include <stdio.h>

int main(void){
    const int MONEY = 3000;
    int pocket = MONEY;
    printf("apple ");
    while (pocket >= 120) {
        printf("*");
        pocket = pocket - 120;
    }
    printf("Remaining ¥¥%d¥n", pocket);

    pocket = MONEY;
    printf("Orange ");
    while(pocket >= 400){
        for (int i=1; i <= 6; i++){
            printf("*");
        }
        pocket = pocket - 400;
    }
    printf("Remaining ¥¥%d¥n", pocket);

    return 0;
}   