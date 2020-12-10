// Assignment 2 19T1 COMP1511: Pokedex
// pokedex.c
//
// This program was written by z5238695
// on 27/4/2019
//
// Version 1.0.0: Assignment released.
// Version 1.0.1: Minor clarifications about `struct pokenode`.
// Version 1.1.0: Moved destroy_pokedex function to correct location.
// Version 1.1.1: Renamed "pokemon_id" to "id" in change_current_pokemon.

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// Add any extra #includes your code needs here.
//
// But note you are not permitted to use functions from string.h
// so do not #include <string.h>

#include "pokedex.h"

// Add your own #defines here.


// Note you are not permitted to use arrays in struct pokedex,
// you must use a linked list.
//
// You are also not permitted to use arrays in any of the functions
// below.
//
// The only exception is that char arrays are permitted for
// search_pokemon and functions it may call, as well as the string
// returned by pokemon_name (from pokemon.h).
//
// You will need to add fields to struct pokedex.
// You may change or delete the head field.

struct pokedex {
    struct pokenode *head;
    struct pokenode *current;
};


// You don't have to use the provided struct pokenode, you are free to
// make your own struct instead.
// If you use the provided struct pokenode, you will need to add fields
// to it to store other information.

#define STATUS_NOT_FOUND 0
#define STATUS_FOUND 1


//Struct the pokenode if we found match then give 1 else give 0,
//also give the id of pokemon that can be evkoed
struct pokenode {
    struct pokenode *next;
    Pokemon         pokemon;
    int             status;  
    int             to_id;
};

// Add any other structs you define here.


// Add prototypes for any extra functions you create here.
//Return the pokenode with its pokemon ID
static struct pokenode *find_node_by_id(Pokedex pokedex, int id);

//The brief message will be shown is this function
static void brief_show_node(struct pokenode *p);

//Add them with the order
void add_pokemon_by_order(Pokedex pokedex, struct pokenode *p);

static int char_equal(char a, char b);

//If S1 contain the S2, then we will return S1
static int strcontain(const char *s1, const char *s2);

// You need to implement the following 20 functions.
// In other words, replace the lines calling fprintf & exit with your code.
// You can find descriptions of what each function should do in pokedex.h


Pokedex new_pokedex(void) {
    Pokedex new_pokedex = malloc(sizeof (struct pokedex));
    assert(new_pokedex != NULL);
    // add your own code here
    new_pokedex->head = NULL;
    return new_pokedex;
}

////////////////////////////////////////////////////////////////////////
//                         Stage 1 Functions                          //
////////////////////////////////////////////////////////////////////////

void add_pokemon(Pokedex pokedex, Pokemon pokemon) {
    struct pokenode *newNode = (struct pokenode *)malloc(sizeof(struct pokenode));
    newNode->pokemon = pokemon;
    newNode->status = STATUS_NOT_FOUND;
    newNode->to_id = -1;    
    newNode->next = NULL;
    assert(newNode != NULL);
    assert(pokedex != NULL);
    
    //If pokedex head is null then end function
    if (pokedex->head == NULL) {
        pokedex->head = newNode;
        pokedex->current = newNode;
        return;
    }
    
    //else keep collecting the new pokemon
    struct pokenode *p = pokedex->head;
    while (p->next) {
        p = p->next;
    }
    p->next = newNode;
}

void detail_pokemon(Pokedex pokedex) {
    int i, length = 0;
    struct pokenode *q = pokedex->current;
    if (q == NULL)
    return;
    Pokemon p = q->pokemon;
    printf("Id: %03d\n", pokemon_id(p));
    if(q->status == STATUS_NOT_FOUND) {
        printf("Name: ");
        char *name = pokemon_name(p);
        for(i = 0; name[i] != '\0'; i++) {
            length++;
        }
        i = 0;
        while (i < length) {
            printf("*");
            i++;
        }
        printf("\n");
        printf("Height: --\n");
        printf("Weight: --\n");
        printf("Type: --\n");
    } else {
        printf("Name: %s\n", pokemon_name(p));
        printf("Height: %.1lfm\n", pokemon_height(p));
        printf("Weight: %.1lfkg\n", pokemon_weight(p));
        if (pokemon_first_type(p) != NONE_TYPE) {
            printf("Type: %s ", pokemon_type_to_string(pokemon_first_type(p)));
        } 
        if (pokemon_second_type(p) != NONE_TYPE) {
            printf("%s", pokemon_type_to_string(pokemon_second_type(p)));
        }
        printf("\n");
    }
}

Pokemon get_current_pokemon(Pokedex pokedex) {
    if (pokedex == NULL || (pokedex->head == NULL)) {
        fprintf(stderr, "exiting because the pokedex is NULL\n");
        exit(1);
    }
    return pokedex->current->pokemon;
}

void find_current_pokemon(Pokedex pokedex) {
    struct pokenode *p = pokedex->current;
    if(p == NULL) {
        return;
    }
    p->status = STATUS_FOUND;
}

void print_pokemon(Pokedex pokedex) {
    struct pokenode *q = pokedex->head;
    if (q == NULL) {
        return;
    }
    
    while(q) {
        if (q == pokedex->current) {
            printf("--> ");
        } else {
            printf("    ");
        }
        Pokemon p = q->pokemon;
        if (q->status) {
            printf("#%03d: %s\n", pokemon_id(p), pokemon_name(p));
        } else {
            int i, length = 0;
            char *name = pokemon_name(p);
            for(i = 0;name[i] != '\0'; i++) {
                length++;
            }
            printf("#%03d: ", pokemon_id(p));
            i = 0;
            while (i < length) {
                putchar('*');
                i++;
            }
            printf("\n");
        }
        q = q->next;
    }
}

////////////////////////////////////////////////////////////////////////
//                         Stage 2 Functions                          //
////////////////////////////////////////////////////////////////////////

void next_pokemon(Pokedex pokedex) {
    assert(pokedex != NULL);
    if (pokedex->current == NULL)
    return;
    if(pokedex->current->next) {
        pokedex->current = pokedex->current->next;
    }
}

void prev_pokemon(Pokedex pokedex) {
    assert(pokedex != NULL);
    
    struct pokenode *q = pokedex->head;
    if(q == NULL || q == pokedex->current)
    return;
    while (q && q->next != pokedex->current) {
        q = q->next;
    }
    if (q->next == pokedex->current) {
        pokedex->current = q;
    }
}

void change_current_pokemon(Pokedex pokedex, int id) {
    assert(pokedex != NULL);
    
    struct pokenode *q = pokedex->head;
    while(q) {
        if(pokemon_id(q->pokemon) == id) {
            pokedex->current = q;
            return;
        }
        q = q->next;
    }
}

void remove_pokemon(Pokedex pokedex) {
    assert(pokedex != NULL);
    
    struct pokenode *q = pokedex->head;
    if(q == NULL) 
    return;
    if(q == pokedex->current) {
        pokedex->head = pokedex->head->next;
        pokedex->current = pokedex->head;
        destroy_pokemon(q->pokemon);
        free(q);
        return;
    }
    while (q && q->next != pokedex->current) {
        q = q->next;
    }
    if (q->next == pokedex->current) {
        q->next = pokedex->current->next;
        destroy_pokemon(pokedex->current->pokemon);
        free(pokedex->current);
        pokedex->current = q->next;
        if(pokedex->current == NULL) {
            pokedex->current = q;
        }
    } else {
        fprintf(stderr, "exiting because no current pokemon in the dex\n");
        exit(1);
    }
}

void destroy_pokedex(Pokedex pokedex) {
    assert(pokedex != NULL);
    
    struct pokenode *q = pokedex->head;
    struct pokenode *p;
    while(q) {
        p = q->next;
        destroy_pokemon(q->pokemon);
        free(q);
        q = p;
    }
    free(pokedex);
}

////////////////////////////////////////////////////////////////////////
//                         Stage 3 Functions                          //
////////////////////////////////////////////////////////////////////////

void go_exploring(Pokedex pokedex, int seed, int factor, int how_many) {
    assert(pokedex != NULL);
    int possible = 0;
    assert(pokedex != NULL);
    
    struct pokenode *q = pokedex->head;
    while(q) {
        if (pokemon_id(q->pokemon) < factor && pokemon_id(q->pokemon) >= 0) {
            possible = 1;
        }
        q = q->next;
    }
    
    if (possible == 0) {
        fprintf(stderr, 
        "exiting because no pokemon_id is between 0 and factor-1\n");
        exit(1);
    }
    
    srand(seed);
    int encountered = 0;
    while(encountered < how_many) {
        int rd = rand() % factor;
        q = pokedex->head;
        while(q) {
            if(pokemon_id(q->pokemon) == rd) {
                encountered += 1;
                q->status = STATUS_FOUND;
                break;
            }
            q = q->next;
        }
    }
}

int count_found_pokemon(Pokedex pokedex) {
    assert(pokedex != NULL);
    
    struct pokenode *q = pokedex->head;
    
    int count = 0;
    
    while(q) {
        if(q->status == STATUS_FOUND) {
            count++;
        }
        q = q->next;
    }
    return count;
}

int count_total_pokemon(Pokedex pokedex) {
    assert(pokedex != NULL);
    
    struct pokenode *q = pokedex->head;
    
    int found = 0;
    while(q) {
        found++;
        q = q->next;
    }
    return found;
}

////////////////////////////////////////////////////////////////////////
//                         Stage 4 Functions                          //
////////////////////////////////////////////////////////////////////////

void add_pokemon_evolution(Pokedex pokedex, int from_id, int to_id) {
    assert(pokedex != NULL);
    struct pokenode *p = find_node_by_id(pokedex, from_id);
    if(p) {
        p->to_id = to_id;
    }
}

void show_evolutions(Pokedex pokedex) {
    if(pokedex == NULL || pokedex->head == NULL) 
    return;
    int id = pokemon_id(pokedex->current->pokemon);
    struct pokenode *q = pokedex->head;
    int found = 0;
    while(q) {
        if(q->to_id == id) {
            found++;
            brief_show_node(q);
            printf(" -->  ");
            brief_show_node(pokedex->current);
            
            int to_id = pokedex->current->to_id;
            if(to_id != -1) {
                struct pokenode *p = find_node_by_id(pokedex, to_id);
                if(p) {
                    printf(" -->  ");   
                    brief_show_node(p);
                }
            }
            printf("\n");
        }
        q = q->next;
    }
}

int get_next_evolution(Pokedex pokedex) {
    if(pokedex == NULL || pokedex->head == NULL) {
        fprintf(stderr, 
        "exiting because the pokedex is empty in the"
        " get_next_evoution function in pokedex.c\n");
        exit(1);
    }
    if(pokedex->current->to_id > 0) {
        return pokedex->current->to_id;
    } else {
        return DOES_NOT_EVOLVE;
    }
}

////////////////////////////////////////////////////////////////////////
//                         Stage 5 Functions                          //
////////////////////////////////////////////////////////////////////////

Pokedex get_pokemon_of_type(Pokedex pokedex, pokemon_type type) {
    if(type == MAX_TYPE || type == NONE_TYPE || type == INVALID_TYPE) {
        fprintf(stderr, 
            "exiting because the type is incorrect in the "
            "get_pokemon_of_type function in pokedex.c\n");
        exit(1);
    }
    Pokedex newdex = new_pokedex();
    struct pokenode *q = pokedex->head;
    
    while(q) {
        if(q->status == STATUS_NOT_FOUND) {
            q = q->next;
            continue;
        }
        if(pokemon_first_type(q->pokemon) == type 
        || pokemon_second_type(q->pokemon) == type) {
            add_pokemon(newdex, clone_pokemon(q->pokemon));
        }
        q = q->next;
    }
    return newdex;
}

Pokedex get_found_pokemon(Pokedex pokedex) {
    Pokedex newdex = new_pokedex();
    struct pokenode *q = pokedex->head;
    
    while(q) {
        if(q->status == STATUS_NOT_FOUND) {
            q = q->next;
            continue;
        }
        add_pokemon_by_order(newdex, q);
        q = q->next;
    }
    return newdex;
}

Pokedex search_pokemon(Pokedex pokedex, char *text) {
    Pokedex newdex = new_pokedex();
    struct pokenode *q = pokedex->head;
    while(q) {
        if(q->status == STATUS_NOT_FOUND) {
            q = q->next;
            continue;
        }
        if(strcontain(pokemon_name(q->pokemon), text)) {
            add_pokemon(newdex, clone_pokemon(q->pokemon));
        }
        q = q->next;
    }
    return newdex;
}

// Add definitions for your own functions below.
// Make them static to limit their scope to this fil
static struct pokenode *find_node_by_id(Pokedex pokedex, int id) {
    assert(pokedex != NULL);
    
    struct pokenode *q = pokedex->head;
    
    while(q) {
        if(pokemon_id(q->pokemon) == id) {
            return q;
        }
        q = q->next;
    }
    return NULL;    
}

static void brief_show_node(struct pokenode *p) {
    if(p == NULL)
    return;
    if(p->status) {
        pokemon_type first, second;
        first = pokemon_first_type(p->pokemon);
        second = pokemon_second_type(p->pokemon);
        if(second != NONE_TYPE) {
            printf("#%03d %s [%s %s]", pokemon_id(p->pokemon), 
            pokemon_name(p->pokemon), pokemon_type_to_string(first), 
            pokemon_type_to_string(second));
        } else {
            printf("#%03d %s [%s]", pokemon_id(p->pokemon), 
            pokemon_name(p->pokemon), pokemon_type_to_string(first));
        }       
    } else {
        printf("#%03d ???? [????]", pokemon_id(p->pokemon));
    }
}

void add_pokemon_by_order(Pokedex pokedex, struct pokenode *p) {
    struct pokenode *newNode = 
    (struct pokenode *)malloc(sizeof(struct pokenode));
    newNode->pokemon = clone_pokemon(p->pokemon);
    newNode->status = p->status;
    newNode->to_id = p->to_id;
    newNode->next = NULL;
    int newid = pokemon_id(p->pokemon);
    assert(newNode != NULL);
    assert(pokedex != NULL);
    
    if(pokedex->head == NULL) {
        pokedex->head = newNode;
        pokedex->current = newNode;
        return;
    }
    
    p = pokedex->head;
    if(pokemon_id(p->pokemon) >newid) {
        newNode->next = pokedex->head;
        pokedex->head = newNode;
        pokedex->current = newNode;
        return;
    }
    
    struct pokenode *q = p;
    while(p && pokemon_id(p->pokemon) <= newid) {
        q = p;
        p = p->next;
    }
    q->next = newNode;
    newNode->next = p;
}

static int char_equal(char a, char b) {
    if(a == b)
    return 1;
    if(a >= 'a' && a <= 'z') {
        return b - a == 'A' - 'a';
    } else if(a >= 'A' && a <= 'Z') {
        return b - a == 'a' - 'A';
    }
    return 0;
}

static int strcontain(const char *s1, const char *s2) {
    int i = 0, lengthi = 0;
    int j = 0, lengthj = 0;
    for(i = 0; s1[i] != '\0'; i++) {
        lengthi++;
    }
    for(j = 0; s2[j] != '\0'; j++) {
        lengthj++;
    }
    
    while(i < lengthi) {
        j = 0;
        while(char_equal(s1[i+j], s2[j])) {
            j++;
            if(j == lengthj)
            return 1;
        }
        i++;
    }
    return 0;
}
