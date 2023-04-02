#include <iostream>
#include <vector>
#include <string>

#define AND 1
#define OR 2
#define IMPLY 3

struct Predicate
{
    bool sign = true; // true = positive, false = negative
    string name;
    int arity = 0;
    vector<string> arguments;
};

struct FOL
{
    bool sign = true; // true = positive, false = negative
    FOL *left = nullptr;
    FOL *right = nullptr;
    int operatorType = AND;
    Predicate predicate;
};

extern vector<Predicate> Clause; // Clause
extern vector<vector<Predicate>> KB; // Knowledge Base (vector of clauses) 

FOL *stringToFOL(string s);
void printFOL(FOL *fol);