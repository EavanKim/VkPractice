#pragma once

#include <iostream>
#include <fstream>
#include <math.h>
#include <algorithm>
#include <stdexcept>
#include <optional>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <utility>
#include <set>
#include <array>
#include <direct.h>
#include <cstdint>

#include "Graphic.h"

namespace Command
{
	class GRAPHIC_API Graphic
	{
	public:
		static void run()
		{
			initVulkan();
			mainLoop();
			cleanup();
		}

	private:
		static void initVulkan()
		{
			Graphic_Vulkan::GetInstance()->initWindow();
			Graphic_Vulkan::GetInstance()->initVulkan();
		}

		static void mainLoop()
		{
			Graphic_Vulkan::GetInstance()->mainLoop();
		}

		static void cleanup()
		{
			Graphic_Vulkan::GetInstance()->cleanup();
		}
	};
}
