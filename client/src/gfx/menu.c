/**
 * @file        menu.c
 * @brief       Menu implementation
 */

#include "menu.h"

#include "../input/input_table.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// The global menu stack
static menu_stack g_menu_stack = { .top = -1 };

void menu_push(menu* m) {
    if (g_menu_stack.top < MENU_STACK_DEPTH - 1) {
        g_menu_stack.stack[++g_menu_stack.top] = m;
    } else {
        fprintf(stderr, "Menu stack overflow\n");
    }
}

void menu_pop() {
    if (g_menu_stack.top >= 0) {
        g_menu_stack.top--;
    } else {
        fprintf(stderr, "Menu stack underflow\n");
    }
}

menu* menu_current() {
    if (g_menu_stack.top >= 0) {
        return g_menu_stack.stack[g_menu_stack.top];
    } else {
        return NULL;
    }

}

void menu_clear_stack() {
    g_menu_stack.top = -1;
}

char menu_opened() {
    return g_menu_stack.top >= 0;
}

void menu_select_next(menu* m) {
    m->selected_index = (m->selected_index + 1) % m->item_count;
}

void menu_select_previous(menu* m) {
    m->selected_index = (m->selected_index - 1 + m->item_count) % m->item_count;
}

void menu_execute_selected(menu* m) {

    // Switch based on menu logic
    switch (m->items[m->selected_index].type)
    {
        case MA_CALLBACK:
            if (m->items[m->selected_index].action.callback)
                m->items[m->selected_index].action.callback();
            break;
        case MA_SUBMENU:
            if (m->items[m->selected_index].action.submenu)
                menu_push(m->items[m->selected_index].action.submenu);
        default:
            break;
    }
}

void update_number(number_action_desc* n, int dx) {

    int old = *n->value;

    if (*n->value + dx > n->upper) *n->value = n->upper;
    else if (*n->value + dx < n->lower) *n->value = n->lower;
    else *n->value += dx;

    if (n->on_change && old != *n->value)
        n->on_change(*n->value);
}

void process_menu_keyboard_state(menu* menu) {
    
    if (is_edge_pressed(g_input.ui_down, &fired.ui_down_edge))
        menu_select_next(menu);

    if (is_edge_pressed(g_input.ui_up, &fired.ui_up_edge))
        menu_select_previous(menu);

    if (is_edge_pressed(g_input.ui_select, &fired.ui_select_edge))
        menu_execute_selected(menu);

    if (is_edge_pressed(g_input.ui_back, &fired.ui_back_edge)) 
        menu_pop();

    if (menu->items[menu->selected_index].type == MA_NUMBER) {
        if (is_edge_pressed(g_input.ui_left, &fired.ui_left_edge)) 
            update_number(menu->items[menu->selected_index].action.number, -menu->items[menu->selected_index].action.number->increment);
        if (is_edge_pressed(g_input.ui_right, &fired.ui_right_edge)) 
            update_number(menu->items[menu->selected_index].action.number, menu->items[menu->selected_index].action.number->increment);
    }
}

void menu_update(menu* m) {
    process_menu_keyboard_state(m);
}