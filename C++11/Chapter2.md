## Chapter 2

**<1> `assert`和静态断言`static_assert`**

* `assert`允许在运行时进行断言，用于排除在设计的逻辑上不应该产生的情况。当发生异常时可以强制程序退出。原型为`assert(expression)`, `#include <cassert>`
* `static_assert`则在编译时进行断言，可以在编译期进行报错，原型为`static_assert(expression,warnning)`，第一个参数必须是常量表达式，因为必须在编译期就确定真假，第二个参数是一个字符串，用于定位断言发生位置

**<2> `noexcept`修饰符与`noexcept`操作符**

相比于断言适用于排除逻辑上不可能存在的状态，异常通常是用于逻辑上可能发生的错误

`noexcept`表示其修饰的函数不会抛出异常，与`throw()`动态异常声明不同的是，`noexcept`修饰的函数抛出了异常，编译器可以直接选择调用`std::terminate()`函数来终止程序

`noexcept`修饰符有两种形式

	void excpt_func() noexcept;
	void excpt_func() noexcept(常量表达式);//常量表达式的结果被转换成一个bool类型的值，若为true,表示返回不会抛出异常

在C++11中，析构函数是默认为`noexcept`的

**<3> 快速初始化成员变量**

C++11以前，类的成员变量是不能“就地”初始化的，除非该成员是`const`静态成员，且类型为`int`或`enum`。

	class Init {
	public:
		Init():a(0){}
		Init(int d):a(d){}
	private:
		int a;
		const static int b = 0; //常量静态成员，且类型为整型，OK
		int c = 1; //直接初始化非静态成员，error
		static int d = 0;	//非常量静态成员，error
		static const double e = 1.1; //常量静态成员，但类型不是整型或枚举，在C++标准中是错误的。但在GNU中是正确的，因为GNU对其进行了优化
	};

C++11支持在类中直接初始化成员变量，即使该变量不是`int` or `enum`型的常量静态变量

	class Init {
	public:
		...
	private:
		int a{3};
	};

`Init`对象的数据成员`int a`默认值为3

**<4> `final` & `override`**

`final`可以阻止类中的虚函数被派生类所覆盖

如：
	
	//一个抽象类，提供接口
	class Interface {
	public:
		virtual void func() = 0;
	};
	
	class Base:public Interface {
	public:
		void func() final; //继承Base的类不能覆盖void func();
	};

由于C++允许对于基类声明为`virtual`的函数，之后的派生类中覆盖版本不需要再声明该函数为`virtual`，这会导致后续的阅读困难，因为需要判断一个函数是否为虚函数，需要找到最早的基类。

C++提供虚函数的描述符`override`，如：

	class Derived:public Base {
	public:
		void func() override;
	}

如果没有`override`描述符，对`void func()`的拼写，参数等不一致行为会导致该函数变成非继承而来的函数！

**<5> 模板函数支持默认模板参数**

	void f(int n = 1) {}
	template <typename T = int> class C {};
	template <typename T = int> void f2() {} //在C++98里编译错误，在C++11里编译通过！

同时，为多个默认模板参数指定默认值时，程序员必须“从右到左“的规则进行指定，而函数模板则不需要，如：
	
	template <typename T = int, typename U> class C {}; //编译错误
	template <typename T, typename U = int> class C {}; //编译通过
	template <typename T = int, typename U> void f(T a, U b); //编译通过

**<6> 外部模板**

在C语言中就有“外部（extern）“的说法，其好处是可以减少冗余代码，如在a.c中包含了`int i`的声明，要把a.c和b.c编译成同一可执行文件，且b.c的代码中使用了a.c中的`int i`，只需在b.c中进行外部声明`extern int i`就可以让编译器知道该变量存在于b.c的外部。如果不进行外部声明，链接器就不知道如何处理这两个标识符相同的变量，而导致报错。

在传统的模板中，会存在类似的问题：

	template <typename T> void func(T) {} // a.h
	
	//a.cpp
	#include "a.h"
	func(3);

	//b.cpp
	#include "a.h"
	func(4);

这样同时编译a.cpp和b.cpp就会导致实例化了两个这样的函数`func<int>(int)`，编译器并不知道这样的代码是可以共享的

在C++11中则可以使用外部模板声明，由此告诉编译器实例化同一份代码：

	template <typename T> void func(T) {} //a.h
	
	//a.cpp
	#include "a.h"
	template void func<int>(int); //在全局中声明
	...
	func(3);
	
	//b.cpp
	#include "a.h"
	extern void func<int>(int); //在全局中声明
	...
	func(4);
