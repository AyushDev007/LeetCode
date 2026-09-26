class Solution {
public:
    string evaluate(string s, vector<vector<string>>& knowledge) {
        unordered_map<string,string> mp;
        mp.reserve(knowledge.size());
        for (auto& kv : knowledge) mp[kv[0]] = kv[1];

        string res, key;
        res.reserve(s.size());
        bool inside = false;

        for (char c : s) {
            if (c == '(') { inside = true; key.clear(); }
            else if (c == ')') {
                inside = false;
                auto it = mp.find(key);
                res += (it != mp.end()) ? it->second : "?";
            }
            else if (inside) key += c;
            else res += c;
        }
        return res;
    }
};