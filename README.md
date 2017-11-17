# raft cpp implementation

[TOC]

## 简介


## 编码规范
- 采用google编码规范，如google规范与下面有冲突，以下面本文内容为准。

- 函数命明规则
 > public: 任意单词首字母大写<br>
 > protected: 除首个单词之外的其余单词均首字母大写<br>
 > private: 单词间通过"_"连接，均小写字母。
 
- 变量命明规则
 > 类的成员变量: m_[type][name] -- type(i(int), s(str), v(vector), l(list), e(enum)...)小写，name参考函数的public。<br>
 > 类的静态成员: s_[type][name] -- 同类的成员变量。<br>
 > 文件全局变量: g_[type][name] -- 同类的成员变量。<br>
 > 函数局部变量: name -- 写法同函数的protected。<br>
 > 函数出入参数: 同局部变量。
