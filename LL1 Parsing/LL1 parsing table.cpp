#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <iomanip>
#include <algorithm>
using namespace std;

map<string, vector<vector<string>>> grammar;
map<string, set<string>> firstSets, followSets;
map<string, map<string, vector<string>>> parsingTable;
set<string> terminals, nonTerminals;
string startSymbol = "E";

bool isTerminal(const string& symbol) {
    return !(symbol[0] >= 'A' && symbol[0] <= 'Z');
}

set<string> computeFirst(const string& symbol) {
    if (firstSets.count(symbol)) return firstSets[symbol];

    set<string> result;
    if (isTerminal(symbol)) {
        result.insert(symbol);
        return firstSets[symbol] = result;
    }

    for (auto& production : grammar[symbol]) {
        bool epsilon_in_all = true;
        for (auto& sym : production) {
            set<string> first_sym = computeFirst(sym);
            for (auto& f : first_sym) {
                if (f != "ε") result.insert(f);
            }
            if (first_sym.find("ε") == first_sym.end()) {
                epsilon_in_all = false;
                break;
            }
        }
        if (epsilon_in_all) result.insert("ε");
    }

    return firstSets[symbol] = result;
}

set<string> computeFollow(const string& symbol) {
    if (followSets.count(symbol)) return followSets[symbol];

    set<string>& result = followSets[symbol];
    if (symbol == startSymbol) result.insert("$");

    for (auto& rule : grammar) {
        for (auto& production : rule.second) {
            for (size_t i = 0; i < production.size(); ++i) {
                if (production[i] == symbol) {
                    bool epsilon_in_all = true;
                    for (size_t j = i + 1; j < production.size(); ++j) {
                        set<string> firstNext = computeFirst(production[j]);
                        for (auto& f : firstNext) {
                            if (f != "ε") result.insert(f);
                        }
                        if (firstNext.find("ε") == firstNext.end()) {
                            epsilon_in_all = false;
                            break;
                        }
                    }
                    if (i + 1 == production.size() || epsilon_in_all) {
                        if (rule.first != symbol) {
                            set<string> follow_lhs = computeFollow(rule.first);
                            result.insert(follow_lhs.begin(), follow_lhs.end());
                        }
                    }
                }
            }
        }
    }

    return result;
}

void buildParsingTable() {
    for (auto& rule : grammar) {
        string nonTerm = rule.first;
        for (auto& production : rule.second) {
            set<string> firstSet;
            bool epsilon_in_all = true;

            for (auto& symbol : production) {
                set<string> first = computeFirst(symbol);
                for (auto& f : first) {
                    if (f != "ε") firstSet.insert(f);
                }
                if (first.find("ε") == first.end()) {
                    epsilon_in_all = false;
                    break;
                }
            }

            if (epsilon_in_all) firstSet.insert("ε");

            for (auto& terminal : firstSet) {
                if (terminal != "ε") {
                    parsingTable[nonTerm][terminal] = production;
                }
            }

            if (firstSet.find("ε") != firstSet.end()) {
                set<string> followSet = computeFollow(nonTerm);
                for (auto& f : followSet) {
                    parsingTable[nonTerm][f] = { "ε" };
                }
            }
        }
    }
}

void printSimpleAlignedTable() {
    vector<string> terminalsVec = { "id", "+", "*", "(", ")", "$" };
    vector<string> nonTermsVec = { "E", "E'", "T", "T'", "F" };
    int colWidth = 22;

    // Print header row
    cout << left << setw(8) << "";
    for (const auto& t : terminalsVec) {
        cout << setw(colWidth) << t;
    }
    cout << endl;

    // Print rows
    for (const auto& nt : nonTermsVec) {
        cout << left << setw(8) << nt;
        for (const auto& t : terminalsVec) {
            if (parsingTable[nt].count(t)) {
                string prod = nt + " --> ";
                for (auto& s : parsingTable[nt][t]) prod += s;
                cout << setw(colWidth) << prod;
            } else {
                cout << setw(colWidth) << "";
            }
        }
        cout << endl;
    }
}

int main() {
    // Grammar for:
    // E  --> T E'
    // E' --> + T E' | ε
    // T  --> F T'
    // T' --> * F T' | ε
    // F  --> id | ( E )

    grammar["E"] = { {"T", "E'"} };
    grammar["E'"] = { {"+", "T", "E'"}, {"ε"} };
    grammar["T"] = { {"F", "T'"} };
    grammar["T'"] = { {"*", "F", "T'"}, {"ε"} };
    grammar["F"] = { {"id"}, {"(", "E", ")"} };

    for (auto& g : grammar) {
        nonTerminals.insert(g.first);
        for (auto& prod : g.second) {
            for (auto& sym : prod) {
                if (isTerminal(sym) && sym != "ε") terminals.insert(sym);
            }
        }
    }
    terminals.insert("$");

    for (auto& nt : nonTerminals) computeFirst(nt);
    for (auto& nt : nonTerminals) computeFollow(nt);

    buildParsingTable();
    printSimpleAlignedTable();

    return 0;
}
