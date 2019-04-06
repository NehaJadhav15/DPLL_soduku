#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PATH "./data/n.cnf"
#define RES_PATH "./data/n.res"
#define UNKNOWN 0
#define NEG -1
#define UP 'w'
#define DOWN 's'
#define LEFT 'a'
#define RIGHT 'd'


typedef struct Node {
    int data;
    struct Node *next;
} Node;

typedef struct List {
    // int len;    // len of sub_list
    struct List *next;
    Node *head;
} List;

typedef struct StackCell {
    // the unit of stack, used for DPLL()
    List * L, * new_L;
    int received_value, prefer_v, tried_another;
} Cell;

List *parser(int file_no, int * nvp, int *ncp) {
    /*
    To find that if a cnf file in correct format, and read data from file.
    */
    FILE *fp;
    int start_read = 0, tmp, over = 0;
    char path[] = PATH;
    List *L = (List *)malloc(sizeof(List)), *lp = L, *pre = NULL;
    Node *np = NULL;
    path[7] = file_no + 48;
    fp = fopen(path, "r");
    if (!fp) {
        printf("> ERROR: open file failed.\n");
        exit(-1);
    }
    int i = 0;
    while (1) {
        i++;
        if (over)
            break;
        if (start_read == 0) {
            char head_c;        // the first character of a line
            int res = fscanf(fp, "%c", &head_c);
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
                exit(-1);
            }
        } else {
            int num = 0, status = 1;
            lp->head = (Node *)malloc(sizeof(Node));
            np = lp->head;
            while (1) {
                status = fscanf(fp, "%d", &tmp);
                if (status != 1) {
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
                    // lp->len = num;
                    lp->next = (List *)malloc(sizeof(List));
                    pre = lp;
                    lp = lp->next;
                    lp->next = NULL;
                    break;
                }
            }
        }
    }
    pre->next = NULL;
    fclose(fp);
    printf("> INFO: File loaded over: nc=%d, nv=%d.\n", *ncp, *nvp);
    return L;
}

int echo(List *L) {
    printf("------\n");
    List *lp = L;
    int i = 0;
    Node *np = NULL;
    while (lp) {
        np = lp->head;
        while(np) {
            printf("%d ", np->data);
            np = np->next;
            if (np)
                printf("∨ ");
        }
        printf("\n");
        lp = lp->next;
        i++;
    }
}

int abs(int num) {
    return num > 0 ? num : -num;
}

int sign(int num) {
    return num > 0 ? 1 : -1;
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

List * copy_sub_L(List* L) {
    /*
    * get a copy of sub_List L.
    * L->N->N->N->...->N->NULL
    * L can be empty.
    */
    List * L_copy = (List *)malloc(sizeof(List));
    L_copy->next = NULL;
    // L_copy->len = L->len;
    if (!L->head) {
        L_copy->head = NULL;
        return L_copy;
    }
    L_copy->head = (Node *)malloc(sizeof(Node));
    Node * np = L->head, * np_new = L_copy->head;
    while(np) {
        np_new->data = np->data;
        if (!np->next) {
            np_new->next = NULL;
            break;
        }
        np_new->next = (Node *)malloc(sizeof(Node));
        np_new = np_new->next;
        np = np->next;
    }
    return L_copy;
}

List* copy_L(List * L) {
    // echo(L);
    List * L_copy = copy_sub_L(L);
    List *lp = L->next, *lp_new = L_copy;
    while (lp) {
        lp_new->next = copy_sub_L(lp);
        if (!lp->next)
            break;
        lp_new = lp_new->next;
        lp = lp->next;
    }
    return L_copy;
}

int remove_v(List ** L, int v) {
    List *lp = *L, *pre_lp = *L;
    while (lp) {
        if (lp->head == NULL)
            return -1;
        int remove_clause = 0;
        Node *np = lp->head, *pre_np = lp->head;
        while(np) {
            if (np->data == -v) {
                // remove -v:
                if (np == lp->head) {
                    // remove the first node:
                    lp->head = lp->head->next;
                    free(np);
                    np = lp->head;
                    if (np == NULL)
                        return -1;
                } else {
                    pre_np->next = np->next;
                    free(np);
                    np = pre_np->next;
                }
                // lp->len --;
                continue;
            } else if (np->data == v) {
                // remove clause:
                Node *head = lp->head;
                if (lp == *L) {
                    *L = (*L)->next;
                    free(lp);
                    lp = (*L);
                    if (*L == NULL)
                        return 1;   // finish dpll.
                } else {
                    pre_lp->next = lp->next;
                    free(lp);
                    lp = pre_lp->next;
                }
                Node * bak = NULL;
                while(head) {
                    bak = head;
                    head = head->next;
                    free(bak);
                }
                remove_clause = 1;
                break;      // np was freed, skip the loop;
            }
            pre_np = np;
            np = np->next;
        }
        if (!remove_clause) {
            pre_lp = lp;
            lp = lp->next;
        }
    }
    return 0;   // normal status.
}

int delete_L(List * L) {
    List *lp = L, * pre_l = NULL;
    Node *np = NULL;
    int i = 0;
    while (lp) {
        Node *pre = NULL;
        np = lp->head;
        while(np) {
            pre = np;
            np = np->next;
            free(pre);
        }
        pre_l = lp;
        lp = lp->next;
        free(pre_l);
        i++;
    }
}

int add_v(List ** L, int v_data) {
    List *p = (List *)malloc(sizeof(List));
    // p->len = 1;
    p->head = (Node *)malloc(sizeof(Node));
    p->head->data = v_data;
    p->head->next = NULL;
    p->next = (*L);
    *L = p;
}

int get_next_v(List * L, int nv) {
    int max_num = 0, next_v = 0;
    for (int i = 0; i < nv; i++) {
        int now_pos_num = 0, now_neg_num = 0;
        List *lp = L;
        Node *np = NULL;
        while (lp) {
            np = lp->head;
            while(np) {
                if (np->data == i + 1)
                    now_pos_num ++;
                else if (np->data == -i - 1)
                    now_neg_num ++;
                np = np->next;
            }
            lp = lp->next;
        }
        if (now_neg_num + now_pos_num > max_num) {
            next_v = now_pos_num > now_neg_num ? i + 1 : -i - 1;
            max_num = now_neg_num + now_pos_num;
        }
    }
    return next_v;
}

int dpll_valilla(List * L, int nv, int model[], int choose_v) {
    List * new_L = copy_L(L);
    // to find if there is single clause:
    while (1) {
        List *lp = new_L;
        Node *np = NULL;
        int have_single = 0;
        while (lp) {
            np = lp->head;
            while (np) {
                if (lp->head && !lp->head->next) {
                    int value = np->data > 0 ? 1 : NEG;
                    model[abs(np->data) - 1] = value;
                    have_single = 1;
                    // printf("> INFO: Assign clause [%d].\n", np->data);
                    int res = remove_v(&new_L, np->data);
                    // echo(new_L);
                    if (res != 0)
                        return res;
                    break;
                } else if (lp->head == NULL) {
                    delete_L(new_L);
                    return -1;
                }
                np = np->next;
            }
            if (have_single)
                break;
            lp = lp->next;
        }
        if (!have_single)
            break;
    }
    // echo(L);
    if (new_L == NULL)    // empty, succes.
        return 1;
    // choose v and assign:
    int next_v;
    if (choose_v)
        next_v = get_next_v(new_L, nv);
    else
        next_v = new_L->head->data;
    model[abs(next_v) - 1] = next_v > 0 ? 1 : NEG;
    add_v(&new_L, next_v);
    if(dpll_valilla(new_L, nv, model, choose_v) == 1) {
        delete_L(new_L);
        return 1;
    } else {
        List *bak = new_L;
        new_L = new_L->next;
        free(bak->head);
        free(bak);
        add_v(&new_L, -next_v);
        return dpll_valilla(new_L, nv, model, choose_v);
    }
}

int dpll_decay(List * L, int nv, int model[], float decay[]) {
    List * new_L = copy_L(L);
    // to find if there is single clause:
    while (1) {
        List *lp = new_L;
        Node *np = NULL;
        int have_single = 0;
        while (lp) {
            np = lp->head;
            while (np) {
                if (lp->head && !lp->head->next) {
                    int value = np->data > 0 ? 1 : NEG;
                    model[abs(np->data) - 1] = value;
                    have_single = 1;
                    // printf("> INFO: Assign clause [%d].\n", np->data);
                    int res = remove_v(&new_L, np->data);
                    // echo(new_L);
                    if (res != 0)
                        return res;
                    break;
                } else if (lp->head == NULL) {
                    delete_L(new_L);
                    return -1;
                }
                np = np->next;
            }
            if (have_single)
                break;
            lp = lp->next;
        }
        if (!have_single)
            break;
    }
    // echo(L);
    if (new_L == NULL)    // empty, succes.
        return 1;
    // choose v and assign:
    int next_v = 0 ;
    for (int i = 0; i < nv; i++)
        if (decay[i] > decay[next_v])
            next_v = i;
    if (next_v + 1 > nv)
        next_v = -(next_v - nv);
    model[abs(next_v) - 1] = next_v > 0 ? 1 : NEG;
    add_v(&new_L, next_v);
    if(dpll_decay(new_L, nv, model, decay) == 1) {
        delete_L(new_L);
        return 1;
    } else {
        List *bak = new_L;
        decay[(new_L->head->data > 0) ? (new_L->head->data - 1) : -(new_L->head->data) + nv - 1] *= 0.95;
        new_L = new_L->next;
        free(bak->head);
        free(bak);
        add_v(&new_L, -next_v);
        return dpll_decay(new_L, nv, model, decay);
    }
}

int save2file(int model[], int nv, int res, double cost, int file_no) {
    char path[] = RES_PATH;
    path[7] = file_no + 48;
    FILE *fp = fopen(path, "wt");
    fprintf(fp, "%d\n", res);
    // fwrite(&res, sizeof(int), 1, fp);
    if (res == 1) {
        for (int i = 0; i < nv; i++) {
            char s;
            if (model[i] == 1)
                fprintf(fp, "%d ", i + 1);
            else
                fprintf(fp, "-%d ", i + 1);
        }
        fprintf(fp, "\n");
    }
    fprintf(fp, "%lf", cost * 1000);
}

int check(List * L, int model[], int nv) {
    for (int i = 0; i < nv; i++) {
        if (model[i] == NEG) {
            add_v(&L, -(i + 1));
        } else {
            add_v(&L, (i + 1));
        }
        remove_v(&L, i + 1);
        if (!L->head) {
            printf("> Check over! Right answer!\n");
            return 1;
        }
    }
    // echo_model(model, nv);
    // echo(L);
    if (!L->head)
        printf("> Check over! Right answer!\n");
    else
        printf("> Check over! Wrong answer!\n");
}