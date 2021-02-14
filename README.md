## Λειτουργικά Συστήματα

## Νικόλας Ηλιόπουλος Α.Μ. 1115201800332

## Άσκηση 2

---

### **Μεταγλώττιση & Εκτέλεση**

Εμπεριέχεται Makefile το οποίο μεταγλωττίζει συνολικά όλα τα αρχεία C με την εντολή make. Η εκτέλεση του προγράμματος γίνεται με την εντολή ./prog [-arg1] [-arg2] [-arg3] [-arg4]

-   arg1 : Επιλογή αλγόριθμου αντικατάστασης (LRU ή SecondChange)
-   arg2 : Επιλογή του πλήθους των frames της ram (> 0)
-   arg3 : Επιλογή του πλήθους q το οποίο ορίζει πόσα pages θα διαβάζει από κάθε διεργασία εναλλάξ (> 0)
-   arg4 : Επιλογή του πλήθους ΜΑΧ το οποίο ορίζει πόσα pages θα διαβαστούν συνολικά και από τις 2 διεργασίες (> 0 ή -1 για όλο το πλήθος των αρχείων)

---

### **utils.c & utils.h**

Σε αυτά τα αρχεία περιέχονται όλες οι απαράτητες συναρτήσεις και struct για την λειτουργία του προγράμματος όπως η υλοποίηση της λίστας και των συναρτήσεων της, η ανανέωση και η εισαγωγή στην ram κ.λ.π

---
### **Λογική Hash Tables**

Έχουμε δύο hash table, ένα για κάθε διεργασία, μεγέθους όσων των frames της ram για να υπαρχει συμμετρικότητα.

Κάθε entry του hash table έχει έναν δείκτη σε μία μονά συνδεδεμένη λίστα με nodes που περιέχουν pageNumber και frameNumber.

---

### **Λογική Ram**

```c
typedef struct ram
{
    int size;       // the number of the frames that the ram has
    frame *frames;  // points to the frames
} ram;
```
Η ram είναι ένα struct με πολλούς δείκτες σε struct frame.

Για ευκολία τα frameNumbers είναι τα index από 0 έως size-1. 
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

### **Λογική Προγράμματος**

Το πρόγραμμα είναι μία while() η οποία σταματάει όταν διαβαστούν Max Pages από τις δύο διεργασίες.

Έχουμε τις εξής περιπτώσεις:
-   Το pageNumber που διαβάσαμε υπάρχει στο hash table της συγκεκριμένης διεργασίας. Αυτό σημαίνει ότι το Page υπάρχει σε ένα frame της ram. Οπότε απλά πάμε στο συγκεκριμένο frame και ανανεώνουμε τον χρόνο του frame και επίσης αν εχουμε W κάνουμε και το isWritten του frame 1 για να ξέρουμε ότι κάναμε write στο Page αυτό.

-   Το pageNumber που διαβάσαμε **ΔΕΝ** υπάρχει στο hash table της συγκεκριμένης διεργασίας.
    -   Χωράει στην ram άρα το προσθέτουμε και στην ram και στο hash table.
    -   **ΔΕΝ** χωράει στην ram. Άρα με τον αλγόριθμο που επιλέκτηκε επιλέγουμε ένα page μέσα στην ram για να το αντικαταστήσουμε με το καινούργιο. Επίσης αποθηκεύουμε το page που αντικαταστήσαμε στον δίσκο, το βγάζουμε από το hash table και προσθέτουμε το καινούργιο Page στο hash table. (Eπειδή η άσκηση είναι εικονική για να καταλάβουμε την λειτουργία του paging και δεν έχουμε πληροφορία σαν Page για να αποθηκεύσουμε απλά κάνω μία μεταβλητή 1 για να αναπαραστίσω το γράψιμο στον δίσκο)

---

### **Hashing**

Για να κάνω hash χρησιμοποιώ την συνάρτηση **SHA1**.

Για να μετατρέψω από δεκαεξαδικό σε δεκαδικό σύστημα χρησιμοποιώ μία συνάρτηση μέσα στο util.c που κάνει το κατάλληλο shift σε ascii χαρακτήρες.

To HashTableIndex είναι hexadecimalToDecimal(Hash) % sizeof(ram);

---

### **LRU**

Διαλέγουμε να αντικαταστήσουμε το page με τον πιο μικρό χρόνο. Δηλαδή το page που εισήχθη πιο παλιά.

---

### **SecondChange**

Ο SecondChange είναι ίδιος με τον LRU απλά δίνει δεύτερη ευκαρία σε όσα pages έχουν χρησιμοποηθεί πρόσφατα.(Δηλαδή refBit = 1)

Δηλαδή μπορεί να υπάρχει ένα page (το οποίο έχει μπει πρώτο) και επειδή χρησιμοποιείται συνέχεια(άρα το refBit του γίνεται 1) ο second change του δίνει δεύτερη ευκαιρία και διαλλέγει ένα άλλο frame που έχει refBit == 0 και min time.

Αφού δωθεί η δεύτερη ευκαρία το refBit γίνεται 0 σε όλα τα pages.