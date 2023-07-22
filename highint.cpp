#include <cstdlib>
#include <cctype>
#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <algorithm>
#include <limits>
#include <stdexcept>
using namespace std;

namespace bigInt
{
	class BigInt final
	{
	public:
		// ȥ��ǰ׺��ĺ���
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

		// ���캯��
		BigInt() : mNegativeFlag(false) {}
		BigInt(const BigInt& other) : c(other.c), mNegativeFlag(other.mNegativeFlag) {}
		BigInt(BigInt&& right) : c(move(right.c)),
			mNegativeFlag(move(right.mNegativeFlag)) {}
		BigInt(string str) : c(str.size())
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

		BigInt(long long num) : BigInt(to_string(num)) {}

		// ��ֵ������
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

		// ȡ��������
		BigInt operator-()
		{
			return BigInt(c, !mNegativeFlag);
		}

		// Ĭ�ϵ���������
		~BigInt() = default;

		// ��Ԫ��������
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

		template <typename T>
		explicit BigInt(const T& num, bool negativeFlag = false) : c(num),
			mNegativeFlag(negativeFlag) {}
	public:
		// �ӵ��ڲ�����
		BigInt& operator+=(const BigInt& other)
		{
			return *this = *this + other;
		}

		// �����ڲ�����
		BigInt& operator-=(const BigInt& other)
		{
			return *this = *this - other;
		}

		// �˵��ڲ�����
		BigInt& operator*=(const BigInt& other)
		{
			return *this = *this * other;
		}

		// �����ڲ�����
		BigInt& operator/=(const BigInt& other)
		{
			return *this = *this / other;
		}

		// ģ���ڲ�����
		BigInt& operator%=(const BigInt& other)
		{
			return *this = *this % other;
		}

		// ǰ������������
		BigInt& operator++()
		{
			return *this += 1;
		}

		// ��������������
		BigInt operator++(int)
		{
			BigInt temp = *this;
			++*this;

			return temp;
		}

		// ǰ���Լ�������
		BigInt& operator--()
		{
			return *this -= 1;
		}

		// �����Լ�������
		BigInt operator--(int)
		{
			BigInt temp = *this;
			--*this;

			return temp;
		}

		// ���ؾ���ֵ��BigInt���Ա����
		BigInt abs() const
		{
			return BigInt(c);
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
			result.reserve(c.size());

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
			// �Ӻ���ǰ�Ƚ�
			for (decltype(BigInt::c)::size_type i = length - 1;
				i < numeric_limits<decltype(i)>::max(); --i)
			{
				if (a.c[i] != b.c[i])
					return negativeFlag != a.c[i] > b.c[i];    // �����ǿ��ǵ��������
			}

			return false;    // ���ڵ����
		}
	}

	// BigInt��С�ڲ������ĺ���ʵ��
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
			// �Ӻ���ǰ�Ƚ�
			for (decltype(BigInt::c)::size_type i = length - 1;
				i < numeric_limits<decltype(i)>::max(); --i)
			{
				if (a.c[i] != b.c[i])
					return negativeFlag != a.c[i] < b.c[i];    // �����ǿ��ǵ��˵��ڵ����
			}

			return false;    // ���ڵ����
		}
	}

	// BigInt����ڵ��ڱȽϲ������ĺ���ʵ��
	inline bool operator>=(const BigInt& a, const BigInt& b)
	{
		return !(a < b);
	}

	// BigInt��С�ڵ��ڱȽϲ������ĺ���ʵ��
	inline bool operator<=(const BigInt& a, const BigInt& b)
	{
		return !(a > b);
	}

	// BigInt����ڱȽϲ������ĺ���ʵ��
	inline bool operator==(const BigInt& a, const BigInt& b)
	{
		return a.mNegativeFlag == b.mNegativeFlag && a.c == b.c;
	}

	// BigInt��Ӳ������ĺ���ʵ��
	BigInt operator+(const BigInt& a, const BigInt& b)
	{
		if (a.mNegativeFlag == b.mNegativeFlag)
		{
			decltype(BigInt::c)::size_type maxLength = max(a.c.size(), b.c.size());
			BigInt result;
			bool carryFlag = false;
			deque<short> copyA = a.c, copyB = b.c;
			// ���ǰ׺�㣬��ֹԽ��
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

	// BigInt����������ĺ���ʵ��
	BigInt operator-(BigInt a, BigInt b)
	{
		if (a.mNegativeFlag == b.mNegativeFlag)
		{
			decltype(BigInt::c)::size_type maxLength = max(a.c.size(), b.c.size());
			BigInt result(deque<short>(), a.mNegativeFlag);
			bool borrowFlag = false;
			a.c.resize(maxLength); b.c.resize(maxLength);

			// ����С����
			if (a.abs() < b.abs())
			{
				swap(a, b);
				result.mNegativeFlag = !result.mNegativeFlag;
			}

			short bitResult;    // ÿһλ�ϵĽ��
			for (decltype(BigInt::c)::size_type i = 0; i < maxLength; ++i)
			{
				bitResult = a.c[i] - b.c[i] - borrowFlag;
				if (bitResult < 0)
				{
					// ��λ
					bitResult += 10;
					borrowFlag = true;
				}
				else
					borrowFlag = false;

				// ����һλ�ϵĽ������result.c
				result.c.push_back(bitResult);
			}

			// ȥ��ǰ����
			BigInt::removeHeadZero(result);

			return result;
		}
		else if (a.mNegativeFlag && !b.mNegativeFlag)
			return -(a.abs() + b);
		else
			return a + b;
	}

	// BigInt��˲������ĺ���ʵ��
	BigInt operator*(BigInt a, BigInt b)
	{
		if (a.c.size() > b.c.size())
			swap(a, b);

		decltype(BigInt::c)::size_type l = a.c.size() + b.c.size(), k;
		// ���������������λ�ã�Ԥ������λ��
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

	// BigInt����������ĺ���ʵ��
	BigInt operator/(BigInt a, BigInt b)
	{
		if (b == 0)
			throw invalid_argument("Divide by zero!");

		if (a == 0)
			return 0;

		if (a < b)
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

	// BigInt��ȡģ�������ĺ���ʵ��
	inline BigInt operator%(const BigInt& a, const BigInt& b)
	{
		return a - a / b * b;
	}

	// ���������
	inline ostream& operator<<(ostream& out, const BigInt& value)
	{
		if (value.mNegativeFlag)
			out.put('-');

		for (auto iter = value.c.rbegin(); iter != value.c.rend(); ++iter)
			out.put(*iter + '0');

		return out;
	}

	// ���������
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

	if (numA.tryToInt())
		cout << "numA to int: " << numA.toInt() << '\n';

	if (numB.tryToInt())
		cout << "numB to int: " << numB.toInt() << '\n';

	return EXIT_SUCCESS;
}