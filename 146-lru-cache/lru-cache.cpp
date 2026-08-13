class LRUCache {
public:

    class Node {
    public:
        int key;
        int value;
        Node* prev;
        Node* next;

        Node(int k, int v) {
            key = k;
            value = v;
            prev = NULL;
            next = NULL;
        }
    };

    int capacity;

    unordered_map<int, Node*> mp;

    Node* head;
    Node* tail;


    LRUCache(int capacity) {
        this->capacity = capacity;

        head = new Node(-1, -1);
        tail = new Node(-1, -1);

        head->next = tail;
        tail->prev = head;
    }


    // Add node just after head
    void addNode(Node* node) {

        node->next = head->next;
        node->prev = head;

        head->next->prev = node;
        head->next = node;
    }


    // Remove a node from the list
    void deleteNode(Node* node) {

        node->prev->next = node->next;
        node->next->prev = node->prev;
    }


    int get(int key) {

        // Key doesn't exist
        if (mp.find(key) == mp.end()) {
            return -1;
        }

        Node* node = mp[key];

        // Since we used it, make it most recently used
        deleteNode(node);
        addNode(node);

        return node->value;
    }


    void put(int key, int value) {

        // Key already exists
        if (mp.find(key) != mp.end()) {

            Node* node = mp[key];

            node->value = value;

            // Make it most recently used
            deleteNode(node);
            addNode(node);

            return;
        }


        // Create new node
        Node* node = new Node(key, value);

        mp[key] = node;

        // Add to front
        addNode(node);


        // Cache exceeded capacity
        if (mp.size() > capacity) {

            // Least recently used node
            Node* lru = tail->prev;

            deleteNode(lru);

            mp.erase(lru->key);

            delete lru;
        }
    }
};
/**
 * Your LRUCache object will be instantiated and called as such:
 * LRUCache* obj = new LRUCache(capacity);
 * int param_1 = obj->get(key);
 * obj->put(key,value);
 */