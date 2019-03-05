#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "dpll.h"
static unsigned int randseed = 233;
#define HOLE_NUM 40
#define ORIGIN_FILLED -1
#define USR_FILLED 1
#define EMPTY 0


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
    srand(randseed);
    randseed += rand();
    return min + rand() % (max - min) + 1;
}


int ix2no(int ix1, int ix2) {
    // convert 2 index (from 0 ~ 8) to pos (from 0~80)
    return ix1 * 9 + ix2;
}


int ehco_model_sat(int model[], int nv) {
    for (int x = 0; x < nv; x ++) {
        if (model[x] == NEG)
            printf("0 ");
        else if (model[x] == UNKNOWN)
            printf("N ");
        else
            printf("1 ");
        if (!((x + 1) % 9))
            printf("\n ");
        if (!((x + 1) % 81))
            printf("[x=%d]\n\n ", (x + 1) / 81);
    }
}


int solve(int board[][9], int info) {
    int nv, nc, res_n = 1;
    clock_t start, finish1, finish2;
    List *L = parser(9, &nv, &nc);
    // init model:
    int *model = (int *)malloc(nv * sizeof(int));
    for (int i = 0; i < nv; i++)
        model[i] = UNKNOWN;
    float *decay = (float *)malloc(2 * nv * sizeof(float));
    for (int i = 0; i < 2 * nv; i++)
        decay[i] = 100;
    for (int x = 0; x < 9; x ++)
        for (int y = 0; y < 9; y ++)
            if (board[x][y]) {
                remove_v(&L, (x * 9 + y) * 9 + board[x][y]);
                model[(x * 9 + y) * 9 + board[x][y] - 1] = 1;
                for (int i = 1; i <= 9; i++) {
                    if (i != board[x][y]) {
                        remove_v(&L, -((x * 9 + y) * 9 + i));
                        model[(x * 9 + y) * 9 + i - 1] = NEG;
                    }
                }
            }
    // ehco_model_sat(model, nv);
    start = clock();
    int res = dpll_decay(L, nv, model, decay);
    finish1 = clock();
    double cost1 = (double)(finish1 - start) / CLOCKS_PER_SEC;
    if (info)
        printf("> INFO: CLOCKS_PER_SEC=%ld, Time cost: %lf ms (%lf s)\n", CLOCKS_PER_SEC, cost1 * 1000, cost1);
    for (int x = 0; x < 9; x ++)
        for (int y = 0; y < 9; y ++)
            for (int z = 1; z <= 9; z ++)
                if (model[(x * 9 + y) * 9 + z - 1] == 1)
                    board[x][y] = z;
    return res_n;
}


int init(int board[][9]) {
    randseed = time(0);
    for (int value = 1; value <= 9; value ++) {
        int pos = randn(0, 81) - 1;     // (1~81) - 1 -> 0~80
        board[pos / 9][pos % 9] = value;    // board[x, y]
    }
    // echo_soduku(board);
    // int tmp = solutionCount(board, 2);
    // printf("> res num: %d\n", tmp);
    solve(board, 0);
}


int isLegal(int board[][9], int value, int ix1, int ix2, int info) {
    // pos: int from 1~81.
    // to find if have conflict
    // int ix1 = pos / 9, ix2 = pos % 9;
    // if (board[ix1][ix2] == value)
    //     return 1;
    for (int i = 0; i < 9; i ++)
        if (board[ix1][i] == value || board[i][ix2] == value) {
            if (info)
                printf("> Conf: (%d, %d) || (%d, %d)\n", ix1, i, i, ix2 );
            return 0;      // conflict.
        }
    int block_pos1 = ix1 / 3, block_pos2 = ix2 / 3;
    // printf("%d %d ix: (%d, %d)\n", block_pos1, block_pos2, ix1, ix2);
    // echo_soduku(board);
    for (int i = 0; i <  3 ; i ++)
        for (int j = 0; j < 3; j ++)
            if (board[block_pos1 * 3 + i][block_pos2 * 3 + j] == value) {
                if (info)
                    printf("> Conf: (%d, %d)\n", block_pos1 * 3 + i, block_pos2 * 3 + j );
                return 0;
            }
    return 1;
}


int solutionCount(int board[][9], int solutionsCuttoff) {
    //this function destroys the board in the process of counting the number of solutions
    int z, count, full = 1, ix1, ix2, over = 0;
    // echo_soduku(board);
    for (int x = 0; x < 9; x ++) {
        for (int y = 0; y < 9; y ++)
            if (!board[x][y]) {
                ix1 = x;
                ix2 = y;
                full = 0;
                over = 1;
                break;
            }
        if (over)
            break;
    }
    //if the board is full up, we are done
    if (full) {
        count = 1;
    } else {
        count = 0;
        z = 1;
        while ((count < solutionsCuttoff) && (z <= 9)) {
            if (isLegal(board, z, ix1, ix2, 0)) {
                board[ix1][ix2] = z;
                if (solutionCount(board, 1)) {
                    count++;
                }
                //reset to a blank value to try another solution
                board[ix1][ix2] = 0;
            }
            z++;
        }
    }
    return count;
}


int dig(int board[][9], int hole_n) {
    // dig holes at a completed board.
    // echo_soduku(board);
    for (int i = 0; i < hole_n; i ++) {
        int res_n = 0, flag = 0;
        while(1) {
            randseed = time(0);
            int pos;     // (1~81) - 1 -> 0~80
            do
                pos = randn(0, 81) - 1;
            while (!board[pos / 9][pos % 9]);
            int bak = board[pos / 9][pos % 9];    // board[x, y]
            board[pos / 9][pos % 9] = 0;
            res_n = solutionCount(board, 2);
            // printf("%d\n", res_n);
            if (res_n > 1) {
                board[pos / 9][pos % 9] = bak;
                continue;
            } else
                break;
        }
    }
}


int get_v(int x, int y, int z) {
    // convert x, y (from 1~9) and z(from 1~9) to num from 1~729
    return ((x - 1) * 9 + y - 1) * 9 + z;
}


int echo_soduku_play(int board[][9], int x, int y, char value, int empty_pos[][9]) {
    system("clear");
    printf("Press wsad to move curser.\nPress number from 1~9 to fill a cell.\nPress \'-\' to remove a number.\n");
    for (int i = 0; i < 9; i ++) {
        if (!(i % 3))
            printf("+─────────+─────────+─────────+\n");
        for (int j = 0; j < 9; j ++) {
            if (!(j % 3))
                printf("│");
            if (i == x && j == y) {
                if (value == '0')
                    printf(" ▮ ");
                else if (value == '-')
                    printf(" · ");
                else if (value != '0') {
                    printf(" %c ", value);
                }
            } else if (!board[i][j])
                printf(" · ");
            else {
                if (empty_pos[i][j] == ORIGIN_FILLED)
                    // printf(" x ", board[i][j]);
                    printf("\033[1;33m %d \033[0m", board[i][j]);
                else
                    printf(" %d ", board[i][j]);
            }
        }
        printf("│\n");
    }
    printf("+─────────+─────────+─────────+\n");
}

int isFull(int board[][9]) {
    for (int i = 0; i < 9; i ++)
        for (int j = 0; j < 9; j ++)
            if (!board[i][j])
                return 0;
    return 1;
}


int play(int board[][9]) {
    int empty_pos[9][9];    // 0:have origin num
    int ix = 0, x = 0, y = 0;
    for (int i = 0; i < 9; i ++)
        for (int j = 0; j < 9; j ++) {
            empty_pos[i][j] = EMPTY;
            if(board[i][j])
                empty_pos[i][j] = ORIGIN_FILLED;   // 1: empty
        }
    while(1) {
        echo_soduku_play(board, x, y, '0', empty_pos);
        char in = getchar();
        if (in == 'q')
            return -1;      // interupt
        else if (in == 'w' && x != 0) {
            echo_soduku_play(board, x, y, '0', empty_pos);
            x--;
        } else if (in == 's' && x != 8) {
            echo_soduku_play(board, x, y, '0', empty_pos);
            x++;
        } else if (in == 'a' && y != 0) {
            echo_soduku_play(board, x, y, '0', empty_pos);
            y--;
        } else if (in == 'd' && y != 8) {
            echo_soduku_play(board, x, y, '0', empty_pos);
            y++;
        } else if (in >= '1' && in <= '9' && empty_pos[x][y] != ORIGIN_FILLED) {
            echo_soduku_play(board, x, y, in,  empty_pos);
            empty_pos[x][y] = USR_FILLED;   // 0:filled num
            int value = in - 48;
            if (!isLegal(board, value, x, y, 1)) {
                printf("> Conflict found! Try again.\n");
                getchar();
                continue;
            }
            board[x][y] = value;
            if (isFull(board))
                printf("> Game Finished!\n");
        } else if (in == '-' && empty_pos[x][y] == USR_FILLED) {
            board[x][y] = 0;
            empty_pos[x][y] = EMPTY;
            echo_soduku_play(board, x, y, '-', empty_pos);
        }
    }
}


int GenSodukuSAT() {
    /*
    * To encode soduku board to SAT problem:
    * (cell: 1x1, grid: 3x3, board:9x9, v_xyz:num(x, y)=z )
    * Constraint:
    * ------
    * 1. each cell has at least one number from 1~9:
    * clause: v_xy1, v_xy2, v_xy3 ... v_xy9
    *
    * 2. each cell has at most one number from 1~9:
    * cnf: for x in 1~9:
    *       for y in 1~9:
    *        for z in 1~8:
    *         for i in z+1~9:
    *           gen_clause(-v_xyz, -v_xyi)
    *
    * 3. Each number appears at least once in each row:
    * clause: v_1yz, v_2yz, v_3yz, ... v_9yz     // y, z from 1~9
    *
    * 4. Each number appears at least once in each column:
    * clause: v_x1z, v_x2z, v_x3z, ... v_x9z     // y, z from 1~9
    *
    * 5. Each number appears at most once in each row:
    * cnf: for y in 1~9:
    *       for z in 1~9:
    *        for x in 1~8:
    *         for i in (x+1)~9:
    *           gen_clause(-v_xyz, -v_iyz)
    *
    * 6. Each number appears at most once in each column:
    * cnf: for x in 1~9:
    *       for z in 1~9:
    *        for y in 1~8:
    *         for i in (y+1)~9:
    *           gen_clause(-v_xyz, -v_xiz)
    *
    * 7. Each number appears at least once in each 3×3 sub-grid:
    * cnf: for z in 1~9:
    *       for i in 0~2:
    *        for j in 0~2:
    *         for x in 1~3:
    *          for y in 1~3:
    *           gen_clause v_(3i+x)(3j+y)z      // x, y from 1~9
    *
    * 8. Each number appears at most once in each 3x3 sub-grid:
    * cnf: for z in 1~9:
    *       for i in 0~2:
    *        for j in 0~2:
    *         for x in 1~3:
    *          for y in 1~3:
    *           for k in (y+1)~3:
    *            gen_clause -v_(3i+x)(3j+y)z, -v_(3i+x)(3j+k)z
    * cnf: for z in 1~9:
    *       for i in 0~2:
    *        for j in 0~2:
    *         for x in 1~3:
    *          for y in 1~3:
    *           for k in (x+1)~3:
    *            for l in 1~3:
    *             gen_clause -v_(3i+x)(3j+y)z, -v_(3i+k)(3j+l)z
    */
    int nv = 0, nc = 0;
    // 9.cnf for default soduku cnf file.
    FILE * fp = fopen("./data/9.cnf", "w");
    fprintf(fp, "p cnf 729 11907\n");
    // rule 1:
    for(int x = 1; x <= 9; x++)
        for(int y = 1; y <= 9; y++) {
            for(int z = 1; z <= 9; z++)
                fprintf(fp, "%d ", get_v(x, y, z));
            fprintf(fp, "0\n");
            nc ++;
        }
    // rule 2:
    for(int x = 1; x <= 9; x++)
        for(int y = 1; y <= 9; y++)
            for(int z = 1; z <= 8; z++)
                for(int i = z + 1; i <= 9; i++) {
                    fprintf(fp, "-%d -%d 0\n", get_v(x, y, z), get_v(x, y, i));
                    nc ++;
                }
    // rule 3, 4, 5, 6:
    for(int x = 1; x <= 9; x++)
        for(int z = 1; z <= 9; z++) {
            for(int y = 1; y <= 9; y++)
                fprintf(fp, "%d ", get_v(x, y, z));
            fprintf(fp, "0\n");
            nc ++;
        }
    for(int y = 1; y <= 9; y++)
        for(int z = 1; z <= 9; z++) {
            for(int x = 1; x <= 9; x++)
                fprintf(fp, "%d ", get_v(x, y, z));
            fprintf(fp, "0\n");
            nc ++;
        }
    for(int y = 1; y <= 9; y++)
        for(int z = 1; z <= 9; z++)
            for(int x = 1; x <= 8; x++)
                for(int i = x + 1; i <= 9; i++) {
                    fprintf(fp, "-%d -%d 0\n", get_v(x, y, z), get_v(i, y, z));
                    nc ++;
                }
    for(int x = 1; x <= 9; x++)
        for(int z = 1; z <= 9; z++)
            for(int y = 1; y <= 8; y++)
                for(int i = y + 1; i <= 9; i++) {
                    fprintf(fp, "-%d -%d 0\n", get_v(x, y, z), get_v(x, i, z));
                    nc ++;
                }
    // rule 7:
    for(int i = 0; i <= 2; i++)
        for(int j = 0; j <= 2; j++)
            for(int x = 1; x <= 3; x++)
                for(int y = 1; y <= 3; y++) {
                    for(int z = 1; z <= 9; z++)
                        fprintf(fp, "%d ", get_v(3 * i + x, 3 * j + y, z));
                    fprintf(fp, "0\n");
                    nc ++;
                }
    // rule 8:
    for(int z = 1; z <= 9; z++)
        for(int i = 0; i <= 2; i++)
            for(int j = 0; j <= 2; j++)
                for(int x = 1; x <= 3; x++)
                    for(int y = 1; y <= 3; y++)
                        for(int k = y + 1; k <= 3; k++) {
                            fprintf(fp, "-%d -%d 0\n", get_v(3 * i + x, 3 * j + y, z), get_v(3 * i + x, 3 * j + k, z));
                            nc ++;
                        }
    for(int z = 1; z <= 9; z++)
        for(int i = 0; i <= 2; i++)
            for(int j = 0; j <= 2; j++)
                for(int x = 1; x <= 3; x++)
                    for(int y = 1; y <= 3; y++)
                        for(int k = x + 1; k <= 3; k++)
                            for(int l = 1; l <= 3; l++) {
                                fprintf(fp, "-%d -%d 0\n", get_v(3 * i + x, 3 * j + y, z), get_v(3 * i + k, 3 * j + l, z));
                                nc ++;
                            }
    fclose(fp);
}