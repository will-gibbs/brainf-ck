# brainf-ck

This program is an interpreter for the brainf*ck esoteric programming language, and is written in C.

# How to Use:
  To use the interpreter, compile and run `bf.c` with a brainf*ck program as an argument.

# Brainf*ck language fundamentals:
  The brainf*ck language uses a list of data cells, a data pointer, and an instruction pointer. Each cell contains one byte.
  The instruction pointer advances one instruction at a time and executes that command.
  There are only 8 commands, which are as follows:
    ['<'] - Move the data pointer one cell to the left.
    ['>'] - Move the data pointer one cell to the right.
    ['+'] - Increment the value in the data cell at the data pointer's current location.
    ['-'] - Decrement the value in the data cell at the data pointer's current location.
    ['.'] - Print out the ASCII character representation of the value in the data cell at the data pointer's current location.
    [','] - Receive a single character and store it into the data cell at the data pointer's current location.
    ['['] - Begin loop.
    [']'] - End loop.
      Each opening bracket ('[') must have a matching closing bracket (']'), and vice-versa. Loops may be nested.
      When the instruction pointer encounters a closing bracket (']'), if the value of the data cell at the data pointer's current location >= 0, the instruction pointer jumps to the first instruction after it's matching opening bracket ('['). Else, it procedes to the next instruction.
      
# Code and Design Details:
  The program uses two linked lists: one traditional for the command list, and one doubly-linked for the data cells.
