## Chapter 3

**<1> 继承构造函数**

在传统C++标准中，派生类通过这样的方式对基类部分进行初始化：

	class Derived:public Base {
	public:
		Derived():Base() {}
		...
	};

但是如果`Base`包含了多个版本的构造函数，就会导致`Derived`类要跟着写同样多版本的构造函数

在C++11中，可以用`using`进行继承基类的构造函数

	class Derived:public Base {
	public:
		using Base::Base;
		...
	};

使用这种方法的好处是，C++把继承构造函数设计为跟派生类中的各种默认函数（默认构造，析构，拷贝构造等）一样，是隐式声明的。这意味着如果一个继承构造函数不被相关代码使用，编译器不会为其产生真正的函数代码！