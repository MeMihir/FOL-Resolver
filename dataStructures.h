#include <iostream>
#include <vector>
#include <string>

#define NONE 0
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
    int operatorType = 0;
    Predicate predicate;

    FOL()
    {
        left = nullptr;
        right = nullptr;
        operatorType = NONE;
    }
    FOL(FOL *l, FOL *r, int op)
    {
        left = l;
        right = r;
        operatorType = op;
    }
    FOL(Predicate p)
    {
        predicate = p;
    }
};

extern vector<vector<Predicate>> KB; // Knowledge Base (vector of clauses) 

FOL *stringToFOL(string s);
void printFOL(FOL *fol);
FOL *FOLtoCNF(FOL *fol);
FOL *deMorgan(FOL *fol);
FOL *distributeCNF(FOL *fol);
void buildKB(FOL *fol);
void printKB();