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


	




