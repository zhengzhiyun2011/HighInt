#include <cstdlib>
#include <cmath>
#include <cctype>
#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <future>
#include <chrono>
#include <type_traits>
using namespace std;

namespace bigInt
{
	class HighInt final
	{
	public:
		// 去除前缀零的函数
		static size_t removeHeadZero(HighInt& num)
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
		HighInt() : mNegativeFlag(false) {}
		HighInt(const HighInt& other) : c(other.c),
			mNegativeFlag(other.mNegativeFlag) {}
		HighInt(HighInt&& right) : c(move(right.c)),
			mNegativeFlag(move(right.mNegativeFlag)) {}
		HighInt(string str) : c(str.size())
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

		template <typename T, typename enable_if<is_integral<T>::value>::type* = nullptr>
		HighInt(T num) : mNegativeFlag(num < 0)
		{
			num = std::abs(num);

			if (num == 0)
				c.push_back(0);

			while (num >= 1)
			{
				c.push_back(num % 10);
				num /= 10;
			}
		}

		// 赋值操作符
		HighInt& operator=(const HighInt& other)
		{
			c = other.c;
			mNegativeFlag = other.mNegativeFlag;
			return *this;
		}

		HighInt& operator=(HighInt&& right)
		{
			c = move(right.c);
			mNegativeFlag = move(right.mNegativeFlag);
			return *this;
		}

		// 取反操作符
		HighInt operator-()
		{
			return HighInt(c, !mNegativeFlag);
		}

		// 默认的析构函数
		~HighInt() = default;

		// 友元函数声明
		friend bool operator>(const HighInt& a, const HighInt& b);
		friend bool operator<(const HighInt& a, const HighInt& b);
		friend bool operator>=(const HighInt& a, const HighInt& b);
		friend bool operator<=(const HighInt& a, const HighInt& b);
		friend bool operator==(const HighInt& a, const HighInt& b);
		friend bool operator!=(const HighInt& a, const HighInt& b);
		friend HighInt operator+(const HighInt& a, const HighInt& b);
		friend HighInt operator-(const HighInt& a, const HighInt& b);
		friend HighInt operator*(HighInt a, HighInt b);
		friend HighInt operator/(HighInt a, HighInt b);
		friend HighInt operator%(HighInt a, HighInt b);
		friend ostream& operator<<(ostream& out, const HighInt& value);
		friend istream& operator>>(istream& in, HighInt& value);
	private:
		deque<short> c;
		bool mNegativeFlag;

		template <typename T>
		explicit HighInt(const T& num, bool negativeFlag) : c(num),
			mNegativeFlag(negativeFlag) {}

		// 检查下标是否越界的函数
		static bool checkPos(size_t pos, const HighInt& num)
		{
			return pos < num.c.size();
		}
	public:
		// 加等于操作符
		HighInt& operator+=(const HighInt& other)
		{
			return *this = *this + other;
		}

		// 减等于操作符
		HighInt& operator-=(const HighInt& other)
		{
			return *this = *this - other;
		}

		// 乘等于操作符
		HighInt& operator*=(const HighInt& other)
		{
			return *this = *this * other;
		}

		// 除等于操作符
		HighInt& operator/=(const HighInt& other)
		{
			return *this = *this / other;
		}

		// 模等于操作符
		HighInt& operator%=(const HighInt& other)
		{
			return *this = *this % other;
		}

		// 前置自增操作符
		HighInt& operator++()
		{
			return *this += 1;
		}

		// 后置自增操作符
		HighInt operator++(int)
		{
			HighInt temp = *this;
			++*this;

			return temp;
		}

		// 前置自减操作符
		HighInt& operator--()
		{
			return *this -= 1;
		}

		// 后置自减操作符
		HighInt operator--(int)
		{
			HighInt temp = *this;
			--*this;

			return temp;
		}

		// 返回绝对值的BigInt类成员函数
		HighInt abs() const
		{
			return HighInt(c, false);
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
			result.reserve(c.size() + 1);

			if (mNegativeFlag)
				result += '-';

			for (auto iter = c.rbegin(); iter != c.rend(); ++iter)
				result += *iter + '0';

			return result;
		}

		// 尝试获取高精度整数的数字表示形式
		bool tryToInt() const
		{
			return *this >= numeric_limits<long long>::min()
				&& *this <= numeric_limits<long long>::max();
		}

		// 获取高精度整数的数字表示形式（如果可以的话）
		long long toInt() const
		{
			if (tryToInt())
			{
				long long result = 0;
				for (auto iter = c.rbegin(); iter != c.rend(); ++iter)
					result = result * 10 + *iter;

				return result;
			}
			else
				throw out_of_range("Cannot be converted to an integer!");
		}
	};

	// BigInt类大于比较操作符的函数实现
	bool operator>(const HighInt& a, const HighInt& b)
	{
		if (a.mNegativeFlag != b.mNegativeFlag)
			return !a.mNegativeFlag && b.mNegativeFlag;
		else if (a.c.size() != b.c.size())
			return a.c.size() > b.c.size();
		else
		{
			decltype(HighInt::c)::size_type length = a.c.size();
			bool negativeFlag = a.mNegativeFlag;
			// 从后向前比较
			for (decltype(HighInt::c)::size_type i = length - 1;
				i < numeric_limits<decltype(i)>::max(); --i)
			{
				if (a.c[i] != b.c[i])
					return negativeFlag != a.c[i] > b.c[i];    // 这里是考虑到负数情况
			}

			return false;    // 等于的情况
		}
	}

	// BigInt类小于操作符的函数实现
	bool operator<(const HighInt& a, const HighInt& b)
	{
		if (a.mNegativeFlag != b.mNegativeFlag)
			return a.mNegativeFlag && !b.mNegativeFlag;
		else if (a.c.size() != b.c.size())
			return a.c.size() < b.c.size();
		else
		{
			decltype(HighInt::c)::size_type length = a.c.size();
			bool negativeFlag = a.mNegativeFlag;
			// 从后向前比较
			for (decltype(HighInt::c)::size_type i = length - 1;
				i < numeric_limits<decltype(i)>::max(); --i)
			{
				if (a.c[i] != b.c[i])
					return negativeFlag != a.c[i] < b.c[i];    // 这里是考虑到了等于的情况
			}

			return false;    // 等于的情况
		}
	}

	// BigInt类大于等于比较操作符的函数实现
	inline bool operator>=(const HighInt& a, const HighInt& b)
	{
		return !(a < b);
	}

	// BigInt类小于等于比较操作符的函数实现
	inline bool operator<=(const HighInt& a, const HighInt& b)
	{
		return !(a > b);
	}

	// BigInt类等于比较操作符的函数实现
	inline bool operator==(const HighInt& a, const HighInt& b)
	{
		return a.mNegativeFlag == b.mNegativeFlag && a.c == b.c;
	}

	// BigInt类不等于比较操作符的函数实现
	inline bool operator!=(const HighInt& a, const HighInt& b)
	{
		return !(a == b);
	}

	// BigInt类加操作符的函数实现
	HighInt operator+(const HighInt& a, const HighInt& b)
	{
		if (a.mNegativeFlag == b.mNegativeFlag)
		{
			decltype(HighInt::c)::size_type maxLength = max(a.c.size(), b.c.size());
			HighInt result;
			result.mNegativeFlag = a.mNegativeFlag;
			bool carryFlag = false;

			for (decltype(HighInt::c)::size_type i = 0; i < maxLength; ++i)
			{
				short temp;
				if (HighInt::checkPos(i, a) && HighInt::checkPos(i, b))
					temp = a.c[i] + b.c[i] + carryFlag;
				else if (HighInt::checkPos(i, a))
					temp = a.c[i] + carryFlag;
				else
					temp = b.c[i] + carryFlag;

				carryFlag = temp / 10;
				result.c.emplace_back(temp % 10);
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
	HighInt operator-(const HighInt& a, const HighInt& b)
	{
		if (a == b)
			return 0;
		else if (a.mNegativeFlag == b.mNegativeFlag)
		{
			decltype(HighInt::c)::size_type maxLength = max(a.c.size(), b.c.size());
			HighInt result(deque<short>(), a.mNegativeFlag);
			bool borrowFlag = false, flag = false;

			// 处理小减大
			if (a.abs() < b.abs())
			{
				result.mNegativeFlag = !result.mNegativeFlag;
				flag = true;
			}

			short bitResult;    // 每一位上的结果
			for (decltype(HighInt::c)::size_type i = 0; i < maxLength; ++i)
			{
				if (HighInt::checkPos(i, a) && HighInt::checkPos(i, b) && !flag)
					bitResult = a.c[i] - b.c[i] - borrowFlag;
				else if (HighInt::checkPos(i, a) && HighInt::checkPos(i, b) && flag)
					bitResult = b.c[i] - a.c[i] - borrowFlag;
				else if (!flag)
					bitResult = a.c[i] - borrowFlag;
				else
					bitResult = b.c[i] - borrowFlag;

				if (bitResult < 0)
				{
					// 借位
					bitResult += 10;
					borrowFlag = true;
				}
				else
					borrowFlag = false;

				// 把这一位上的结果放入result.c
				result.c.emplace_back(bitResult);
			}

			// 去除前导零
			HighInt::removeHeadZero(result);

			return result;
		}
		else if (a.mNegativeFlag && !b.mNegativeFlag)
			return -(a.abs() + b);
		else
			return a + b;
	}

	// BigInt类乘操作符的函数实现
	HighInt operator*(HighInt a, HighInt b)
	{
		if (a.c.size() > b.c.size())
			swap(a, b);

		decltype(HighInt::c)::size_type l = a.c.size() + b.c.size();
		// 结果多申请了两个位置，预留给进位的
		HighInt result(deque<short>(l + 2), a.mNegativeFlag != b.mNegativeFlag);
		deque<future<void>> resList;

		// 并行乘法
		for (decltype(HighInt::c)::size_type i = 0; i < a.c.size(); ++i)
		{
			resList.emplace_back(async(launch::async, [i, &result, &a, &b]() {
				for (decltype(HighInt::c)::size_type j = 0; j < b.c.size(); ++j)
				{
					decltype(HighInt::c)::size_type k = i + j;
					result.c[k] += a.c[i] * b.c[j];
					result.c[k + 1] += result.c[k] / 10;
					result.c[k] %= 10;
				}
				}));

			while (resList.front().valid()
				&& resList.front().wait_for(chrono::seconds(0)) == future_status::ready)
				resList.pop_front();
		}

		for (const auto& res : resList)
		{
			if (res.valid())
				res.wait();
		}

		HighInt::removeHeadZero(result);

		return result;
	}

	// BigInt类除操作符的函数实现
	HighInt operator/(HighInt a, HighInt b)
	{
		if (b == 0)
			throw invalid_argument("Divide by zero!");

		if (a == 0)
			return 0;

		if (a.abs() < b.abs())
			return 0;

		decltype(HighInt::c)::size_type i = a.c.size() - 1;
		HighInt subset;
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
				HighInt::removeHeadZero(x);
			}

			--i;
		}

		return HighInt(ans, negativeFlag);
	}

	// BigInt类取模操作符的函数实现
	HighInt operator%(HighInt a, HighInt b)
	{
		if (b == 0)
			throw invalid_argument("Divide by zero!");

		if (a == 0)
			return 0;

		if (a.abs() < b.abs())
			return HighInt(a.c, a.mNegativeFlag != b.mNegativeFlag);

		decltype(HighInt::c)::size_type i = a.c.size() - 1;
		HighInt subset;
		subset.c.push_back(a.c.back());
		bool negativeFlag = a.mNegativeFlag != b.mNegativeFlag;
		a = a.abs(); b = b.abs();

		while (subset < b && i < numeric_limits<decltype(i)>::max())
		{
			--i;
			subset.c.push_front(a.c[i]);
		}

		auto& x = subset;

		while (i < numeric_limits<decltype(i)>::max())
		{
			while (x >= b)
				x -= b;

			if (i > 0)
			{
				x.c.push_front(a.c[i - 1]);
				HighInt::removeHeadZero(x);
			}

			--i;
		}

		x.mNegativeFlag = negativeFlag;

		return x;
	}

	// 输出操作符
	inline ostream& operator<<(ostream& out, const HighInt& value)
	{
		return out << value.toStr();
	}

	// 输入操作符
	inline istream& operator>>(istream& in, HighInt& value)
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

		return in;
	}
}

int main()
{
	using namespace bigInt;

	HighInt a, b;
	cin >> a >> b;
	cout << a - b << '\n';
	
	return EXIT_SUCCESS;
}