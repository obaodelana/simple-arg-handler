#include <stdio.h>
#include <time.h>
#include "../simplearghandler.h"

void Wait(float waitTime)
{
    // Starting time
    float startTime = (float) clock() / CLOCKS_PER_SEC;

    // Wait until the current time is greater than start time + time to wait
    while (((float) clock() / CLOCKS_PER_SEC) < startTime + waitTime);
}

int main(int argc, const char *argv[])
{
    AddOptionalArg("n", "number", "Show line number", ARG_BOOL);
    AddPositionalArg("File path", "Path to file", ARG_STRING);
    AddOptionalArg("e", "line-end", "Show line end with $", ARG_BOOL);
    AddOptionalArg("ec", "end-char", "Change line end character", ARG_CHAR);
    AddOptionalArg("l", "max-lines", "Show a specified number of lines", ARG_INT);
    AddOptionalArg("w", "wait", "Show line at specified interval (s)", ARG_FLOAT);
    InitArgs(argc, argv, "Concatenate a file to standard output");

    const char *filePath = GetStringArg("File path");
    FILE *fileToPrint = fopen(filePath, "r");
    if (fileToPrint != NULL)
    {
        bool showNumber = GetBoolArg("n"), showLineEnd = GetBoolArg("e"), lineMax = false, wait = false;
        int ch, n = 1, maxLines = 0;
        if (HasArg("max-lines"))
        {
            lineMax = true;
            maxLines = GetIntArg("max-lines");
        }

        float waitTime = 0.0f;
        if (HasArg("w"))
        {
            wait = true;
            waitTime = GetFloatArg("w");
        }

        char endCharacter = '$';
        if (HasArg("ec"))
        {
            showLineEnd = true;
            endCharacter = GetCharArg("ec");
        }
        
        // Show first number
        if (showNumber) printf("%i ", n);
        // While character is not end of line character
        while ((ch = fgetc(fileToPrint)) != EOF)
        {
            // If new line character
            if (ch == '\n')
            {
                // Increment line count
                n++;
                // Show character at line end
                if (showLineEnd) printf("%c", endCharacter);
            }
            // Print character
            printf("%c", ch);
            // Stop at specified max
            if (lineMax && n > maxLines) break;
            // Show line number at the beginning of a new line, if allowed
            if (ch == '\n' && showNumber) printf("%i ", n);

            // If wait is allowed and new line, wait for specified amount of time
            if (wait && ch == '\n') Wait(waitTime);
        }
        fclose(fileToPrint);
    }
    else 
        printf("cat: %s: No such file or directory\n", filePath);

    FreeArgs();
}