## Chapter 3

**(1) 继承构造函数**

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

**(2) 委托构造函数**

在传统C++标准中，如下代码是很常见的：

	class Base {
	private:
		int x;
		double y;
		void init();
	public:
		Base() x(0), y(0.0) { init(); }
		Base(int xx) x(xx), y(0.0) { init(); }
	};

这样显得很多“冗余”代码，看起来每个构造函数都调用了`init()`

C++11允许这样的写法，称为委托构造函数

	class Base {
	private:
		...
	public:
		Base() x(0), y(0.0) { init(); }
		Base(int xx): Base() { x = xx; }
		// Base(int xx): Base(), x(xx) {} 编译错误，不允许委托构造函数同时使用初始化列表
	};

然而，上述做法并不好，因为委托构造函数不能和初始化列表同时使用。初始化列表是被提倡的做法，因为它先于构造函数完成（在编译期完成），可以减少程序员发生错误。可以定义一个`private`的构造函数作为被委托的构造函数，并把`init()`放进该函数中：

	class Base {
	private:
		...
		Base(int xx, double yy): x(xx), y(yy) { init(); }
	public:
		Base(): Base(0, 0.0) {}
		Base(int xx): Base(xx, 0.0) {}
	};

委托构造函数的一个有用的应用是，编写模板构造函数：

	class Temp {
	private:
		template <typename T> Temp<T first, T last): l(first, last) {}
		list <int> l;
	public:
		Temp(vector<short> &v): Temp(v.begin(), v.end()) {}
		Temp(deque<int> &d): Temp(d.begin(), d.end()) {};
	};

此外，在异常处理方面，如果委托构造函数中使用`try`的话，那么从目标构造函数中产生的异常，都可以在为委托构造函数中被捕捉到。

**(3) 右值引用：移动语义和完美转发**

当类中包含一个指针成员的时候，务必拒绝编译器生成的拷贝构造函数，取而代之的是自行编写“深复制”的拷贝函数

	class Base {
	public:
		Base(): d(new int(0)) {}
		Base(const Base &b): d(b.d) {}
		~Base() { delete d; }
		int *d;
	};
	...
	Base a;
	Base b(a);

这样的代码会导致`Base a`和`Base b`的`int *d`指向同一块内存，一旦其中一个调用了析构函数，释放了内存，另外一个变量的指针将指向一个无效的内存，成为“悬挂指针（dangling pointer）”。出现这样的问题，在C++中，被称为“浅拷贝（shollow copy）”。

这里只需把拷贝构造函数做如下改动：

	Base(const Base &b): d(new int(*b.d)) {}

***-移动语义***

考虑如下例子：

	//test.cpp
	class Base {
	public:
	    Base(): x(new int(0))
	    {
		printf("construct %d\n", ++c_construct);
	    }

	    Base(const Base &b): x(new int(*b.x))
	    {
		printf("copy construct %d\n", ++c_copy);
	    }

	    ~Base()
	    {
		delete x;
		printf("destruct %d\n", ++c_destruct);
	    }
	    int *x;
	    static int c_construct;
	    static int c_copy;
	    static int c_destruct;
	};

	int Base::c_construct = 0;
	int Base::c_copy = 0;
	int Base::c_destruct = 0;

	Base get_obj()
	{
	    return Base();
	}

	int main()
	{
	    Base a = get_obj();
	    return 0;
	}
	//g++ test.cpp -fno-elide-constructors，可打印出析构函数的函数体执行结果

这个例子中，构造函数被调用了一次（在`get_obj()`中显示调用`Base()`），接着，复制构造函数被调用一次和析构函数被调用一次（用于返回，因为之前的申请的局部变量在函数结束后被销毁，需要把该资源进行复制作为返回值），然后，复制构造函数再次被调用（用于把返回值复制到对象`Base a`中），最后在主函数结束后调用再次调用两次析构函数。

这些操作对于程序员来说几乎是透明的，且不影响正确性。但如果`Base`类中的指针指向的是一个非常大的堆内存，这笔开销就会严重影响效率了。

C++11支持“移动构造函数”，也就是在析构临时对象的时候，不释放其所指的堆内存，而是把该内存直接转移到需要被复制的对象中。这种操作也成为“移动语义（move semantics）”。

移动构造函数：

	Base(Base &&b): x(b.x) 
	{
		b.x = nullptr; //把临时值的指针置空！否则，临时变量调用了析构函数，复制该内存的指针便又成为悬挂指针！
		...
	}

***-左值，右值与右值引用***

在传统C++中，可以取地址的／有名字的称为左值，反之，不能取地址的／没有名字的就是右值

C++11中，把右值分为两种类型：将亡值（xvalue,eXpiring Value)和纯右值（prvalue,Pure Rvalue）
	
C++98标准的“右值”就是纯右值，比如非引用返回的函数返回的临时变量，运算表达式（1+3），字面常量（2，'c'），lambda表达式等

将亡值则是C++11新增的跟右值引用相关的表达式，这样的表达式通常是将要被移动的对象。右值引用主要是为了移动语义，需要右值可以被修改的。所以常量右值引用就没有意义了，像这样`const T && obj = get_obj()`是没有意义的，因为obj无法修改右值

常量左值引用是“万能”的引用类型，在C++98中，常被用作减少临时对象开销的手段。在定义移动构造函数的时候，同时定义一个以常量左值为参数的构造函数是一个好的做法，因为至少在移动失败的时候，还可以进行拷贝。

在`<type_traits>`中可用`is_rvalue_reference`，`is_lvalue_reference`，`is_reference`来判断某变量是何种引用，比如`cout << is_rvalue_reference<string &&>::value`

***-`std::move`：强制转换成右值***

在C++11中，`std::move`包含在`<utility>`中，其功能在于把左值强制转换成右值，然后可以使用右值引用使用该值，用于移动语义。

在类设计中，如果一个类的数据成员中包含另外一个类对象，比如：

	class B {
	public:
		int *ptr;
		...
	};
	class A {
	public:
		...
		B b_obj;
		...
	};

在对A进行右值引用时，也要同时对B中的数据成员进行右值引用（即在编写A的移动构造函数时，通过`move`把`b_obj.ptr`转换成右值），否则移动语义就失效了！

***-移动语义的其他问题***

如果类作者未定义默认构造函数，拷贝构造函数，移动构造函数，析构函数中的任何一个，编译器会隐式地生成一个（隐式表示如果不被使用就不生成），但如果定义了其中的一个，那么就不会生成其中的任何一个了。**所以，在C++11中，这些函数要么同时定义，要么同时不定义**

有些资源只有移动语义，即只可以被移动，不可以被拷贝。可以使用如下`is_move_constructible`，`is_trivially_move_constructile`，`is_nothrow_move_constructible`来判断一个类型是否可以被移动

	#include <type_traits>
	std::cout << is_move_constructible<UnknowType>::value;

另外，移动构造函数一旦出现异常，就很可能出现悬挂指针，可以使用`noexcept`描述移动构造函数，`move`也可以用`move_if_noexcept`来替代
