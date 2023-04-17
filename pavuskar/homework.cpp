#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <cstdlib>
#include <stack>
#include <queue>
#include <fstream>
#include <chrono>

using namespace std;

#define NONE 0
#define AND 1
#define OR 2
#define IMPLY 3


// ====================== DATASTRUCTURES =======================================

#define ClauseVect vector<Predicate>

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

    bool compare(Predicate p);
    void print();
};

struct PredicateHash {
    size_t operator()(const Predicate& p) const {
        size_t hashVal = 0;
        hashVal ^= hash<bool>{}(p.sign);
        hashVal ^= hash<string>{}(p.name);
        hashVal ^= hash<int>{}(p.arity);
        // for (int i = 0; i < p.arity; i++)
        // {
        //     if(isVariable(p.arguments[i]))
        //         hashVal ^= hash<string>{}("v");
        //     else
        //         hashVal ^= hash<string>{}(p.arguments[i]);
        // }
        return hashVal;
    }
};

struct Clause {
    ClauseVect clause;

    Clause()
    {
        clause = ClauseVect();
    }

    void insert(Predicate p);
    void clear();
    ClauseVect::iterator find(Predicate p);
    ClauseVect::iterator findStrict(Predicate p);
    void erase(ClauseVect::iterator it);
    size_t size();
    bool empty();
    bool compare(Clause c);
    void print();
    bool checkTautology();
    Clause standardize();
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

struct ClauseCompare {
  bool operator()(Clause c1, Clause c2) const {
    return c1.size() > c2.size();
  }
};

vector<Clause> KB; // Knowledge Base (vector of clauses) 
unordered_map<Predicate, unordered_set<int>, PredicateHash> KBMap; // Knowledge Base Table (map of predicate to clause number)
vector<Clause> visited;
unordered_map<Predicate, vector<int>, PredicateHash> visitedMap;
vector<string>input;

// ============================================================================================================
// ======================================= PREDICATE FUNCTIONS ===================================================


void Predicate::print()
{
    if (sign == false)
    {
        cout << "NOT ";
    }
    cout << name << "(";
    for (int i = 0; i < arity; i++)
    {
        cout << arguments[i];
        if (i != arity - 1)
        {
            cout << ",";
        }
    }
    cout << ") ";
}

bool Predicate::compare(Predicate predicate)
{
    if (sign != predicate.sign || name != predicate.name || arity != predicate.arity)
        return false;
    for (int i = 0; i < arity; i++)
    {
        if (arguments[i] != predicate.arguments[i])
            return false;
    }
    return true;
}

bool isVariable(string s)
{
    if (s[0] >= 'a' && s[0] <= 'z')
        return true;
    return false;
}

// ============================================================================================================
// ======================================= FOL FUNCTIONS ===================================================
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

void printFOL(FOL *fol)
{
    if (fol->left == nullptr && fol->right == nullptr)
    {
        fol->predicate.print();
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

void addClause2KB(Clause clause) {
    if(clause.size() == 0)
        return;
    KB.push_back(clause);
    for(Predicate predicate : clause.clause) {
        for(int i=0; i<predicate.arity; i++)
            predicate.arguments[i] = "v";
        KBMap[predicate].insert(KB.size()-1);
    }
}

FOL *FOLtoCNF(FOL *fol)
{

    // cout << "Orignal\t: "; printFOL(fol); cout << endl; // debugM

    // if fol is a predicate
    if (fol->left == nullptr && fol->right == nullptr)
    {
        // add fol to KB
        Clause clause;
        clause.insert(fol->predicate);
        addClause2KB(clause);
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

// ============================================================================================================
// ======================================= CLAUSE FUNCTIONS ===================================================
void Clause::insert(Predicate predicate) 
{
    // predicate.sign = !predicate.sign;
    auto it = findStrict(predicate);

    // if (it != clause.end()) {
    //     erase(it);
    //     return;
    // }
    
    // predicate.sign = !predicate.sign;
    it = findStrict(predicate);
    if (it == clause.end()){
        // printPredicate(predicate);
        clause.push_back(predicate);
    }
}

ClauseVect::iterator Clause::find(Predicate predicate) 
{
    for (auto it = clause.begin(); it != clause.end(); it++) {
        bool flag = true;
        if (it->sign == predicate.sign && it->name == predicate.name && it->arity == predicate.arity) {
            for (int i = 0; i < it->arity; i++) {
                if (!isVariable(it->arguments[i]) 
                    && !isVariable(predicate.arguments[i]) 
                    && (it->arguments[i] != predicate.arguments[i])
                ) {
                    flag = false;
                    break;
                }
            }
            if (flag)
                return it;
        }
    }
    return clause.end();
}

ClauseVect::iterator Clause::findStrict(Predicate predicate) 
{
    for (auto it = clause.begin(); it != clause.end(); it++) {
        if (it->sign == predicate.sign && it->name == predicate.name && it->arity == predicate.arity) {
            bool flag = true;
            for (int i = 0; i < it->arity; i++) {
                if (it->arguments[i] != predicate.arguments[i]) {
                    flag = false;
                    break;
                }
            }
            if (flag)
                return it;
        }
    }
    return clause.end();
}

void Clause::erase(ClauseVect::iterator it) 
{
    clause.erase(it);
}

size_t Clause::size() 
{
    return clause.size();
}

void Clause::clear() 
{
    clause.clear();
}

bool Clause::empty() 
{
    return clause.empty();
}

bool Clause::compare(Clause c)
{
    if (c.size() != size())
        return false;
    for (auto it = clause.begin(); it != clause.end(); it++) {
        if (c.find(*it) == c.clause.end())
            return false;
    }
    return true;
}

// ?use predicate print
void Clause::print()
{
    for (auto it = clause.begin(); it != clause.end(); it++) {
        if (it != clause.begin())
            cout << " | ";
        if (!it->sign)
            cout << "~";
        cout << it->name << "(";
        for (int i = 0; i < it->arity; i++) {
            cout << it->arguments[i];
            if (i != it->arity - 1)
                cout << ",";
        }
        cout << ")";
    }
    cout << endl;
}

bool Clause::checkTautology()
{
    for (auto it = clause.begin(); it != clause.end(); it++) {
        Predicate predicate = *it;
        predicate.sign = !predicate.sign;
        if (find(predicate) != clause.end())
            return true;
    }
    return false;
}

Clause Clause::standardize()
{
    Clause result;
    for(auto it = clause.begin(); it != clause.end(); it++)
    {
        Predicate p = *it;
        for(int i=0; i<p.arity; i++)
        {
            if(isVariable(p.arguments[i]))
            {
                p.arguments[i] = p.arguments[i][0] + to_string(KB.size());
            }
        }
        result.insert(p);
    }

    return result;
}

// ============================================================================================================
// ======================================= KB TELL FUNCTIONS =======================================================

void tellKB(FOL *fol)
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
            addClause2KB(clause);
            clause.clear();
        }
        else
        {
            if(curr->predicate.arity != 0) {
                clause.insert(curr->predicate);
            }
        }

        curr = curr->right;
    }
    addClause2KB(clause);
}

void printKB() {
    // print KB
    cout << "KB" << endl;
    for (int i = 0; i < KB.size(); i++)
    {
        cout << "clause " << i << " : ";
        KB[i].print();
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
        for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++)
        {
            cout << *it2 << " ";
        }
        cout << endl;
    }
}

// Standardize variables in KB
void standardizeKB() {
    for (int i = 0; i < KB.size(); i++)
    {
        for (int j = 0; j < KB[i].size(); j++)
        {
            for (int k = 0; k < KB[i].clause[j].arity; k++)
            {
                if (isVariable(KB[i].clause[j].arguments[k]))
                {
                    KB[i].clause[j].arguments[k] = KB[i].clause[j].arguments[k] + to_string(i);
                }
            }
        }
    }
}
// ============================================================================================================
// ======================================= KB ASK FUNCTIONS =======================================================

vector <Clause> getKBClauses(Predicate target)
{
    vector <Clause> clauses;
    target.sign = !target.sign;
    
    // vector <int> clauseIndices = KBMap[target];
    unordered_set <int> clauseIndices = KBMap[target];
    // cout<<"Clauses\t: "<<clauseIndices.size()<<" : ";  // debugM

    for(auto it = clauseIndices.begin(); it != clauseIndices.end(); it++)
    {
        ClauseVect::iterator it2 = KB[*it].find(target);
        if(it2 == KB[*it].clause.end()) {
            // cout<<*it<<":CU "; // debug
            continue;
        }
        Predicate p = *it2;
        clauses.push_back(KB[*it]);
        // cout<<*it<<" "; // debugM
    }
    // cout<<endl; // debugM
    return clauses;
}

void printClauses(vector<Clause> clauses) 
{
    // print KB
    cout << "Clauses" << endl;
    for (int i = 0; i < clauses.size(); i++)
    {
        cout << "clause " << i << " : "; clauses[i].print();
    }
}

vector<Clause> unifyClauses(Clause query, Clause clause, Predicate target)
{
    vector<Clause> result;

    for(auto it = clause.clause.begin(); it != clause.clause.end(); it++)
    {
        Predicate p = *it;
        if(p.name == target.name && p.arity == target.arity  && p.sign == !target.sign)
        {
            unordered_map<string, string> substitution;
            bool canUnify = true;

            // create substitution map
            for(int i=0; i<p.arity; i++)
            {
                if(p.arguments[i] != target.arguments[i])
                {
                    if(!isVariable(p.arguments[i]) && !isVariable(target.arguments[i]) && p.arguments[i] != target.arguments[i]) {
                        canUnify = false;
                        break;
                    }
                    else if(isVariable(p.arguments[i]) && isVariable(target.arguments[i])) {
                        if(substitution.find(p.arguments[i]) != substitution.end() || substitution.find(target.arguments[i]) != substitution.end()) 
                        {
                            canUnify = false;
                            break;
                        }
                        substitution[p.arguments[i]] = target.arguments[i];
                    }
                    else if(isVariable(p.arguments[i])) {
                        if(substitution.find(p.arguments[i]) != substitution.end() && substitution[p.arguments[i]] != target.arguments[i]) {
                            canUnify = false;
                            break;
                        }
                        substitution[p.arguments[i]] = target.arguments[i];
                    }
                    else if(isVariable(target.arguments[i])) {
                        if(substitution.find(target.arguments[i]) != substitution.end() && substitution[target.arguments[i]] != p.arguments[i]) {
                            canUnify = false;
                            break;
                        }
                        substitution[target.arguments[i]] = p.arguments[i];
                    }
                }
            }

            // unify query and clause
            if(!canUnify) continue;
            Clause unified;

            for(int i=0; i<target.arity; i++)
                if(substitution.find(target.arguments[i]) != substitution.end())
                    target.arguments[i] = substitution[target.arguments[i]];

            for(auto it = query.clause.begin(); it != query.clause.end(); it++)
            {
                Predicate queryPredicate = *it;
                for(int i=0; i<queryPredicate.arity; i++)
                {
                    if(substitution.find(queryPredicate.arguments[i]) != substitution.end())
                        queryPredicate.arguments[i] = substitution[queryPredicate.arguments[i]];
                }
                if(!queryPredicate.compare(target))
                    unified.insert(queryPredicate);
            }
            target.sign = !target.sign;
            for(auto it = clause.clause.begin(); it != clause.clause.end(); it++)
            {
                Predicate clausePredicate = *it;
                for(int i=0; i<clausePredicate.arity; i++)
                {
                    if(substitution.find(clausePredicate.arguments[i]) != substitution.end())
                        clausePredicate.arguments[i] = substitution[clausePredicate.arguments[i]];
                }
                if(!clausePredicate.compare(target))
                    unified.insert(clausePredicate);
            }

            if(unified.checkTautology()) continue;
            result.push_back(unified);
        }
    }

    return result;
}

void addClauseToVisited(Clause clause)
{
    visited.push_back(clause);
    for(auto it = clause.clause.begin(); it != clause.clause.end(); it++)
    {
        Predicate p = *it;
        visitedMap[p].push_back(visited.size()-1);
    }
}

bool isClauseVisited(Clause clause)
{
    Predicate p = *clause.clause.begin();

    vector <int> clauseIndices = visitedMap[p];
    for(int i=0; i<clauseIndices.size(); i++)
    {
        if(clause.compare(visited[clauseIndices[i]]))
            return true;
    }
    return false;
}

bool askKB(Predicate target)
{
    Clause query; // clause
    query.insert(target);

    // priority queue of clauses
    priority_queue<Clause, vector<Clause>, ClauseCompare> resolver;
    resolver.push(query);

    while(!resolver.empty())
    {
        query = resolver.top();
        resolver.pop();
        if(isClauseVisited(query))
            continue;
        addClauseToVisited(query);

        query = query.standardize();
        // cout<<"\n\nQuery\t: "; query.print(); cout<<endl; // debugM

        if (query.empty()) return true;

        for(auto it = query.clause.begin(); it != query.clause.end(); it++)
        {
            Predicate p = *it;
            // cout<<"Pred\t: "; p.print(); cout<<endl; // debugM
            vector <Clause> clauses = getKBClauses(p);
            for (int i=0; i<clauses.size(); i++)
            {
                // cout<<"Clause\t: "; clauses[i].print(); // debugM
                vector<Clause> unifiedResults = unifyClauses(query, clauses[i], p);
                // cout<<"Unified\n ";  // debugM
                for(int j=0; j<unifiedResults.size(); j++) {
                    // cout<<"\t"; unifiedResults[j].print(); // debugM
                    if(unifiedResults[j].size() != 0)
                       resolver.push(unifiedResults[j]);
                    else 
                        return true;
                }
            }
            // printClauses(clauses); // debug
        }
        // cout<<"Visited\t: "<< visited.size() <<endl; // debugM
        // cout<<"Resolve\t: "<< resolver.size() <<endl<<endl; // debugM

        addClause2KB(query);
    }

    // vector <Clause> clauses = getKBClauses(target);
    // printClauses(clauses); // debug

    return false;
}

// ============================================================================================================
// ======================================= FILE IO =======================================================

string readInput()
{
    string s, target;
    freopen("input.txt", "r", stdin);
    cin>>target;

    int n;
    cin>>n;

    getline(cin, s);

    for (int i = 0; i < n; i++)
    {
        getline(cin, s);
        input.push_back(s);
    }
    fclose(stdin);

    return target;
}

void writeOutput(bool result)
{
    fstream fout;
    fout.open("output.txt", ios::out);
    if (result)
        fout << "TRUE";
    else
        fout << "FALSE";
    fout.close();
}

// ============================================================================================================
// ======================================= MAIN =======================================================

int main()
{
    string target = readInput();

    Predicate query(target);
    query.sign = !query.sign;
    // query.print(); cout<<endl<<endl; // debugM

    auto start = chrono::high_resolution_clock::now();
    for(int i = 0; i < input.size(); i++)
    {
        FOL *fol = stringToFOL(input[i]);
        fol = FOLtoCNF(fol);
        tellKB(fol);
    }
    standardizeKB();
    // cout<<endl; printKB(); cout<<endl; // debugM

    bool ans = askKB(query);
    // cout<<"FINAL ANSWER \t: "<<ans<<endl<<endl<<endl; // debugM
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    int minutes = chrono::duration_cast<chrono::minutes>(duration).count();
    int seconds = chrono::duration_cast<chrono::seconds>(duration).count() - minutes * 60;
    int milliseconds = chrono::duration_cast<chrono::milliseconds>(duration).count() - minutes * 60 * 1000 - seconds * 1000;
    // cout << "Time taken: " << minutes << " minutes " << seconds << " seconds " << milliseconds << " milliseconds" << endl; // debugM
    
    // printKB(); // debugM
    writeOutput(ans);
    // cout<<"=============================================================\n"; // debugM

    return 0;
}