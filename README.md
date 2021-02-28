# Simple Arg Handler

I wanted to make a terminal app with flags / optional args, but most libraries were too complex (or I didn't search well 😅). So I made my own! This was made in a couple of days, and I learnt new stuff, which was my goal when building it. It is a single header file (600 lines), I tried to make it "simple and easy to use".

## Usage
### Adding arguments
```c
#include simplearghandler.h

// void AddOptionalArg(const char *shortName, const char *longName, const char *help, int argType)
AddOptionalArg("n", "number", "Number arg", ARG_INT);
AddOptionalArg("c", "character", "Character arg", ARG_CHAR);
AddOptionalArg("b", "boolean", "Bool arg", ARG_BOOL);

// void AddPositionalArg(const char *name, const char *help, int argType)
AddPositionalArg("string", "String positional arg", ARG_STRING);
AddPositionalArg("float", "Float positional arg", ARG_FLOAT);
```

### Accessing arguments
```c
if (HasArg("n"))
    int number = GetIntArg("number");
bool boolean = GetBoolArg("b");
char *string = GetStringArg("string");
```

### Call at the end of your program
```c
FreeArgs();
```

## Examples
I created two examples `cat` and `todo` in the examples folder. Call `make cat` or `make todo` to test it out.

---
Feel free to give me feedback on my code, I'm trying to improve my `C` skills 🤓.