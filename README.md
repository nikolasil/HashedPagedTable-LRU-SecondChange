# This project was made during my studies in UOA University and especially for the course Operation Systems.

---

> There are comments all over my code if there is something that i didn't covered here.

This is a programm that simmulates 2 programms (bzip and gcc) with pages that are stored in a ram and when the frames of the ram are full then page replacement happens with LRU or Second Change Algorithm. This was an exersice that i did on the Operation System class in University to learn Paging.

## Compile & Run*

Includes Makefile which compiles all C files with the make command. The program is executed with the command ./prog [-arg1] [-arg2] [-arg3] [-arg4]

- arg1: Selection of replacement algorithm (LRU or SecondChange)
- arg2: Select the number of ram frames (> 0)
- arg3: Select the number q which determines how many pages to read from each process alternately (> 0)
- arg4: Selection the MAX number which determines how many pages will be read in total by both processes (> 0 or -1 for the entire number of files)

---

## utils.c & utils.h

These files contain all the necessary functions and structures for the operation of the program such as the implementation of the list and its functions, the renewal and the introduction to the ram, etc.

---

## Hash Tables

We have two hash tables, one for each process, the size of the ram frames for symmetry.
Each entry in the hash table has a pointer to a single linked list of nodes containing pageNumber and frameNumber.

---

## Ram

```c
typedef struct ram
{
    int size;       // the number of the frames that the ram has
    frame *frames;  // points to the frames
} ram;
```

Ram is a struct with many pointers in a struct frame.

For convenience, frameNumbers are indexes from 0 to size-1.

```c
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
```

---

## Program Logic

The program is a while() which stops when Max Pages are read by both processes.

We have the following cases:
- The pageNumber we read is in the hash table of this process. This means that the Page exists in a frame of the ram. So we just go to the specific frame and refresh the frame time and also if we have W we do the isWritten of frame 1 to know that we wrote to this Page.

- The pageNumber we read **does NOT** exist in the hash table of the specific process.
    - It fits in the ram so we add it to both the ram and the hash table.
    - **Does NOT** fit on the ram. So with the selected algorithm we select a page in the ram to replace it with the new one. We also save the page we replaced on the disk, remove it from the hash table and add the new Page to the hash table. (Because the exercise is virtual to understand the function of paging and we do not have information like Page to save I just do a variable 1 to represent the writing on the disk)

---

## Hashing

To do a hash I use the function **SHA1**.
To convert from hexadecimal to decimal I use a function inside util.c that makes the appropriate shift to ascii characters.
HashTableIndex is hexadecimalToDecimal (Hash)% sizeof (ram);

---

## LRU

We choose to replace the page with the smallest time. That is, the page that was introduced earlier.

---

## SecondChange

SecondChange is the same as LRU, it just gives a second chance to all the pages that have been used recently (refBit = 1)

That is, there may be a page (which has been entered first) and because it is used continuously (so its refBit becomes 1) the second change gives it a second chance and chooses another frame that has refBit == 0 and min time.

After the second opportunity is given, refBit becomes 0 on all pages.
