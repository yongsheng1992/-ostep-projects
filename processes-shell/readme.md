# processes-shell

主要完成以下功能：
* 内建命令
    * `path` 添加路径
    * `exit` 退出shell
    * `cd` 切换目录
* 重定向
* 并行命令
* 批处理

## 关于C语言编程

### 字符串处理

如果函数入参为字符串指针，且函数会修复指针的时候，一般复制一份当做入参。
```c

int main() {
    char *line;
    size_t len = 0;
    getline(&line, &len, 1);
    char *s = line; // strsep传入的是指针s的地址，且会改变s指向内存的值。所以拷贝一份在传入。
    strsep(&s, ";");
    free(line); // strsep处理的是指针s，line并没有变更，可以直接释放内存。
    return 0;
}
```


## Requirements

需要基本的C语言编程基础。包括字符串处理、进程API、管道和文件。


## To do list

- [x] 使用fork+execv+waitpid在子进程执行命令
- [x] 使用`dup2`重定向子进程的标准输出到文件
- [x] `exit`
- [x] 并行处理
- [ ] 批处理
- [ ] cmake 编译项目

