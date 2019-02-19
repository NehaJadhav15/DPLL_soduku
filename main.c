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
#include "dpll.h"


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
    printf("2. Input formula (fmt: l l l, l l l, l l l)\n");
    printf("3. Main menu\n");
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
    int opt;
    List head, *L = &head;
    int quit = 0;
    while (1) {
        main_ui();
        scanf("%d", &opt);
        switch (opt) {
            // solve SAT problem:
            case 1:
                quit = 0;
                sat_ui();
                while (1) {
                    if (quit)
                        break;
                    printf("> Enter your option: ");
                    scanf("%d", &opt);
                    switch (opt) {
                        case 1:
                            printf("> Enter the NO of .cnf file:\n");
                            int file_no, nv, nc;
                            scanf("%d", &file_no);
                            parser(file_no, &nv, &nc, L);
                            // echo(L);
                            // To init the assign model, -1 for unknown:
                            int *model = (int *)malloc(nv * sizeof(int));
                            /*
                            during counting:
                            - count1: count apprence times of each v when v==1;
                            - count2: count apprence times of each v when v==0;
                            after counting:
                            - count1: store total apprence times of each v (no matter v == 0 or 1)
                            - count2: for v, store 1 if v_pos_time > v_neg_time.
                            ------------
                            first sort variblese by apprence times, and assign v with this order. if if v_pos_time > v_neg_time, v = 1, else v = 0. count2 stores the value of each v.
                            */
                            int *count1 = (int *)malloc(nv * sizeof(int));
                            int *count2 = (int *)malloc(nv * sizeof(int));
                            int *assign_order = (int *)malloc(nv * sizeof(int));
                            for(int i = 0; i < nv; i++)
                                model[i] = UNKNOWN;
                            count_times(L, nv, count1, count2);
                            get_order(count1, nv, assign_order);
                            remove_pure_l(&L, &nv, &nc, model);
                            remove_single_l(&L, &nv, &nc, model);
                            // echo(L);
                            int *res_p = dpll(L, nv, model, assign_order, count2, 0);
                            if (res_p != model)
                                echo_model(res_p, nv);
                            else
                                printf("> INFO: Can\'t find solution.\n");
                            break;
                        case 2:
                            printf("> Input CNF:\n");
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
                    scanf("%d", &opt);
                    switch (opt) {
                        case 1:
                            printf("> Game init:\n");
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
