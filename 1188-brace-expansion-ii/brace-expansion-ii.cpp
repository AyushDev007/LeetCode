class Solution {
    string s;
    int pos = 0;

    set<string> parseUnion() {
        set<string> res = parseConcat();
        while (pos < s.size() && s[pos] == ',') {
            pos++;                                  // consume ','
            set<string> nxt = parseConcat();
            res.insert(nxt.begin(), nxt.end());
        }
        return res;
    }

    set<string> parseConcat() {
        set<string> res = {""};                     // identity for concatenation
        while (pos < s.size() && s[pos] != ',' && s[pos] != '}') {
            set<string> nxt = parseAtom();
            set<string> tmp;
            for (const string& a : res)
                for (const string& b : nxt)
                    tmp.insert(a + b);
            res = move(tmp);
        }
        return res;
    }

    set<string> parseAtom() {
        if (s[pos] == '{') {
            pos++;                                  // consume '{'
            set<string> res = parseUnion();
            pos++;                                  // consume '}'
            return res;
        }
        return { string(1, s[pos++]) };             // single letter
    }

public:
    vector<string> braceExpansionII(string expression) {
        s = expression;
        pos = 0;
        set<string> res = parseUnion();
        return vector<string>(res.begin(), res.end());
    }
};