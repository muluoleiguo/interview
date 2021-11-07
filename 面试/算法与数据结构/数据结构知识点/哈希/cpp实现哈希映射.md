
```cpp
#define MAX_LEN 100000            // the amount of buckets

class MyHashMap {
private:
    vector<pair<int, int>> map[MAX_LEN];       // hash map implemented by array
    
    /** Returns the corresponding bucket index. */
    int getIndex(int key) {
        return key % MAX_LEN;
    }
    
    /** Search the key in a specific bucket. Returns -1 if the key does not existed. */
    int getPos(int key, int index) {
        // Each bucket contains a vector. Iterate all the elements in the bucket to find the target key.
        for (int i = 0; i < map[index].size(); ++i) {
            if (map[index][i].first == key) {
                return i;
            }
        }
        return -1;
    }
    
public:
    /** Initialize your data structure here. */
    MyHashMap() {
        
    }
    
    /** value will always be positive. */
    void put(int key, int value) {
        int index = getIndex(key);
        int pos = getPos(key, index);
        if (pos < 0) {
            map[index].push_back(make_pair(key, value));
        } else {
            map[index][pos].second = value;
        }
    }
    
    /** Returns the value to which the specified key is mapped, or -1 if this map contains no mapping for the key */
    int get(int key) {
        int index = getIndex(key);
        int pos = getPos(key, index);
        if (pos < 0) {
            return -1;
        } else {
            return map[index][pos].second;
        }
    }
    
    /** Removes the mapping of the specified value key if this map contains a mapping for the key */
    void remove(int key) {
        int index = getIndex(key);
        int pos = getPos(key, index);
        if (pos >= 0) {
            map[index].erase(map[index].begin() + pos);
        }
    }
};

/**
 * Your MyHashMap object will be instantiated and called as such:
 * MyHashMap obj = new MyHashMap();
 * obj.put(key,value);
 * int param_2 = obj.get(key);
 * obj.remove(key);
 */
```
