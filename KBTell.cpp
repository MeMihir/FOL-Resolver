#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <unordered_set>

using namespace std;
#include "KBTell.h"

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
    Predicate p(s);
    fol->predicate = p;

    return fol;
}

void printPredicate(Predicate p)
{
    if (p.sign == false)
    {
        cout << "NOT ";
    }
    cout << p.name << "(";
    for (int i = 0; i < p.arity; i++)
    {
        cout << p.arguments[i];
        if (i != p.arity - 1)
        {
            cout << ",";
        }
    }
    cout << ") ";
}

void printFOL(FOL *fol)
{
    if (fol->left == nullptr && fol->right == nullptr)
    {
        printPredicate(fol->predicate);
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
        Clause clause;
        clause.insert(fol->predicate);
        insertClause(clause);
        clause.clear();
        return new FOL();
    }

    // if fol is a IMPLICATION remove it
    if (fol->operatorType == IMPLY)
    {
        // add fol to KB
        fol->left->sign = false;
        fol->operatorType = OR;
    }

    // cout << "No =>\t: "; printFOL(fol); cout << endl; // debug

    // De Morgan's Law
    fol = deMorgan(fol);
    // cout << "deMorgan\t: "; printFOL(fol); cout << endl; // debug

    // Distributive Law
    fol = distributeCNF(fol);
    // cout << "distribute\t: "; printFOL(fol); cout << endl; // debug

    return fol;
}

bool isVariable(string s)
{
    if (s[0] >= 'a' && s[0] <= 'z')
        return true;
    return false;
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
// insert predicate in clause
Clause insertPredicate(Predicate predicate, Clause clause) {
    predicate.sign = !predicate.sign;
    auto it = clause.find(predicate);
    if (it != clause.end())
        clause.erase(it);
    else {
        predicate.sign = !predicate.sign;
        clause.insert(predicate);
    }
    return clause;
}

void insertClause(Clause clause) {
    if(clause.size() == 0)
        return;
    KB.push_back(clause);
    for(Predicate predicate : clause) {
        KBMap[predicate].push_back(KB.size()-1);
    }
}

// build KB
void buildKB(FOL *fol)
{
    // KB
    Clause clause;
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
            insertClause(clause);
            clause.clear();
        }
        else
        {
            if(curr->predicate.arity != 0) {
                clause = insertPredicate(curr->predicate, clause);
            }
        }

        curr = curr->right;
    }
    insertClause(clause);
}

void printKB() {
    // print KB
    cout << "KB" << endl;
    for (int i = 0; i < KB.size(); i++)
    {
        cout << "clause " << i << " : ";
        for (auto it = KB[i].begin(); it != KB[i].end(); it++)
        {   
            if(it != KB[i].begin())
                cout << " | ";
            if (it->sign == false)
                cout << "~";
            cout << it->name << "(";
            for (int j = 0; j < it->arity; j++)
            {
                cout << it->arguments[j];
                if (j != it->arity - 1)
                    cout << ",";
            }
            cout << ") ";
        }
        cout << endl;
    }

    // print KBMap
    cout << "KBMap" << endl;
    for (auto it = KBMap.begin(); it != KBMap.end(); it++)
    {
        if (it->first.sign == false)
            cout << "~";
        cout << it->first.name << "(";
        for (int j = 0; j < it->first.arity; j++)
        {
            cout << it->first.arguments[j];
            if (j != it->first.arity - 1)
                cout << ",";
        }
        cout << ") : ";
        for (int i = 0; i < it->second.size(); i++)
        {
            cout << it->second[i] << " ";
        }
        cout << endl;
    }
}