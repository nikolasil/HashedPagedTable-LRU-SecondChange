# RAM Pagination & Memory Management Simulator

A high-performance C implementation of a memory management unit (MMU) simulator. This project explores the efficiency of **Hashed Page Tables** and compares the performance of **Least Recently Used (LRU)** versus **Second Chance** page replacement algorithms using real-world memory trace files (bzip and gcc).



---

## ## Core Architecture

### ### 1. Hashed Page Tables
To optimize page lookups, the system implements a Hashed Page Table for each process. 
* **Hashing Engine:** Uses the **SHA1** algorithm for uniform distribution.
* **Collision Handling:** Implemented via single-linked lists (chaining) at each hash bucket.
* **Mapping:** Transparently maps `pageNumber` to physical `frameNumber`.

### ### 2. RAM & Frame Simulation
The RAM is simulated as a structured contiguous block of memory frames. Each frame tracks metadata critical for replacement logic:
* `refBit`: Used for the Second Chance algorithm.
* `isWritten`: Tracks "dirty" pages to simulate disk write-back overhead.
* `time`: High-precision timestamp for LRU logic.



---

## ## Page Replacement Algorithms

The simulator evaluates two primary strategies for handling page faults when the physical RAM is exhausted:

### ### Least Recently Used (LRU)
Identifies the "coldest" page—the one with the earliest access timestamp—and evicts it to make room for new data. This minimizes the page fault rate by assuming recently accessed pages will be accessed again soon.

### ### Second Chance (Clock)
An optimized version of FIFO/LRU that uses a `refBit`. If a page is scheduled for eviction but its `refBit` is 1, it is given a "second chance": its bit is cleared to 0, its timestamp is updated, and the algorithm moves to the next candidate. This protects frequently accessed pages from being evicted prematurely.

---

## ## Logic & Page Fault Handling

The simulator process alternates between two concurrent traces (`q` pages at a time) and handles requests as follows:

1.  **Page Hit:** The `pageNumber` exists in the Hashed Page Table. The simulator updates the access timestamp and sets the `isWritten` flag if the operation is a Write (W).
2.  **Page Fault (Space Available):** The page is loaded into an empty RAM frame and indexed in the Hashed Page Table.
3.  **Page Fault (RAM Full):** * The selected algorithm (LRU or Second Chance) identifies an eviction candidate.
    * If the evicted page was "dirty" (`isWritten == 1`), a disk write-back is simulated.
    * The old mapping is purged from the Hash Table, and the new page is loaded.

---

## ## Getting Started

### ### Compilation
Build the project using the included `Makefile`:
```bash
make
```

### Execution
Run the simulator using the following syntax:
```bash
./prog <algorithm> <num_frames> <q_pages> <max_total_pages>
```

Arguments:
- algorithm: LRU or SecondChange.
- num_frames: Number of physical frames available in RAM.
- q_pages: Quantum size (how many pages to read from each process before switching).
- max_total_pages: Total pages to simulate (use -1 for the full trace).## Technical DetailsLanguage: C (C11 standard)Hashing: SHA1 implementation for address translation.

Data Structures: Doubly linked lists for RAM frame management and Hashed Page Tables for $O(1)$ average-case lookups.

### Academic Context: This project was developed for the Operating Systems course at the National and Kapodistrian University of Athens (UoA).
