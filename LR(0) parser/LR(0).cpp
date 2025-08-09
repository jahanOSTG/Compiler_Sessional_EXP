#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
using namespace std;

struct Item {
    string lhs;
    vector<string> rhs;
    int dot; // position of dot in rhs

    bool operator<(const Item& other) const {
        if(lhs != other.lhs) return lhs < other.lhs;
        if(rhs != other.rhs) return rhs < other.rhs;
        return dot < other.dot;
    }
    bool operator==(const Item& other) const {
        return lhs == other.lhs && rhs == other.rhs && dot == other.dot;
    }
};

vector<string> splitRHS(const string& s) {
    vector<string> res;
    for(size_t i = 0; i < s.size(); i++) {
        if(s[i] != ' ') res.push_back(string(1,s[i]));
    }
    return res;
}

map<string, vector<vector<string>>> productions;
set<string> terminals;
set<string> nonterminals;
string startSymbol;

vector<Item> closure(const vector<Item>& items) {
    vector<Item> C = items;
    bool added = true;
    while(added) {
        added = false;
        vector<Item> toAdd;
        for(auto &item : C) {
            if(item.dot < (int)item.rhs.size()) {
                string B = item.rhs[item.dot];
                if(nonterminals.count(B)) {
                    for(auto &prod : productions[B]) {
                        Item newItem = {B, prod, 0};
                        if(find(C.begin(), C.end(), newItem) == C.end() &&
                           find(toAdd.begin(), toAdd.end(), newItem) == toAdd.end()) {
                            toAdd.push_back(newItem);
                            added = true;
                        }
                    }
                }
            }
        }
        C.insert(C.end(), toAdd.begin(), toAdd.end());
    }
    sort(C.begin(), C.end());
    return C;
}

vector<Item> goTo(const vector<Item>& I, const string& X) {
    vector<Item> J;
    for(auto &item : I) {
        if(item.dot < (int)item.rhs.size() && item.rhs[item.dot] == X) {
            J.push_back({item.lhs, item.rhs, item.dot + 1});
        }
    }
    return closure(J);
}

bool equalStates(const vector<Item>& I1, const vector<Item>& I2) {
    if(I1.size() != I2.size()) return false;
    for(size_t i=0; i<I1.size(); i++) {
        if(!(I1[i] == I2[i])) return false;
    }
    return true;
}

int findState(const vector<vector<Item>>& states, const vector<Item>& I) {
    for(size_t i=0; i<states.size(); i++) {
        if(equalStates(states[i], I)) return (int)i;
    }
    return -1;
}

int main() {
    // Grammar input
    // Augmented grammar: S' -> S
    startSymbol = "S";
    productions["S'"].push_back({"S"});
    nonterminals.insert("S'");
    nonterminals.insert("S");
    nonterminals.insert("A");
    terminals.insert("a");
    terminals.insert("b");

    // Given productions
    productions["S"].push_back({"A","A"});
    productions["A"].push_back({"a","A"});
    productions["A"].push_back({"b"});

    // Construct canonical collection of LR(0) items
    vector<vector<Item>> states;
    vector<Item> startItems = closure({{ "S'", {"S"}, 0 }});
    states.push_back(startItems);

    map<pair<int,string>, int> transitions;

    // Build states
    for(size_t i=0; i<states.size(); i++) {
        // For each grammar symbol
        for(auto &X : nonterminals) {
            vector<Item> nxt = goTo(states[i], X);
            if(!nxt.empty()) {
                int idx = findState(states, nxt);
                if(idx == -1) {
                    states.push_back(nxt);
                    idx = (int)states.size()-1;
                }
                transitions[{(int)i,X}] = idx;
            }
        }
        for(auto &X : terminals) {
            vector<Item> nxt = goTo(states[i], X);
            if(!nxt.empty()) {
                int idx = findState(states, nxt);
                if(idx == -1) {
                    states.push_back(nxt);
                    idx = (int)states.size()-1;
                }
                transitions[{(int)i,X}] = idx;
            }
        }
    }

    // Build ACTION and GOTO tables
    map<int, map<string,string>> ACTION;
    map<int, map<string,int>> GOTO;

    for(size_t i=0; i<states.size(); i++) {
        for(auto &item : states[i]) {
            // If dot is not at the end
            if(item.dot < (int)item.rhs.size()) {
                string a = item.rhs[item.dot];
                if(terminals.count(a)) {
                    if(transitions.count({(int)i,a})) {
                        ACTION[i][a] = "S" + to_string(transitions[{(int)i,a}]);
                    }
                } else if(nonterminals.count(a)) {
                    if(transitions.count({(int)i,a})) {
                        GOTO[i][a] = transitions[{(int)i,a}];
                    }
                }
            } else {
                // Dot at end -> reduce or accept
                if(item.lhs == "S'" && item.rhs.size() == 1 && item.rhs[0] == startSymbol) {
                    ACTION[i]["$"] = "accept";
                } else {
                    // Reduce by item.lhs -> item.rhs
                    // Find which production number
                    string prodLHS = item.lhs;
                    vector<string> prodRHS = item.rhs;
                    int prodNum = -1;
                    int countNum = 0;
                    for(auto &p : productions[prodLHS]) {
                        if(p == prodRHS) {
                            prodNum = countNum; break;
                        }
                        countNum++;
                    }
                    string rule = "r" + to_string(prodNum);
                    for(auto &t : terminals) {
                        if(ACTION[i].count(t) && ACTION[i][t] != rule) {
                            // conflict (should not happen for this grammar)
                        }
                        ACTION[i][t] = rule;
                    }
                    // Also $ end marker reduce
                    if(ACTION[i].count("$") && ACTION[i]["$"] != rule) {
                        // conflict
                    }
                    ACTION[i]["$"] = rule;
                }
            }
        }
    }

    // Output formatting to match your image
    // Headers:
    cout << " \t" << "ACTION\t\t\t" << "GOTO" << endl;
    cout << " \t" << "a\tb\t$\t" << "A\t" << "S" << endl;

    for(size_t i=0; i<states.size(); i++) {
        cout << i << "\t";
        // ACTION columns: a,b,$
        if(ACTION[i].count("a")) cout << ACTION[i]["a"]; else cout << " ";
        cout << "\t";
        if(ACTION[i].count("b")) cout << ACTION[i]["b"]; else cout << " ";
        cout << "\t";
        if(ACTION[i].count("$")) cout << ACTION[i]["$"]; else cout << " ";
        cout << "\t";
        // GOTO columns: A, S
        if(GOTO[i].count("A")) cout << GOTO[i]["A"]; else cout << " ";
        cout << "\t";
        if(GOTO[i].count("S")) cout << GOTO[i]["S"]; else cout << " ";
        cout << endl;
    }

    return 0;
}
