#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <malloc.h>

#include "util.h"

int writes = 0;
int reads = 0;
int evictsGCC = 0;
int evictsBZIP = 0;
int pageFaults = 0;

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        printf("Arguments: \n (1) Algorithm [LRU or SecondChange]\n (2) Memory frames [Integer] \n (3) q [Integer]\n (4) Max [Integer] [-1 for maxed references]\n");
        return 1;
    }
    if (!(!strcmp(argv[1], "LRU") || !strcmp(argv[1], "SecondChange")))
    {
        printf("Argument 1 must be LRU or SecondChange\n");
        return 1;
    }
    if (atoi(argv[2]) < 0)
    {
        printf("Argument 2 must be an integer bigger than 0\n");
        return 1;
    }
    if (atoi(argv[3]) < 0)
    {
        printf("Argument 3 must be an integer bigger than 0\n");
        return 1;
    }
    if (atoi(argv[4]) < -1)
    {
        printf("Argument 4 must be an integer bigger than 0 or -1\n");
        return 1;
    }

    hashTableEntry hashTableBZIP[atoi(argv[2])];
    hashTableEntry hashTableGCC[atoi(argv[2])];
    FILE *fp1;
    FILE *fp2;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    data *record;

    fp1 = fopen("traces/gcc.trace", "r");
    fp2 = fopen("traces/bzip.trace", "r");
    if (fp1 == NULL || fp2 == NULL)
    {
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < atoi(argv[2]); i++)
    {
        header *listBZIP = list_create();
        header *listGCC = list_create();

        hashTableBZIP[i].header = listBZIP;
        hashTableGCC[i].header = listGCC;
    }

    int turnBZIP = 0;
    int turnGCC = -1;
    int q = atoi(argv[3]);
    unsigned long long MAX = atoi(argv[4]);
    if (atoi(argv[4]) == -1)
    {
        MAX = 90000000000000000;
    }
    int memory_size = atoi(argv[2]);
    ram *memory = createRam(memory_size);
    int count = 0;
    while (1)
    {
        if (count == MAX)
        {
            break;
        }
        count++;
        if (turnBZIP >= 0)
        {
            printf("BZIP\n");
            read = getline(&line, &len, fp2);
            if (read == -1)
            {
                break;
            }
            turnBZIP++;
            if (turnBZIP == q)
            {
                turnBZIP = -1;
                turnGCC = 0;
            }
            char PageNumber[6];
            char Offset[4];
            char end;
            for (int i = 0; i < 5; i++)
            {
                PageNumber[i] = line[i];
            }
            PageNumber[5] = '\0';
            for (int j = 0; j < 3; j++)
            {
                Offset[j] = line[5 + j];
            }
            Offset[3] = '\0';
            end = line[9];

            int PageNumberInt = hexadecimalToDecimal(PageNumber);
            int OffsetInt = hexadecimalToDecimal(Offset);

            char *hash = malloc(SHA_DIGEST_LENGTH * sizeof(char));
            generateHash(PageNumberInt, hash);
            int hashTableIndex = getIndex(hash, atoi(argv[2]));
            printf("\nhashTableIndex %d\n", hashTableIndex);
            printf("PageNumber=%s Offset=%s end=%c --- PageNumberInt=%d\n", PageNumber, Offset, end, PageNumberInt);

            int frame = list_exists(hashTableBZIP[hashTableIndex].header, PageNumberInt);
            if (frame >= 0) // if the page is in the ram
            {
                printf("Page exists with frame %d \n", frame);
                insert_update(memory, frame, PageNumberInt, end);
            }
            else // if the page is not in the ram
            {
                printf("Page Don't exists\n");
                pageFaults++;
                reads++;
                int frame = ramHasSpace(memory);
                if (frame >= 0) // ram has space to put the page
                {
                    printf("Ram has space\n");
                    record = createData(PageNumberInt, frame);
                    list_insert(hashTableBZIP[hashTableIndex].header, record);

                    insert_update(memory, frame, PageNumberInt, end);
                }
                else // ram dont has space to put the page
                {
                    evictsBZIP++;
                    if (strcmp(argv[1], "LRU") == 0)
                    {
                        frame = LRU(memory);
                    }
                    else
                    {
                        frame = secondChange(memory);
                    }

                    printf("Ram does not has space. Frame %d evicted\n", frame);
                    saveToDisk(memory, frame);

                    node *nodeBZIP;
                    node *nodeGCC;
                    int i = 0;

                    while (1)
                    {
                        nodeBZIP = list_findByFrame(hashTableBZIP[i].header, frame);
                        nodeGCC = list_findByFrame(hashTableGCC[i].header, frame);
                        i++;
                        if (nodeBZIP != NULL)
                        {

                            list_delete(hashTableBZIP[i - 1].header, nodeBZIP);

                            break;
                        }
                        else if (nodeGCC != NULL)
                        {

                            list_delete(hashTableGCC[i - 1].header, nodeGCC);

                            break;
                        }
                    }
                    record = createData(PageNumberInt, frame);
                    list_insert(hashTableBZIP[hashTableIndex].header, record);

                    insert_update(memory, frame, PageNumberInt, end);
                }
            }
            free(hash);
        }
        else if (turnGCC >= 0)
        {
            printf("GCC\n");
            read = getline(&line, &len, fp1);
            if (read == -1)
            {
                break;
            }
            turnGCC++;
            if (turnGCC == q)
            {
                turnGCC = -1;
                turnBZIP = 0;
            }
            char PageNumber[6];
            char Offset[4];
            char end;
            for (int i = 0; i < 5; i++)
            {
                PageNumber[i] = line[i];
            }
            PageNumber[5] = '\0';
            for (int j = 0; j < 3; j++)
            {
                Offset[j] = line[5 + j];
            }
            Offset[3] = '\0';
            end = line[9];

            int PageNumberInt = hexadecimalToDecimal(PageNumber);
            int OffsetInt = hexadecimalToDecimal(Offset);

            char *hash = malloc(SHA_DIGEST_LENGTH * sizeof(char));
            generateHash(PageNumberInt, hash);
            int hashTableIndex = getIndex(hash, atoi(argv[2]));
            printf("\nhashTableIndex %d\n", hashTableIndex);
            printf("PageNumber=%s Offset=%s end=%c --- PageNumberInt=%d\n", PageNumber, Offset, end, PageNumberInt);

            int frame = list_exists(hashTableGCC[hashTableIndex].header, PageNumberInt);
            if (frame >= 0) // if the page is in the ram
            {
                printf("Page exists with frame %d \n", frame);
                insert_update(memory, frame, PageNumberInt, end);
            }
            else // if the page is not in the ram
            {
                printf("Page Don't exists\n");
                pageFaults++;
                reads++;
                int frame = ramHasSpace(memory);
                if (frame >= 0) // ram has space to put the page
                {
                    printf("Ram has space\n");
                    record = createData(PageNumberInt, frame);
                    list_insert(hashTableGCC[hashTableIndex].header, record);
                    insert_update(memory, frame, PageNumberInt, end);
                }
                else // ram dont has space to put the page
                {
                    evictsGCC++;
                    if (strcmp(argv[1], "LRU") == 0)
                    {
                        frame = LRU(memory);
                    }
                    else
                    {
                        frame = secondChange(memory);
                    }
                    printf("Ram does not has space. Frame %d evicted\n", frame);
                    saveToDisk(memory, frame);

                    node *nodeBZIP;
                    node *nodeGCC;
                    int i = 0;
                    while (1)
                    {
                        nodeBZIP = list_findByFrame(hashTableBZIP[i].header, frame);
                        nodeGCC = list_findByFrame(hashTableGCC[i].header, frame);
                        i++;

                        if (nodeBZIP != NULL)
                        {

                            list_delete(hashTableBZIP[i - 1].header, nodeBZIP);

                            break;
                        }
                        else if (nodeGCC != NULL)
                        {

                            list_delete(hashTableGCC[i - 1].header, nodeGCC);

                            break;
                        }
                    }
                    record = createData(PageNumberInt, frame);
                    list_insert(hashTableGCC[hashTableIndex].header, record);
                    insert_update(memory, frame, PageNumberInt, end);
                }
            }

            free(hash);
        }
    }
    printf("\n");
    if (strcmp(argv[1], "LRU") == 0)
    {
        printf("LRU\n");
    }
    else
    {
        printf("SecondChange\n");
    }
    if (atoi(argv[4]) == -1)
    {
        printf("%d Records Imported\n", count - 1);
    }
    else
    {
        printf("%d Records Imported\n", count);
    }
    printf("\npageFaults %d\n", pageFaults);
    printf("reads %d\n", reads);
    printf("writes %d\n", writes);
    printf("number of frames %d\n", memory_size);
    printf("q %d\n", q);
    printf("total evicts %d\n", evictsBZIP + evictsGCC);
    printf("evictsBZIP %d\n", evictsBZIP);
    printf("evictsGCC %d\n", evictsGCC);

    for (int i = 0; i < atoi(argv[2]); i++)
    {
        list_destruction(hashTableGCC[i].header);
        list_destruction(hashTableBZIP[i].header);
    }

    deleteRam(memory);
    fclose(fp1);
    fclose(fp2);
    if (line)
    {
        free(line);
        exit(EXIT_SUCCESS);
    }
    return 0;
}