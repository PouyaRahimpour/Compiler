#include "utils.h"

#define GRAMMAR_PATH "../grammar.txt"
#define TABLE_PATH "../table.txt"
#define START_VAR "program"

enum rule_type {
    VALID,
    SYNCH,
    EMPTY
};
