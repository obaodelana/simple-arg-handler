#include "../simplearghandler.h"

const int stringMax = 100;

int TextToArray(FILE *textFile, char array[stringMax][stringMax])
{
    int lastLine = 0;
    // If textFile is available
    if (textFile != NULL)
    {
        char line[stringMax];
        // Read a line, make sure the line isn't empty
        while(fgets(line, stringMax, textFile) != NULL && line[0] != '\0')
        {
            // Check if line contains a new character
            char *newLine = strchr(line, '\n');
            // If it does
            if (newLine != NULL)
            {
                // Get the index in string (e.g 0x05 - 0x01, index of new line is 4)
                int newLineIndex = (int) (newLine - line);
                // Remove new line, by ending the string there
                line[newLineIndex] = '\0';
            }
            // Copy line to array
            strcpy(array[lastLine++], line);
        }
        // When done, close file
        fclose(textFile);
    }
    return lastLine;
}

int main(int argc, const char *argv[])
{
    AddOptionalArg("a", "add", "Add a todo", ARG_STRING);
    AddOptionalArg("r", "remove", "Remove a todo (by index)", ARG_INT);
    AddOptionalArg("d", "done", "Todo done (by index)", ARG_INT);
    AddOptionalArg("c", "clear", "Clear all todos", ARG_BOOL);
    InitArgs(argc, argv, "Todo list");

    char todos[stringMax][stringMax];
    // Set todos array from file and get last line
    int lastLine = TextToArray(fopen("todo.txt", "r"), todos);

    // Open file for writing
    FILE *todoList = fopen("todo.txt", "w");
    if (todoList == NULL)
    {
        printf("Insufficient space\n");
        return EXIT_FAILURE;
    }

    // If add flag used, add given string to array
    if (HasArg("add"))
        strcpy(todos[lastLine++], GetStringArg("add"));

    // If remove flag used, remove string by index from array
    else if (HasArg("remove"))
    {
        int removeIndex = GetIntArg("remove");
        // Make sure index is within bounds
        if (removeIndex <= 0 || removeIndex > lastLine)
        {
            printf("Invalid index\n");
            FreeArgs();
            return EXIT_FAILURE;
        }
        // Remove from array, by setting is empty
        strcpy(todos[removeIndex - 1], "");
    }

    // If done flag used, concatenate ' (done)' to line
    else if (HasArg("d"))
    {
        int doneIndex = GetIntArg("done");
        // Make sure index is within bounds
        if (doneIndex <= 0 || doneIndex > lastLine)
        {
            printf("Invalid index\n");
            FreeArgs();
            return EXIT_FAILURE;
        }
        // Check if done has already been added
        if (strstr(todos[doneIndex - 1], " (done)") == NULL)
            // If not add ' (done)'
            strcat(todos[doneIndex - 1], " (done)");
    }

    // If clear flag used
    else if (GetBoolArg("clear"))
    {
        lastLine = 1;
        strcpy(todos[0], "");
    }

    // If todo empty, or cleared ^^
    if (lastLine == 0 || todos[0][0] == '\0')
    {
        // Print message
        printf("Todo list empty, add a todo with -a/--add \"todo\"\n");
        // Print nothing to text
        fprintf(todoList, "%s", todos[0]);
    }

    // If not empty
    else
    {
        // Go through each line (in array)
        for (int i = 0, n = 1; i < lastLine; i++)
        {
            // If current line is not empty
            if (todos[i][0] != '\0')
            {
                // Print todo with number to stdout
                printf("%i. %s\n", n++, todos[i]);
                // Print todo to file
                fprintf(todoList, "%s", todos[i]);
                // Add a new line for lines that are not the last line
                if (i < (lastLine - 1)) fprintf(todoList, "\n");
            }
        }
    }

    FreeArgs();
    return 0;
}