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
		// ȥ��ǰ׺��ĺ���
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

		// ���캯��
		HighInt() : mNegativeFlag(false) {}
		HighInt(const HighInt& other) : c(other.c),
			mNegativeFlag(other.mNegativeFlag) {}
		HighInt(HighInt&& right) : c(move(right.c)),
			mNegativeFlag(move(right.mNegativeFlag)) {}
		HighInt(string str) : c(str.size())
		{
			string::size_type length = str.size();

			// �ж��ַ�����ͷ�Ƿ��и���
			if (str.front() == '-')
			{
				mNegativeFlag = true;
				str.front() = '0';    // �Ѹ��ű��ǰ׺�㣬������������ǰ׺��
			}
			else
				mNegativeFlag = false;

			// ����洢
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

		// ��ֵ������
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

		// ȡ��������
		HighInt operator-()
		{
			return HighInt(c, !mNegativeFlag);
		}

		// Ĭ�ϵ���������
		~HighInt() = default;

		// ��Ԫ��������
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

		// ����±��Ƿ�Խ��ĺ���
		static bool checkPos(size_t pos, const HighInt& num)
		{
			return pos < num.c.size();
		}
	public:
		// �ӵ��ڲ�����
		HighInt& operator+=(const HighInt& other)
		{
			return *this = *this + other;
		}

		// �����ڲ�����
		HighInt& operator-=(const HighInt& other)
		{
			return *this = *this - other;
		}

		// �˵��ڲ�����
		HighInt& operator*=(const HighInt& other)
		{
			return *this = *this * other;
		}

		// �����ڲ�����
		HighInt& operator/=(const HighInt& other)
		{
			return *this = *this / other;
		}

		// ģ���ڲ�����
		HighInt& operator%=(const HighInt& other)
		{
			return *this = *this % other;
		}

		// ǰ������������
		HighInt& operator++()
		{
			return *this += 1;
		}

		// ��������������
		HighInt operator++(int)
		{
			HighInt temp = *this;
			++*this;

			return temp;
		}

		// ǰ���Լ�������
		HighInt& operator--()
		{
			return *this -= 1;
		}

		// �����Լ�������
		HighInt operator--(int)
		{
			HighInt temp = *this;
			--*this;

			return temp;
		}

		// ���ؾ���ֵ��BigInt���Ա����
		HighInt abs() const
		{
			return HighInt(c, false);
		}

		// �ָ�δ����ֵ��״̬
		void reset()
		{
			c.clear();
			mNegativeFlag = false;
		}

		// ��ȡ�Ƿ�δ�趨��ʼֵ
		bool empty() const
		{
			return c.empty();
		}

		// ��ȡ�߾����������ַ�����ʾ��ʽ
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

		// ���Ի�ȡ�߾������������ֱ�ʾ��ʽ
		bool tryToInt() const
		{
			return *this >= numeric_limits<long long>::min()
				&& *this <= numeric_limits<long long>::max();
		}

		// ��ȡ�߾������������ֱ�ʾ��ʽ��������ԵĻ���
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

	// BigInt����ڱȽϲ������ĺ���ʵ��
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
			// �Ӻ���ǰ�Ƚ�
			for (decltype(HighInt::c)::size_type i = length - 1;
				i < numeric_limits<decltype(i)>::max(); --i)
			{
				if (a.c[i] != b.c[i])
					return negativeFlag != a.c[i] > b.c[i];    // �����ǿ��ǵ��������
			}

			return false;    // ���ڵ����
		}
	}

	// BigInt��С�ڲ������ĺ���ʵ��
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
			// �Ӻ���ǰ�Ƚ�
			for (decltype(HighInt::c)::size_type i = length - 1;
				i < numeric_limits<decltype(i)>::max(); --i)
			{
				if (a.c[i] != b.c[i])
					return negativeFlag != a.c[i] < b.c[i];    // �����ǿ��ǵ��˵��ڵ����
			}

			return false;    // ���ڵ����
		}
	}

	// BigInt����ڵ��ڱȽϲ������ĺ���ʵ��
	inline bool operator>=(const HighInt& a, const HighInt& b)
	{
		return !(a < b);
	}

	// BigInt��С�ڵ��ڱȽϲ������ĺ���ʵ��
	inline bool operator<=(const HighInt& a, const HighInt& b)
	{
		return !(a > b);
	}

	// BigInt����ڱȽϲ������ĺ���ʵ��
	inline bool operator==(const HighInt& a, const HighInt& b)
	{
		return a.mNegativeFlag == b.mNegativeFlag && a.c == b.c;
	}

	// BigInt�಻���ڱȽϲ������ĺ���ʵ��
	inline bool operator!=(const HighInt& a, const HighInt& b)
	{
		return !(a == b);
	}

	// BigInt��Ӳ������ĺ���ʵ��
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

	// BigInt����������ĺ���ʵ��
	HighInt operator-(const HighInt& a, const HighInt& b)
	{
		if (a == b)
			return 0;
		else if (a.mNegativeFlag == b.mNegativeFlag)
		{
			decltype(HighInt::c)::size_type maxLength = max(a.c.size(), b.c.size());
			HighInt result(deque<short>(), a.mNegativeFlag);
			bool borrowFlag = false, flag = false;

			// ����С����
			if (a.abs() < b.abs())
			{
				result.mNegativeFlag = !result.mNegativeFlag;
				flag = true;
			}

			short bitResult;    // ÿһλ�ϵĽ��
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
					// ��λ
					bitResult += 10;
					borrowFlag = true;
				}
				else
					borrowFlag = false;

				// ����һλ�ϵĽ������result.c
				result.c.emplace_back(bitResult);
			}

			// ȥ��ǰ����
			HighInt::removeHeadZero(result);

			return result;
		}
		else if (a.mNegativeFlag && !b.mNegativeFlag)
			return -(a.abs() + b);
		else
			return a + b;
	}

	// BigInt��˲������ĺ���ʵ��
	HighInt operator*(HighInt a, HighInt b)
	{
		if (a.c.size() > b.c.size())
			swap(a, b);

		decltype(HighInt::c)::size_type l = a.c.size() + b.c.size();
		// ���������������λ�ã�Ԥ������λ��
		HighInt result(deque<short>(l + 2), a.mNegativeFlag != b.mNegativeFlag);
		deque<future<void>> resList;

		// ���г˷�
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

	// BigInt����������ĺ���ʵ��
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

	// BigInt��ȡģ�������ĺ���ʵ��
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

	// ���������
	inline ostream& operator<<(ostream& out, const HighInt& value)
	{
		return out << value.toStr();
	}

	// ���������
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