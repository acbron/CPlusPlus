### extern "C"

`extern "C"`的主要作用是为了能够正确实现C++代码调用其他C语言代码，告诉编译器这段代码按C语言的方式进行编译。这是因为C++允许函数重载，其函数编译后的符号与C函数的不同，这个语句就是为了使用C语言编写的代码而使用的。

总结：

1. 允许C++代码调用C语言代码

2. 在C++的头文件使用

3. 在多人协调开发中，允许C和C++一起使用

例子：

	//在a.h中，引用C头文件b.h
	#ifndef A_H
	#define A_H
	ifdef __cplusplus
	extern "C" {
	#include "b.h"
	}
	#endif
	#endif

### ifndef / define / endif

在C/C++中，编译时允许只对函数进行声明而不定义，把定义交给链接时进行符号解析。利用`ifndef / define / endif`可以防止一个头文件在被多个文件所包含时导致的编译时出错