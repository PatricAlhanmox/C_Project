#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <math.h>
#include <ctype.h>
#include <sysexits.h>
#include <stdbool.h>
#include "invertedIndex.h"

#define MAX 100

typedef struct charListRep *CharList;
struct charListRep {
    int num;
    struct FileListNode *first;
    struct FileListNode *last;
}

char * normaliseWord(char *str);
char puncuation(char c);
InvertedIndexBST generateInvertedIndex(char *collectionFilename);
void printInvertedIndex(InvertedIndexBST T); 
InvertedIndexBST insertBSTree(InvertedIndexBST T, char *word);
InvertedIndexBST newInvertedIndexBSTNode (char *v);
InvertedIndexBST searchBSTree(InvertedIndexBST T, char *v);
FileList searchList(FileList L, char *v); 
FileList insertNode(FileList L, char *s);
CharList newNode(FileList L, char *v)
FileList newList(void);
int countSameWord (InvertedIndexBST T, char *collectionFilename); 
double calculateTfIdfsum (double *tf, int D, double numFile);


//Test function:
/*
int main () {
    //A file pointer to opening the file
    FILE * fPointer;
    
    //Opening collection file
    fPointer = fopen("collection.txt", "r");
    
    //make sure this file does exist
    if (fPointer == NULL) {
        fprintf(stderr,"file does not exist\n");
        return 1;
    }
    
    //call a memory location to store the words
    char *s = malloc(sizeof(char)*100);

    int countFile, countWord, tf;
    
    
    //normalise the words
    countFile = countWord = 0;
    while (fscanf(fPointer, "%s", s) != EOF) {
        countFile++;
        
        //create a list for each of the file
        FileList wordFileList = newList(s);
        
        //A pointer that pointed to each file in collection
        FILE *innerPointer;
        
        char *keyWord = malloc(sizeof(char)*100);  
        keyWord = s;
        
        
        innerPointer = fopen(s, "r");
        //read each subfiles in the file
        if (innerPointer == NULL) {
            fprintf(stderr, "file does not exist\n");
            return 1;
        }
        int countWord, sameWord;
        countWord = 0;
        sameWord = 0;
        while (fscanf(innerPointer, "%s", s) != EOF) {
            normaliseWord(s);
            countWord++;
            if(strcmp(keyWord, s) == 0) sameWord++;
            tf = generateInvertedIndex(s);
        }
        fclose(innerPointer);
    }
    
    
    free(s);
    
    fclose(fPointer);
}
*/

//Normalise the words
char * normaliseWord(char *str) { 
    int i = 0;
    int len = strlen(str);
    while(i < len) {
        putchar(tolower(str[i]));
        if (i == (len - 1)) {
            if (puncuation(str[i]))  str[i] = '\0';
        }
        i++;
    } 
    return str;  
}

//Check if there are the same symbols 
char puncuation(char c) {   
    if (c == '.' || c == ',' || c == ';' || c == '?' || c == ' ') {
        return 0;
    }
    else return 1;
} 

InvertedIndexBST generateInvertedIndex(char *collectionFilename) {
    //number of word repeat
    int num_word_repeated = 0;
    
    //the pointer opening collection.txt file
    FILE * fPointer;
    
    //the pointer opening nasa.txt file
    FILE * innerPointer;
    fPointer = fopen(collectionFilename, "r");
    
    //make sure this file does exist
    if (fPointer == NULL) {
        fprintf(stderr,"file does not exist\n");
        return NULL;
    }
    
    //create a new tree
    InvertedIndexBST newTree = NULL;
    
    //create an array to collect subfiles name
    char collection[100];
    
    //countfile counts num of file; countTotalword count total word; 
    //countword count how much word for each of file
    int countFile, countTotalWord;
    countTotalWord = 0;
    countFile = 0;
    
    //Start scanning file
    while (fscanf(fPointer, "%s", collection) != EOF) {
    
        //create a new pointer for subfiles
        innerPointer = fopen(collection, "r");
        char array[100];
        
        //count how many files there are
        countFile++;
        int countFileWord = 0;
        
        //make sure this file does exist
        if (innerPointer == NULL) {
            fprintf(stderr,"file does not exist\n");
            return NULL;
        }
        
        //Create a linked list for the subfiles
        FileList L = malloc(sizeof(FileList));
        L = newList();
        newNode(L, collection);
        insertNode(L, collection);
        
        //scanning each subfile
        while (fscanf(fPointer, "%s", array) != EOF) {
            countFileWord++;
            countTotalWord++;
            
            //count the num of duplication of words
            if (strcmp(newTree->word, array) == 0) {
                num_word_repeated = countSameWord(newTree, array);
            }
            
            //check if the key word already exist, if no insert into tree
            //and create list followed it; if yes add it directly to the list
            if(searchBSTree(newTree, array) == NULL) {
                //insert tree
                insertBSTree(newTree, array);               
                //add the linked list to the tree
                newTree->fileList = L;    
            } else {
                //already exist insert directly into exist list          
                insertNode(L, array);
            }
            //count D
            while(newTree->fileList != NULL) {
                num_word_repeated++;
                newTree->fileList = newTree->fileList->next;
            }
            
            //reset the pointer for subfiles
            rewind(innerPointer);
        }
        fclose(innerPointer);
        
        //print out the name of node and corresponded files
        printInvertedIndex(newTree);
     }
     
     //close the file
    fclose(fPointer);
}

void printInvertedIndex(InvertedIndexBST T) {
    if (T == NULL) return;
    printf("%s ", T->word);
    while(T->fileList != NULL) {
        printf("%s ", T->fileList->filename);
        T->fileList = T->fileList->next;
    }
    printf("\n");
    printInvertedIndex(T->left);
    printInvertedIndex(T->right);
}

//create a new list empty list
FileList newList(void) {
    struct charListRep *L = malloc (sizeof(L));
    if (L == NULL) err (EX_OSERR, "couldn't allocate IntList");
    L->num = 0
    L->first = L->last = NULL;
    
    return L;
}

//create a new node to the list
CharList newNode(FileList L, char *v) {
    FileList new = malloc(sizeof(new));
    strcmp(L->filename, v);
    new->next = NULL;
    L->last->next = new;
    L->last = new;
    L->tf = 0;
    
    return L;
}

//search the whole list that linked from tree
FileList searchList(FileList L, char *v) {
    while (L != NULL) {
        if (strcmp(L->filename, v) != 0) return NULL;
        else return L;
        L = L->next;
    }
}

//create a new tree;
InvertedIndexBST newInvertedIndexBSTNode (char *v) {
    InvertedIndexBST new = malloc(sizeof(InvertedIndexBST));
    if (new == NULL) err (EX_OSERR, "couldn't allocate node");
    new->fileList = NULL;
    new->word = malloc(sizeof(char)*100);
	strcpy(new->word, v);
	new->left = new->right = NULL;
	return new;
}

//insert the node to list
FileList insertNode(CharList L, char *s) {
    
    FileList n = newNode(L, s);
    if(L->first != NULL) {
        L->first = L->last = n;
    } else {
        L->last->next = n;
        L->last = n;
    }
    L->num++;
    return L;
}

//insert the node into tree
InvertedIndexBST insertBSTree(InvertedIndexBST T, char *v) {
    if (T == NULL) return newInvertedIndexBSTNode(v);
    else if (strcmp(T->word, v) > 0) { 
        T->left = insertBSTree(T->left, v);
    }
    else if (strcmp(T->word, v) < 0) { 
        T->right = insertBSTree(T->right, v);
    }
    
}

//search the tree
InvertedIndexBST searchBSTree(InvertedIndexBST T, char *v) {
    if (T == NULL) return NULL;
    
    InvertedIndexBST tmp = NULL;
    
    if (strcmp(T->word, v) > 0) {
        tmp = searchBSTree(T->right, T->word);
    } else if (strcmp(T->word, v) < 0) {
        tmp = searchBSTree(T->left, T->word);
    } else if (strcmp(T->word, v) == 0) {
        tmp = T;
    }
    
    return tmp;
}

//print tree node
void showBSTreeNode (InvertedIndexBST T) {
	if (T == NULL) return;
	showBSTreeNode (T->left);
	printf("%s ", T->word);
	showBSTreeNode (T->right);
}

//count the same word
int countSameWord (InvertedIndexBST T, char *collectionFilename) {
    int count = 0;
    if(strcmp(T->word, collectionFilename) == 0) {
        count++;
    }
    return count;
}

//calculate the TfIdfsum
double calculateTfIdfsum (double* tf, int D, double numFile) {
    double sum = 0;
    sum = tf * log(D / numFile);
    return sum;
}
