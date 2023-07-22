#include <cstdlib>
#include <cctype>
#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <algorithm>
#include <limits>
#include <type_traits>
#include <stdexcept>
using namespace std;

namespace bigInt
{
	class BigInt final
	{
	public:
		// 去除前缀零的函数
		static size_t removeHeadZero(BigInt& num)
		{
			size_t i = 0;
			while (num.c.size() > 1 && num.c.back() == 0)
			{
				num.c.pop_back();
				++i;
			}

			return i;
		}

		// 构造函数
		BigInt() : mNegativeFlag(false) {}
		BigInt(const BigInt& other) : c(other.c), mNegativeFlag(other.mNegativeFlag) {}
		BigInt(BigInt&& right) : c(move(right.c)),
			mNegativeFlag(move(right.mNegativeFlag)) {}
		BigInt(string str) : c(str.size())
		{
			string::size_type length = str.size();

			// 判断字符串开头是否有负号
			if (str.front() == '-')
			{
				mNegativeFlag = true;
				str.front() = '0';    // 把负号变成前缀零，函数最后会消除前缀零
			}
			else
				mNegativeFlag = false;

			// 倒序存储
			for (string::size_type i = 0; i < length; ++i)
				c[i] = str[length - 1 - i] - '0';

			removeHeadZero(*this);
		}

		BigInt(long long num) : BigInt(to_string(num)) {}

		template <typename T>
		explicit BigInt(const T& num, bool negativeFlag = false) : c(num),
			mNegativeFlag(negativeFlag) {}

		// 赋值操作符
		BigInt& operator=(const BigInt& other)
		{
			c = other.c;
			mNegativeFlag = other.mNegativeFlag;
			return *this;
		}

		BigInt& operator=(BigInt&& right)
		{
			c = move(right.c);
			mNegativeFlag = move(right.mNegativeFlag);
			return *this;
		}

		// 取反操作符
		BigInt operator-()
		{
			return BigInt(c, !mNegativeFlag);
		}

		// 默认的析构函数
		~BigInt() = default;

		// 友元函数声明
		friend bool operator>(const BigInt& a, const BigInt& b);
		friend bool operator<(const BigInt& a, const BigInt& b);
		friend bool operator>=(const BigInt& a, const BigInt& b);
		friend bool operator<=(const BigInt& a, const BigInt& b);
		friend bool operator==(const BigInt& a, const BigInt& b);
		friend BigInt operator+(const BigInt& a, const BigInt& b);
		friend BigInt operator-(BigInt a, BigInt b);
		friend BigInt operator*(BigInt a, BigInt b);
		friend BigInt operator/(BigInt a, BigInt b);
		friend BigInt operator%(const BigInt& a, const BigInt& b);
		friend ostream& operator<<(ostream& out, const BigInt& value);
		friend istream& operator>>(istream& in, BigInt& value);
	private:
		deque<short> c;
		bool mNegativeFlag;
	public:
		// 加等于操作符
		BigInt& operator+=(const BigInt& other)
		{
			return *this = *this + other;
		}

		// 减等于操作符
		BigInt& operator-=(const BigInt& other)
		{
			return *this = *this - other;
		}

		// 乘等于操作符
		BigInt& operator*=(const BigInt& other)
		{
			return *this = *this * other;
		}

		// 除等于操作符
		BigInt& operator/=(const BigInt& other)
		{
			return *this = *this / other;
		}

		// 模等于操作符
		BigInt& operator%=(const BigInt& other)
		{
			return *this = *this % other;
		}

		// 前置自增操作符
		BigInt& operator++()
		{
			return *this += 1;
		}

		// 后置自增操作符
		BigInt operator++(int)
		{
			BigInt temp = *this;
			++*this;

			return temp;
		}

		// 前置自减操作符
		BigInt& operator--()
		{
			return *this -= 1;
		}

		// 后置自减操作符
		BigInt operator--(int)
		{
			BigInt temp = *this;
			--*this;

			return temp;
		}

		// 返回绝对值的BigInt类成员函数
		BigInt abs() const
		{
			return BigInt(c);
		}

		// 恢复未定义值的状态
		void reset()
		{
			c.clear();
			mNegativeFlag = false;
		}

		// 获取是否未设定初始值
		bool empty() const
		{
			return c.empty();
		}

		// 获取高精度整数的字符串表示形式
		string toStr() const
		{
			string result;
			result.reserve(c.size());

			for (auto iter = c.rbegin(); iter != c.rend(); ++iter)
				result += *iter + '0';

			return result;
		}
	};

	// BigInt类大于比较操作符的函数实现
	bool operator>(const BigInt& a, const BigInt& b)
	{
		if (a.mNegativeFlag != b.mNegativeFlag)
			return !a.mNegativeFlag && b.mNegativeFlag;
		else if (a.c.size() != b.c.size())
			return a.c.size() > b.c.size();
		else
		{
			decltype(BigInt::c)::size_type length = a.c.size();
			bool negativeFlag = a.mNegativeFlag;
			// 从后向前比较
			for (decltype(BigInt::c)::size_type i = length - 1;
				i < numeric_limits<decltype(i)>::max(); --i)
			{
				if (a.c[i] != b.c[i])
					return negativeFlag != a.c[i] > b.c[i];    // 这里是考虑到负数情况
			}

			return false;    // 等于的情况
		}
	}

	// BigInt类小于操作符的函数实现
	bool operator<(const BigInt& a, const BigInt& b)
	{
		if (a.mNegativeFlag != b.mNegativeFlag)
			return a.mNegativeFlag && !b.mNegativeFlag;
		else if (a.c.size() != b.c.size())
			return a.c.size() < b.c.size();
		else
		{
			decltype(BigInt::c)::size_type length = a.c.size();
			bool negativeFlag = a.mNegativeFlag;
			// 从后向前比较
			for (decltype(BigInt::c)::size_type i = length - 1;
				i < numeric_limits<decltype(i)>::max(); --i)
			{
				if (a.c[i] != b.c[i])
					return negativeFlag != a.c[i] < b.c[i];    // 这里是考虑到了等于的情况
			}

			return false;    // 等于的情况
		}
	}

	// BigInt类大于等于比较操作符的函数实现
	inline bool operator>=(const BigInt& a, const BigInt& b)
	{
		return !(a < b);
	}

	// BigInt类小于等于比较操作符的函数实现
	inline bool operator<=(const BigInt& a, const BigInt& b)
	{
		return !(a > b);
	}

	// BigInt类等于比较操作符的函数实现
	inline bool operator==(const BigInt& a, const BigInt& b)
	{
		return a.mNegativeFlag == b.mNegativeFlag && a.c == b.c;
	}

	// BigInt类加操作符的函数实现
	BigInt operator+(const BigInt& a, const BigInt& b)
	{
		if (a.mNegativeFlag == b.mNegativeFlag)
		{
			decltype(BigInt::c)::size_type maxLength = max(a.c.size(), b.c.size());
			BigInt result;
			bool carryFlag = false;
			deque<short> copyA = a.c, copyB = b.c;
			// 添加前缀零，防止越界
			copyA.resize(maxLength); copyB.resize(maxLength);

			for (decltype(BigInt::c)::size_type i = 0; i < maxLength; ++i)
			{
				result.c.push_back((carryFlag + copyA[i] + copyB[i]) % 10);
				carryFlag = (carryFlag + copyA[i] + copyB[i]) / 10;
			}

			if (carryFlag)
				result.c.push_back(1);

			return result;
		}
		else if (a.mNegativeFlag && !b.mNegativeFlag)
			return b - a.abs();
		else
			return a - b.abs();
	}

	// BigInt类减操作符的函数实现
	BigInt operator-(BigInt a, BigInt b)
	{
		if (a.mNegativeFlag == b.mNegativeFlag)
		{
			decltype(BigInt::c)::size_type maxLength = max(a.c.size(), b.c.size());
			BigInt result(deque<short>(), a.mNegativeFlag);
			bool borrowFlag = false;
			a.c.resize(maxLength); b.c.resize(maxLength);

			// 处理小减大
			if (a.abs() < b.abs())
			{
				swap(a, b);
				result.mNegativeFlag = !result.mNegativeFlag;
			}

			short bitResult;    // 每一位上的结果
			for (decltype(BigInt::c)::size_type i = 0; i < maxLength; ++i)
			{
				bitResult = a.c[i] - b.c[i] - borrowFlag;
				if (bitResult < 0)
				{
					// 借位
					bitResult += 10;
					borrowFlag = true;
				}
				else
					borrowFlag = false;

				// 把这一位上的结果放入result.c
				result.c.push_back(bitResult);
			}

			// 去除前导零
			BigInt::removeHeadZero(result);

			return result;
		}
		else if (a.mNegativeFlag && !b.mNegativeFlag)
			return -(a.abs() + b);
		else
			return a + b;
	}

	// BigInt类乘操作符的函数实现
	BigInt operator*(BigInt a, BigInt b)
	{
		if (a.c.size() > b.c.size())
			swap(a, b);

		decltype(BigInt::c)::size_type l = a.c.size() + b.c.size(), k;
		// 结果多申请了两个位置，预留给进位的
		BigInt result(deque<short>(l + 2), a.mNegativeFlag != b.mNegativeFlag);

		for (decltype(BigInt::c)::size_type i = 0, j; i < a.c.size(); ++i)
		{
			for (j = 0; j < b.c.size(); ++j)
			{
				k = i + j;
				result.c[k] += a.c[i] * b.c[j];
				result.c[k + 1] += result.c[k] / 10;
				result.c[k] %= 10;
			}
		}

		BigInt::removeHeadZero(result);

		return result;
	}

	// BigInt类除操作符的函数实现
	BigInt operator/(BigInt a, BigInt b)
	{
		if (b == 0)
			throw invalid_argument("Divide by zero!");

		if (a == 0)
			return 0;

		decltype(BigInt::c)::size_type i = a.c.size() - 1;
		BigInt subset;
		subset.c.push_back(a.c.back());
		bool negativeFlag = a.mNegativeFlag != b.mNegativeFlag;
		a = a.abs(); b = b.abs();

		while (subset < b && i < numeric_limits<decltype(i)>::max())
		{
			--i;
			subset.c.push_front(a.c[i]);
		}

		auto& x = subset;
		deque<short> ans;
		short bitResult;

		while (i < numeric_limits<decltype(i)>::max())
		{
			bitResult = 0;
			while (x >= b)
			{
				x -= b;
				++bitResult;
			}

			ans.push_front(bitResult);

			if (i > 0)
			{
				x.c.push_front(a.c[i - 1]);
				BigInt::removeHeadZero(x);
			}

			--i;
		}

		return BigInt(ans, negativeFlag);
	}

	// BigInt类取模操作符的函数实现
	inline BigInt operator%(const BigInt& a, const BigInt& b)
	{
		return a - a / b * b;
	}

	// 输出操作符
	inline ostream& operator<<(ostream& out, const BigInt& value)
	{
		if (value.mNegativeFlag)
			out.put('-');

		for (auto iter = value.c.rbegin(); iter != value.c.rend(); ++iter)
			out.put(*iter + '0');

		return out;
	}

	// 输入操作符
	inline istream& operator>>(istream& in, BigInt& value)
	{
		string input;
		in >> input;

		for (string::size_type i = 0; i < input.size(); ++i)
		{
			if (i == 0 && input[i] == '-')
				continue;
			else if (!isdigit(input[i]))
			{
				in.setstate(ios::badbit);
				return in;
			}
		}

		value = input;
		BigInt::removeHeadZero(value);

		return in;
	}
}

int main()
{
	using namespace bigInt;

	BigInt numA, numB;
	cout << boolalpha << "numA.empty(): " << numA.empty()
		<< " numB.empty(): " << numB.empty() << '\n';

	cout << "Please press the numA and the numB:\n";
	cin >> numA >> numB;

	cout << "numA: " << numA << " numB: " << numB << '\n';
	cout << "numA + numB: " << numA + numB << '\n';
	cout << "numA - numB: " << numA - numB << '\n';
	cout << "numA * numB: " << numA * numB << '\n';
	cout << "numA / numB: " << numA / numB << '\n';
	cout << "numA % numB: " << numA % numB << '\n';

	return EXIT_SUCCESS;
}