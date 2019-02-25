/*
<SAT Solver using DPLL Algorithm>
Author: Haoyuan Wang
Huazhong University of Science and Technolodge
Time: Feb 6, 2019
------

Args:
------
f: CNF formula, f = {c1, c2, ..., c_n}
c: clause of f
l: literal, which value == 0 or 1
v: varibles
m: the set of assignment of v

Related link: https://image.hanspub.org/Html/6-2690183_16947.htm#txtF4

Algorithm (given f, to get Satisfiability s):
------
1. sort v by apprence times (v1>v2>...vm), got the assigning order.
2. divide c to:
    - c1: only have a literal
    - c2: others
3. assign all varibles in c1
4. infer varibles in c2 according to c1 (if possible)
5. if v appears only as `v` or `-v`, then let `v` or `-v` = 1, remove all clauses that v appeared.
6. Using DPLL laws to update m (using backtracing method):
    - Using Recursive function.

Others:
------
- data(.cnf files) path: ./data
*/

#include <stdio.h>
#include <time.h>
#include "dpll.h"
#include "soduku.h"


void main_ui() {
    system("clear");
    printf("----------------\n");
    printf("1. Solve SAT\n");
    printf("2. Play soduku\n");
    printf("3. Exit\n");
    printf("----------------\n");
    printf("Welcome to DPLL program! \nEnter your option: ");
}

void sat_ui() {
    // system("clear");
    printf("\n----------------\n");
    printf("1. Read formula from files (./data)\n");
    // printf("2. Input formsula (fmt: l l l, l l l, l l l)\n");
    printf("2. Main menu\n");
    printf("----------------\n");
}

void soduku_ui() {
    // system("clear");
    printf("\n----------------\n");
    printf("1. Game Init\n");
    printf("2. Solve Soduku\n");
    printf("3. Save game to .cnf file\n");
    printf("4. Main menu\n");
    printf("----------------\n");
}


int main() {
    clock_t start, finish;
    int opt;
    List *L = (List *)malloc(sizeof(List));
    int quit = 0;
    while (1) {
        main_ui();
        if(scanf("%d", &opt) != 1) {
            printf("> ERROR: Input format error!\n");
            return -1;
        }
        switch (opt) {
            // solve SAT problem:
            case 1:
                quit = 0;
                sat_ui();
                while (1) {
                    if (quit)
                        break;
                    printf("> Enter your option: ");
                    if (scanf("%d", &opt) != 1) {
                        printf("> ERROR: Input format error!\n");
                        return -1;
                    }
                    switch (opt) {
                        case 1:
                            printf("> Enter the NO of .cnf file:\n");
                            int file_no, nv, nc;
                            if(scanf("%d", &file_no) != 1) {
                                printf("> ERROR: Input format error!\n");
                                break;
                            }
                            List *L = parser(file_no, &nv, &nc);
                            // To init the assign model, 0 for unknown:
                            int *model = (int *)malloc(nv * sizeof(int));
                            for (int i = 0; i < nv; i++)
                                model[i] = UNKNOWN;
                            start = clock();
                            // remove_v(&L, 2, &nc);
                            // echo(L);
                            int res = dpll(L, nv, model);
                            // int res = dpll_new(L, nv, model);
                            if (res == 1) {
                                printf("> INFO: Found solution:\n");
                                echo_model(model, nv);
                            } else printf("> INFO: No solution found.\n");
                            finish = clock();
                            double cost = (double)(finish - start) / CLOCKS_PER_SEC;
                            printf("> INFO: CLOCKS_PER_SEC=%ld, Time cost: %lf ms (%lf s)\n", CLOCKS_PER_SEC, cost * 1000, cost);
                            printf("Goodbye!\n");
                            save2file(model, nv, res, cost, file_no);
                            // free(model);
                            delete_L(L);
                            exit(0);
                            break;
                        // case 2:
                        //     printf("> Input CNF:\n");
                        default:
                            quit = 1;
                            break;
                    }
                }
                break;
            // play soduku game:
            case 2:
                soduku_ui();
                quit = 0;
                while (1) {
                    if (quit)
                        break;
                    printf("> Enter your option: ");
                    if (scanf("%d", &opt) != 1) {
                        printf("> ERROR: Input format error!\n");
                        return -1;
                    }
                    switch (opt) {
                        case 1:
                            printf("> Game init:\n");
                            int given[9][9];
                            for (int i = 0; i < 9; i ++)
                                for (int j = 0; j < 9; j ++)
                                    given[i][j] = 0;
                            init(given);
                            echo_soduku(given);
                            break;
                        case 2:
                            printf("> Solve soduku:\n");
                            break;
                        case 3:
                            printf("> Save to .cnf file:\n");
                            break;
                        default:
                            quit = 1;
                            break;
                    }
                }
                break;
            default:
                return 0;
        }
    }
    return 0;
}