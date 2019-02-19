#include <stdio.h>
#include <stdlib.h>

#define ROOT "./data/"
#define PATH "./data/n.cnf"
#define UNKNOWN 0
#define NEG -1


typedef struct Node {
    int data;
    struct Node *next;
} Node;

typedef struct List {
    int len;    // len of sub_list
    struct List *next;
    Node *head;
} List;

int parser(int file_no, int * nvp, int *ncp, List *L) {
    /*
    To find that if a cnf file in correct format, and read data from file.
    */
    FILE *fp;
    int start_read = 0, tmp, over = 0;
    // char path[] = ROOT "n.cnf";
    char path[] = PATH;
    List *lp = L;
    Node *np = NULL;
    path[7] = file_no + 48;
    fp = fopen(path, "r");
    if (!fp) {
        printf("> ERROR: open file failed.\n");
        return -1;
    }
    while (1) {
        if (over)
            break;
        if (start_read == 0) {
            char head_c;        // the first character of a line
            fscanf(fp, "%c", &head_c);
            if (head_c == 'p') {
                start_read = 1;
                // to getchar 'cnf':
                for (int i = 0; i < 4; i++)
                    fgetc(fp);
                fscanf(fp, "%d %d", nvp, ncp);
            } else if (head_c == 'c')
                // while(fgetc(fp) != '\n')
                //     ;
            {
                while (1) {
                    char c = fgetc(fp);
                    if (c == '\n')
                        break;
                    // printf("%c", c);
                }
            } else {
                printf("> ERROR: file format error.\n");
                return -1;
            }
        } else {
            int num = 0, status = 1;
            lp->head = (Node *)malloc(sizeof(Node));
            np = lp->head;
            while (1) {
                status = fscanf(fp, "%d", &tmp);
                if (status == -1) {
                    over = 1;
                    break;
                }
                if (!num) {
                    np->data = tmp;
                    np->next = NULL;
                    num ++;
                    continue;
                }
                if (tmp) {
                    np->next = (Node *)malloc(sizeof(Node));
                    np->next->data = tmp;
                    np->next->next = NULL;
                    np = np->next;
                    num ++;
                } else {
                    lp->len = num;
                    lp->next = (List *)malloc(sizeof(List));
                    lp = lp->next;
                    lp->next = NULL;
                    break;
                }
            }
        }
    }
    printf("> INFO: File loaded over: nc=%d, nv=%d.\n", *ncp, *nvp);
    return 1;
}

int echo(List *L) {
    printf("------\n");
    List *lp = L;
    int i = 0;
    Node *np = NULL;
    while (lp->next) {
        np = lp->head;
        while(np) {
            printf("%d ", np->data);
            np = np->next;
        }
        printf("\n");
        lp = lp->next;
        i++;
    }
}

int abs(num) {
    return num > 0 ? num : -num;
}

int sign(num) {
    return num > 0 ? 1 : -1;
}

int count_times(List *L, int nv, int count1[], int count2[]) {
    // log:
    printf("> INFO: Count times of V:\n");
    for (int i = 0; i < nv; i++)
        printf("---");
    printf("---------");
    printf("\nVaribles: ");
    for (int i = 0; i < nv; i++)
        printf("%-2d ", i + 1);
    // count appearence times of v in cnf (len(c) > 1)
    List *lp = L;
    Node *np = NULL;
    while (lp->next) {
        np = lp->head;
        if (lp->len > 1) {
            while(np) {
                if (np->data > 0)
                    count1[abs(np->data) - 1] ++;
                else
                    count2[abs(np->data) - 1] ++;
                np = np->next;
            }
        }
        lp = lp->next;
    }
    printf("\nPosCount: ");
    for (int i = 0; i < nv; i++)
        printf("%-2d ", count1[i]);
    printf("\nNegCount: ");
    for (int i = 0; i < nv; i++)
        printf("%-2d ", count2[i]);
    // change count2 and count1:
    for (int i = 0; i < nv; i++) {
        int tmp = count1[i];
        count1[i] = count1[i] + count2[i];
        if (tmp > count2[i])
            count2[i] = 1;
        else
            count2[i] = NEG;
    }
    // log:
    printf("\nAllCount: ");
    for (int i = 0; i < nv; i++)
        printf("%-2d ", count1[i]);
    printf("\nAsgnVale: ");
    for (int i = 0; i < nv; i++)
        printf("%-2d ", (count2[i] + 1) / 2);
    printf("\n");
}

int get_value(List *L, int model[], int nv) {
    // to get the value of cnf.
    int i = 0;
    List *lp = L;
    Node *np = NULL;
    while (lp->next) {
        i ++;
        int c_value = 0;
        np = lp->head;
        while (np) {
            if (model[abs(np->data) - 1] * np->data > 0 || model[abs(np->data) - 1] == UNKNOWN) {
                c_value = 1;   // true or unknown
                break;
            }
            np = np->next;
        }
        lp = lp->next;
        if (c_value == 0) {
            // echo_model(model, nv);
            return 0;
        }
    }
    return 1;   // unknown or true.
}

int get_order(int count[], int nv, int order[]) {
    for (int i = 0; i < nv; i++)
        order[i] = i;
    for (int i = 0; i < nv; i++) {
        int max = 0, max_ix, tmp;
        for (int j = i; j < nv; j++) {
            if (count[j] > max) {
                max = count[j];
                max_ix = j;
            }
        }
        tmp = count[i];
        count[i] = count[max_ix];
        count[max_ix] = tmp;
        tmp = order[i];
        order[i] = order[max_ix];
        order[max_ix] = tmp;
    }
    printf("AgnOrder: ");
    for (int i = 0; i < nv; i++)
        printf("%-3d", order[i] + 1);
    printf("\n");
    for (int i = 0; i < nv; i++)
        printf("---");
    printf("---------");
    printf("\n");
}

int echo_model(int model[], int nv) {
    printf("> INFO: Now assignment model:\n");
    for(int i = 0; i < nv; i++)
        printf("---");
    printf("--------\nVarible:");
    for(int i = 0; i < nv; i++) {
        printf("%2d ", i + 1);
    }
    printf("\nAssign: ");
    for(int i = 0; i < nv; i++) {
        if(model[i] != UNKNOWN)
            printf("%2d ", (model[i] + 1) / 2);
        else
            printf("%2c ", 'N');
    }
    printf("\n");
    for(int i = 0; i < nv; i++)
        printf("---");
    printf("--------\n");
}

int remove_sub_list(List ** L, int ix) {
    // remove *L[ix], L = &List_p, ix is index.
    List * pre = *L, * p = *L;
    if (!ix) {
        *L = (*L)->next;
        // free(pre);
        return 1;
    } else {
        for (int i = 0; i != ix; i++) {
            pre = p;
            p = p->next;
        }
        pre->next = p->next;
        // free(p);
        return 1;
    }
}

int remove_c(List ** L, int v) {
    // remove c that has pure V.
    List *lp = *L, * pre = *L;
    Node *np = NULL;
    while (lp->next) {
        int removed = 0;
        np = lp->head;
        while(np) {
            if (abs(np->data) == v) {
                if (lp == *L)
                    *L = (*L)->next;
                else
                    pre->next = lp->next;
                removed = 1;
                break;
            }
            np = np->next;
        }
        if (!removed)
            pre = lp;
        lp = lp->next;
    }
}

int remove_pure_l(List ** L, int * nvp, int * ncp, int model[]) {
    // remove clauses with V that only appear as + or only -
    List *lp = *L;
    Node *np = NULL;
    for (int i = 0; i < *nvp; i++) {
        if (model[i] != UNKNOWN)
            continue;
        int v_sign = 0, remove_v = 1;
        lp = *L;
        while (lp->next) {
            np = lp->head;
            while(np) {
                if (i + 1 == abs(np->data)) {
                    if (v_sign * sign(np->data) < 0)
                        remove_v = 0;
                    v_sign = sign(np->data);
                }
                np = np->next;
            }
            lp = lp->next;
        }
        if (remove_v) {
            printf("> INFO: Varible[%d] only havs sign [%d], Removed.\n", i + 1, v_sign);
            model[i] = v_sign > 0 ? 1 : NEG;
            *ncp -= remove_c(L, i + 1);
        }
    }
    printf("> INFO: Pure literal removed.\n");
}

int remove_single_l(List ** L, int * nvp, int * ncp, int model[]) {
    // remove clauses with V that have single clause.
    List *lp = *L;
    Node *np = NULL;
    for (int i = 0; i < *nvp; i++) {
        if (model[i] != UNKNOWN)
            continue;
        int remove_v = 0;
        lp = *L;
        while (lp->next) {
            np = lp->head;
            while(np) {
                if (i + 1 == abs(np->data) && lp->len == 1) {
                    printf("> INFO: Varible[%d] has single clause, Removed.\n", i + 1);
                    model[i] = np->data > 0 ? 1 : NEG;
                    *ncp -= remove_c(L, i + 1);
                }
                np = np->next;
            }
            lp = lp->next;
        }
    }
    printf("> INFO: Single literal removed.\n");
}

int *dpll(List * L, int nv, int model[], int assign_order[], int pre_assign_value[], int n) {
    /*
    Args:
    ------
    - nv: num of varibles.
    - model: Assignment of varibles.
    - assign_order: the assignment order of v. stores index of each v.
    - pre_assign_value: how to assign each v.
    - n: the num of varibles that had been assigned.
    */
    // To get a backup of model:
    int *new_model = (int *)malloc(nv * sizeof(int));
    for (int i = 0; i < nv; i++)
        new_model[i] = model[i];
    if (n >= nv)
        return new_model;
    // Choose next varible to assign:
    int ix = assign_order[n], try_another = 0, skip = 0;
    if (new_model[ix] == UNKNOWN) {
        new_model[ix] = pre_assign_value[ix];
        printf("> INFO: [%d] Assign Varible NO.%d=%d\n", n, ix + 1, pre_assign_value[ix]);
    } else {
        // model[ix] already had value, skip.
        try_another = 0;
        skip = 1;
    }
    if (!skip) {
        // To check conflict:
        if (!get_value(L, new_model, nv)) {
            printf("> INFO: Conflict found, Try another value.\n");
            new_model[ix] = (pre_assign_value[ix] == 1) ? NEG : 1;
            try_another = 1;
            if (!get_value(L, new_model, nv)) {
                printf("> INFO: Conflict found, Backtrace.\n");
                free(new_model);
                return model;
            }
        }
        // To infer other varibles:
        List * lp = L;
        Node *np = NULL;
        while (lp->next) {
            np = lp->head;
            // R1: 0 0 0 0 N -> 0 0 0 0 1
            int neg_num = 0, unknown_num = 0, pos_ix = 0;
            while(np) {
                if (np->data == NEG)
                    neg_num ++;
                else if (np->data == UNKNOWN) {
                    pos_ix = abs(np->data) - 1;
                    unknown_num ++;
                }
                np = np->next;
            }
            if (unknown_num == 1 && neg_num == lp->len - 1)
                new_model[pos_ix] = 1;
            lp = lp->next;
        }
    }
    // recursive:
    int *bak_model = dpll(L, nv, new_model, assign_order, pre_assign_value, n + 1);
    if (bak_model == new_model) {
        if (try_another) {
            free(new_model);
            return model;
        } else {
            int value = (pre_assign_value[ix] == 1) ? NEG : 1;
            new_model[ix] = value;
            printf("> INFO: [%d] Get conflict, Assign Varible NO.%d=%d\n", n, ix + 1, value);
            bak_model = dpll(L, nv, new_model, assign_order, pre_assign_value, n + 1);
            if (bak_model == new_model) {
                free(new_model);
                return model;
            }
        }
    }
    // bak_model != new_model, get solution:
    printf("> INFO: Backtracing finish, return res...\n");
    return bak_model;
}