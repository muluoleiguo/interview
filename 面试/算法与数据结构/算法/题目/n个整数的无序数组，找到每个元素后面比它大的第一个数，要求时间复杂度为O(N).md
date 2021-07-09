
```c++
vector<int> findMax(vector<int>num)
{
    if(num.size()==0)return num;
    vector<int> res(num.size());
    int i=0;
    stack<int> s;
    while(i<num.size())
    {
        if(s.empty()||num[s.top()]>=num[i])//存序号
        {
            s.push(i++);
        }
        else
        {
            res[s.top()]=num[i];
            s.pop();
        }
    }
    while(!s.empty())
    {
        res[s.top()]=INT_MAX;
        s.pop();
    }
    for(int i=0; i<res.size(); i++)
        cout<<res[i]<<endl;
    return res;
    }
```
