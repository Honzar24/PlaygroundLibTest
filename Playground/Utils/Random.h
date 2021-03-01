#ifndef MY_RANDOM_H
#define MY_RANDOM_H

#include <type_traits>
#include <random>



namespace MyUtils
{
	/// <summary>
	/// https://stackoverflow.com/questions/19036141/vary-range-of-uniform-int-distribution
	/// </summary>
	template <typename T>
	class Random 
	{
	public:
		Random() = default;
		Random(std::mt19937::result_type seed) : 
			eng(seed) 
		{}

		T Generate(T min, T max);
		
		std::string CreateRandomString(int len, bool withNewLines = false);

	private:
		std::mt19937 eng{ std::random_device{}() };
	};

	template <typename T>
	T Random<T>::Generate(T min, T max)
	{
		if constexpr (std::is_floating_point<T>::value)
		{
			return (T)(std::uniform_real_distribution<T>{min, max}(eng));
		}
		else
		{
			return (T)(std::uniform_int_distribution<T>{min, max}(eng));
		}
	};

	/// <summary>
	/// Creates random string 
	/// No matter if T is int or float type
	/// </summary>
	/// <param name="len"></param>
	/// <returns></returns>
	template <typename T>
	std::string Random<T>::CreateRandomString(int len, bool withNewLines)
	{
		static const char alphanum[] =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz\n";

		std::string r = "";
		for (int i = 0; i < len; ++i)
		{
			int index = static_cast<int>(this->Generate(0, (sizeof(alphanum) - 1 - !withNewLines)));
			r += alphanum[index];
		}

		return r;
	};

}

#endif
