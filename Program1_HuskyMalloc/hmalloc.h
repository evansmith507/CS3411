/* You should not need to edit this file. Prototypes for all externally 
 * visible hmalloc functions are declared here. The functions themselves
 * are implemented in hmalloc.c (Your edits should be in hmalloc.c)*/

void traverse();
void addMetaData(int size, void* pointer); //add for ease of setting up meta data
void *hmalloc(int bytes_to_alloc);
void *hcalloc(int bytes_to_alloc);
void hfree(void *ptr);
