class LRUCache {
public:
    int cap;

    list<pair<int, int>> lru;
    unordered_map<int, list<pair<int, int>>::iterator> mp;

    LRUCache(int capacity) {
        cap = capacity;
    }

    int get(int key) {

        if (mp.find(key) == mp.end())
            return -1;

        auto it = mp[key];

        int value = it->second;

        // Move this key to front
        lru.erase(it);
        lru.push_front({key, value});

        // Update iterator
        mp[key] = lru.begin();

        return value;
    }

    void put(int key, int value) {

        // Already exists
        if (mp.find(key) != mp.end()) {

            lru.erase(mp[key]);
        }

        // Add as most recently used
        lru.push_front({key, value});

        mp[key] = lru.begin();

        // Capacity exceeded
        if (lru.size() > cap) {

            int keyToRemove = lru.back().first;

            mp.erase(keyToRemove);

            lru.pop_back();
        }
    }
};
/**
 * Your LRUCache object will be instantiated and called as such:
 * LRUCache* obj = new LRUCache(capacity);
 * int param_1 = obj->get(key);
 * obj->put(key,value);
 */