# Simple Arg Handler

I wanted to make a terminal app with flags / optional args, but most libraries were too complex (or I didn't search well 😅). So I made my own! This was made in a couple of days, and I learnt new stuff, which was my goal when building it. It is a single header file (600 lines), I tried to make it "simple and easy to use".

## Functions
- `void AddOptionalArg(const char *shortName, const char *longName, const char *help, int argType)`
- `void AddPositionalArg(const char *name, const char *help, int argType`
- `void InitArgs(int argc, const char* argv[], const char* appDescription)`
- `bool HasArg(const char *key)`
- `bool GetBoolArg(const char *key)`
- `int GetIntArg(const char *key)`
- `float GetFloatArg(const char *key)`
- `char GetCharArg(const char *key)`
- `const char* GetStringArg(const char *key)`

### Argument types (`int argType`)
```c
enum
{ 
    ARG_INT,
    ARG_BOOL,
    ARG_CHAR,
    ARG_FLOAT,
    ARG_STRING
};
```

## Usage
### cat example
Let's create a program that prints out the contents of file, it takes in a required argument which is the file name and some other optional arguments like:
- -n/--number: Shows line numbers
- -l/--max-lines: Shows a maximum number of lines
- -w/--wait: Shows a line after given seconds

Full code in `examples` folder (to build, type `make cat`).

```c
#include "../simplearghandler.h"

int main(int argc, const char *argv[])
{
    AddPositionalArg("File path", "Path to file", ARG_STRING);
    AddOptionalArg("n", "number", "Show line number", ARG_BOOL);
    AddOptionalArg("l", "max-lines", "Show a specified number of lines", ARG_INT);
    AddOptionalArg("w", "wait", "Show line at specified interval (second)", ARG_FLOAT);
    InitArgs(argc, argv, "Concatenate a file to standard output");

    const char *filePath = GetStringArg("File path");
    FILE *fileToPrint = fopen(filePath, "r");
    if (fileToPrint != NULL)
    {
        bool showNumber = GetBoolArg("n");
        // Check if l/max-lines and w/wait arguments were set
        bool lineMax = HasArg("max-lines"), wait = HasArg("w");
        int ch, n = 1, maxLines = 0;
        float waitTime = 0.0f;
        
        // If the arguments were set, get argument values
        if (lineMax) maxLines = GetIntArg("max-lines");
        if (wait) waitTime = GetFloatArg("w");
        
        // Show first line number
        if (showNumber) printf("%i ", n);
        // While character is not end of line character
        while ((ch = fgetc(fileToPrint)) != EOF)
        {
            // If new line character, increment line count
            if (ch == '\n') n++;
            // Print character
            printf("%c", ch);
            // Stop at specified max, if allowed
            if (lineMax && n > maxLines) break;
            // Show line number at the beginning of a new line, if allowed
            if (ch == '\n' && showNumber) printf("%i ", n);
            // If wait is allowed and current character is a new line, wait for specified amount of time
            if (wait && ch == '\n') Wait(waitTime);
        }
        fclose(fileToPrint);
    }
    else 
        printf("cat: %s: No such file or directory\n", filePath);

    FreeArgs();
    return 0;
}
```

---
Feel free to give me feedback on my code, I'm trying to improve my `C` skills 🤓.
