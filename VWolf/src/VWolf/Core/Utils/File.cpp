#include "vwpch.h"
#include "File.h"

namespace VWolf {
	std::string File::OpenTextFile(const char* filename)
	{
		std::ifstream ifs(filename);
		VWOLF_CORE_ASSERT(ifs.is_open());
		std::stringstream ss;
		
		while (ifs.good()) {
			ss << (char)ifs.get();
		}

		ifs.close();
		return ss.str().substr(0, ss.str().length() - 1);
	}
}