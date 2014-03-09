/*
 * 遵循《Effective C++》和《C++ Primer》的设计原则
 * 条款18：将设计class当作设计type
 * 1.资源获取：构造函数，析构函数，复制构造函数(支持三种复制构造函数，分别是int,char*,string)
 * 2.函数操作：运算符重载(+,-,*,=,+=,-=,*=,==,!=,>,>=,<,<=,>>,<<,其中>>和<<为输入和输出源算符)。重载运算符的原因是让bigNum可以当作int来使用。暂时未编写*,*=,/,/=和%,%=
 * 3.异常处理：overflow
 */
#include <iostream>
#include <string>
#include <cstring>

const int SIZE = 512;

using namespace std;

class bigNum 
{
	//条款22：将成员变量声明为private
	private:
		char digit[SIZE];
		int length;
		bool isNegative;
	public:
		bigNum() {
			digit[0] = '0';
			digit[1] = '\0';
			length = 1;
			isNegative = false;
		}
		bigNum(const int &);
		bigNum(const char *);
		bigNum(const std::string &);
		bigNum(const bigNum &);
		~bigNum(){}

		//重载输入输出流
		friend istream & operator>> (istream &,bigNum &);
		friend ostream & operator<< (ostream &,const bigNum &);

		//条款10：令operator = 返回一个reference to *this
		//C++Primer 忠告：赋值操作符必须定义为类成员，复合赋值操作符则通常定义为类成员。
		bigNum & operator = (const bigNum &);
		bigNum & operator += (const bigNum &);
		bigNum & operator -= (const bigNum &);
		bigNum & operator *= (const bigNum &);

		//条款3：令函数返回一个常量，避免客户错误造成的意外
		//条款21：必须返回对象的时候，别妄想返回其reference
		//条款24：若所有参数皆需类型转换，请为此采用non-menber函数
		friend const bigNum operator + (const bigNum &,const bigNum &);
		friend const bigNum operator - (const bigNum &,const bigNum &);
		friend const bigNum operator * (const bigNum &,const bigNum &);

		//条款18：像设计type一样设计类
		//C++Primer 忠告：对称的操作符，比如关系操作符，最好定义为非成员函数
		//条款24
		friend bool operator == (const bigNum &,const bigNum &);
		friend bool operator != (const bigNum &,const bigNum &);
		friend bool operator > (const bigNum &,const bigNum &);
		friend bool operator < (const bigNum &,const bigNum &);
		friend bool operator >= (const bigNum &,const bigNum &);
		friend bool operator <= (const bigNum &,const bigNum &);
		
		int getLength();
};

//各种构造函数
bigNum::bigNum(const int &rhs)
{
	char temp[SIZE];
	int index = 0,value;
	if(rhs >= 0)
	{
		isNegative = false;
		value = rhs;
	}
	else
	{
		isNegative = true;
		value = -rhs;
	}
	while(value)
	{
		int x = value % 10;
		temp[index++] = char(x+'0');
		value /= 10;
	}
	temp[index] = '\0';
	for(int i=index-1; i>=0; i--)
		digit[index-i-1] = temp[i];
	length = index;
}

bigNum::bigNum(const char *rhs)
{
	size_t rhs_len = strlen(rhs);
	if(rhs[0] == '-')
	{
		length = rhs_len-1;
		isNegative = true;
		for(int i=1; i<rhs_len; i++)
			digit[i-1] = rhs[i];
		digit[rhs_len-1] = '\0';
	}
	else
	{
		length = rhs_len;
		isNegative = false;
		strcpy(digit,rhs);
	}
}

bigNum::bigNum(const std::string &rhs)
{
	size_t rhs_len = rhs.size();
	if(rhs[0] == '-')
	{
		length = rhs_len-1;
		isNegative = true;
		for(int i=1; i<rhs_len; i++)
			digit[i-1] = rhs[i];
		digit[rhs_len-1] = '\0';
	}
	else
	{
		length = rhs_len;
		isNegative = false;
		for(int i=0; i<rhs_len; i++)
			digit[i] = rhs[i];
		digit[rhs_len] = '\0';
	}
}

bigNum::bigNum(const bigNum &rhs)
{
	strcpy(digit,rhs.digit);
	length = rhs.length;
	isNegative = rhs.isNegative;
}

int bigNum::getLength()
{
	return length;
}

//大数运算的运算符重载
//输入
istream & operator >> (istream &is,bigNum &rhs)
{
	char temp[SIZE];
	is >> temp;
	size_t temp_len = strlen(temp);
	if(temp_len == 0)
	{
		rhs = bigNum();
	}
	else 
	{
		if(temp[0] == '-')
		{
			rhs.length = temp_len-1;
			rhs.isNegative = true;
			for(int i=1; i<temp_len; i++)
			{
				if(temp[i] < '0' || temp[i] > '9')
				{
					rhs = bigNum();
					return is;
				}
				rhs.digit[i-1] = temp[i];
			}
			rhs.digit[temp_len-1] = '\0';
		}
		else
		{
			rhs.length = temp_len;
			rhs.isNegative = false;
			for(int i=0; i<temp_len; i++)
			{
				if(temp[i] < '0' || temp[i] > '9')
				{
					rhs = bigNum();
					return is;
				}
				rhs.digit[i] = temp[i];
			}
			rhs.digit[temp_len] = '\0';
		}
	}
	return is;
}
//输出
ostream & operator << (ostream &os,const bigNum &rhs)
{
	if(rhs.isNegative == true)
		os << '-';
	os << rhs.digit;
	return os;
}
//赋值
bigNum & bigNum::operator = (const bigNum &rhs)
{
	strcpy(digit,rhs.digit);
	this->length = rhs.length;
	this->isNegative = rhs.isNegative;
	return *this;
}
//比较
bool operator == (const bigNum &lhs,const bigNum &rhs)
{
	return (lhs.length == rhs.length && strcmp(lhs.digit,rhs.digit) == 0 && lhs.isNegative == rhs.isNegative);
}
bool operator != (const bigNum &lhs,const bigNum &rhs)
{
	return !(lhs == rhs);
}
bool operator < (const bigNum &lhs,const bigNum &rhs)
{
	if(lhs == rhs)
		return false;
	if(lhs.isNegative == false && rhs.isNegative == true)
		return false;
	if(lhs.isNegative == true && rhs.isNegative == false)
		return true;
	if(lhs.length > rhs.length)
	{
		if(lhs.isNegative == false)
			return false;
		else
			return true;
	}
	else if(lhs.length < rhs.length)
	{
		if(lhs.isNegative == false)
			return true;
		else
			return false;
	}
	int len = lhs.length;
	for(int i=0; i<len; i++)
	{
		if(lhs.digit[i] > rhs.digit[i])
		{
			if(lhs.isNegative == false)
				return false;
			else
				return true;
		}
		if(lhs.digit[i] < rhs.digit[i])
		{
			if(lhs.isNegative == false)
				return true;
			else
				return false;
		}
	}
	return false;
}
bool operator > (const bigNum &lhs,const bigNum &rhs)
{
	if(lhs == rhs)
		return false;
	return !(lhs < rhs);
}
bool operator <= (const bigNum &lhs,const bigNum &rhs)
{
	return (lhs == rhs || lhs < rhs);
}
bool operator >= (const bigNum &lhs,const bigNum &rhs)
{
	return (lhs == rhs || lhs > rhs);
}
//复合赋值
bigNum & bigNum::operator += (const bigNum &rhs)
{
	if((this->isNegative && rhs.isNegative) || (!this->isNegative && !rhs.isNegative))
	{
		char temp[SIZE];
		int temp_ptr = 0;
		int lhs_ptr = this->length-1,rhs_ptr = rhs.length-1;
		int carry = 0;
		while(lhs_ptr >= 0 && rhs_ptr >= 0)
		{
			int value = (this->digit[lhs_ptr--]-'0')+(rhs.digit[rhs_ptr--]-'0')+carry;
			temp[temp_ptr++] = (value%10 + '0');
			carry = value/10;
		}
		while(lhs_ptr >= 0)
		{
			int value = (this->digit[lhs_ptr--]-'0')+carry;
			temp[temp_ptr++] = (value%10 + '0');
			carry = value/10;
		}
		while(rhs_ptr >= 0)
		{
			int value = (rhs.digit[rhs_ptr--]-'0')+carry;
			temp[temp_ptr++] = (value%10 + '0');
			carry = value/10;
		}
		if(carry)
			temp[temp_ptr++] = (carry+'0');
		this->length = temp_ptr;
		for(int i=temp_ptr-1; i>=0; i--)
			digit[temp_ptr-i-1] = temp[i];
		digit[temp_ptr] = '\0';
	}
	else
	{
		bigNum temp(rhs);
		if(this->isNegative)
		{
			this->isNegative = false;
			temp.operator -=(*this);
			strcpy(this->digit,temp.digit);
			this->length = temp.length;
			this->isNegative = temp.isNegative;
		}
		else if(temp.isNegative)
		{
			temp.isNegative = false;
			this->operator -=(temp);
		}
	}
	return *this;
}
bigNum & bigNum::operator -= (const bigNum &rhs)
{
	bigNum left(*this),right(rhs);
	left.isNegative = right.isNegative = false;
	bigNum maxi,mini;
	char temp[SIZE];
	int temp_ptr = 0;
	bigNum lhs(*this);
	if(left > right)
	{
		maxi = left;
		mini = right;
	}
	else
	{
		maxi = right;
		mini = left;
	}
	if((!lhs.isNegative && !rhs.isNegative) || (lhs.isNegative && rhs.isNegative)) 
	{
		int lhs_ptr = maxi.length-1,rhs_ptr = mini.length-1;
		int borrow = 0;
		while(lhs_ptr >= 0 && rhs_ptr >= 0)
		{
			int value = (maxi.digit[lhs_ptr--]-'0'-borrow)-(mini.digit[rhs_ptr--]-'0');
			borrow = 0;
			if(value < 0)
				value += 10,borrow = 1;
			temp[temp_ptr++] = (value+'0');
		}
		while(lhs_ptr >= 0)
		{
			int value = (maxi.digit[lhs_ptr--]-'0'-borrow);
			borrow = 0;
			if(value < 0)
				value += 10,borrow = 1;
			temp[temp_ptr++] = (value+'0');
		}
		temp[temp_ptr] = '\0';
		while(temp_ptr > 0 && temp[temp_ptr-1] == '0')
			--temp_ptr;
		if(temp_ptr == 0)
		{
			this->digit[0] = '0';
			this->digit[1] = '\0';
			this->length = 1;
			this->isNegative = false;
			return *this;
		}
		for(int i=temp_ptr-1; i>=0; i--)
			this->digit[temp_ptr-i-1] = temp[i];
		this->digit[temp_ptr] = '\0';
		this->length = temp_ptr;
		if(!lhs.isNegative && !rhs.isNegative)
		{
			if(lhs < rhs)
				this->isNegative = true;
			else
				this->isNegative = false;
		}
		else if(lhs.isNegative && rhs.isNegative)
		{
			if(lhs < rhs)
				this->isNegative = false;
			else
				this->isNegative = true;
		}
	}
	else
	{
		maxi.operator +=(mini);
		strcpy(this->digit,maxi.digit);
		this->length = maxi.length;
		if(lhs.isNegative)
			this->isNegative = true;
		else
			this->isNegative = false;
	}
	return *this;
}
//算术操作符
const bigNum operator + (const bigNum &lhs,const bigNum &rhs)
{
	bigNum ret(lhs);
	ret.operator +=(rhs);
	return ret;
}
const bigNum operator - (const bigNum &lhs,const bigNum &rhs)
{
	bigNum ret(lhs);
	ret.operator -=(rhs);
	return ret;
}
