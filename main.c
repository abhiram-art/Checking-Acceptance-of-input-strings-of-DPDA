#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct stack {
    char content;
    struct stack *next;
} Stack;

typedef struct transistion {
    char current_state;
    char input_symbol;
    char pull;
    char new_state;
    char push;
} Transistion;

typedef struct list {
    Transistion *content;
    struct list *next;
} List;

typedef struct pda {
    char *input_alpha;
    char *stack_alpha;
    char *input;
    char start;
    char *accept;
    List *transistion;
} PDA;

Stack *create_stack( void ) {
    Stack *s = calloc(1,sizeof(Stack));
    if(s==NULL) {
        printf("Out of Memory!");
        exit(1);
    }
    return s;
}

int isempty( Stack *s ) {
    return (s->next==NULL)? 1:0;
}

Stack *push_stack (Stack *s, char c) {
    Stack *new = calloc(1,sizeof(Stack));
    if(new ==NULL) {
        printf("Out of Memory!");
        exit(1);
    }
    new -> content = c;
    new -> next = s;
    return new;
}

Stack *pull_stack (Stack *s) {
    Stack *head;
    if(isempty(s)) {
        return '\0';
    }
    head = s;
    s = head -> next;
    return s;
}

char top (Stack *s) {
    return s->content;
}


Stack *replace(Stack *sta, char ontop, char newvalue, char epsilon) {
    if(ontop == epsilon && newvalue == epsilon) return sta;
    if(ontop == epsilon && newvalue != epsilon) {
        sta = push_stack(sta,newvalue);
        return sta;
    }
    if(ontop != epsilon && newvalue == epsilon) {
        if(ontop != top(sta)) return NULL;
        sta = pull_stack(sta);
        return sta;
    }
    if(ontop != top(sta)) return NULL;
    sta = pull_stack(sta);
    sta = push_stack(sta,newvalue);
    return sta;
}

Transistion *get_transistion(char *s) {
    Transistion *t = calloc(1,sizeof(Transistion));
    t->current_state = s[0];
    t->input_symbol = s[1];
    t->pull = s[2];
    t->new_state = s[3];
    t->push = s[4];
    return t;
}

List *insert_list( List *l, char *elem ) {
    List *t = calloc(1,sizeof(List));
    List *head = l;
    while(l->next!=NULL)
        l = l->next;
    t->content = get_transistion(elem);
    t->next = NULL;
    l->next = t;
    return head;
}

List *insert_list_transistion( List *l, Transistion *tr) {
    List *t = calloc(1,sizeof(List));
    List *head = l;
    while(l->next!=NULL)
        l = l->next;
    t->content = tr;
    t->next = NULL;
    l->next = t;
    return head;
}

int contains ( char c, char *s ) {
    int i=0;
    while(1) {
        if(c== s[i]) return 1;
        if(s[i] == '\0') return 0;
        i++;
    }
}

int is_valid_input( char *input_alpha, char *input ) {
    int i=0;
    char c;
    while(1) {
        c = input[i];
        if(c == '\0') break;
        if(!contains(c,input_alpha)) return 0;
        i++;
    }
    return 1;
}

int is_valid_transistion ( List *l, PDA *m) {
    Transistion *t;
    while(1) {
        if(l==NULL) break;
        t = l->content;
        if(!contains(t->input_symbol,m->input_alpha)) return 0;
        if(!contains(t->pull,m->stack_alpha)) return 0;
        if(!contains(t->push,m->stack_alpha)) return 0;
        l = l->next;
    }
    return 1;
}

PDA *createPDA (char *input) {
    PDA *m = calloc(1,sizeof(PDA));
    List *tr = calloc(1,sizeof(List));
    char *buffer;
    char *epsilon = calloc(1,sizeof(char));
    
    buffer = strtok(input,":");
    if(buffer == NULL) {
        printf("Error in reading input alphabet!\n");
        exit(1);
    }
    m->input_alpha = buffer;
    epsilon[0] = m->input_alpha[0];
    buffer = strtok(NULL,":");

    if(buffer == NULL) {
        printf("Error in reading stack alphabet!\n");
        exit(1);
    }
    m->stack_alpha = buffer;

    buffer = strtok(NULL,":");

    if(buffer == NULL) {
        printf("Error in reading input sequence!\n");
        exit(1);
    }

    if(!is_valid_input(m->input_alpha,buffer)) {
        printf("Error! Input contains some invalid characters that don't match the input alphabet!\n");
        exit(1);
    }

    m->input = buffer;
    
    buffer = strtok(NULL,":");
    if(buffer == NULL) {
        printf("Invalid string!\n");
        exit(1);
    }
    m->start = buffer[0];

    buffer = strtok(NULL,":");
    if(buffer == NULL) {
        printf("Invalid string!\n");
        exit(1);
    }
    m->accept = buffer;

    while(1) {
        buffer = strtok(NULL,":");
        if(buffer == NULL) break;
        tr = insert_list(tr,buffer);
    }

    if(!is_valid_transistion(tr->next,m)) {
        printf("Error! Invalid transistion functions!\n");
        exit(1);
    }

    m->transistion = tr->next;
    return m;
}

void print_stack2(Stack *s) {
    if(s==NULL) {
        return;
    }
    print_stack2(s->next);
    printf("%c",s->content);
}

void print_stack(Stack *s) {
    print_stack2(s);
    printf("\n");
}

Transistion *find_transistion(List *list,char input,char current,char e) {
    Transistion *t;
    while(1) {
        if(list==NULL) return NULL;
        t = list -> content;
        if(t->current_state == current && t->input_symbol == input)
            return t;
        if(t->current_state == current && t->input_symbol == e)
            return t;
        list = list->next;
    }
}

int isAccept(char current, char* accept) {
    int i=0;
    while(1) {
        if(accept[i]=='\0') return 0;
        if(accept[i]==current) return 1;
        i++;
    }
}

void simulate(PDA *m) {
    const char epsilon = m->input_alpha[0];
    char current_state = m->start;
    char input;
    int i=0;
    Stack *sta = create_stack();
    Transistion *current_transistion;
    Stack *backup;

    while(1) {
        
        input = m->input[i];

        if(input == '\0'&&isAccept(current_state,m->accept)) {
            printf("Accept\n");
            print_stack(sta);
            break;
        }

        current_transistion = find_transistion(m->transistion,input,current_state,epsilon);

        if(current_transistion==NULL) {
            printf("Reject\n");
            print_stack(sta);
            break;
        }

        current_state = current_transistion->new_state;
        backup = sta;
        sta = replace(sta, current_transistion->pull, current_transistion->push,epsilon);
        if(sta == NULL) {
            printf("Reject\n");
            print_stack(backup);
            break;
        }
        if(current_transistion->input_symbol != epsilon&&current_transistion->input_symbol != '\0')
            i++;
    }
}

void print(PDA *m) {
    printf("input alphabet:%s\n",m->input_alpha);
    printf("stack alphabet:%s\n",m->stack_alpha);
    printf("input sequence:%s\n",m->input);
    printf("start state:%c\n",m->start);
    printf("accept state:%s\n",m->accept);
}

int main(void) {
    char s[300];
    PDA *p;
    scanf("%s",s);
    p = createPDA(s);
    simulate(p);
    print(p);
    return 0;
}
