string.h directory主要记录<string.h>内函数的实现方法。

1.void指针：无类型指针，可以指向任何数据类型对象。

2.常见的函数都返回了void指针，其目的是用于连续操作，比如
size_t len = strlen(memcpy(dest,src,length));

3.void指针不能进行算术操作，所以通常要把它们强制转换成可操作的指针，比如char*


