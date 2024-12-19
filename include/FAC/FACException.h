#pragma once
#include <exception>

namespace FAC
{
	enum ExceptionType
	{
		UNKNOWN,
	};
	class Exception : public std::exception
	{
	public:
		inline Exception(ExceptionType type, const std::string& message)
			: type(type), errorMessage(message) {
		}

		inline const char* what() const noexcept override {
			return errorMessage.c_str();
		}

		inline ExceptionType getExceptionType() const {
			return type;
		}


	private:
		ExceptionType type;
		std::string errorMessage;
	};
}