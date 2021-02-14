#ifndef UTIL_H
#define UTIL_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <malloc.h>

extern int writes;
extern int reads;
extern int evictsGCC;
extern int evictsBZIP;
extern int pageFaults;
extern int evicts;

typedef unsigned long long time;

typedef struct frame
{
    int exists;     // if 1 this frame contains a page otherwise not
    int isWritten;  // if 1 this frame has been accessed to be written
                    // at least ones
    int refBit;     // the reference bit for the second change algorithm
    int inDisk;     // this bit set to 1 to visualize that the page has
                    // been saved to the disk
    int pageNumber; // just to have the pageNumber
    time time;      // the time that the last access or modify has been occured
} frame;

typedef struct ram
{
    int size;      // the number of the frames that the ram has
    frame *frames; // points to the frames
} ram;

typedef struct data
{
    int PageNumber;
    int FrameNumber;
} data;

typedef struct node
{
    data *data;
    struct node *next;
} node;

typedef struct header
{
    int size;
    node *start;
} header;

typedef struct hashTableEntry
{
    // int listExists;
    header *header;
} hashTableEntry;

int LRU(ram *memory);
int secondChange(ram *memory);

void generateHash(int key, char *returnHash);
int hexadecimalToDecimal(char hexVal[]);
int getIndex(char *hash, int a);

data *createData(int PageNumber, int FrameNumber);
void printData(data *data);

//  --- LIST ---
header *list_create();
void list_destruction(header *list);

int list_empty(header *list);
int list_exists(header *list, int page);

data *list_value(header *list, node *p);
void list_change(header *list, node *p, data *value);

void list_insert(header *list, data *value);
void list_delete(header *list, node *p);

node *list_next(header *list, node *p);
node *list_prev(header *list, node *p);
node *list_first(header *list);
node *list_last(header *list);
node *list_find(header *list, data *data);
node *list_findByFrame(header *list, int frame);
void list_print(header *list);

ram *createRam(int size);
void deleteRam(ram *memory);
void insert_update(ram *memory, int frameNumber, int pageNumber, char rw);
void removePageFromFrame(ram *memory, int frameNumber);
void saveToDisk(ram *memory, int frameNumber);
int ramHasSpace(ram *memory);
void printRam(ram *memory);
void printFrame(ram *memory, int frame);
time GetTimeStamp();
#endif