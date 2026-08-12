class MyCircularQueue {
    int* arr;
    int k;
    int currsize;
    int f,r;
public:
    MyCircularQueue(int k) {
        this->k=k;
        arr=new int[k];
        currsize=0;
        f=r=-1;
        
    }
    
    bool enQueue(int value) {
        if(isFull()){
            return false;
        }
        if (currsize == 0) {f = 0;r=-1;}
        r=(r+1)%k;
        arr[r]=value;
        currsize++;
        return true;
        
    }
    
    bool deQueue() {
        if(isEmpty()){
            return false;
        }
        f=(f+1)%k;
        currsize--;
        return true;
        
    }
    
    int Front() {
        if(isEmpty()) return -1;
        return arr[f];
    }
    
    int Rear() {
        if(isEmpty()) return -1;
        return arr[r];
    }
    
    bool isEmpty() {
        if(currsize==0) return true;
        return false;
    }
    
    bool isFull() {
        if(currsize==k) return true;
        return false;
    }
};

/**
 * Your MyCircularQueue object will be instantiated and called as such:
 * MyCircularQueue* obj = new MyCircularQueue(k);
 * bool param_1 = obj->enQueue(value);
 * bool param_2 = obj->deQueue();
 * int param_3 = obj->Front();
 * int param_4 = obj->Rear();
 * bool param_5 = obj->isEmpty();
 * bool param_6 = obj->isFull();
 */