// Assignment 1 19T1 COMP1511 Coco
//
// This program was written by (z5238695)
// on 24/3/2018
//

#include <stdio.h>
#include <assert.h>
#include <math.h>


#define ACTION_PLAYER_NAME    0
#define ACTION_DISCARD        1
#define ACTION_PLAY_CARD      2

#define N_CARDS              40
#define N_CARDS_INITIAL_HAND 10
#define N_PLAYERS             4
#define N_CARDS_DISCARDED     3

#define CARD_MIN             10
#define CARD_MAX             49
#define DOUGLAS              42


// IF YOU NEED MORE #defines ADD THEM HERE
#define N_PLAYED_PRE_MAX     36
#define N_HAVE_PLAYED         3


void print_player_name(void);
void choose_discards(void);
void choose_card_to_play(void);


// ADD PROTOTYPES FOR YOUR FUNCTIONS HERE
//Check is the card prime or not
int primeCardArray(int tony_card); 
//Check is the card  cocomposite 
int is_cocompositeCard(int card, int other_card);  

void scan_array (int length, int array[]);         

//as long as their are prime card then pick it otherwise play the first card
int prime_card(int tony_card[], int length, int x);       

//as long as other people play cocomposite play cocomposite otherwise play first
int cocompositeCard(int m, int tony_card[], int length, int x);  

//Check does card array have any prime card
int prime_in_array(int tony_card[], int length);            

// If round have prime or douglas try not to win
int dont_win(int tony_card);                                      
int avoidWining (int tony_card[], int length);

// You should not need to change this main function

int main(void) {

    int which_action = 0;
    scanf("%d", &which_action);
    if (which_action == ACTION_PLAYER_NAME) {
        print_player_name();
    } else if (which_action == ACTION_DISCARD) {
        choose_discards();
    } else if (which_action == ACTION_PLAY_CARD) {
        choose_card_to_play();
    }

    return 0;
}

void print_player_name(void) {

    printf("Tony\n");      
    
}

void choose_discards() {

    int counter, i;                 
                                   
    int tony_card[N_CARDS_INITIAL_HAND];        
    int tag[N_CARDS_INITIAL_HAND];
    counter = 0;
    i = 0;
    while(counter < N_CARDS_INITIAL_HAND) {
        scanf("%d ", &tony_card[counter]);
        tag[counter] = 0;
        counter++;
    }
    
    // CHOOSEING AND PRINTING THE CARDS YOU WISH TO DISCARD
    int discard[N_CARDS_INITIAL_HAND] = {0, 0, 0};      
    counter = 0;
    while (counter < N_CARDS_INITIAL_HAND && i < N_CARDS_INITIAL_HAND) {    
        if(dont_win(tony_card[counter]) == 0) {
            discard[i] = tony_card[counter];
            tag[counter] = 1;
            i++;
        }
        counter++;
    }

    // WHICH IS NOT LEGAL AND WILL RECEIVE PENALTY POINTS
    // YOU MUST CHANGE IT TO DISCARD CARDS FROM YOUR HAND.
   i = 0;
   while(i < N_CARDS_INITIAL_HAND) {                 
       for(counter = 0;counter < N_CARDS_INITIAL_HAND;counter++) {
           if(tag[counter] != 1) {
               discard[i] = tony_card[counter];
               tag[counter] = 1;
               break;
           }
       }
       i++;
   }
   printf("%d, %d, %d\n", discard[0], discard[1], discard[2]);
}


void choose_card_to_play(void) {
    // ADD CODE TO READ THE FIRST THREE NUMBERS WHICH ARE:   
    // NUMBER OF CARDS IN YOUR HAND,  NUMBER OF CARDS PLAYED THIS ROUND, TABLE POSITION
    int num_holding, num_played, num_played_before, pos, i, x;
    int noPrime = 0;
    int cards_in_hand [N_CARDS_INITIAL_HAND];
    int recv_card[N_CARDS_DISCARDED];
    int played[N_CARDS_INITIAL_HAND];
    int player_discard[N_CARDS_DISCARDED];
    int cards_played_before[N_PLAYED_PRE_MAX];
    int cards_this[N_HAVE_PLAYED];
    int card_to_play = 0;
    scanf("%d ", &num_holding);
    scanf("%d ", &num_played);
    scanf("%d ", &pos);
    num_played_before = (N_CARDS_INITIAL_HAND - num_holding) * N_PLAYERS;

    // Read the card in my hand to array    
    scan_array (num_holding, cards_in_hand);

    // Read card previously played in this round to array   
    scan_array (num_played, played);

    // ADD CODE TO READ THE CARDS PLAYED IN THE HISTORY OF THE GAME INTO AN ARRAY
    
    scan_array (num_played_before, cards_played_before);
    
    scan_array (N_CARDS_DISCARDED, player_discard);
    
    scan_array (N_CARDS_DISCARDED, recv_card);
    
    //When the player is not playing at first position
    // This is checking cards in first round
    if (num_played > 0) {                           
        x = avoidWining(cards_in_hand, num_holding);  
        //if first have prime then play prime 
        //if no prime, then play cocomposite 
        if(primeCardArray(played[0])) {
            card_to_play = prime_card(cards_in_hand, num_holding, x);   
        } else {
            card_to_play = cocompositeCard(played[0], cards_in_hand, num_holding, x);
        }
    //we are playing as the first  and check restrinction
    } else if (prime_in_array(cards_played_before, num_played_before) == 1){  
        card_to_play = prime_card(cards_in_hand, num_holding, 0);    
    } else {
        card_to_play = noPrime;
    }
    
    printf("%d\n", card_to_play);

}

// ADD YOUR FUNCTIONS HERE
//This function is used to given if there are prime number

//scan the array that we need 
void scan_array (int length, int array[]) {
    int i = 0;
    while (i < length) {
        scanf("%d ", &array[i]);
        i++;
    }
}

//Discard prime card
int primeCardArray(int tony_card) {        
    int primeArray[] = {11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47};
    
    //sizeof(Array) / sizeof(array type) = array size
    const int numberPrime = sizeof(primeArray) / sizeof(int);         
                                                                      
    int counter = 0;
    // Check is there any prime number in Player hand
    while(counter < numberPrime) {                
        if(tony_card == primeArray[counter]) {
            return 1;
        }
        counter++;
    }
    return 0;
}

//Player will select prime card to play
int prime_card(int tony_card[], int length, int x) {
    int i;
    if(x) {
        i = 0;
        while (i < length) {
            if(primeCardArray(tony_card[i]) == 1) {
                return tony_card[i];
            }
            i++;
        }
        return tony_card[0];
    } else {
        i = length - 1;
        while(i >= 0) {
            if(primeCardArray(tony_card[i]) == 1) {
                return tony_card[i];    
            }
            i--;
        }
        return tony_card[0];
    }
    
}

//Test if they are cocomposite
int is_cocompositeCard(int card, int other_card) {           
    int i;
    if(card > other_card) {
        i = 2;
        while(i < (sqrt(other_card))) {
            if(card % i == 0 && other_card % i == 0) {
                return 1;
            }
            i++;
        }
    } 
    else if (card < other_card) {
        i = 2;
        while(i < (sqrt(card))) {
            if(card % i == 0 && other_card % i == 0) {
                return 1;
            }
            i++;
        }
    } 
    
    return 0;
}

//Player will select cocomposite card to play
int cocompositeCard(int m, int tony_card[], int length, int x) {
    int i;
    i = 0;
    if(x) {
        while(i < length) {
            if(is_cocompositeCard(m, tony_card[i]) == 1) {
                return tony_card[i];
            }
            i++;
        }
        return tony_card[0]; 
    } else {
        i = length - 1;
        while(i > 0) {
            if(is_cocompositeCard(m, tony_card[i]) == 1) {
                return tony_card[i];
            }
            i--;
        }
        
        //if there are no cocomposite card then play the first card
        return tony_card[0];
    }    
}

//Check if cards have prime
int prime_in_array(int tony_card[], int length) {
    int i = 0;
    while(i < length) {
        if(primeCardArray(tony_card[i]) == 1) {
            return 1;
        }
        i++;
    }
    return 0;
}

// This function is use to avoid winning and being an indicator
int dont_win(int tony_card) {  
    //If there are any number equal to prime or 42 we will return 1          
    if(primeCardArray(tony_card) == 1 || tony_card == DOUGLAS) {   
                                                                   
        return 1;
    } else {
        return 0;
    }
}

//Try not to win
int avoidWining (int tony_card[], int length) {    
    int i = 0;
    while(i < length) {
        if(dont_win(tony_card[i])) {
            return 1;
        }
        i++;
    }
    return 0;
}
