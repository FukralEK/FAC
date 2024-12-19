#pragma once
#include <exception>

namespace FAC
{
	enum ExceptionType
	{
		INVALID_VERSION,
		WRONG_HEADER,
		NOT_EXISTING_FILE,
		PATH_IS_DIRECTORY,
		PATH_IS_FILE,
		BINARY_NUMBER_SIZE_NOT_MATCH,
		FILE_NOT_FOUND,
		NO_PERMISSION,
		WRONG_INDEX,
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