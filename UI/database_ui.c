#include <dlfcn.h>
#include <fcntl.h>
#include <ncurses.h>
#include <string.h>
#include <malloc.h>
#include <gdbm-ndbm.h>

typedef struct {
    char * key;
    char * value;
} Object;


void init_ncurses(void);

_Noreturn void display_results(void);

int main(int argc, char *argv[]){

    // connect to the db and get the values
    DBM * db = dbm_open("webdatabase", O_CREAT | O_RDWR, 0666);
    if (!db) {
        fprintf(stderr, "Failed to open database.\n");
    }
    init_ncurses();
    display_results();
    return 0;
}


void init_ncurses(void){
    initscr();
    cbreak();
    noecho();
}

_Noreturn void display_results(void){

    while (1){
        mvprintw(0, 0, "RESULTS FROM THE DATABASE");
        mvprintw(2, 0, "Object key: %s", "dummy");
        mvprintw(3, 0, "Object value: %s", "dummy2");
        refresh();
    }
}

Object * load_object(DBM* db, char * id) {
    Object *obj = malloc(sizeof(Object));
    datum key, value; // NOLINT(readability-isolate-declaration)
    key.dptr = id;
    key.dsize = (int)strlen(key.dptr);
    value = dbm_fetch(db, key); // NOLINT(concurrency-mt-unsafe)
    if (value.dptr != NULL) {
        obj->key = strdup(key.dptr);
        obj->value = malloc(value.dsize + 1);
        memcpy(obj->value, value.dptr, value.dsize);
        obj->value[value.dsize] = '\0';
    }
    return obj;
}

