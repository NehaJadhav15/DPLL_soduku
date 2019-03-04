/*
* <SAT Solver using DPLL Algorithm>
* Author: Haoyuan Wang
* Huazhong University of Science and Technolodge
* Time: Feb 6, 2019
* ------
*
* Args:
* ------
* f: CNF formula, f = {c1, c2, ..., c_n}
* c: clause of f
* l: literal, which value == 0 or 1
* v: varibles
* m: the set of assignment of v
*
* Related link: https://image.hanspub.org/Html/6-2690183_16947.htm#txtF4
*
* Algorithm (given f, to get Satisfiability s):
* ------
* 1. sort v by apprence times (v1>v2>...vm), got the assigning order.
* 2. divide c to:
*     - c1: only have a literal
*     - c2: others
* 3. assign all varibles in c1
* 4. infer varibles in c2 according to c1 (if possible)
* 5. if v appears only as `v` or `-v`, then let `v` or `-v` = 1, remove all clauses that v appeared.
* 6. Using DPLL laws to update m (using backtracing method):
*     - Using Recursive function.
*
* Others:
* ------
* - data(.cnf files) path: ./data
*/

#include <stdio.h>
#include <time.h>
// #include "dpll.h"
#include "soduku.h"


void main_ui(int id) {
    system("clear");
    printf("Welcome to DPLL program! \n");
    printf("----------------\n");
    if (id == 1)
        printf("\033[1;32m> Solve SAT\033[0m\n");
    else
        printf("  Solve SAT\n");
    if (id == 2)
        printf("\033[1;32m> Play Soduku\033[0m\n");
    else
        printf("  Play Soduku\n");
    if (id == 3)
        printf("\033[1;32m> Exit\033[0m\n");
    else
        printf("  Exit\n");
    printf("----------------\n");
}

void sat_ui(int id) {
    system("clear");
    printf("Solve SAT \n");
    printf("----------------\n");
    if (id == 1)
        printf("\033[1;33m> Read from CNF\033[0m\n");
    else
        printf("  Read from CNF\n");
    if (id == 2)
        printf("\033[1;33m> Main Menu\033[0m\n");
    else
        printf("  Main Menu\n");
    printf("----------------\n");
}

void soduku_ui(int id) {
    system("clear");
    printf("Play Soduku \n");
    printf("----------------\n");
    if (id == 1)
        printf("\033[1;35m> Game init\033[0m\n");
    else
        printf("  Game init\n");
    if (id == 2)
        printf("\033[1;35m> Show board\033[0m\n");
    else
        printf("  Show board\n");
    if (id == 3)
        printf("\033[1;35m> Play Soduku\033[0m\n");
    else
        printf("  Play Soduku\n");
    if (id == 4)
        printf("\033[1;35m> Solve Soduku\033[0m\n");
    else
        printf("  Solve Soduku\n");
    if (id == 5)
        printf("\033[1;35m> Main Menu\033[0m\n");
    else
        printf("  Main Menu\n");
    printf("----------------\n");
}


int main() {
    clock_t start, finish;
    system("stty -icanon");     // close flush.
    int quit = 0;
    int given[9][9];
    for (int i = 0; i < 9; i ++)
        for (int j = 0; j < 9; j ++)
            given[i][j] = 0;
    List *L = (List *)malloc(sizeof(List));
    while (1) {
        main_ui(1);
        int opt = 1;
        char in = 0;
        // fflush(stdin);
        while(1) {
            char in = getchar();
            if (in == 'w' && opt != 1) {
                opt --;
                main_ui(opt);
            } else if (in == 's' && opt != 3) {
                opt ++;
                main_ui(opt);
            } else if(in == '\n' || in == 'd')
                break;
            else
                main_ui(opt);
        }
        switch (opt) {
            case 1:
                quit = 0;
                opt = 1;
                sat_ui(1);
                while (1) {
                    if (quit)
                        break;
                    // int opt = 1;
                    while(1) {
                        char in = getchar();
                        if (in == 'w' && opt != 1) {
                            opt --;
                            sat_ui(opt);
                        } else if (in == 's' && opt != 2) {
                            opt ++;
                            sat_ui(opt);
                        } else if(in == '\n' || in == 'd')
                            break;
                        else
                            sat_ui(opt);
                    }
                    switch (opt) {
                        case 1:
                            printf("> Enter the NO of .cnf file: ");
                            int file_no, nv, nc;
                            if(scanf("%d", &file_no) != 1) {
                                printf("\n> ERROR: Input format error!\n");
                                getchar();
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
                soduku_ui(1);
                quit = 0;
                opt = 1;
                while (1) {
                    if (quit)
                        break;
                    while(1) {
                        char in = getchar();
                        if (in == 'w' && opt != 1) {
                            opt --;
                            soduku_ui(opt);
                        } else if (in == 's' && opt != 5) {
                            opt ++;
                            soduku_ui(opt);
                        } else if(in == '\n' || in == 'd')
                            break;
                        else
                            soduku_ui(opt);
                    }
                    switch (opt) {
                        case 1:
                            printf("> Game init:\n");
                            for (int i = 0; i < 9; i ++)
                                for (int j = 0; j < 9; j ++)
                                    given[i][j] = 0;
                            // GenSodukuSAT();
                            init(given);
                            dig(given);
                            echo_soduku(given);
                            printf("Press any key to continue...\n");
                            break;
                        case 2:
                            printf("> Show board:\n");
                            echo_soduku(given);
                            printf("Press any key to continue...\n");
                            break;
                        case 3:
                            printf("> Play soduku:\n");
                            play(given);
                            break;
                        case 4:
                            printf("> Solve soduku:\n");
                            solve(given);
                            echo_soduku(given);
                            printf("Press any key to continue...\n");
                            break;
                        case 5:
                            printf("> Main Menu:\n");
                            quit = 1;
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