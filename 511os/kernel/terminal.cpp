// include header file with 4 functions
#include "terminal.h"

static volatile unsigned short* const video = (unsigned short*)0xB8000; // VGA text memory

// Keep track of cursor position
static int cursor_x = 0;
static int cursor_y = 0;

static const int WIDTH = 80; // 80 Columns
static const int HEIGHT = 25; // 25 Rows

static const unsigned char COLOR = 0x07; // Set white text

// Change bits to display color and text in a cell
static unsigned short create_cell(char c){
    return ((unsigned short)COLOR << 8) | (unsigned char)c;
}

// Function to scroll up or down on terminal by one row
static void terminal_scroll()
{
    // Copy the last row into the row above
    for (int y = 1; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            video[(y-1) * WIDTH + x] = video[y * WIDTH + x];
        }
    }

    // Clear the bottom row
    for (int x = 0; x < WIDTH; x++)
    {
        video[(HEIGHT - 1) * WIDTH + x] = create_cell(' ');
    }

    // Moves cursor to the bottom row
    cursor_y = HEIGHT - 1;
}

// Function clears terminal / screen
void terminal_clear(){

    // Create empty cells
    for (int i = 0; i < WIDTH * HEIGHT; i++){
        video[i] = create_cell(' ');
    }

    // Set cursor back to top left
    cursor_x = 0;
    cursor_y = 0;
}

// Clear terminal for intialization
void terminal_initialize(){
    terminal_clear();
}

// Function to input one character, used for newline, return, backspace
void terminal_putchar(char c)
{
    // if newline, move cursor to new line
    if (c == '\n'){
        cursor_x = 0;
        cursor_y++;
    } // if return, set cursor to start of row
    else if (c == '\r'){
        cursor_x = 0;
    }
    // if backspace, set cursor left by 1
    else if (c == '\b'){
        if (cursor_x > 0)
        {
            cursor_x--;
        }
        // if cant go left, move up one row
        else if (cursor_y > 0)
        {
            cursor_y--;
            cursor_x = WIDTH - 1;
        }
        // Replace cell with a space (nothing)
        video[cursor_y * WIDTH + cursor_x] = create_cell(' ');
    }
    // if any other character, put character
    else {
        video[cursor_y * WIDTH + cursor_x] = create_cell(c);

        cursor_x++;
    }
    // if cursor about to go off screen, move down
    if (cursor_x >= WIDTH){
        cursor_x = 0;
        cursor_y++;
    }
    // if below row 25, scroll
    if (cursor_y >= HEIGHT){
        terminal_scroll();
    }
}
// function for printing a string, processes each character and puts on screen
void terminal_print(const char* text){
    for (int i = 0; text[i] != '\0'; i++)
    {
        terminal_putchar(text[i]);
    }
}