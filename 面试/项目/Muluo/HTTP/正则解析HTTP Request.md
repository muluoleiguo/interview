首先需要谈一谈正则，C++里有regex正则，方便使用。

std::regex_match: 正则表达式需要匹配整个字符串序列，也就是说正则表达式要与字符串完全匹配。因此，它是单次匹配，否则匹配失败。
更多的时候我们希望能够获得匹配结果（字符串），对结果进行操作。这时就需要对匹配结果进行存储，共有两种存储方式。

```c++
match_results<string::const_iterator> result;
smatch result;			//推荐
```

在匹配规则中，**以括号()的方式来划分组别，**
regex_match的第二个参数smatch可以通过下标方式，或者smatch.str(i)获取某个组的字符

#### 一些常见的正则匹配符号

位置限定符：

* ^ 匹配词在字符后，匹配输入字符串的开始位置，除非在方括号表达式中使用，当该符号在方括号表达式中使用时，表示不接受该方括号表达式中的字符集合。

* \$ 匹配输入字符串的结尾位置。要匹配 ​\$ 字符本身，请使用 \$。（似乎\$ 也匹配 '\n' 或 '\r'）

字符类： 

* [] 匹配括号中任意一个字符
* . 匹配任意一个字符

数量限定符： 

* \* 紧跟在它前面的单元应该匹配一次或者多次
* ? 紧跟在它前面的单元应该匹配零次或者一次



```c++
//GET /index.html HTTP/1.1
//这是请求行，组成：请求方式是GET，请求路径是/，协议版本是HTTP/1.1
bool HttpRequest::ParseRequestLine_(const string& line) {
    //真是粗暴的匹配方式
    //([^ ]*) GET method_
    //([^ ]*) /index.html path_
    
    regex patten("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
    smatch subMatch;
    if(regex_match(line, subMatch, patten)) {   
        method_ = subMatch[1];
        path_ = subMatch[2];
        version_ = subMatch[3];
        state_ = HEADERS;
        return true;
    }
    //LOG_ERROR("RequestLine Error");
    return false;
}

//请求报头 有些类似key value的感觉
//Accept:text/html 表明客户端希望接受html文本。
//Content-Type : Text/XML  指明该请求的消息体中包含的是纯文本的XML类型的数据
//这里一次解析一行啊
void HttpRequest::ParseHeader_(const string& line) {
    regex patten("^([^:]*): ?(.*)$");
    smatch subMatch;
    //std::unordered_map<std::string, std::string> header_;
    if(regex_match(line, subMatch, patten)) {
        header_[subMatch[1]] = subMatch[2];
    }
    else {
        state_ = BODY;
    }
}

void HttpRequest::ParseBody_(const string& line) {
    body_ = line;
    //GET没有报文体，所以直接调用POST，合理，但扩展性堪忧
    ParsePost_();
    state_ = FINISH;
    //LOG_DEBUG("Body:%s, len:%d", line.c_str(), line.size());
}


//表单上传编码格式为application/x-www-form-urlencoded(Request Headers中)，参数的格式为key=value&key=value。

//服务器知道参数用符号&间隔，如果参数值中需要&，则必须对其进行编码。
//编码格式就是application/x-www-form-urlencoded（将键值对的参数用&连接起来，如果有空格，将空格转换为+加号；有特殊符号，将特殊符号转换为ASCII HEX值）

//multipart/form-data接收POST请求暂且按下不表，用时再说吧

void HttpRequest::ParseFromUrlencoded_() {
    if(body_.size() == 0) { return; }

    string key, value;
    int num = 0;
    int n = body_.size();
    int i = 0, j = 0;

    for(; i < n; i++) {
        char ch = body_[i];
        switch (ch) {
        case '=':
            key = body_.substr(j, i - j);
            j = i + 1;
            break;
        case '+':
            body_[i] = ' ';
            break;
        case '%':
            num = ConverHex(body_[i + 1]) * 16 + ConverHex(body_[i + 2]);
            body_[i + 2] = num % 10 + '0';
            body_[i + 1] = num / 10 + '0';
            i += 2;
            break;
        case '&':
            value = body_.substr(j, i - j);
            j = i + 1;
            post_[key] = value;
            LOG_DEBUG("%s = %s", key.c_str(), value.c_str());
            break;
        default:
            break;
        }
    }
    assert(j <= i);
    if(post_.count(key) == 0 && j < i) {
        value = body_.substr(j, i - j);
        post_[key] = value;
    }
}

```

