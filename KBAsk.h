
#include <unordered_set>
#include <unordered_map>
#include <vector>

extern vector<Clause> visited;
extern unordered_map<Predicate, vector<int>, PredicateHash> visitedMap;

bool queryKB(Predicate target);
vector <Clause> getKBClauses(Predicate target);
vector <Clause> unifyClauses(Clause query, Clause clause, Predicate target);
void printClauses(vector<Clause> clauses);
void printClause(Clause clause);
void addClauseToVisited(Clause clause);
bool isClauseVisited(Clause clause);