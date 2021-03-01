# Simple Arg Handler

`./program PositionalArg --flag "something" -f 2`

I wanted to make a terminal app with flags / optional args, but most libraries were too complex (or I didn't search well 😅). So I made my own! This was made in a couple of days, and I learnt new stuff, which was my goal when building it. It is a single header file (600 lines), I tried to make it "simple and easy to use".

## Usage
### Adding arguments
```c
#include simplearghandler.h

// void AddOptionalArg(const char *shortName, const char *longName, const char *help, int argType)
AddOptionalArg("b", "boolean", "Bool arg", ARG_BOOL);
AddOptionalArg("i", "integer", "Integer arg", ARG_INT);
AddOptionalArg("f", "floating", "Float arg", ARG_FLOAT);
AddOptionalArg("c", "character", "Character arg", ARG_CHAR);
AddOptionalArg("s", "string", "String arg", ARG_STRING);

// void AddPositionalArg(const char *name, const char *help, int argType)
AddPositionalArg("Name", "Name arg", ARG_STRING);
AddPositionalArg("Phone", "Phone arg", ARG_INT);
AddPositionalArg("Speed", "Speed arg", ARG_FLOAT);
AddPositionalArg("First letter", "Letter arg", ARG_CHAR);
```

### Accessing arguments
```c
// Check if an argument exists
if (HasArg(const char *key))
{
    .....
}

// Get argument
int phone = GetIntArg("Phone");
float speed = GetFloatArg("Speed");
bool boolean = GetBoolArg("b");
char letter = GetCharArg("First letter");
char *string = GetStringArg("string");
```

### Call at the end of your program
```c
FreeArgs();
```

## Examples
I created two examples `cat` and `todo` in the examples folder. Call `make` and test it out.

---
Feel free to give me feedback on my code, I'm trying to improve my `C` skills 🤓.