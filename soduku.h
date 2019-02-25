#include <stdio.h>
#include <stdlib.h>


int echo_soduku(int board[][9]) {
    for (int i = 0; i < 9; i ++) {
        if (!(i % 3))
            printf("+─────────+─────────+─────────+\n");
        for (int j = 0; j < 9; j ++) {
            if (!(j % 3))
                printf("│");
            if (!board[i][j])
                printf(" · ");
            else
                printf(" %d ", board[i][j]);
        }
        printf("│\n");
    }
    printf("+─────────+─────────+─────────+\n");
}


int randn(int min, int max) {
    // return random value between (min, max], not including `min`.
    // use: randn(0, 9): get value from 1~9.
    return min + rand() % (max - min) + 1;
}


int assign(int board[][9], int value, int pos) {
    // pos: int from 1~81.
    // to find if have conflict and assign `value` at `pos`.
    int ix1 = pos / 9, ix2 = pos % 9;
    for (int i = 0; i < 9; i ++)
        if (board[ix1][i] == value || board[i][ix2] == value)
            return -1;      // conflict.
    // TODO: 遍历3 * 3:
}

int init(int board[][9]) {
    int rand9pos[9];
    for (int i = 0; i < 9; i ++) {
        int pos = randn(0, 81) - 1;     // 1~81
        printf("%d\n", pos);
        board[pos / 9][pos % 9] = i + 1;
    }
    for (int i = 0; i < 9; i ++) {
    }
}


