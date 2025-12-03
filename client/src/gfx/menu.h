/**
 * @file        menu.h
 * @brief       Menu system
 */

#ifndef MENU_H
#define MENU_H

#define MAX_LABEL_LENGTH 64
#define MENU_STACK_DEPTH 8

/**
 * The menu stack allows a "global menu state", the menu at the top is the one being displayed
 * If we need to go back, we pop the stack, if we go into a submenu, we push onto the stack.
 * To show that no menu is active, fully consume the stack
 */ 
typedef struct {
    struct menu_s* stack[MENU_STACK_DEPTH];
    int top;
} menu_stack;

typedef enum {
    MA_CALLBACK, MA_SUBMENU
} menu_action_type;

/**
 * A menu item is a label with functionality
 * We use a union to store either a callback function or a submenu pointer
 * If it is a callback, the type is MA_CALLBACK and the callback function is used on select
 * otherwise we push the submenu onto the menu stack
 */
typedef struct {
    char label[MAX_LABEL_LENGTH];
    menu_action_type type;

    union {
        void (*callback)();
        struct menu_s* submenu;
    } action;
    
} menu_item;

typedef struct menu_s {

    // Array of menu items
    menu_item* items;
    unsigned int item_count;

    // The currently selected index
    unsigned int selected_index;
} menu;

// Stack operations
char menu_opened();
menu* menu_current();
void menu_push(menu* m);
void menu_pop();
void menu_clear_stack();

// Menu operations
void menu_select_next(menu* m);
void menu_select_previous(menu* m);
void menu_execute_selected(menu* m);
void menu_update(menu* m);

#endif