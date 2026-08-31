/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode() : val(0), next(nullptr) {}
 *     ListNode(int x) : val(x), next(nullptr) {}
 *     ListNode(int x, ListNode *next) : val(x), next(next) {}
 * };
 */
class Solution {
public:
    vector<int> nodesBetweenCriticalPoints(ListNode* head) {
        int first = -1, last = -1, prev = -1;
        int minDist = INT_MAX;
        int idx = 1;

        ListNode* a = head;
        ListNode* b = head->next;

        while (b->next) {
            ListNode* c = b->next;
            ++idx;                       // idx is now the position of b

            bool isCritical = (b->val > a->val && b->val > c->val) ||
                              (b->val < a->val && b->val < c->val);

            if (isCritical) {
                if (first == -1) first = idx;
                else minDist = min(minDist, idx - prev);
                prev = idx;
                last = idx;
            }
            a = b;
            b = c;
        }

        if (first == last) return {-1, -1};
        return {minDist, last - first};
    }
};