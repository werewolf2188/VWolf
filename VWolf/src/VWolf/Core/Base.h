#pragma once

namespace VWolf {

	struct InitConfiguration {
		int width;
		int height;
		const char* title;
	};

	enum class DriverType {
		DirectX12,
		OpenGL
	};
}