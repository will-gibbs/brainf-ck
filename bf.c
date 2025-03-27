/**********************************************************************/
/*                                                                    */
/* Written by Will Gibbs                                              */
/* January 4, 2025                                                    */
/*                                                                    */
/* This program interprets code writen in the brainf*ck language,     */
/* as dictated by the commonly-accepted rules of the language.        */
/*                                                                    */
/**********************************************************************/

#include <stdio.h>
#include <stdlib.h>

/**********************************************************************/
/*                         Symbolic Constants                         */
/**********************************************************************/
#define END_OF_COMMANDS_CHAR       COMMAND_LIST_TRAILER
#define CELL_LIST_ALLOC_ERROR      2
#define CELL_ALLOC_ERROR           5
#define COMMAND_ALLOC_ERROR        4
#define COMMAND_LIST_ALLOC_ERROR   3
#define COMMAND_LIST_HEADER        '#'
#define COMMAND_LIST_TRAILER       '!'
#define INITIAL_CELL_VALUE         0
#define INVALID_COMMAND_ERROR      7
#define INVALID_LOOP_CONTROL_ERROR 8
#define MISSING_FILE_NAME_ERROR    1
#define NO_SUCH_FILE_ERROR         6
#define UNKNOWN_ERROR              9

/**********************************************************************/
/*                         Program Structures                         */
/**********************************************************************/
/* The specifications of a cell                                       */
struct cell
{
   struct cell *p_next,     /* Points to the next cell                */
               *p_previous; /* Points to the previous cell            */
   char        cell_value;  /* The character value stored in the cell */
};
typedef struct cell CELL;

/* The specifications of a command                                    */
struct command
{
   struct command *p_next,     /* Points to the next command          */
                  *p_previous; /* Points to the previous command      */
   char           command;     /* The command in the source code      */
};
typedef struct command COMMAND;

/**********************************************************************/
/*                        Function Prototypes                         */
/**********************************************************************/
COMMAND *create_command_list();
   /* Create a list for the source code commands                      */
CELL    *create_cell_list   ();
   /* Create a list for the cells                                     */
void    get_commands        (char file_name[], COMMAND *p_command_list);
   /* Read the commands from the source code into the command list    */
char    read_command        (COMMAND *p_current_command);
   /* Read the next command from the command list                     */
CELL    *process_command    (char    command,      CELL    *p_data_pointer,
                             COMMAND **p_bookmark, COMMAND **p_instruction);
   /* Process the next command from the command list                  */
void    insert_cell         (CELL *p_current_cell);
   /* Insert a new cell at the end of the cell list                   */
void    move_left           (CELL *p_current_cell);
   /* Move the data pointer to the left                          (<)  */
void    move_right          (CELL *p_current_cell);
   /* Move the data pointer to the right                         (>)  */
void    increment_cell      (CELL *p_current_cell);
   /* Increment the value in the current cell                    (+)  */
void    decrement_cell      (CELL *p_current_cell);
   /* Decrement the value in the current cell                    (-)  */
void    print_cell_value    (CELL *p_current_cell);
   /* Print the ASCII character of the value in the current cell (.)  */
void    get_input           (CELL *p_current_cell);
   /* Get a character and input it into the current cell         (,)  */
void    delete_comm_list    (COMMAND *p_command_list);
   /* Delete a list of comamnds                                       */
void    delete_cell_list    (CELL    *p_cell_list);
   /* Delete a list of data cells                                     */

/**********************************************************************/
/*                           Main Function                            */
/**********************************************************************/
int main(int argc, char *argv[])
{
   char    current_command;    /* The current command from the        */
                               /* command list                        */

   COMMAND *p_bookmark = NULL, /* Points to the start of a loop       */
           *p_command_list,    /* The list of commands from the       */
                               /* source code                         */
           *p_instruction;     /* The instruction pointer, or current */
                               /* command being read                  */
                               
   CELL    *p_cell_list,       /* The list of cells being manipulated */
           *p_data;            /* The data pointer, or current cell   */

   /* Create the command and cell lists                               */
   p_command_list = create_command_list();
   p_cell_list    = create_cell_list();
   p_data         = p_cell_list;

   /* Open the source code file and read it into the commands list    */
   if (argc == 1)
   {
      printf("\nError: no file name. Please provide a file name.");
      exit  (MISSING_FILE_NAME_ERROR);
   }
   get_commands(argv[1], p_command_list);
   p_instruction  = p_command_list->p_next;

   /* Loop processing commands until the end of the list              */
   while (current_command = read_command(p_instruction),
          current_command != END_OF_COMMANDS_CHAR)
   {
      p_data = process_command(current_command, p_data, &p_bookmark,
                               &p_instruction);
      p_instruction = p_instruction->p_next;
   }

   delete_comm_list(p_command_list);
   delete_cell_list(p_cell_list);

   return 0;
}

/**********************************************************************/
/*             Create a list for the source code commands             */
/**********************************************************************/
COMMAND *create_command_list()
{
   COMMAND *p_new_command_list; /* Points to a new command list       */

   if ((p_new_command_list = (COMMAND *)malloc(sizeof(COMMAND))) == NULL)
   {
      printf("\nError: unable to allocate memory for a command list");
      exit  (COMMAND_LIST_ALLOC_ERROR);
   }
   p_new_command_list->command    = COMMAND_LIST_HEADER;
   p_new_command_list->p_previous = NULL;
   p_new_command_list->p_next     = NULL;

   return p_new_command_list;
}

/**********************************************************************/
/*                    Create a list for the cells                     */
/**********************************************************************/
CELL *create_cell_list()
{
   CELL *p_new_cell_list; /* Points to a new cell list                */

   if ((p_new_cell_list = (CELL *)malloc(sizeof(CELL))) == NULL)
   {
      printf("Error: unable to allocate memory for a cell list");
      exit  (CELL_LIST_ALLOC_ERROR);
   }
   p_new_cell_list->cell_value = INITIAL_CELL_VALUE;
   p_new_cell_list->p_previous = NULL;
   p_new_cell_list->p_next     = NULL;

   return p_new_cell_list;
}

/**********************************************************************/
/*    Read the commands from the source code into the command list    */
/**********************************************************************/
void get_commands(char file_name[], COMMAND *p_command_list)
{
   char    command;        /* The current command from the source     */
                           /* code file                               */

   COMMAND *p_current_command = p_command_list,
                           /* Points to the current command           */
           *p_new_command; /* Points to a new command                 */

   FILE    *file_in;       /* The source code file                    */

   file_in = fopen(file_name, "r");
   while (fscanf(file_in, "%c", &command) != EOF)
   {
      switch (command)
      {
         case '<':
         case '>':
         case '+':
         case '-':
         case '.':
         case ',':
         case '[':
         case ']':
            if ((p_new_command = (COMMAND *)malloc
                                             (sizeof(COMMAND))) == NULL)
            {
               printf("Error: unable to allocate a new command");
               exit  (COMMAND_ALLOC_ERROR);
            }
            p_new_command    ->command    = command;
            p_new_command    ->p_previous = p_current_command;
            p_new_command    ->p_next     = NULL;
            p_current_command->p_next     = p_new_command;
            p_current_command             = p_new_command;
            break;
         default:
            break;
      }
   }
   if ((p_new_command = (COMMAND *)malloc
                                    (sizeof(COMMAND))) == NULL)
   {
      printf("Error: unable to allocate a new command");
      exit  (COMMAND_ALLOC_ERROR);
   }
   p_new_command->command    = COMMAND_LIST_TRAILER;
   p_new_command->p_previous = p_current_command;
   p_new_command->p_next     = NULL;
   p_current_command->p_next = p_new_command;

   return;
}

/**********************************************************************/
/*            Read the next command from the command list             */
/**********************************************************************/
char read_command(COMMAND *p_current_command)
{
   char command = p_current_command->command; /* The current command  */

   p_current_command = p_current_command->p_next;

   return command;
}

/**********************************************************************/
/*           Process the next command from the command list           */
/**********************************************************************/
CELL *process_command(char    command,      CELL    *p_data_pointer,
                      COMMAND **p_bookmark, COMMAND **p_instruction)
{
   CELL *p_data = p_data_pointer; /* Points to the current data cell  */

   switch (command)
   {
      case '>':
         if (p_data->p_next == NULL)
         {
            insert_cell(p_data);
         }
         p_data = p_data->p_next;
         break;
      case '<':
         if (p_data->p_previous != NULL)
         {
            p_data = p_data->p_previous;
         }
         break;
      case '+':
         increment_cell(p_data);
         break;
      case '-':
         decrement_cell(p_data);
         break;
      case '.':
         print_cell_value(p_data);
         break;
      case ',':
         get_input(p_data);
         break;
      case '[':
         *p_bookmark = *p_instruction;
         break;
      case ']':
         if (*p_bookmark == NULL)
         {
            printf("\n\nError; invalid loop control.");
            printf(  "\n   Each ']' must have a matching '['.");
            exit  (INVALID_LOOP_CONTROL_ERROR);
         }
         if (p_data->cell_value > 0)
         {
            *p_instruction = *p_bookmark;
         }
         else
         {
            *p_bookmark = NULL;
         }
         break;
      default:
         printf("\nError: invalid command encountered");
         exit  (INVALID_COMMAND_ERROR);
         break;
   }

   return p_data;
}

/**********************************************************************/
/*           Insert a new cell at the end of the cell list            */
/**********************************************************************/
void insert_cell(CELL *p_current_cell)
{
   CELL *p_new_cell; /* The new cell to be inserted                   */

   if ((p_new_cell = (CELL *)malloc(sizeof(CELL))) == NULL)
   {
      printf("\nError: unable to allocate memory for a new cell");
      exit  (CELL_ALLOC_ERROR);
   }
   p_new_cell->cell_value = INITIAL_CELL_VALUE;
   p_new_cell->p_next     = NULL;
   p_current_cell->p_next = p_new_cell;
   p_new_cell->p_previous = p_current_cell;
   
   return;
}

/**********************************************************************/
/*               Move the data pointer to the left (<)                */
/**********************************************************************/
void move_left(CELL *p_current_cell)
{
   if (p_current_cell->p_previous != NULL)
   {
      *p_current_cell = *p_current_cell->p_previous;
   }
   
   return;
}

/**********************************************************************/
/*               Move the data pointer to the right (>)               */
/**********************************************************************/
void move_right(CELL *p_current_cell)
{
   if (p_current_cell->p_next == NULL)
   {
      insert_cell(p_current_cell);
   }
   *p_current_cell = *p_current_cell->p_next;

   return;
}

/**********************************************************************/
/*            Increment the value in the current cell (+)             */
/**********************************************************************/
void increment_cell(CELL *p_current_cell)
{
   p_current_cell->cell_value += 1;

   return;
}

/**********************************************************************/
/*            Decrement the value in the current cell (-)             */
/**********************************************************************/
void decrement_cell(CELL *p_current_cell)
{
   p_current_cell->cell_value -= 1;

   return;
}

/**********************************************************************/
/*   Print the ASCII character of the value in the current cell (.)   */
/**********************************************************************/
void print_cell_value(CELL *p_current_cell)
{
   printf("%c", (char)p_current_cell->cell_value);

   return;
}

/**********************************************************************/
/*       Get a character and input it into the current cell (,)       */
/**********************************************************************/
void get_input(CELL *p_current_cell)
{
   char input; /* The input to be entered into the cell               */

   scanf("%c", &input);
   p_current_cell->cell_value = (int)input;

   return;
}

/**********************************************************************/
/*                     Delete a list of commands                      */
/**********************************************************************/
void delete_comm_list(COMMAND *p_command_list)
{
   COMMAND *p_command = p_command_list->p_next;
                                            /* Points to each command */

   while (p_command != NULL)
   {
      free(p_command_list);
      p_command_list = p_command;
      p_command      = p_command->p_next;
   }
   free(p_command_list);

   return;
}

/**********************************************************************/
/*                    Delete a list of data cells                     */
/**********************************************************************/
void delete_cell_list(CELL *p_cell_list)
{
   CELL *p_cell = p_cell_list->p_next; /* Points to each cell         */

   while (p_cell != NULL)
   {
      free(p_cell_list);
      p_cell_list = p_cell;
      p_cell      = p_cell->p_next;
   }
   free(p_cell_list);

   return;
}
