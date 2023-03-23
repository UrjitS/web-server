#include <dlfcn.h>
#include <fcntl.h>
#include <ncurses.h>
#include <string.h>
#include <malloc.h>
#include <gdbm-ndbm.h>

typedef struct {
    char *key;
    char *value;
} Object;

void init_ncurses(void);

_Noreturn void display_results(DBM *db);

Object *load_object(DBM *db, char *id);

int main(int argc, char *argv[]) {
    // connect to the db and get the values
    DBM * db = dbm_open("webdatabase", O_CREAT | O_RDWR, 0666);
    if (!db) {
        fprintf(stderr, "Failed to open database.\n");
    }
    init_ncurses();
    display_results(db);
    return 0;
}

void init_ncurses(void) {
    initscr();
    cbreak();
    noecho();
}

void display_results(DBM* db){
    while (1) {
        mvprintw(0, 0, "RESULTS FROM THE DATABASE");
        refresh();

        // Loop through all the keys in the database and display the corresponding values
        int y = 2;
        char *key_str;
        for (datum key = dbm_firstkey(db); key.dptr != NULL; key = dbm_nextkey(db)) {
            key_str = (char *)key.dptr;
            Object *object = load_object(db, key_str);
            if (object != NULL) {
                mvprintw(y++, 0, "Object key: %s", object->key);
                mvprintw(y++, 0, "Object value: %s", object->value);
                refresh();
                free(object->key);
                free(object->value);
                free(object);
            } else {
                mvprintw(y++, 0, "Ain't nothing here boi");
                refresh();
            }
        }

        // Wait for user input before repeating
        mvprintw(y++, 0, "Press any key to see the results again.");
        refresh();
        getch();
    }
}


Object *load_object(DBM *db, char *id) {
    Object *obj = malloc(sizeof(Object));
    datum key, value;
    key.dptr = id;
    key.dsize = strlen(id) + 1; // add 1 for null terminator
    value = dbm_fetch(db, key);
    if (value.dptr != NULL) {
        obj->key = strdup(id);
        obj->value = malloc(value.dsize + 1);
        memcpy(obj->value, value.dptr, value.dsize);
        obj->value[value.dsize] = '\0';
    } else {
        obj = NULL;
    }
    return obj;
}

