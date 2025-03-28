# brainf-ck

This program is an interpreter for the brainf*ck esoteric programming language, and is written in C.

# How to Use:
To use the interpreter, compile `bf.c` and run the `.exe` with a brainf*ck program as an argument.

# Brainf*ck language fundamentals:
  The brainf*ck language uses a list of data cells, a data pointer, and an instruction pointer. Each cell contains one byte.
  The instruction pointer advances one instruction at a time and executes that command.
  There are only 8 commands, which are as follows:
    <p>`<` - Move the data pointer one cell to the left.</p>
    <p>`>` - Move the data pointer one cell to the right.</p>
    <p>`+` - Increment the value in the data cell at the data pointer's current location.</p>
    <p>`-` - Decrement the value in the data cell at the data pointer's current location.</p>
    <p>`.` - Print out the ASCII character representation of the value in the data cell at the data pointer's current location.</p>
    <p>`,` - Receive a single character and store it into the data cell at the data pointer's current location.</p>
    <p>`[` - Begin loop.</p>
    <p>`]` - End loop.</p>
      <p>Each opening bracket (`[`) must have a matching closing bracket (`]`), and vice-versa. Loops may be nested.
      <br />
     When the instruction pointer encounters a closing bracket (`]`), if the value of the data cell at the data pointer's current location > 0, the instruction pointer jumps to the first instruction after it's matching opening bracket (`[`). Else, it procedes to the next instruction.</p>
      
# Code and design details:
The brainf\*ck Wikapedia article recommends using an array of at least 30,000 data cells, each containing one byte. This program, however, uses two linked lists: one traditional for the command list, and one doubly-linked for the data cells. I chose this design model for two reasons. The first is because I wanted the challenge.
<br />
<br />
The second reason is that, though this design model complicates the code, it is more memory-efficient. Each `COMMAND` (instruction) and `CELL` (data cell) structure is 24 bytes. Although this increases the memory required for each individual instrucion and data cell significantly, the total required memory is much lower than the minimum required memory for an array-based model—at least 30,000 bytes for the data list and an unknown number of bytes for the instruction list (an instruction list will likely be used because loops will require jumping backwards to previous instructions, though there are solutions that don't use an instruction list). Instead of requiring a set amount of memory, this design model uses only the memory required to store all instructions, and dynamically allocates memory for new data cells as it is needed. Therefore, if a brainf\*ck program uses 100 data cells, this interpreter will only allocate (24 \* 100) or 2,400 bytes, compared to the 30,000 bytes that are allocated when following the traditional method. This dynamic model becomes less memory-efficient only when the number of data cells needed exceeds (30,000 / 24) or 1,250 data cells, which is unlikely.
