#ifndef SIMPLE_ARG_HANDLER
#define SIMPLE_ARG_HANDLER

    #include <stdio.h>
    #include <stdlib.h>
    #include <stdbool.h>
    #include <string.h>
    #include <ctype.h>

    // Arg hash table max (numbers + letters)
    #define MAX_ARGS 10 + 26
    // Short arg name max e.g. la
    #define SHORT_NAME_MAX 2
    // Long arg name max e.g. ilovecodingincandcppppppp
    #define LONG_NAME_MAX 25
    // Max characters for string value
    #define STRING_MAX 100

    // Return index in hash table (for digits return 0 - 9, for letters return 10 - 36)
    #define GetTableIndex(key) (isdigit(key) ? (key - '0') : ((tolower(key) - 'a') + 10))

    // Is InitArgs() called?
    static bool initialised = false;

    static char appName[STRING_MAX + 1] = "app";

    // Names of positional arguments in the correct order
    static char positionalArgNames[MAX_ARGS][LONG_NAME_MAX + 1];

    // Array of used indexes
    static int indexesUsed[MAX_ARGS];
    // Count of used indexex, count of positional arguments
    static int usedCount = 0, positionalCount = 0;

    // Supported types
    enum { ARG_INT, ARG_BOOL, ARG_CHAR, ARG_FLOAT, ARG_STRING };

    typedef struct Arg
    {
        // shortName: h, longName: help, help: "Text to show in help mode"
        char shortName[SHORT_NAME_MAX + 1], longName[LONG_NAME_MAX + 1], help[STRING_MAX + 1];
        // Pointer to value
        void *value;
        // Type from enum
        int argType;
        // optional or positional
        bool optional;
        // Chaining
        struct Arg *next;
    } Arg;

    // Hash table for all arguments
    static Arg *args[MAX_ARGS];

    void FreeArgs(void)
    {
        // Go through all assigned/used arguments
        for (int i = 0; i < usedCount; i++)
        {
            // Pointer to current arg, pointer to next arg
            Arg *current = args[indexesUsed[i]], *next = args[indexesUsed[i]];
            while (current != NULL)
            {
                // Save address of next arg
                next = current->next;
                // If value is not empty, free it
                if (current->value != NULL)
                    free(current->value);
                // Free current arg
                free(current);

                // Go to next arg
                current = next;
            }
        }
    }

    static void PrintAndQuit(const char *text)
    {
        if (strlen(text) > 1) printf("%s\n", text);
        FreeArgs();
        exit(EXIT_FAILURE);
    }

    static int GetHyphenEnd(const char* hyphenatedText)
    {
        int textStart = 1;
        // Increment textStart until hypen end or string end
        for (; hyphenatedText[textStart] != '\0' && hyphenatedText[textStart] == '-'; textStart++);
        // Return position of first letter or number
        return textStart;
    }

    static void PrintUsage(void)
    {
        // Keeps track of last string printed, string to be printed
        char usageText[STRING_MAX + 1];
        
        printf("usage: %s ", appName);
        // Go through all assigned/used arguments
        for (int i = 0; i < usedCount; i++)
        {
            // Current argument
            Arg *current = args[indexesUsed[i]];
            while (current != NULL)
            {
                // Clear string
                strcpy(usageText, "");
                // For optional args use this format: [-shortName --longName]
                if (current->optional)
                    sprintf(usageText, "[-%s --%s]", current->shortName, current->longName);
                // For positional args use this format: 'longName'
                else
                    sprintf(usageText, "'%s'", current->longName);
                printf("%s ", usageText);

                // Go to next arg
                current = current->next;
            }
        }
        printf("\n");

        PrintAndQuit(" ");
    }

    // For args that have a different starting letter for short and long name
    static Arg* LinearSearchTable(const char* key)
    {
        for (int i = 0; i < usedCount; i++)
        {
            Arg *temp = args[indexesUsed[i]];
            while (temp != NULL)
            {
                // If current arg has matching name, arg found
                if (!strcmp(key, temp->shortName) || !strcmp(key, temp->longName))
                    return temp;

                // Move to next arg
                temp = temp->next;
            }
        }

        return NULL;
    }

    static Arg* SearchTable(const char *key)
    {
        // If first character is not alphanumeric, return NULL
        if (!isalnum(key[0])) return NULL;

        // Arg for traversing
        Arg *temp = args[GetTableIndex(key[0])];
        while (temp != NULL)
        {
            // If current arg has matching name, arg found
            if (!strcmp(key, temp->shortName) || !strcmp(key, temp->longName))
                return temp;

            // Move to next arg
            temp = temp->next;
        }

        // If not arg found, try doing a linear search
        return LinearSearchTable(key);
    }

    static void InsertNewIndex(int insertIndex, int index)
    {
        if (usedCount > 0)
        {
            // Go from the last index to insertion index
            for (int i = usedCount; i >= insertIndex; i--)
                // Shift elements backward and when at insertion index add new index
                indexesUsed[i] = (i == insertIndex) ? index : indexesUsed[i - 1];
        }
        else
            indexesUsed[usedCount] = index;
        // Item added
        usedCount++;
    }

    static void AddNewIndex(int index, Arg *arg)
    {
        // Go through all assigned/used arguments
        for (int i = 0; i < usedCount; i++)
            // If index is already in list, exit function
            if (indexesUsed[i] == index) return;

        // Else, add new index to list

        // Add optional argument to end of list
        if (arg->optional)
            indexesUsed[usedCount++] = index;
        // Insert positional argument at the positionalCount index
        else
        {
            InsertNewIndex(positionalCount, index);
            // Add positional arg name to list
            strcpy(positionalArgNames[positionalCount++], arg->longName);
        }
    }

    static void AddToTable(Arg *arg, const char *key)
    {
        // Get index based on first character
        int tableIndex = GetTableIndex(key[0]);

        // If current index is empty
        if (args[tableIndex] == NULL)
            // Set arg to it
            args[tableIndex] = arg;
        else
        {
            Arg *temp = args[tableIndex];
            // Look for null arg
            while (temp->next != NULL)
            {
                // If current arg has the same name as new arg
                if (!strcmp(key, temp->shortName) || !strcmp(key, temp->longName))
                {
                    printf("simplearghandler: Error: Duplicate name found: %s\n", key);
                    PrintAndQuit(" ");
                }

                // Next arg
                temp = temp->next;
            }

            // Set null arg
            temp->next = arg;
        }

        // Add index to indexesUsed
        AddNewIndex(tableIndex, arg);
    } 

    static void SetValue(Arg *arg, const char *value)
    {
        // Set value of arg based on arg type
        switch (arg->argType)
        {
            case ARG_INT:
                // If first letter is not a digit, quit
                if (!isdigit(value[0]) && value[0] != '-')
                {
                    printf("%s: Error: -%s/--%s: invalid int value: %s\n", appName, arg->shortName, arg->longName, value);
                    PrintUsage();
                }

                // Else, allocate memory for an int
                int *integer = (int *) malloc(sizeof(int));
                // Set its value
                *integer = atoi(value);
                // Point arg's value field to new memory address
                arg->value = integer;
                break;

            case ARG_FLOAT:
                // If first letter is not a digit or a decimal sign, quit
                if (!isdigit(value[0]) && (value[0] != '.' || value[0] != '-'))
                {
                    printf("%s: Error: -%s/--%s: invalid float value: %s\n", appName, arg->shortName, arg->longName, value);
                    PrintUsage();
                }

                // Else, allocate memory for a float
                float *floating = (float *) malloc(sizeof(float));
                // Set its value
                *floating = atof(value);
                // Point arg's value field to new memory address
                arg->value = floating;
                break;

            case ARG_CHAR:
                // If string contains more than one letter, quit
                if (strlen(value) > 1)
                {
                    printf("%s: Error: -%s/--%s: invalid char value: %s\n", appName, arg->shortName, arg->longName, value);
                    PrintUsage();
                }

                // Else, allocate memory for a char
                char *character = (char *) malloc(sizeof(char));
                // Set its value
                *character = value[0];
                // Point arg's value field to new memory address
                arg->value = character;
                break;

            case ARG_STRING:
                ; // A label has to be followed by a statement, hence the semi-colon (a declaration is not a statement)
                // Allocate memory for a string of adequate size
                char *string = (char *) malloc(sizeof(char) * (strlen(value) + 1));
                // Set its value
                strcpy(string, value);
                // Point arg's value field to new memory address
                arg->value = string;
                break;

            // Unknown arg type
            default:
                PrintAndQuit("simplearghandler: Error: Unknown argument type\nArgument types: ARG_INT, ARG_BOOL, ARG_CHAR, ARG_FLOAT, ARG_STRING");
                break;
        }

        // Memory allocation unsucessful
        if (arg->value == NULL)
            PrintAndQuit("simplearghandler: Error: Memory allocation failed :(");
    }

    void AddOptionalArg(const char *shortName, const char *longName, const char *help, int argType)
    {
        if (initialised)
            PrintAndQuit("simplearghandler: Error: Can't add more arguments, InitArgs() has already been called");

        // If text exceeds max, print error
        if (strlen(shortName) > SHORT_NAME_MAX || strlen(longName) > LONG_NAME_MAX || strlen(help) > STRING_MAX)
        {
            char startText[11] = "Long name";
            if (strlen(shortName) > SHORT_NAME_MAX) strcpy(startText, "Short name");
            else if (strlen(help) > STRING_MAX) strcpy(startText, "Help text");
            
            int maxNum = (!strcmp(startText, "Long name")) ? LONG_NAME_MAX : (!strcmp(startText, "Help text")) ? STRING_MAX : SHORT_NAME_MAX;

            printf("simplearghandler: Error: %s is too long! Max is %i characters\n", startText, maxNum);
            PrintAndQuit(" ");
        }

        // If name doesn't start with a letter or number
        if (!isalnum(shortName[0]) || !isalnum(longName[0]))
        {
            printf("simplearghandler: Error: Name (%s) should only contain letters and numbers\n", (!isalnum(shortName[0]) ? shortName : longName));
            PrintAndQuit(" ");
        }

        // Allocate memory for a new arg
        Arg *arg = (Arg *) calloc(1, sizeof(Arg));
        if (arg != NULL)
        {
            // Set available values
            strcpy(arg->shortName, shortName);
            strcpy(arg->longName, longName);
            strcpy(arg->help, help);
            arg->argType = argType;
            arg->optional = true;
            arg->value = NULL;
            arg->next = NULL;

            // Add to table use both short and long names
            AddToTable(arg, arg->shortName);
        }

        // Allocation not successful
        else
            PrintAndQuit("simplearghandler: Error: Memory allocation failed :(");
    }

    void AddPositionalArg(const char *name, const char *help, int argType)
    {
        if (initialised)
            PrintAndQuit("simplearghandler: Error: Can't add more arguments, InitArgs() has already been called");

        // If text exceeds max, print error
        if (strlen(name) > LONG_NAME_MAX || strlen(help) > STRING_MAX)
        {
            char startText[11] = "Name";
            if (strlen(help) > STRING_MAX) strcpy(startText, "Help text");
            
            int maxNum = (!strcmp(startText, "Name")) ? LONG_NAME_MAX : STRING_MAX;

            printf("simplearghandler: Error: %s is too long! Max is %i characters\n", startText, maxNum);
            PrintAndQuit(" ");
        }

        // If name doesn't start with a letter or number
        if (!isalnum(name[0]))
        {
            printf("simplearghandler: Error: Name (%s) should only contain letters and numbers\n", name);
            PrintAndQuit(" ");
        }

        // Allocate memory for a new arg
        Arg *arg = (Arg *) calloc(1, sizeof(Arg));
        // If allocation successful
        if (arg != NULL)
        {
            // Set available values
            strcpy(arg->shortName, " ");
            strcpy(arg->longName, name);
            strcpy(arg->help, help);
            arg->argType = argType;
            arg->optional = false;
            arg->value = NULL;
            arg->next = NULL;

            // Add to table using longName as the key
            AddToTable(arg, arg->longName);
        }

        // Allocation not successful
        else
            PrintAndQuit("simplearghandler: Error: Memory allocation failed :(");
    }

    void InitArgs(int argc, const char* argv[])
    {
        if (initialised)
            PrintAndQuit("simplearghandler: Error: Can't initialise more than once");

        // Get app name
        strcpy(appName, argv[0]);

        int positionalsAdded = 0;
        // Go through all arguments excluding first one (app name)
        for (int i = 1; i < argc; i++)
        {
            // Optional argument
            if (argv[i][0] == '-' && argv[i][GetHyphenEnd(argv[i])] != '\0')
            {
                // Search table using text after hyphen
                Arg *arg = SearchTable(&argv[i][GetHyphenEnd(argv[i])]);
                // If arg found
                if (arg != NULL)
                {
                    // If argument is of type bool
                    if (arg->argType == ARG_BOOL)
                    {
                        // Allocate memory for a bool
                        bool *boolean = (bool *) malloc(sizeof(bool));
                        // Allocation not successful
                        if (boolean == NULL)
                            PrintAndQuit("simplearghandler: Error: Memory allocation failed :(");
                        // Set boolean to true
                        *boolean = true;
                        // Point arg's value to boolean
                        arg->value = boolean;
                    }

                    // Else of type: int, float, char, string
                    else
                    {
                        // If there is no value following arg declaration
                        if (i + 1 >= argc || argv[i + 1][0] == '-')
                        {
                            printf("%s: Error: -%s/--%s: expected one argument\n", appName, arg->shortName, arg->longName);
                            PrintUsage();
                        }

                        // Else set arg's value
                        SetValue(arg, argv[++i]);
                    }
                }

                // Argument not found
                else
                {
                    printf("%s: Error: Unrecognised argument: %s\n", appName, argv[i]);
                    PrintUsage();
                }
            }

            // Positional argument
            else
            {
                // If there are still positional arguments left
                if (positionalsAdded < positionalCount)
                {
                    // Search table for positional arg based on index
                    Arg *arg = SearchTable(positionalArgNames[positionalsAdded]);
                    // If arg found
                    if (arg != NULL)
                    {
                        // Set it's value
                        SetValue(arg, argv[i]);
                        // Positional added
                        positionalsAdded++;   
                    }
                    // Shouldn't ever happen
                    else
                        printf("simplearghandler: Error: No positional argument found\n");
                }

                // If no positional arguments are left, or there were none
                else
                {
                    printf("%s: Error: Unrecognised argument: %s\n", appName, argv[i]);
                    PrintUsage();
                }
            }
        }

        // If positional arguments are left
        if (positionalsAdded < positionalCount)
        {
            printf("%s: Error: The following arguments are required: ", appName);
            // List all required arguments
            for (int i = positionalsAdded; i < positionalCount; i++)
                printf("'%s' ", positionalArgNames[i]);
            printf("\n");
            PrintUsage();
        }

        initialised = true;
    }

    // Returns true if argument has been set (don't use for bools)
    bool HasArg(const char *key)
    {
        if (!initialised)
            PrintAndQuit("simplearghandler: Error: Call InitArgs() before accessing arguments!");
        return SearchTable(key)->value != NULL;
    }

    int GetIntArg(const char *key)
    {
        Arg *arg = SearchTable(key);
        if (!HasArg(key))
        {
            printf("simplearghandler: Error: %s not found\n", key);
            PrintAndQuit(" ");
        }

        else if (arg->argType != ARG_INT)
        {
            printf("simplearghandler: Error: %s is not of type INT!\n", key);
            PrintAndQuit(" ");
        }

        return *((int *) arg->value);
    }

    // Returns true if arg is used else false
    bool GetBoolArg(const char *key)
    {
        Arg *arg = SearchTable(key);
        if (arg == NULL)
        {
            printf("simplearghandler: Error: %s not found\n", key);
            PrintAndQuit(" ");
        }

        else if (arg->argType != ARG_BOOL)
        {
            printf("simplearghandler: Error: %s is not of type BOOL!\n", key);
            PrintAndQuit(" ");
        }

        return arg->value != NULL;
    }

    char GetCharArg(const char *key)
    {
        Arg *arg = SearchTable(key);
        if (!HasArg(key))
        {
            printf("simplearghandler: Error: %s not found\n", key);
            PrintAndQuit(" ");
        }

        else if (arg->argType != ARG_CHAR)
        {
            printf("simplearghandler: Error: %s is not of type CHAR!\n", key);
            PrintAndQuit(" ");
        }

        return ((char *) arg->value)[0];
    }

    float GetFloatArg(const char *key)
    {
        Arg *arg = SearchTable(key);
        if (!HasArg(key))
        {
            printf("simplearghandler: Error: %s not found\n", key);
            PrintAndQuit(" ");
        }

        else if (arg->argType != ARG_FLOAT)
        {
            printf("simplearghandler: Error: %s is not of type FLOAT!\n", key);
            PrintAndQuit(" ");
        }

        return *((float *) arg->value);
    }

    const char* GetStringArg(const char *key)
    {
        Arg *arg = SearchTable(key);
        if (!HasArg(key))
        {
            printf("simplearghandler: Error: %s not found\n", key);
            PrintAndQuit(" ");
        }

        else if (arg->argType != ARG_STRING)
        {
            printf("simplearghandler: Error: %s is not of type STRING/CHAR*!\n", key);
            PrintAndQuit(" ");
        }

        return (const char *) arg->value;
    }

#endif