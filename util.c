#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <malloc.h>
#include <sys/time.h>

#include "util.h"

unsigned char hash[SHA_DIGEST_LENGTH];

time GetTimeStamp()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * (time)10000000 + tv.tv_usec;
}

int LRU(ram *memory)
{
    time time = 90000000000000000;
    int pos = -1;
    for (int frame = 0; frame < memory->size; frame++)
    {
        if (time > memory->frames[frame].time)
        {
            time = memory->frames[frame].time;
            pos = frame;
        }
    }
    if (memory->frames[pos].isWritten)
    {
        writes++;
    }
    return pos;
}

int secondChange(ram *memory)
{
    time time = 90000000000000000;
    int pos = -1;
    while (1)
    {
        for (int frame = 0; frame < memory->size; frame++)
        {
            if (memory->frames[frame].refBit == 0)
            {
                if (time > memory->frames[frame].time)
                {
                    time = memory->frames[frame].time;
                    pos = frame;
                }
            }
            else
            {
                memory->frames[frame].refBit = 0;
            }
        }
        if (pos != -1)
        {
            break;
        }
        time = 90000000000000000;
    }
    if (memory->frames[pos].isWritten)
    {
        writes++;
    }
    return pos;
}

void generateHash(int key, char *returnHash)
{
    char data[256];
    sprintf(data, "%d", key);
    size_t length = strlen(data);
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1((const unsigned char *)data, length, hash);
    memcpy(returnHash, (char *)hash, SHA_DIGEST_LENGTH);
}

data *createData(int PageNumber, int FrameNumber)
{
    data *data = malloc(sizeof(data));
    data->PageNumber = PageNumber;
    data->FrameNumber = FrameNumber;
    return data;
}

void printData(data *data)
{
    printf("{FrameNumber = %d , PageNumber = %d}\n", data->FrameNumber, data->PageNumber);
}

int hexadecimalToDecimal(char hexVal[])
{
    int len = strlen(hexVal);
    int base = 1;
    int dec_val = 0;

    for (int i = len - 1; i >= 0; i--)
    {
        if (hexVal[i] >= '0' && hexVal[i] <= '9')
        {
            dec_val += (hexVal[i] - 48) * base;
            // incrementing base by power
            base = base * 16;
        }
        else if (hexVal[i] >= 'A' && hexVal[i] <= 'F')
        {
            dec_val += (hexVal[i] - 55) * base;
            // incrementing base by power
            base = base * 16;
        }
        else if (hexVal[i] >= 'a' && hexVal[i] <= 'f')
        {
            dec_val += (hexVal[i] - 87) * base;
            // incrementing base by power
            base = base * 16;
        }
    }
    return abs(dec_val);
}

int getIndex(char *H, int a)
{
    // printf("a = %d\n", a);
    // printf("hash string %s\n", H);
    // printf("hash int %d\n", hexadecimalToDecimal(H));
    return hexadecimalToDecimal(H) % a;
}
// --- LIST ---
header *list_create()
{
    header *list;
    list = malloc(sizeof(header));
    list->size = 0;
    list->start = NULL;
    return list;
}

void list_destruction(header *list)
{
    node *todel;
    node *todel2;
    todel = list->start;
    while (todel != NULL)
    {
        todel2 = todel;
        todel = todel->next;
        free(todel2->data);
        free(todel2);
    }
    list->start = NULL;
    free(list);
}

int list_empty(header *list)
{
    return (list->start == NULL);
}

int list_exists(header *list, int page)
{
    node *temp = list->start;

    while (temp != NULL)
    {
        if (temp->data->PageNumber == page)
        {
            // printf("Found!\n");
            // printf("%d", temp->data->FrameNumber);
            return temp->data->FrameNumber;
        }
        temp = temp->next;
    }
    return -1;
}

data *list_value(header *list, node *p)
{
    return p->data;
}

void list_change(header *list, node *p, data *value)
{
    p->data = value;
}

void list_insert(header *list, data *value)
{
    if (list->size != 0)
    {
        node *last = list_last(list);
        node *new;
        new = malloc(sizeof(node));
        list_change(list, new, value);
        new->next = NULL;
        last->next = new;
    }
    else
    {
        node *new;
        new = malloc(sizeof(node));
        list_change(list, new, value);
        list->start = new;
        new->next = NULL;
    }
    list->size++;
}

void list_delete(header *list, node *p)
{
    node *last = list_last(list);
    if (list->start == p)
    {
        list->start = p->next;
    }
    else if (last == p)
    {
        node *prev = list_prev(list, p);
        prev->next = NULL;
    }
    else
    {
        node *prevP = list_prev(list, p);
        node *nextP = list_next(list, p);
        prevP->next = nextP;
    }
    free(p->data);
    free(p);
    list->size--;
}

node *list_next(header *list, node *p)
{
    return p->next;
}

node *list_prev(header *list, node *p)
{
    node *temp = list->start;

    while (temp->next != p)
    {
        temp = temp->next;
    }
    return temp;
}

node *list_first(header *list)
{
    return list->start;
}

node *list_last(header *list)
{
    node *temp = list->start;

    while (temp->next != NULL)
    {
        temp = temp->next;
    }
    return temp;
}

node *list_find(header *list, data *data)
{
    node *temp = list->start;

    while (temp != NULL)
    {
        if (temp->data == data)
        {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

node *list_findByFrame(header *list, int frame)
{
    node *temp = list->start;

    while (temp != NULL)
    {
        if (temp->data->FrameNumber == frame)
        {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

void list_print(header *list)
{
    node *temp = list->start;

    while (temp != NULL)
    {
        printData(temp->data);
        temp = temp->next;
    }
    // printData(temp->data);
}

ram *createRam(int size)
{
    ram *memory = malloc(sizeof(ram));
    memory->size = size;
    memory->frames = malloc(size * sizeof(frame));

    for (int i = 0; i < size; i++)
    {
        memory->frames[i].exists = 0;
        memory->frames[i].refBit = 0;
        memory->frames[i].time = 0;
        memory->frames[i].isWritten = 0;
        memory->frames[i].inDisk = 0;
        memory->frames[i].pageNumber = 0;
    }
    return memory;
}

void deleteRam(ram *memory)
{
    free(memory->frames);
    free(memory);
}

void insert_update(ram *memory, int frameNumber, int pageNumber, char rw)
{
    if (memory->frames[frameNumber].exists == 1)
    {
        memory->frames[frameNumber].refBit = 1;
    }
    if (rw == 'W')
    {
        memory->frames[frameNumber].isWritten = 1;
    }
    memory->frames[frameNumber].time = GetTimeStamp();
    memory->frames[frameNumber].exists = 1;
    memory->frames[frameNumber].pageNumber = pageNumber;
}

void removePageFromFrame(ram *memory, int frameNumber)
{
    memory->frames[frameNumber].exists = 0;
    memory->frames[frameNumber].refBit = 1;
    memory->frames[frameNumber].time = 90000000000000000;
    memory->frames[frameNumber].isWritten = 0;
    memory->frames[frameNumber].inDisk = 0;
    memory->frames[frameNumber].pageNumber = 0;
}

void saveToDisk(ram *memory, int frameNumber)
{
    memory->frames[frameNumber].inDisk = 1;
    // visual way to represent that the page in that fram has been stored to the disk
    // after the saved to disk the frame is empty
    removePageFromFrame(memory, frameNumber);
    // empty the frame
}

int ramHasSpace(ram *memory)
{
    for (int frame = 0; frame < memory->size; frame++)
    {
        if (!memory->frames[frame].exists)
        {
            return frame;
        }
    }
    return -1;
}

void printRam(ram *memory)
{
    for (int frame = 0; frame < memory->size; frame++)
    {
        printFrame(memory, frame);
    }
    printf("\n");
}

void printFrame(ram *memory, int frame)
{
    printf("frame = %d ", frame);
    printf("exists = %d ", memory->frames[frame].exists);
    printf("refBit = %d ", memory->frames[frame].refBit);
    printf("time = %llu ", memory->frames[frame].time);
    printf("isWritten = %d ", memory->frames[frame].isWritten);
    printf("inDisk = %d ", memory->frames[frame].inDisk);
    printf("pageNumber = %d \n", memory->frames[frame].pageNumber);
}

// time GetTimeStamp()
// {
//     struct timeval tv;

//     gettimeofday(&tv, NULL);
//     return (((time)tv.tv_sec) * 1000) + (tv.tv_usec / 1000);
// }

// time GetTimeStamp()
// {
//     struct timespec now;
//     timespec_get(&now, TIME_UTC);
//     return ((time)now.tv_sec) * 1000 + ((time)now.tv_nsec) / 1000000;
// }