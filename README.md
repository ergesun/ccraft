# raft cpp implementation

[TOC]

## 简介


## 编码规范
- 采用google编码规范，如与下面有冲突，以本文为准。

- 函数命明规则
 > public: 任意单词首字母大写
 > protected: 除首个单词之外的其余单词均首字母大写
 > private: 单词间通过"_"连接，均小写字母。
 
- 变量命明规则
 > 类的成员变量: m_[type][name] -- type(i(int), s(str), v(vector), l(list), e(enum)...)小写，name参考函数的public。
 > 类的静态成员: s_[type][name] -- 同类的成员变量。
 > 全局变量   : g_[type][name] -- 同类的成员变量。
 > 局部变量   : name -- 写法同函数的protected。
 > 函数参数   : 同局部变量。
