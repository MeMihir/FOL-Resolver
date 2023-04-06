
#include <unordered_set>

bool queryKB(Predicate target);
vector <Clause> getKBClauses(Predicate target);
Clause unifyClauses(Clause query, Clause clause, Predicate target);
void printClauses(vector<Clause> clauses);
void printClause(Clause clause);