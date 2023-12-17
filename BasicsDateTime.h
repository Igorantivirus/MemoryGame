#pragma once
#ifndef GREGORY_DATETIME
#define GREGORY_DATETIME
#include<string>
#include<chrono>
#include<iostream>

namespace sib {
	using namespace std;
	class DateTime;

#pragma region Время
	class Time {
		friend class DateTime;
		friend ostream& operator<<(ostream& out, const DateTime& dt);
		friend void Sleep(const Time& time);
	public:
#pragma region Конструкторы
		Time() : _milliseconds(0) {}
		Time(int hours, int minuts, int seconds, int milliseconds) {
			assign(hours, minuts, seconds, milliseconds);
		}
		Time(int milliseconds) {
			assign(milliseconds);
		}
		Time(const char* time) {
			assign(time);
		}
		Time(const std::string& time) {
			assign(time.c_str());
		}
		Time(const Time& time) : _milliseconds(time._milliseconds) {}
#pragma endregion
#pragma region Методы
		Time& assign(int hours, int minuts, int seconds, int milliseconds) {
			if (!isTimeable(hours, minuts, seconds, milliseconds)) {
				return *this;
			}
			_milliseconds = hours * 3600000 + minuts * 60000 + seconds * 1000 + milliseconds;
			return *this;
		}
		Time& assign(const char* time) {
			if (time == nullptr) {
				return *this;
			}
			int arr[4] = {};
			for (int timeCount = 0; timeCount < 4; timeCount++) {
				if (time[0] == '-' || time[0] == '\0') {
					break;
				}
				arr[timeCount] = atoi(time);
				for (; time[0] >= '0' && time[0] <= '9'; time++) {}
				time++;
			}
			return assign(arr[0], arr[1], arr[2], arr[3]);
		}
		Time& assign(const std::string& timeText) {
			return assign(timeText.c_str());
		}
		Time& assign(int milliseconds) {
			_milliseconds = milliseconds;
			round();
			return *this;
		}
		Time& assign(const Time& time) {
			_milliseconds = time._milliseconds;
			return *this;
		}
		int AllMilliseconds() const {
			return _milliseconds;
		}
		int milliseconds() const {
			return _milliseconds % 1000;
		}
		int seconds() const {
			return _milliseconds / 1000 % 60;
		}
		int minutes() const {
			return _milliseconds / 60000 % 60;
		}
		int hours() const {
			return _milliseconds / 3600000;
		}
		void setAllMillieconds(int milliseconds) {
			_milliseconds = milliseconds;
			round();
		}
		void setMilliseconds(int milliseconds) {
			if (milliseconds >= 1000 || milliseconds < 0) {
				return;
			}
			_milliseconds = _milliseconds / 1000 * 1000 + milliseconds;
			round();
		}
		void setSeconds(int seconds) {
			if (seconds >= 60 || seconds < 0) {
				return;
			}
			_milliseconds = _milliseconds / 60000 * 60000 + seconds * 1000 + _milliseconds % 1000;
			round();
		}
		void setMinutes(int minutes) {
			if (minutes >= 60 || minutes < 0) {
				return;
			}
			_milliseconds = _milliseconds / 3600000 * 3600000 + minutes * 60000 + _milliseconds % 60000;
			round();
		}
		void setHours(int hours) {
			if (hours >= 24 || hours < 0) {
				return;
			}
			_milliseconds = _milliseconds % 3600000 + hours * 3600000;
			round();
		}
		void MakeOpposite() {
			_milliseconds = 86400000 - _milliseconds;
			round();
		}
		friend Time Opposite(const Time& time) {
			return Time(86400000 - time._milliseconds);
		}
#pragma endregion
#pragma region Операторы
		Time& operator=(const Time& time) {
			_milliseconds = time._milliseconds;
			return *this;
		}
		friend ostream& operator<<(ostream& out, const Time& time) {
			int h, m, s, mi = time._milliseconds;
			toTime(h, m, s, mi);
			out << char(h / 10 + '0') << char(h % 10 + '0') << ':' << char(m / 10 + '0') << char(m % 10 + '0') << ':' << char(s / 10 + '0') << char(s % 10 + '0') << ':' << char(mi / 100 + '0') << char(mi / 10 % 10 + '0') << char(mi % 10 + '0');
			return out;
		}
		friend istream& operator>>(istream& in, Time& time) {
			string pr;
			getline(in, pr);
			time.assign(pr.c_str());
			return in;
		}
		operator std::string() {
			int h, m, s, mi = _milliseconds;
			toTime(h, m, s, mi);
			char pr[13] = { char(h / 10 + '0'), char(h % 10 + '0'), ':', char(m / 10 + '0'), char(m % 10 + '0'), ':', char(s / 10 + '0'), char(s % 10 + '0'), ':', char(mi / 100 % 10 + '0'), char(mi / 10 % 10 + '0'), char(mi % 10 + '0'), '\0', };
			return std::string(pr);
		}
#pragma region Булевые
		bool operator<(const Time& time) const {
			return _milliseconds < time._milliseconds;
		}
		bool operator>(const Time& time) const {
			return _milliseconds > time._milliseconds;
		}
		bool operator<=(const Time& time) const {
			return _milliseconds <= time._milliseconds;
		}
		bool operator>=(const Time& time) const {
			return _milliseconds >= time._milliseconds;
		}
		bool operator==(const Time& time) const {
			return _milliseconds == time._milliseconds;
		}
		bool operator!=(const Time& time) const {
			return _milliseconds != time._milliseconds;
		}
#pragma endregion
#pragma region Математические
		Time operator+(const Time& time) const {
			return Time(_milliseconds + time._milliseconds);
		}
		Time operator-(const Time& time) const {
			return Time(_milliseconds - time._milliseconds);
		}
		Time operator*(float value) const {
			return Time(static_cast<int>(_milliseconds * value));
		}
		Time operator/(float value) const {
			return Time(static_cast<int>(_milliseconds / value));
		}
		Time operator%(int value) const {
			return Time(_milliseconds % value);
		}

		Time& operator+=(const Time& time) {
			_milliseconds += time._milliseconds;
			round();
			return *this;
		}
		Time& operator-=(const Time& time) {
			_milliseconds -= time._milliseconds;
			round();
			return *this;
		}
		Time& operator*=(float value) {
			_milliseconds = static_cast<int>(_milliseconds * value);
			round();
			return *this;
		}
		Time& operator/=(float value) {
			_milliseconds = static_cast<int>(_milliseconds / value);
			round();
			return *this;
		}
		Time& operator%=(int value) {
			_milliseconds %= value;
			round();
			return *this;
		}
#pragma endregion
#pragma region Унарные
		Time& operator++() {
			_milliseconds++;
			round();
			return *this;
		}
		Time& operator--() {
			_milliseconds--;
			round();
			return *this;
		}
		Time operator++(int) {
			Time pr = *this;
			++(*this);
			return pr;
		}
		Time operator--(int) {
			Time pr = *this;
			--(*this);
			return pr;
		}
#pragma endregion
#pragma endregion
		static Time Now(float TimeZone = 0.f) {
			return Time((std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() + static_cast<long long>(TimeZone * 3600000)) % 86400000);
		}
		static Time MaxTime() {
			return Time(86399999);
		}
		static Time MinTime() {
			return Time(0);
		}
	private:
		int _milliseconds = 0;
		void round() {
			while (_milliseconds >= 86400000) {
				_milliseconds -= 86400000;
			}
			while (_milliseconds < 0.f) {
				_milliseconds += 86400000;
			}
		}
		static void toTime(int& hours, int& minuts, int& seconds, int& milliseconds) {//переводит миллисекунды во время
			hours = minuts = seconds = 0;
			hours = milliseconds / 3600000;
			milliseconds %= 3600000;
			minuts = milliseconds / 60000;
			milliseconds %= 60000;
			seconds = milliseconds / 1000;
			milliseconds %= 1000;
		}
		static bool isTimeable(int hours, int minuts, int seconds, int milliseconds) {
			return (hours >= 0 && hours < 24) && (minuts >= 0 && minuts < 60) && (seconds >= 0 && seconds < 60) && (milliseconds >= 0 && milliseconds < 1000);
		}
	};
	Time operator""_milliseconds(unsigned long long value) {
		return Time(static_cast<int>(value));
	}
	Time operator""_seconds(unsigned long long value) {
		return Time(static_cast<int>(value) * 1000);
	}
	Time operator""_minutes(unsigned long long value) {
		return Time(static_cast<int>(value) * 60000);
	}
	Time operator""_hours(unsigned long long value) {
		return Time(static_cast<int>(value) * 3600000);
	}
	Time operator""_time(const char* time, size_t len) {
		return Time(time);
	}
	void Sleep(long long milliseconds) {
		long long t = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() + milliseconds;
		while (t > std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) {}
	}
	void Sleep(const Time& time) {
		return Sleep(time._milliseconds);
	}
	Time Milliseconds(int count) {
		return Time(count);
	}
	Time Seconds(int count) {
		return Time(count*1000);
	}
	Time Minutes(int count) {
		return Time(count * 60000);
	}
	Time Hours(int count) {
		return Time(count * 3600000);
	}
#pragma endregion

#pragma region Дата
	bool leap(int year) {
		return (!(year % 4) && (year % 100) || !(year % 400));
	}
	class Date {
		friend class DateTime;
		friend ostream& operator<<(ostream& out, const DateTime& dt);
	public:
#pragma region Конструкторы
		Date() : _days(0) {}
		Date(const char* date) {
			assign(date);
		}
		Date(const string& date) {
			assign(date);
		}
		Date(int days) {
			assign(days);
		}
		Date(int day, int month, int year) {
			assign(day, month, year);
		}
		Date(const Date& date) {
			assign(date);
		}
#pragma endregion
#pragma region Методы
		Date& assign(int day, int month, int year) {
			if (!isDatable(abs(day), abs(month), abs(year))) {
				return *this;
			}
			_days = toDay(day, month, year);
			if (year > 1) {
				_days++;
			}
			return *this;
		}
		Date& assign(const char* date) {
			if (date == nullptr) {
				return *this;
			}
			int arr[3] = {};
			for (int DateCount = 0; DateCount < 3; DateCount++) {
				if (date[0] == '\0') {
					break;
				}
				arr[DateCount] = atoi(date);
				if (date[0] == '-') {
					date++;
				}
				for (; (date[0] >= '0' && date[0] <= '9'); date++) {}
				date++;
			}
			return assign(arr[0], arr[1], arr[2]);
		}
		Date& assign(const std::string& date) {
			return assign(date.c_str());
		}
		Date& assign(int days) {
			_days = days;
			return *this;
		}
		Date& assign(const Date& date) {
			_days = date._days;
			return *this;
		}
		bool isLeap() const {
			int d, m, g;
			toDate(d, m, g);
			return (!(g % 4) && (g % 100) || !(g % 400));
		}
		int AllDays() const {
			return _days;
		}
		int days() const {
			int d = _days, m = 0, g = 0;
			toDate(d, m, g);
			return d;
		}
		int months() const {
			int d = _days, m = 0, g = 0;
			toDate(d, m, g);
			return m;
		}
		int years() const {
			int d = _days, m = 0, g = 0;
			toDate(d, m, g);
			return g;
		}
		int weeks() const {
			return _days / 7;
		}
		int dayWeek() const {
			return (_days % 7) + 1;
		}
		void setAllDays(int days) {
			_days = days;
		}
		void setDay(int day) {
			int d = _days, m, g;
			toDate(d, m, g);
			assign(day, m, g);
		}
		void setMonth(int month) {
			int d = _days, m, g;
			toDate(d, m, g);
			assign(d, month, g);
		}
		void setYear(int year) {
			int d = _days, m, g;
			toDate(d, m, g);
			assign(d, m, year);
		}
		void setWeeks(int weeks) {
			_days = weeks * 7 - 1;
		}
		void MakeOpposite() {
			_days = (_days + 1) * -1;
		}
		friend Date Opposite(const Date& date) {
			return Date((date._days + 1) * -1);
		}
#pragma endregion
#pragma region Операторы
		Date& operator=(const Date& date) {
			_days = date._days;
			return *this;
		}
#pragma region Булевые
		bool operator<(const Date& date) const {
			return _days < date._days;
		}
		bool operator>(const Date& date) const {
			return _days > date._days;
		}
		bool operator<=(const Date& date) const {
			return _days <= date._days;
		}
		bool operator>=(const Date& date) const {
			return _days >= date._days;
		}
		bool operator==(const Date& date) const {
			return _days == date._days;
		}
		bool operator!=(const Date& date) const {
			return _days != date._days;
		}
#pragma endregion
#pragma region Математические
		Date operator+(const Date& date) const {
			return Date(_days + date._days);
		}
		Date operator-(const Date& date) const {
			return Date(_days - date._days);
		}
		Date operator*(int value) const {
			return Date(_days * value);
		}
		Date operator/(int value) const {
			return Date(_days / value);
		}
		Date operator%(int value) const {
			return Date(_days % value);
		}

		Date& operator+=(const Date& date) {
			_days += date._days;
			return *this;
		}
		Date& operator-=(const Date& date) {
			_days -= date._days;
			return *this;
		}
		Date& operator*=(int value) {
			_days *= value;
			return *this;
		}
		Date& operator/=(int value) {
			_days /= value;
			return *this;
		}
		Date& operator%=(int value) {
			_days %= value;
			return *this;
		}
#pragma endregion
#pragma region Унарные
		Date operator++() {
			_days++;
			return *this;
		}
		Date operator--() {
			_days--;
			return *this;
		}
		Date operator++(int) {
			Date pr = *this;
			++(*this);
			return pr;
		}
		Date operator--(int) {
			Date pr = *this;
			--(*this);
			return pr;
		}
#pragma endregion
		friend ostream& operator<<(ostream& out, const Date& date) {
			int d = date._days, m = 0, g = 0, zn = 1;
			if (d < 0) {
				out << '-';
			}
			toDate(d, m, g);
			d = abs(d);
			m = abs(m);
			g = abs(g);
			out << d / 10 << d % 10 << ':' << m / 10 << m % 10 << ':' << g;
			return out;
		}
		friend istream& operator>>(istream& in, Date& date) {
			string pr;
			getline(in, pr);
			date.assign(pr);
			return in;
		}
		operator std::string() {
			string result = "";
			int d = _days, m, g;
			if (d < 0) {
				result = '-';
			}
			toDate(d, m, g);
			d = abs(d);
			m = abs(m);
			g = abs(g);
			char pr[7] = { char(d / 10 + '0'), char(d % 10 + '0'), ':',char(m / 10 + '0'), char(m % 10 + '0'), ':', '\0' };
			result += pr;
			return result + to_string(g);

		}
#pragma endregion
		static Date Now(float TimeZone = 0.f) {
			return Date(static_cast<int>((std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() + static_cast<long long>(TimeZone * 3600000)) / 86400000) + 719162);
		}
		static Date MaxDate() {
			return Date(2147483646);
		}
		static Date MinDate() {
			return Date(-2147483647);
		}
	private:
		int _days = 0;
		static void toDate(int& d, int& m, int& g) {//переводит день d в дату d:m:g
			bool lowerZero = false;
			if (d < 0) {
				lowerZero = true;
				d *= -1;
				d--;
			}
			d++;
			g = 1;
			bool leapYear = (!(g % 4) && (g % 100) || !(g % 400));
			while (d > (leapYear ? 366 : 365)) {
				d -= (leapYear ? 366 : 365);
				g++;
				leapYear = (!(g % 4) && (g % 100) || !(g % 400));
			}
			m = 0;
			const int* arr = leapYear ? sp_mes_v : sp_mes;
			while (d > arr[m]) {
				d -= arr[m++];
			}
			m++;
			if (lowerZero) {
				d *= -1;
			}
		}
		static int toDay(int d, int m, int g) {//дата переводится в кол-во дней, прошедших с 01.01.1 по выведенной формуле: (d + (дни в месяце) + g * 365 + (g - 1) / 4 - g / 100 + g / 400 - 365)
			bool lowerZero = false;
			if (d < 0 || m < 0 || g < 0) {
				d = abs(d);
				m = abs(m);
				g = abs(g);
				lowerZero = true;
			}
			int result = static_cast<int>(d + ((((g % 4 == 0) && (g % 100 != 0)) || (g % 400 == 0)) ? (sum_mes_v[m]) : (sum_mes[m])) + g * 365.2425 - 365 + ((g == 1) ? (0) : (-1)));
			if (lowerZero) {
				result = (result + 1) * -1;
			}
			return result-1;
		}
		static bool isDatable(int d, int m, int g) {
			if (m > 12 || m < 1 || g < 1) {
				return false;
			}
			if (!leap(g)) {
				if (d < 1 || d > sp_mes[m-1]) {
					return false;
				}
			}
			else if (d < 1 || d > sp_mes_v[m-1]) {
				return false;
			}
			return true;
		}
		static const int sp_mes[12];
		static const int sp_mes_v[12];
		static const int sum_mes[14];
		static const int sum_mes_v[14];
	};
	const int Date::sp_mes[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	const int Date::sp_mes_v[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	const int Date::sum_mes[14] = { 0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
	const int Date::sum_mes_v[14] = { 0, 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 };
	Date operator""_days(unsigned long long days) {
		return Date(static_cast<int>(days));
	}
	Date operator""_day(unsigned long long days) {
		return Date(static_cast<int>(days), 1, 1);
	}
	Date operator""_month(unsigned long long months) {
		int years = static_cast<int>((months - 1) / 12 + 1);
		months -= (years - 1) * 12;
		return Date(1, static_cast<int>(months), years);
	}
	Date operator""_year(unsigned long long years) {
		return Date(1, 1, static_cast<int>(years));
	}
	Date operator""_date(const char* date, size_t len) {
		return Date(date);
	}
	Date Days(int count) {
		return Date(count);
	}
#pragma endregion

#pragma region Дата время
	class DateTime {
		friend ostream& operator<<(ostream& out, const DateTime& dt);
		friend class Time;
		friend class Date;
		friend void Sleep(const DateTime& dt);
	public:
#pragma region Конструкторы
		DateTime() : _milliseconds(0), _days(0) {}
		DateTime(int day, int month, int year, int hour, int minut, int second, int millisecond = 0) {
			assign(day, month, year, hour, minut, second, millisecond);
		}
		DateTime(const char* dt) {
			assign(dt);
		}
		DateTime(const std::string& dt) {
			assign(dt.c_str());
		}
		DateTime(int day, int month, int year) {
			assign(day, month, year, 0, 0, 0, 0);
		}
		DateTime(int hour, int minut, int seconds, int millisecond) {
			assign(1, 1, 1, hour, minut, seconds, millisecond);
		}
		DateTime(int days, int milliseconds) : _days(days), _milliseconds(milliseconds) {
			round();
		}
		DateTime(int days) : _days(days), _milliseconds(0) {}
		DateTime(const Time& time) : _milliseconds(time._milliseconds), _days(0) {}
		DateTime(const Date& date) : _milliseconds(0), _days(date._days) {}
		DateTime(const DateTime& dt) : _days(dt._days), _milliseconds(dt._milliseconds) {}
#pragma endregion
#pragma region Методы
		DateTime& assign(int day, int month, int year, int hour, int minut, int second, int millisecond = 0) {
			if (Date::isDatable(abs(day), abs(month), abs(year)) && Time::isTimeable(hour, minut, second, millisecond)) {
				_days = Date::toDay(day, month, year);
				if (year > 1) {
					_days++;
				}
				_milliseconds = hour * 3600000 + minut * 60000 + second * 1000 + millisecond;
				round();
			}
			return *this;
		}
		DateTime& assign(const char* dt) {
			if (dt == nullptr) {
				return *this;
			}
			int arr[7] = {};
			int end = 7;
			for (size_t timeCount = 0; timeCount < 7; timeCount++) {
				if (dt[0] == '\0') {
					end = static_cast<int>(timeCount);
					break;
				}
				arr[timeCount] = atoi(dt);
				if (dt[0] == '-') {
					dt++;
				}
				for (; dt[0] >= '0' && dt[0] <= '9'; dt++) {}
				dt++;
			}
			if (end < 3 || end == 4) {
				return assign(0, 0, 0, arr[0], arr[1], arr[2], arr[3]);
			}
			else if (end == 3) {
				if (Date::isDatable(arr[0], arr[1], arr[2])) {
					return assign(arr[0], arr[1], arr[2], 0, 0, 0, 0);
				}
				else {
					return assign(0, 0, 0, arr[0], arr[1], arr[2], 0);
				}
			}
			else {
				return assign(arr[0], arr[1], arr[2], arr[3], arr[4], arr[5], arr[6]);
			}
		}
		DateTime& assign(const std::string& dt) {
			return assign(dt.c_str());
		}
		DateTime& assign(int day, int month, int year) {
			return assign(day, month, year, 0, 0, 0, 0);
		}
		DateTime& assign(int hour, int minut, int seconds, int millisecond) {
			return assign(1, 1, 1, hour, minut, seconds, millisecond);
		}
		DateTime& assign(int days, int milliseconds) {
			_days = days;
			_milliseconds = milliseconds;
			round();
			return *this;
		}
		DateTime& assign(int days) {
			_days = days;
			_milliseconds = 0;
			return *this;
		}
		DateTime& assign(const Time& time) {
			_milliseconds = time._milliseconds;
			_days = 0;
			return *this;
		}
		DateTime& assign(const Date& date) {
			_days = date._days;
			_milliseconds = 0;
			return *this;
		}
		DateTime& assign(const DateTime& dt) {
			_days = dt._days;
			_milliseconds = dt._milliseconds;
			return *this;
		}
		int AllMilliseconds() const {
			return _milliseconds;
		}
		int milliseconds() const {
			return _milliseconds % 1000;
		}
		int seconds() const {
			return _milliseconds / 1000 % 60;
		}
		int minutes() const {
			return _milliseconds / 60000 % 60;
		}
		int hours() const {
			return _milliseconds / 3600000;
		}
		void setAllMillieconds(int milliseconds) {
			_milliseconds = milliseconds;
			round();
		}
		void setMilliseconds(int milliseconds) {
			if (milliseconds >= 1000 || milliseconds < 0) {
				return;
			}
			_milliseconds = _milliseconds / 1000 * 1000 + milliseconds;
		}
		void setSeconds(int seconds) {
			if (seconds >= 60 || seconds < 0) {
				return;
			}
			_milliseconds = _milliseconds / 60000 * 60000 + seconds * 1000 + _milliseconds % 1000;
			round();
		}
		void setMinutes(int minutes) {
			if (minutes >= 60 || minutes < 0) {
				return;
			}
			_milliseconds = _milliseconds / 3600000 * 3600000 + minutes * 60000 + _milliseconds % 60000;
			round();
		}
		void setHours(int hours) {
			if (hours >= 24 || hours < 0) {
				return;
			}
			_milliseconds = _milliseconds % 3600000 + hours * 3600000;
		}
		bool isLeap() const {
			int d, m, g;
			Date::toDate(d, m, g);
			return (!(g % 4) && (g % 100) || !(g % 400));
		}
		int AllDays() const {
			return _days;
		}
		int days() const {
			int d = _days, m = 0, g = 0;
			Date::toDate(d, m, g);
			return d;
		}
		int months() const {
			int d = _days, m = 0, g = 0;
			Date::toDate(d, m, g);
			return m;
		}
		int years() const {
			int d = _days, m = 0, g = 0;
			Date::toDate(d, m, g);
			return g;
		}
		int weeks() const {
			return _days / 7;
		}
		int dayWeek() const {
			return (_days % 7) + 1;
		}
		void setAllDays(int days) {
			_days = days;
		}
		void setDay(int day) {
			int d = _days, m, g;
			Date::toDate(d, m, g);
			assign(Date::toDay(day, m, g) + 1, _milliseconds);
		}
		void setMonth(int month) {
			int d = _days, m, g;
			Date::toDate(d, m, g);
			assign(Date::toDay(d, month, g) + 1, _milliseconds);
		}
		void setYear(int year) {
			int d = _days, m, g;
			Date::toDate(d, m, g);
			assign(Date::toDay(d, m, year) + 1, _milliseconds);
		}
		void setWeeks(int weeks) {
			_days = weeks * 7 - 1;
		}
		void setTime(const Time& time) {
			_milliseconds = time._milliseconds;
		}
		void setDate(const Date& time) {
			_days = time._days;
		}
		void MakeOpposite() {
			_days = (_days + 1) * -1;
			_milliseconds = 86400000 - _milliseconds;
		}
		void MakeOppositeDate() {
			_days = (_days + 1) * -1;
		}
		void MaleOppositeTime() {
			_milliseconds = 86400000 - _milliseconds;
		}
		friend DateTime Opposite(const DateTime& dt) {
			return DateTime((dt._days + 1) * -1, 86400000 - dt._milliseconds);
		}
		friend DateTime OppositeDate(const DateTime& dt) {
			return DateTime((dt._days + 1) * -1, dt._milliseconds);
		}
		friend DateTime OppositeTime(const DateTime& dt) {
			return DateTime(dt._days, 86400000 - dt._milliseconds);
		}
#pragma endregion
#pragma region Операторы
		DateTime& operator=(const DateTime& dt) {
			_days = dt._days;
			_milliseconds = dt._milliseconds;
			return *this;
		}
		friend ostream& operator<<(ostream& out, const DateTime& dt) {
			int d1 = dt._days, m1, g1;
			if (d1 < 0) {
				out << '-';
			}
			Date::toDate(d1, m1, g1);
			d1 = abs(d1);
			m1 = abs(m1);
			g1 = abs(g1);
			out << d1 / 10 << d1 % 10 << ':' << m1 / 10 << m1 % 10 << ':' << g1;
			out << '-';
			int h2, m2, s2, mi2 = dt._milliseconds;
			Time::toTime(h2, m2, s2, mi2);
			out << h2 / 10 << h2 % 10 << ':' << m2 / 10 << m2 % 10 << ':' << s2 / 10 << s2 % 10 << ':' << mi2 / 100 << mi2 / 10 % 10 << mi2 % 10;
			return out;
		}
		friend istream& operator>>(istream& in, DateTime& dt) {
			string pr;
			getline(in, pr);
			dt.assign(pr.c_str());
			return in;
		}
		operator std::string() const {
			string result;
			int d1 = _days, m1, g1;
			if (d1 < 0) {
				result += '-';
			}
			Date::toDate(d1, m1, g1);
			d1 = abs(d1);
			m1 = abs(m1);
			g1 = abs(g1);
			char pr1[] = { static_cast<char>(d1 / 10 + '0'), static_cast<char>(d1 % 10 + '0'), ':', static_cast<char>(m1 / 10 + '0'), static_cast<char>(m1 % 10 + '0'), ':', '\0' };
			result += pr1;
			result += to_string(g1) + '-';
			int h2, m2, s2, mi2 = _milliseconds;
			Time::toTime(h2, m2, s2, mi2);
			char pr2[] = { static_cast<char>(h2 / 10 + '0'), static_cast<char>(h2 % 10 + '0'), ':' , static_cast<char>(m2 / 10 + '0'), static_cast<char>(m2 % 10 + '0'), ':' , static_cast<char>(s2 / 10 + '0'), static_cast<char>(s2 % 10 + '0'), ':' , static_cast<char>(mi2 / 100 + '0'), static_cast<char>(mi2 / 10 % 10 + '0'), static_cast<char>(mi2 % 10 + '0'), '\0' };
			result += pr2;
			return result;
		}
		operator Time() const {
			return Time(_milliseconds);
		}
		operator Date() const {
			return Date(_days);
		}
#pragma region Булевые
		bool operator<(const DateTime& dt) const {
			if (_days < dt._days) {
				return true;
			}
			if (_days == dt._days) {
				if (_days < 0) {
					return _milliseconds > dt._milliseconds;
				}
				else {
					return _milliseconds < dt._milliseconds;
				}
			}
			else {
				return false;
			}
		}
		bool operator>(const DateTime& dt) const {
			if (_days > dt._days) {
				return true;
			}
			if (_days == dt._days) {
				if (_days < 0) {
					return _milliseconds < dt._milliseconds;
				}
				else {
					return _milliseconds > dt._milliseconds;
				}
			}
			else {
				return false;
			}
		}
		bool operator==(const DateTime& dt) const {
			return _days == dt._days && _milliseconds == dt._milliseconds;
		}
		bool operator!=(const DateTime& dt) const {
			return !(_days == dt._days && _milliseconds == dt._milliseconds);
		}
		bool operator<=(const DateTime& dt) const {
			return !(*this > dt);
		}
		bool operator>=(const DateTime& dt) const {
			return !(*this < dt);
		}
#pragma endregion
#pragma region Математические
		DateTime operator+(const DateTime& dt) const {
			return DateTime(_days + dt._days, _milliseconds + ((_days >= 0) ? (dt._milliseconds) : (-dt._milliseconds)));
		}
		DateTime operator-(const DateTime& dt) const {
			return DateTime(_days - dt._days, _milliseconds - ((_days >= 0) ? (dt._milliseconds) : (-dt._milliseconds)));
		}
		DateTime operator*(float value) const {
			return DateTime(static_cast<int>(_days * value), static_cast<int>(_milliseconds * ((_days >= 0) ? (value) : (-value))));
		}
		DateTime operator/(float value) const {
			return DateTime(static_cast<int>(_days / value), static_cast<int>(_milliseconds / ((_days >= 0) ? (value) : (-value))));
		}
		DateTime operator%(int value) const {
			return DateTime(_days % value, _milliseconds % ((_days >= 0) ? (value) : (-value)));
		}

		DateTime& operator+=(const DateTime& dt) {
			return assign(_days + dt._days, _milliseconds + ((_days >= 0) ? (dt._milliseconds) : (-dt._milliseconds)));
		}
		DateTime& operator-=(const DateTime& dt) {
			return assign(_days - dt._days, _milliseconds - ((_days >= 0) ? (dt._milliseconds) : (-dt._milliseconds)));
		}
		DateTime& operator*=(float value) {
			return assign(static_cast<int>(_days * value), static_cast<int>(_milliseconds * ((_days >= 0) ? (value) : (-value))));
		}
		DateTime& operator/=(float value) {
			return assign(static_cast<int>(_days / value), static_cast<int>(_milliseconds / ((_days >= 0) ? (value) : (-value))));
		}
		DateTime& operator%=(int value) {
			return assign(_days % value, _milliseconds % ((_days >= 0) ? (value) : (-value)));
		}
#pragma endregion
#pragma endregion
		static DateTime Now(float TimeZone = 0.f) {
			long long milliseconds = (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() + static_cast<long long>(TimeZone * 3600000));
			return DateTime(static_cast<int>(milliseconds / 86400000 + 719162), static_cast<int>(milliseconds % 86400000));
		}
		static DateTime MaxDateTime() {
			return DateTime(2147483646, 86399999);
		}
		static DateTime MinDateTime() {
			return DateTime(-2147483647, 86399999);
		}
	private:
		int _milliseconds = 0;
		int _days = 0;
		void round() {
			while (_milliseconds >= 86400000) {
				_milliseconds -= 86400000;
				if (_days >= 0) {
					_days++;
				}
				else {
					_days--;
				}
			}
			while (_milliseconds < 0) {
				_milliseconds += 86400000;
				if (_days >= 0) {
					_days--;
				}
				else {
					_days++;
				}
			}
		}
	};
	using TimeDate = DateTime;
	DateTime operator""_dt(const char* dt, size_t len) {
		return DateTime(dt);
	}
	void Sleep(const DateTime& dt) {
		Sleep(dt._milliseconds);
	}
#pragma endregion
}
#endif