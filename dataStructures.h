#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
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

    Predicate()
    {
        name = "";
        arity = 0;
    }

    Predicate(string s)
    {
        int pos = s.find("(");
        if (s[0] == '~')
        {
            sign = false;
            name = s.substr(1, pos - 1);
        }
        else
        {
            name = s.substr(0, pos);
        }
        string args = s.substr(pos + 1, s.length() - pos - 2);
        // find all , in args
        // if found, split args into multiple strings
        while (args.find(",") != string::npos)
        {
            pos = args.find(",");
            arguments.push_back(args.substr(0, pos));
            args = args.substr(pos + 1, args.length() - pos - 1);
        }
        arguments.push_back(args);
        arity = arguments.size();
    }

    bool operator==(const Predicate& p) const {
        return (sign == p.sign && name == p.name && arity == p.arity);
    }
};

struct PredicateHash {
    size_t operator()(const Predicate& p) const {
        size_t hashVal = 0;
        hashVal ^= hash<bool>{}(p.sign);
        hashVal ^= hash<string>{}(p.name);
        hashVal ^= hash<int>{}(p.arity);
        return hashVal;
    }
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

extern vector<unordered_set<Predicate, PredicateHash>> KB; // Knowledge Base (vector of clauses) 
extern unordered_map<Predicate, vector<int>, PredicateHash> KBMap; // Knowledge Base Table (map of predicate to clause number)

FOL *stringToFOL(string s);
void printPredicate(Predicate p);
void printFOL(FOL *fol);
FOL *FOLtoCNF(FOL *fol);
FOL *deMorgan(FOL *fol);
FOL *distributeCNF(FOL *fol);
unordered_set<Predicate, PredicateHash> insertPredicate(Predicate p, unordered_set<Predicate, PredicateHash> Clause);
void insertClause(unordered_set<Predicate, PredicateHash> Clause);
void buildKB(FOL *fol);
void printKB();