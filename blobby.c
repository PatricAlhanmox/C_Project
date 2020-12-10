// blobby.c
// blob file archiver
// COMP1521 20T3 Assignment 2
// Written by z5238695

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h> 

// the first byte of every blobette has this value
#define BLOBETTE_MAGIC_NUMBER          0x42

// number of bytes in fixed-length blobette fields
#define BLOBETTE_MAGIC_NUMBER_BYTES    1
#define BLOBETTE_MODE_LENGTH_BYTES     3
#define BLOBETTE_PATHNAME_LENGTH_BYTES 2
#define BLOBETTE_CONTENT_LENGTH_BYTES  6
#define BLOBETTE_HASH_BYTES            1

// maximum number of bytes in variable-length blobette fields
#define BLOBETTE_MAX_PATHNAME_LENGTH   65535
#define BLOBETTE_MAX_CONTENT_LENGTH    281474976710655


// ADD YOUR #defines HERE


typedef enum action {                                   //Enum means each element value is from 0 and then add one
    a_invalid,
    a_list,
    a_extract,
    a_create
} action_t;


void usage(char *myname);
action_t process_arguments(int argc, char *argv[], char **blob_pathname,
                           char ***pathnames, int *compress_blob);

void list_blob(char *blob_pathname);
void extract_blob(char *blob_pathname);
void create_blob(char *blob_pathname, char *pathnames[], int compress_blob);

uint8_t blobby_hash(uint8_t hash, uint8_t byte);


// ADD YOUR FUNCTION PROTOTYPES HERE
void iterFunction(FILE* input_stream_pointer, char* pathnames, int repeat_time);
int check_dot(char* store, int start);
// YOU SHOULD NOT NEED TO CHANGE main, usage or process_arguments

int main(int argc, char *argv[]) {
    char *blob_pathname = NULL;
    char **pathnames = NULL;
    int compress_blob = 0;
    action_t action = process_arguments(argc, argv, &blob_pathname, &pathnames,
                                        &compress_blob);

    switch (action) {
    case a_list:
        list_blob(blob_pathname);
        break;

    case a_extract:
        extract_blob(blob_pathname);
        break;

    case a_create:
        create_blob(blob_pathname, pathnames, compress_blob);
        break;

    default:
        usage(argv[0]);
    }

    return 0;
}

// print a usage message and exit

void usage(char *myname) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "\t%s -l <blob-file>\n", myname);
    fprintf(stderr, "\t%s -x <blob-file>\n", myname);
    fprintf(stderr, "\t%s [-z] -c <blob-file> pathnames [...]\n", myname);
    exit(1);
}

// process command-line arguments
// check we have a valid set of arguments
// and return appropriate action
// **blob_pathname set to pathname for blobfile
// ***pathname set to a list of pathnames for the create action
// *compress_blob set to an integer for create action

action_t process_arguments(int argc, char *argv[], char **blob_pathname,
                           char ***pathnames, int *compress_blob) {
    extern char *optarg;
    extern int optind, optopt;
    int create_blob_flag = 0;
    int extract_blob_flag = 0;
    int list_blob_flag = 0;
    int opt;
    while ((opt = getopt(argc, argv, ":l:c:x:z")) != -1) {
        switch (opt) {
        case 'c':
            create_blob_flag++;
            *blob_pathname = optarg;
            break;

        case 'x':
            extract_blob_flag++;
            *blob_pathname = optarg;
            break;

        case 'l':
            list_blob_flag++;
            *blob_pathname = optarg;
            break;

        case 'z':
            (*compress_blob)++;
            break;

        default:
            return a_invalid;
        }
    }

    if (create_blob_flag + extract_blob_flag + list_blob_flag != 1) {
        return a_invalid;
    }

    if (list_blob_flag && argv[optind] == NULL) {
        return a_list;
    } else if (extract_blob_flag && argv[optind] == NULL) {
        return a_extract;
    } else if (create_blob_flag && argv[optind] != NULL) {
        *pathnames = &argv[optind];
        return a_create;
    }

    return a_invalid;
}


// list the contents of blob_pathname

void list_blob(char *blob_pathname) {
    
    FILE* fd = fopen(blob_pathname, "r");
    if (fd == NULL) perror(blob_pathname);
    int c = fgetc(fd), mode, file_length, indicated, byte_count;
    char* fileName;                                                 //Name of the target file
    int* indicator;                                                 //Pointer to the hash value
    long content_length;                                            //How much words in the file
    while(1) {
        mode = 0, file_length = 0, indicated = 0, byte_count = 0;
        content_length = 0;
        fileName = NULL;
        while (byte_count <= 12+file_length+content_length) {  
            if (byte_count == 0) {                                  //Take the B into hash table and move pointer
                indicated = blobby_hash(0, c);                 
                indicator = &indicated;
                byte_count++;
                c = fgetc(fd);
            } else if (byte_count >= 1 && byte_count < 4) {         //Calucluate the mode and move the pointer
                if (byte_count == 1) {
                    mode = c << 16;
                } else if (byte_count == 2) {
                    mode = mode | c << 8;
                } else if (byte_count == 3) {
                    mode = mode | c;
                }
                indicated = blobby_hash(*indicator, c);
                indicator = &indicated;
                byte_count++;
                c = fgetc(fd);
            } else if (byte_count >= 4 && byte_count <= 5) {        //Calculate the file length and move the hash pointer
                if (byte_count == 4) file_length = c << 8;
                else file_length = file_length | c;
                indicated = blobby_hash(*indicator, c);
                indicator = &indicated;
                byte_count++;
                c = fgetc(fd);
            } else if (byte_count >= 12 && byte_count < 12 + file_length) {     //Get the File Names and move the hash pointer
                if (byte_count== 12) {
                    fileName = malloc(sizeof(char)*file_length+1);
                    fileName[byte_count - 12] = c;
                    fileName[file_length] = '\0';
                } else fileName[byte_count - 12] = c;
                indicated = blobby_hash(*indicator, c);
                indicator = &indicated;
                byte_count++;
                c = fgetc(fd);
            } else if (byte_count > 5 && byte_count < 12) {          //Calculate the lenth of the file and move the pointer back
                for (int tmp = 0; tmp < 6; tmp++) {
                    content_length = content_length << 8;
                    content_length += c;
                    indicated = blobby_hash(*indicator, c);
                    indicator = &indicated;
                    byte_count++;
                    c = fgetc(fd);
                }
            } else {                                                // Keep moving the pointer to the end of file
                indicated = blobby_hash(*indicator, c);
                indicator = &indicated;
                if (indicator == NULL) break;
                byte_count++;
                c = fgetc(fd);
            }
            
        }
        printf("%06lo %5lu %s\n", (long)mode, content_length, fileName);
        free(fileName);
        if(fgetc(fd) == -1) break;
        fseek(fd, -1, SEEK_CUR);
        
    }
    fclose(fd);
}


// extract the contents of blob_pathname

void extract_blob(char *blob_pathname) {

    FILE* output_stream_pointer, *input_stream_pointer = fopen(blob_pathname, "r");  // Create two pointer for files u need to compare
    int c = fgetc(input_stream_pointer), byte_count, file_length, content_length, hashed_value, mode;
    char* fileName;
    int* indicator;
    while (1) {
        byte_count = 0, file_length = 0, content_length = 0, mode = 0;
        fileName = NULL;
        hashed_value = blobby_hash(0, c);
        indicator = &hashed_value;
        while(byte_count < 12+file_length+content_length) {
            if (byte_count >= 4 && byte_count <= 5) {
                if (byte_count == 4) file_length = c << 8;
                else file_length = file_length | c;
            } else if (byte_count >= 1 && byte_count < 4) {
                if (byte_count == 1) {
                    mode = c << 16;
                } else if (byte_count == 2) {
                    mode = mode | c << 8;
                } else if (byte_count == 3) {
                    mode = mode | c;
                }
            } else if (byte_count > 7 && byte_count < 12) {
                if (c != 0x48 && byte_count != 11) content_length = content_length | c << ((11-byte_count)*8);
                else content_length = content_length | c;
            } else if (byte_count >= 12 && byte_count < 12 + file_length) {
                if (byte_count== 12) {
                    fileName = malloc(sizeof(char)*file_length+1);
                    fileName[byte_count - 12] = c;
                    fileName[file_length] = '\0';
                } else fileName[byte_count - 12] = c;
            }
            byte_count++;
            c = fgetc(input_stream_pointer);                    //File pointer moving forward
            hashed_value = blobby_hash(*indicator, c);          //hash pointer moving forward to hash
            indicator = &hashed_value;
        }
        printf("Extracting: %s\n", fileName);
        output_stream_pointer = fopen(fileName, "w");
        fseek(input_stream_pointer, -content_length-1, SEEK_CUR); //Jump back to the text
        int cc = fgetc(input_stream_pointer);
        for (int count = 0; count < content_length; count++) {  //Put text into the file
            fputc(cc, output_stream_pointer);
            cc = fgetc(input_stream_pointer);
            hashed_value = blobby_hash(*indicator, c);
            indicator = &hashed_value;
        }
         
        hashed_value = blobby_hash(*indicator, c);
        indicator = &hashed_value;
        fclose(output_stream_pointer);
        chmod(fileName, mode);
        if(fgetc(input_stream_pointer) == -1) break;
        
        free(fileName);
    }

    fclose(input_stream_pointer);
}

// create blob_pathname from NULL-terminated array pathnames
// compress with xz if compress_blob non-zero (subset 4)

void create_blob(char *blob_pathname, char *pathnames[], int compress_blob) {
    FILE* input_stream_pointer = fopen(blob_pathname, "w");
    for (int indi = 0; pathnames[indi] != NULL; indi++) {
        if (compress_blob == 0) {
            iterFunction(input_stream_pointer, pathnames[indi], 0);
        } else {
            printf("Adding: %s\n", pathnames[indi]);
        }
    }
    fclose(input_stream_pointer);
}


// ADD YOUR FUNCTIONS HERE
void iterFunction(FILE* fp, char* pathnames, int repeat_time) {
    int hash_value = 0;
    if (repeat_time != 0) printf("Adding is: %s\n", pathnames);
    fputc('B', fp);
    hash_value = blobby_hash(hash_value, 'B');
    struct stat target;
    int path_length = strlen(pathnames);
    lstat(pathnames, &target);
    for (int i = 2; i >= 0; i--) {
        fputc(target.st_mode >> (i*8), fp);
        hash_value = blobby_hash(hash_value, target.st_mode >> (i*8));
    }
    
    for (int i = 1; i >= 0; i--) {
        fputc(path_length >> (8*i), fp);
        hash_value = blobby_hash(hash_value, path_length >> (8*i));
    }
    for (int i = 5; i >= 0; i--) {
        fputc(target.st_size >> (i*8), fp);
        hash_value = blobby_hash(hash_value, target.st_size >> (i*8));
    }
    for (int i = 0; i < path_length; i++) {
        fputc(pathnames[i], fp);
        hash_value = blobby_hash(hash_value, pathnames[i]);
    }
    
    if ((S_IFDIR & target.st_mode)) {
        
        for (int i = 5; i >= 0; i--) {
            fputc(0, fp);
        } 
        
        char FIleName[999] = {0};
        char example[100], copy_example[100];
        memset(FIleName, '\0', sizeof(FIleName));
        const char s[2] = "/";
        strcpy(FIleName, pathnames);
        char* token = strtok(FIleName, s);
        strcpy(example, token);
        strcpy(copy_example, example);
        int countPart = 0;
        while (token != NULL) {
            token = strtok(NULL, s);
            if (token != NULL) {
                strcat(example, "/");
                strcat(example, token);
            }
            countPart++;
        }
        
        if (repeat_time == 0) printf("Adding is: %s\n", copy_example);
        repeat_time++;
        
        DIR* d = opendir(example);
        if (d == NULL) {
            perror(pathnames);
            return;
        }
        
        struct dirent* dir;
        int x, y;
        char *store = NULL;
        DIR* direc_pointer = opendir(example);
        while ((dir = readdir(direc_pointer)) != NULL) {
            x = strlen(dir->d_name);
            y = strlen(example);
            store = malloc(sizeof(char)*(path_length+x+2));
            sprintf(store, "%s/%s", example, dir->d_name);
            if (check_dot(store, y) == 1) {
                iterFunction(fp, store, repeat_time);
            } 
        }
        free(store);
        closedir(d);
    } else {
        FILE* file = fopen(pathnames, "r");
        int c = fgetc(file);
        for (int i = 0; i < target.st_size; i++) {
            fputc(c, fp);
            hash_value = blobby_hash(hash_value, c);
            c = fgetc(file);
        }
        fclose(file);
    }
    fputc(hash_value, fp);
}

// YOU SHOULD NOT CHANGE CODE BELOW HERE
int check_dot(char* store, int start) {
    int result = 1;
    int cop_start = start;
    for (int i = start; i < strlen(store); i++) {
        if (strcmp(&store[i], ".") == 0 && i != cop_start) {
            result = 0;
            return result;
        }
    }
    return result;
}

// Lookup table for a simple Pearson hash
// https://en.wikipedia.org/wiki/Pearson_hashing
// This table contains an arbitrary permutation of integers 0..255

const uint8_t blobby_hash_table[256] = {
    241, 18,  181, 164, 92,  237, 100, 216, 183, 107, 2,   12,  43,  246, 90,
    143, 251, 49,  228, 134, 215, 20,  193, 172, 140, 227, 148, 118, 57,  72,
    119, 174, 78,  14,  97,  3,   208, 252, 11,  195, 31,  28,  121, 206, 149,
    23,  83,  154, 223, 109, 89,  10,  178, 243, 42,  194, 221, 131, 212, 94,
    205, 240, 161, 7,   62,  214, 222, 219, 1,   84,  95,  58,  103, 60,  33,
    111, 188, 218, 186, 166, 146, 189, 201, 155, 68,  145, 44,  163, 69,  196,
    115, 231, 61,  157, 165, 213, 139, 112, 173, 191, 142, 88,  106, 250, 8,
    127, 26,  126, 0,   96,  52,  182, 113, 38,  242, 48,  204, 160, 15,  54,
    158, 192, 81,  125, 245, 239, 101, 17,  136, 110, 24,  53,  132, 117, 102,
    153, 226, 4,   203, 199, 16,  249, 211, 167, 55,  255, 254, 116, 122, 13,
    236, 93,  144, 86,  59,  76,  150, 162, 207, 77,  176, 32,  124, 171, 29,
    45,  30,  67,  184, 51,  22,  105, 170, 253, 180, 187, 130, 156, 98,  159,
    220, 40,  133, 135, 114, 147, 75,  73,  210, 21,  129, 39,  138, 91,  41,
    235, 47,  185, 9,   82,  64,  87,  244, 50,  74,  233, 175, 247, 120, 6,
    169, 85,  66,  104, 80,  71,  230, 152, 225, 34,  248, 198, 63,  168, 179,
    141, 137, 5,   19,  79,  232, 128, 202, 46,  70,  37,  209, 217, 123, 27,
    177, 25,  56,  65,  229, 36,  197, 234, 108, 35,  151, 238, 200, 224, 99,
    190
};

// Given the current hash value and a byte
// blobby_hash returns the new hash value
//
// Call repeatedly to hash a sequence of bytes, e.g.:
// uint8_t hash = 0;
// hash = blobby_hash(hash, byte0);
// hash = blobby_hash(hash, byte1);
// hash = blobby_hash(hash, byte2);
// ...

uint8_t blobby_hash(uint8_t hash, uint8_t byte) {
    return blobby_hash_table[hash ^ byte];
}


            /*if (byte_count == 2) {
                i = *indicator << 8;
                printf("%x\n", i);
            } else if (byte_count == 3) {
                i = i | (*indicator);
                printf("last is: %d\n", *indicator);
                printf("%x\n", i);
            }*/
            /*fseek(fd, 0, SEEK_END);
            int size = ftell(fd);*/
