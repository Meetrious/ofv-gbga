#pragma once
#include <exception>

namespace BGA {

	class MyException : public std::exception {
	public:
		virtual const char* what()const noexcept final {
			return "Some local shenanigan happened:";
		}

		virtual const char* what_exactly() const noexcept = 0;

	};

	class external_file_allocation_error : public MyException {
	public:
		virtual const char* what_exactly() const noexcept final {
			return "Failed to allocate external file to the stream.";
		}

	};
	
}
