#include <iostream>
#include <string>
#include <vector>

using namespace std;
#include "dataStructures.h"


// convert string to FOL
FOL *stringToFOL(string s)
{
    int pos;

    // find => in s
    // if found, split s into two strings
    // else return FOL with empty strings
    FOL *fol = new FOL();
    pos = s.find(" => ");
    if (pos != string::npos)
    {
        string left = s.substr(0, pos);
        string right = s.substr(pos + 4, s.length() - pos - 4);
        fol->left = stringToFOL(left);
        fol->right = stringToFOL(right);
        fol->operatorType = IMPLY;
        return fol;
    }

    // find | in s
    // if found, split s into two strings
    // else return FOL with empty strings
    pos = s.find(" | ");
    if (pos != string::npos)
    {
        string left = s.substr(0, pos);
        string right = s.substr(pos + 3, s.length() - pos - 3);
        fol->left = stringToFOL(left);
        fol->right = stringToFOL(right);
        fol->operatorType = OR;
        return fol;
    }

    // find & in s
    // if found, split s into two strings
    // else return FOL with empty strings
    pos = s.find(" & ");
    if (pos != string::npos)
    {
        string left = s.substr(0, pos);
        string right = s.substr(pos + 3, s.length() - pos - 3);
        fol->left = stringToFOL(left);
        fol->right = stringToFOL(right);
        fol->operatorType = AND;
        return fol;
    }

    // PREDICATE
    Predicate p;
    // find ( in s
    // if found, split s into two strings
    pos = s.find("(");
    if (pos != string::npos)
    {
        p.name = s.substr(0, pos);
        string args = s.substr(pos + 1, s.length() - pos - 2);
        // find all , in args
        // if found, split args into multiple strings
        while (args.find(",") != string::npos)
        {
            pos = args.find(",");
            p.arguments.push_back(args.substr(0, pos));
            args = args.substr(pos + 1, args.length() - pos - 1);
        }
        p.arguments.push_back(args);
        p.arity = p.arguments.size();
        fol->predicate = p;
    }
    return fol;
}

void printFOL(FOL *fol)
{
    if (fol->left == nullptr && fol->right == nullptr)
    {
        cout << fol->predicate.name << "(";
        for (int i = 0; i < fol->predicate.arity; i++)
        {
            cout << fol->predicate.arguments[i];
            if (i != fol->predicate.arity - 1)
            {
                cout << ",";
            }
        }
        cout << ")" << " ";
        return;
    }
    if (fol->left != nullptr)
    {
        cout<<"(";
        printFOL(fol->left);
    }
    if (fol->operatorType == AND)
    {
        cout << "AND" << " ";
    }
    else if (fol->operatorType == OR)
    {
        cout << "OR" << " ";
    }
    else if (fol->operatorType == IMPLY)
    {
        cout << "IMPLY" << " ";
    }
    if (fol->right != nullptr)
    {
        printFOL(fol->right);
        cout<<")";
    }
}