#pragma once

#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <chrono>
#include <glm/glm.hpp>

#include "Client/Console.h"

namespace Esteem
{
	class Debug
	{
	private:
		static std::mutex outLock;

	public:
	#pragma region High Resolution Timer
		static void StartHighResolutionTimer()
		{
			startHighResolutionClock = std::chrono::high_resolution_clock::now();
		}

		template<typename T, typename P>
		static T StopAndGetHighResolutionTimer()
		{
			auto endHighResolutionClock = std::chrono::high_resolution_clock::now();
			T output = std::chrono::duration<T, P>(endHighResolutionClock - startHighResolutionClock);

			return output;
		}

		template<typename T, typename P>
		static void StopAndLogHighResolutionTimer(T divideBy = 1)
		{
			auto endHighResolutionClock = std::chrono::high_resolution_clock::now();
			T output = std::chrono::duration<T, P>(endHighResolutionClock - startHighResolutionClock);

			return output / divideBy;
		}
	#pragma endregion

		inline static void Log(const std::string& output)
		{			
			std::lock_guard lk(outLock);

			std::cout << output << std::endl;
			Console::WriteLine(output);
		}

		template <typename... Ts>
		inline static void Log(const Ts&... output)
		{
			std::lock_guard lk(outLock);

			(std::cout << ... << output) << "\n";
			Console::WriteLine(output...);
			
		}

		template <typename... Ts>
		inline static void LogError(const Ts&... output)
		{
			Log("ERROR: ", output...);
		}

		template <typename... Ts>
		inline static void LogWarning(const Ts&... output)
		{
			Log("WARNING: ", output...);
		}

		static void Log(const glm::mat4& from, bool transpose = false)
		{
			std::lock_guard lk(outLock);

			std::cout << "Debug >> drawing matrix: " << std::endl;
			for(int i = 0; i < 4; i++)
			{
				for(int j = 0; j < 4; j++)
				{
					std::string out(std::to_string(from[transpose ? j : i][transpose ? i : j]));
					int extraSpaces = 12 - (int)out.size();
					for(int s = 0; s < extraSpaces; s++)
					{
						out += " ";
					}
					std::cout << out;
				}
				std::cout << std::endl;
			}
		}

		static void Log(const glm::mat3& matrix, bool transpose = false)
		{
			std::cout << std::fixed;

			std::lock_guard lk(outLock);
			if (!transpose)
			{
				std::cout << "Matrix3x3("
					<< "\n\t" << std::setw(9) << matrix[0][0] << ' ' << std::setw(9) << matrix[0][1] << ' ' << std::setw(9) << matrix[0][2]
					<< "\n\t" << std::setw(9) << matrix[1][0] << ' ' << std::setw(9) << matrix[1][1] << ' ' << std::setw(9) << matrix[1][2]
					<< "\n\t" << std::setw(9) << matrix[2][0] << ' ' << std::setw(9) << matrix[2][1] << ' ' << std::setw(9) << matrix[2][2]
					<< ")\n";
			}
			else
			{
				std::cout << "Matrix3x3("
					<< "\n\t" << matrix[0][0] << ' ' << matrix[1][0] << ' ' << matrix[2][0]
					<< "\n\t" << matrix[0][1] << ' ' << matrix[1][1] << ' ' << matrix[2][1]
					<< "\n\t" << matrix[0][2] << ' ' << matrix[1][2] << ' ' << matrix[2][2]
					<< ")\n";
			}
		}

	private:
		static std::chrono::high_resolution_clock::time_point startHighResolutionClock;
	};
}