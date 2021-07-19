## awk

作用：
样式扫描和处理语言，是一个强大的文本分析工具。它允许创建简短的程序，这些程序读取输入文件、为数据排序、处理数据、对输入执行计算以及生成报表等

内置变量:

| 变量 | 描述                                |
| :--- | :---------------------------------- |
| NR   | 已经读出的记录数，就是行号，从1开始 |

用法:

Linux下得到一个文件的100到200行

```shell
awk 'NR>=100&&NR<=200{print}' inputfile
```

同样效果还可以用sed 和 head

```shell
sed -n '100,200p' inputfile
```

```shell
head -200 inputfile|tail -100
```






更多详见https://www.runoob.com/linux/linux-comm-awk.html



对比：

-  grep 更适合单纯的查找或匹配文本
-  sed 更适合编辑匹配到的文本
-  awk 更适合格式化文本，对文本进行较复杂格式处理





