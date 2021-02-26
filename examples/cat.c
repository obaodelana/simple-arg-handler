#include <stdio.h>
#include "../simplearghandler.h"

int main(int argc, const char *argv[])
{
    // max lines, highlight character, transitions
    AddPositionalArg("File path", "Path to file", ARG_STRING);
    AddOptionalArg("n", "number", "Show line numbers", ARG_BOOL);
    AddOptionalArg("e", "line-end", "Show line end with $", ARG_BOOL);
    AddOptionalArg("ec", "end-char", "Change line end character", ARG_CHAR);
    InitArgs(argc, argv);

    if (HasArg("File path"))
    {
        const char *filePath = GetStringArg("File path");
        FILE *fileToPrint = fopen(filePath, "r");
        if (fileToPrint != NULL)
        {
            bool showNumber = GetBoolArg("n");
            bool showLineEnd = GetBoolArg("e");

            int ch, n = 1;
            char endCharacter = '$';
            if (HasArg("ec"))
            {
                showLineEnd = true;
                endCharacter = GetCharArg("ec");
            }

            if (showNumber) printf("%i ", n++);
            while ((ch = fgetc(fileToPrint)) != EOF)
            {
                if (showLineEnd && ch == '\n') printf("%c", endCharacter);
                printf("%c", ch);
                if (showNumber && ch == '\n') printf("%i ", n++);
            }
            fclose(fileToPrint);
        }
        else 
            printf("cat: %s: No such file or directory\n", filePath);
    }

    FreeArgs();
}