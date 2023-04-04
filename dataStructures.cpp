#include <iostream>
#include <string>
#include <vector>
#include <stack>

using namespace std;
#include "dataStructures.h"

// =================================================================================================
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
        string name = s.substr(0, pos);
        if (name[0] == '~')
        {
            p.sign = false;
            p.name = name.substr(1, name.length() - 1);
        }
        else
        {
            p.name = name;
        }
        string args = s.substr(pos + 1, s.length() - pos - 2);
        // find all , in args
        // if found, split args into multiple strings
        while (args.find(",") != string::npos)
        {
            pos = args.find(",");
            string pred = args.substr(0, pos);
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

        if (fol->predicate.sign == false)
        {
            cout << "NOT ";
        }
        cout << fol->predicate.name << "(";
        for (int i = 0; i < fol->predicate.arity; i++)
        {
            cout << fol->predicate.arguments[i];
            if (i != fol->predicate.arity - 1)
            {
                cout << ",";
            }
        }
        cout << ")"
             << " ";
        return;
    }
    if (fol->left != nullptr)
    {
        if (fol->sign == false)
            cout << "NOT ";
        cout << "(";
        printFOL(fol->left);
    }
    if (fol->operatorType == AND)
    {
        cout << "AND"
             << " ";
    }
    else if (fol->operatorType == OR)
    {
        cout << "OR"
             << " ";
    }
    else if (fol->operatorType == IMPLY)
    {
        cout << "IMPLY"
             << " ";
    }
    if (fol->right != nullptr)
    {
        printFOL(fol->right);
        cout << ")";
    }
}

// =================================================================================================
// FOL to CNF

FOL *FOLtoCNF(FOL *fol)
{

    cout << "Orignal\t: ";
    printFOL(fol);
    cout << endl; // debug

    // if fol is a predicate
    if (fol->left == nullptr && fol->right == nullptr)
    {
        // add fol to KB
        vector<Predicate> Clause;
        Clause.push_back(fol->predicate);
        KB.push_back(Clause);
        Clause.clear();
        return new FOL();
    }

    // if fol is a IMPLICATION remove it
    if (fol->operatorType == IMPLY)
    {
        // add fol to KB
        fol->left->sign = false;
        fol->operatorType = OR;
    }

    cout << "No =>\t: "; printFOL(fol); cout << endl; // debug

    // De Morgan's Law
    fol = deMorgan(fol);
    cout << "deMorgan\t: "; printFOL(fol); cout << endl; // debug

    // Distributive Law
    fol = distributeCNF(fol);
    cout << "distribute\t: "; printFOL(fol); cout << endl; // debug
    
    return fol;
}

// De Morgan's Law
FOL *deMorgan(FOL *fol)
{
    if (fol->left == nullptr && fol->right == nullptr)
    {
        if (fol->sign == false)
        {
            fol->sign = true;
            fol->predicate.sign = !fol->predicate.sign;
        }
        return fol;
    }
    if (fol->sign == false)
    {
        if (fol->operatorType == AND)
        {
            fol->sign = true;
            fol->operatorType = OR;
            fol->left->sign = !fol->left->sign;
            fol->right->sign = !fol->right->sign;
            fol->left = deMorgan(fol->left);
            fol->right = deMorgan(fol->right);
            return fol;
        }
        if (fol->operatorType == OR)
        {
            fol->sign = true;
            fol->operatorType = AND;
            fol->left->sign = !fol->left->sign;
            fol->right->sign = !fol->right->sign;
            fol->left = deMorgan(fol->left);
            fol->right = deMorgan(fol->right);
            return fol;
        }
    }
    fol->left = deMorgan(fol->left);
    fol->right = deMorgan(fol->right);
    return fol;
}

// Distributive Law
FOL *distributeCNF(FOL *fol)
{
    if (fol->operatorType == AND)
    {
        fol->left = distributeCNF(fol->left);
        fol->right = distributeCNF(fol->right);
    }
    else if (fol->operatorType == OR)
    {
        if (fol->left->operatorType == AND) {
            FOL* a = fol->left->left;
            FOL* b = fol->left->right;
            FOL* c = fol->right;

            FOL* left = new FOL();
            left->operatorType = AND;
            left->right = distributeCNF(new FOL(distributeCNF(b), distributeCNF(c),  OR));
            left->left = distributeCNF(new FOL(distributeCNF(a), distributeCNF(c),  OR));

            return distributeCNF(left);
        }
        else if (fol->right->operatorType == AND)
        {
            FOL* a = fol->left;
            FOL* b = fol->right->left;
            FOL* c = fol->right->right;

            FOL* left = new FOL();
            left->operatorType = AND;
            left->left = distributeCNF(new FOL(distributeCNF(a), distributeCNF(b), OR));
            left->right = distributeCNF(new FOL(distributeCNF(a), distributeCNF(c), OR));

            return distributeCNF(left);
        }
        else
        {
            fol->left = distributeCNF(fol->left);
            fol->right = distributeCNF(fol->right);
        }

        return fol;
    }
    return fol;
}

// =================================================================================================
// Build KB
void buildKB(FOL *fol)
{
    // KB
    vector<Predicate> Clause;
    stack <FOL*> inorder;

    // inorder traversal of FOL
    FOL *curr = fol;
    while( curr != nullptr || !inorder.empty())
    {   
        while(curr != nullptr)
        {
            inorder.push(curr);
            curr = curr->left;
        }

        curr = inorder.top();
        inorder.pop();

        if (curr->operatorType == AND)
        {
            KB.push_back(Clause);
            Clause.clear();
        }
        else
        {
            if(curr->predicate.arity != 0)
                Clause.push_back(curr->predicate);
        }

        curr = curr->right;
    }
    KB.push_back(Clause);
}

void printKB() {
    // print KB
    for (int i = 0; i < KB.size(); i++)
    {
        for (int j = 0; j < KB[i].size(); j++)
        {
            cout << KB[i][j].name << "(";
            for (int k = 0; k < KB[i][j].arguments.size(); k++)
            {
                cout << KB[i][j].arguments[k];
                if (k < KB[i][j].arguments.size() - 1)
                    cout << ",";
            }
            cout << ")";
            if (j < KB[i].size() - 1)
                cout << " | ";
        }
        cout << endl;
    }

}