#if !defined(UTILS_H)
#define UTILS_H

#define idx(row, col, cols) ((row) * (cols) + (col))
typedef struct pcd_t pcd_t;

typedef struct {
    int max_size;
    int max_element_size;
    void* data;
    int size;
} List;

int listInit(List* l, int max_elmt_size);
void listAddEnd(List* l, void* elmt);
void *listGet(List* l, int index);



#endif // UTILS_H
