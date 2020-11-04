#include "GV.h"

Graphic_Vulkan* Graphic_Vulkan::m_Instance = nullptr;

Graphic_Vulkan* Graphic_Vulkan::GetInstance()
{
	if (nullptr == m_Instance)
		m_Instance = new Graphic_Vulkan();

	return m_Instance;
}


//Default
void Graphic_Vulkan::initWindow()
{
	///
	//default
	glfwInit();
	///

	///
	//Instance
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_Window = glfwCreateWindow(m_WindowWidth, m_WindowHeight, "EW_GV", nullptr, nullptr);
	///

	///
	//Swap Chain Recreation
	glfwSetFramebufferSizeCallback(m_Window, framebufferResizeCallback);
	///
}

void Graphic_Vulkan::initVulkan()
{
	///
	//Default
	createInstance();
	///

	///
	//Validation Layer
	setupDebugMessenger();
	///

	///
	//Window Surface
	createSurface();
	///

	///
	//Physical Device
	pickPhysicalDevice();
	///

	///
	//Logical Device
	createLogicalDevice();
	///

	///
	//Swap Chain
	createSwapChain();
	///

	///
	//Image Views
	createImageViews();
	///

	///
	//Render Passes
	createRenderPass();
	///

	///
	//Uniform Buffer
	createDescriptorSetLayout();
	///

	///
	//Graphics Pipeline
	createGraphicsPipeline();
	///


	///
	//Command Buffer
	createCommandPool();

		///

		//Depth bufferings
		createDepthResource();
		///


		///
		//Framebuffer
		createFramebuffers();
		///

		///
		//image
		createTextureImage();
		///

		///
		//ImageView
		createTextureImageView();
		createTextureSampler();
		///

		///
		//Vertex Buffer Creation
		createVertexBuffer();
		///

		///
		//Index Buffer
		createIndexBuffer();
		///

		///
		//uniform Buffer
		createUniformBuffers();

		createDescriptorPool();
		createDescriptorSets();
		///

	createCommandBuffers();
	///

	///
	//Draw
	createSyncObjects();
	///
}

void Graphic_Vulkan::mainLoop()
{
	while (!glfwWindowShouldClose(m_Window))
	{
		glfwPollEvents();
		DrawFrame();
	}
}

void Graphic_Vulkan::cleanup_old()
{
	///
	//Draw
	for (size_t Count = 0; MAX_FRAMES_IN_FLIGHT > Count; ++Count)
	{
		vkDestroySemaphore(m_Device, m_RenderFinishedSemaphores[Count], nullptr);
		vkDestroySemaphore(m_Device, m_ImageAvailableSemaphores[Count], nullptr);
		vkDestroyFence(m_Device, m_InfFlightFence[Count], nullptr);
	}
	///

	///
	//Framebuffers
	for (VkImage framebuffer : m_SwapChainFramebuffers)
		vkDestroyFramebuffer(m_Device, framebuffer, nullptr);
	///

	///
	//Conclusion
	vkDestroyPipeline(m_Device, m_GraphicsPipeline, nullptr);
	///

	///
	//pipeline
	vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);
	///

	///
	//Render Passes
	vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);
	///

	///
	//Image Views
	for (VkImage imageView : m_SwapChainImageViews)
		vkDestroyImageView(m_Device, imageView, nullptr);
	///

	///
	//Swap Chain
	vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);
	///

	///
	//Command Pool
	vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
	///

	///
	//Logical Device
	vkDestroyDevice(m_Device, nullptr);
	///

	///
	//Validation Layer
	if (enableValidationLayers) DestroyDebugUtilsMessengerEXT(m_Vk, nullptr, m_debugMessenger);
	///


	///
	//Window Surface
	vkDestroySurfaceKHR(m_Vk, m_Surface, nullptr);
	///

	///
	//Instance
	vkDestroyInstance(m_Vk, nullptr);
	///

	///
	//Base
	glfwDestroyWindow(m_Window);

	glfwTerminate();
	///
}

///
//Swap Chain recreation
void Graphic_Vulkan::cleanup()
{
	cleanupSwapChain();

	///
	//ImageView
	if (VK_NULL_HANDLE != m_TextureImageView)
	{
		vkDestroySampler(m_Device, m_TextureSampler, nullptr);
		vkDestroyImageView(m_Device, m_TextureImageView, nullptr);
	}
	///

	///
	//Image
	if (VK_NULL_HANDLE != m_TextureImage)
	{
		vkDestroyImage(m_Device, m_TextureImage, nullptr);
		vkFreeMemory(m_Device, m_TextureImageMemory, nullptr);
	}
	///

	///
	//uniform buffer
	if (VK_NULL_HANDLE != descriptorSetLayout)
	{
		vkDestroyDescriptorSetLayout(m_Device, descriptorSetLayout, nullptr);
	}
	///

	///
	//Index Buffer
	if (VK_NULL_HANDLE != m_IndexBuffer)
	{
		vkDestroyBuffer(m_Device, m_IndexBuffer, nullptr);
		vkFreeMemory(m_Device, m_IndexBufferMemory, nullptr);
	}
	///

	///
	//Vertex buffer creation
	if (VK_NULL_HANDLE != m_VertexBuffer)
	{
		vkDestroyBuffer(m_Device, m_VertexBuffer, nullptr);
		vkFreeMemory(m_Device, m_VertexBufferMemory, nullptr);
	}
	///

	///
	//Draw
	if (0 != m_RenderFinishedSemaphores.size())
	{
		for (size_t Count = 0; MAX_FRAMES_IN_FLIGHT > Count; ++Count)
		{
			vkDestroySemaphore(m_Device, m_RenderFinishedSemaphores[Count], nullptr);
			vkDestroySemaphore(m_Device, m_ImageAvailableSemaphores[Count], nullptr);
			vkDestroyFence(m_Device, m_InfFlightFence[Count], nullptr);
		}
	}
	///

	///
	//Command Pool
	if (VK_NULL_HANDLE != m_CommandPool)
	{
		vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
	}
	///

	///
	//Logical Device
	if (VK_NULL_HANDLE != m_Device)
	{
		vkDestroyDevice(m_Device, nullptr);
	}
	///

	///
	//Validation Layer
	if (enableValidationLayers) DestroyDebugUtilsMessengerEXT(m_Vk, nullptr, m_debugMessenger);
	///


	///
	//Window Surface
	if (VK_NULL_HANDLE != m_Surface)
	{
		vkDestroySurfaceKHR(m_Vk, m_Surface, nullptr);
	}
	///

	///
	//Instance
	if (VK_NULL_HANDLE != m_Vk)
	{
		vkDestroyInstance(m_Vk, nullptr);
	}
	///

	///
	//Base
	if (VK_NULL_HANDLE != m_Window)
	{
		glfwDestroyWindow(m_Window);

		glfwTerminate();
	}///
}
///

//Instance
void Graphic_Vulkan::createInstance()
{
	///
	//Validation Layers
	if (enableValidationLayers && !checkValidationLayerSupport()) {
		throw std::runtime_error("validation layers requested, but not available!");
	}
	///

	///
	//Window Infomation
	VkApplicationInfo VkInfo = {};
	VkInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	VkInfo.pApplicationName = "EW_GV";
	VkInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
	VkInfo.pEngineName = "Eavan's Work";
	VkInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
	VkInfo.apiVersion = VK_API_VERSION_1_0;
	///


	///
	//Vulkan Infomation
	VkInstanceCreateInfo VkInstanceInfo = {};
	VkInstanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	VkInstanceInfo.pApplicationInfo = &VkInfo;
	///

	///
	//Validation Layers
	std::vector<const char*> extensions = getRequiredExtensions();
	VkInstanceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	VkInstanceInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugInfo;
	if (enableValidationLayers)
	{
		VkInstanceInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		VkInstanceInfo.ppEnabledLayerNames = validationLayers.data();

		populateDebugMessengerCreateInfo(debugInfo);
		VkInstanceInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugInfo;
	}
	else
	{
		VkInstanceInfo.enabledLayerCount = 0;

		VkInstanceInfo.pNext = nullptr;
	}
	///

	///
	//Create vk
	if (vkCreateInstance(&VkInstanceInfo, nullptr, &m_Vk) != VK_SUCCESS)
		throw std::runtime_error("failed to create instance!");
	///
}

///
//Validation Layers
bool Graphic_Vulkan::checkValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const VkLayerProperties& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

std::vector<const char*> Graphic_Vulkan::getRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

VKAPI_ATTR VkBool32 VKAPI_CALL Graphic_Vulkan::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT _messageSeverity, VkDebugUtilsMessageTypeFlagsEXT _messageType, const VkDebugUtilsMessengerCallbackDataEXT* _pCallbackData, void* _pUserData)
{
	std::cerr << "validation layer: " << _pCallbackData->pMessage << std::endl;

	//if (_messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) 
	//{
	//	// Message is important enough to show
	//}

	return VK_FALSE;
}

void Graphic_Vulkan::setupDebugMessenger()
{
	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT debugInfo;
	populateDebugMessengerCreateInfo(debugInfo);

	if (CreateDebugUtilsMessengerEXT(m_Vk, &debugInfo, nullptr, &m_debugMessenger) != VK_SUCCESS)
		throw std::runtime_error("failed to setup debug messenger");
}

VkResult Graphic_Vulkan::CreateDebugUtilsMessengerEXT(VkInstance _Vk, const VkDebugUtilsMessengerCreateInfoEXT* _pCreateInfo, const VkAllocationCallbacks* _pAllocator, VkDebugUtilsMessengerEXT* _pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_Vk, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
		return func(_Vk, _pCreateInfo, _pAllocator, _pDebugMessenger);
	else
		return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void Graphic_Vulkan::DestroyDebugUtilsMessengerEXT(VkInstance _Vk, const VkAllocationCallbacks* _pAllocator, VkDebugUtilsMessengerEXT _pDebugMessenger)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_Vk, "vkDestroyDebugUtilsMessengerEXT");

	if (func != nullptr)
		func(_Vk, _pDebugMessenger, _pAllocator);
}

void Graphic_Vulkan::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& _debugInfo)
{
	_debugInfo = {};
	_debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	_debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
								| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
								| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

	_debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
							| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
							| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

	_debugInfo.pfnUserCallback = debugCallback;
}
///

///
//Physical Device
void Graphic_Vulkan::pickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(m_Vk, &deviceCount, nullptr);

	if (deviceCount == 0)
		throw std::runtime_error("failed to find GPUs with Vulkan Support!");

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(m_Vk, &deviceCount, devices.data());

	for (const VkPhysicalDevice& device : devices)
	{
		if (isDeviceSuitable(device))
		{
			m_PhysicalDevice = device;
			break;
		}
	}

	if (m_PhysicalDevice == VK_NULL_HANDLE)
		throw std::runtime_error("failed to find a suitable GPU!");
}

bool Graphic_Vulkan::isDeviceSuitable(VkPhysicalDevice _device)
{
	QueueFamilyIndices indices = findQueueFamilies(_device);

	///
	//Swap Chain
	bool extensionSupported = checkDeviceExtensionSupport(_device);

	bool swapChainAdequate = false;
	if (extensionSupported)
	{
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(_device);
		swapChainAdequate = swapChainSupport.isTakeData();
	}
	///

	///
	//ImageView
	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(_device, &supportedFeatures);
	///

	return indices.isTakeData() && extensionSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

int Graphic_Vulkan::rateDeviceSuitablility(VkPhysicalDevice _device)
{
	return 0;
}
///

///
//Logical Device
void Graphic_Vulkan::createLogicalDevice()
{
	QueueFamilyIndices indices = findQueueFamilies(m_PhysicalDevice);

	///
	//Window Surface
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateinfo = {};
		queueCreateinfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateinfo.queueFamilyIndex = queueFamily;
		queueCreateinfo.queueCount = 1;
		queueCreateinfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateinfo);
	}

	///
	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.samplerAnisotropy = VK_TRUE;

	VkDeviceCreateInfo LogicalDevInfo = {};
	LogicalDevInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	LogicalDevInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	LogicalDevInfo.pQueueCreateInfos = queueCreateInfos.data();

	LogicalDevInfo.pEnabledFeatures = &deviceFeatures;

	///
	//Swap Chain
	LogicalDevInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	LogicalDevInfo.ppEnabledExtensionNames = deviceExtensions.data();
	///

	if (enableValidationLayers)
	{
		LogicalDevInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		LogicalDevInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		LogicalDevInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(m_PhysicalDevice, &LogicalDevInfo, nullptr, &m_Device) != VK_SUCCESS)
		throw std::runtime_error("failed to create logical device!");

	vkGetDeviceQueue(m_Device, indices.graphicsFamily.value(), 0, &m_GraphicsQueue);
	vkGetDeviceQueue(m_Device, indices.presentFamily.value(), 0, &m_PresentQueue);
}
///

///
//Queue Family
QueueFamilyIndices Graphic_Vulkan::findQueueFamilies(VkPhysicalDevice _device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(_device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(_device, &queueFamilyCount, queueFamilies.data());

	int Count = 0;
	for (const VkQueueFamilyProperties& queueFamily : queueFamilies)
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = Count;

		}

		///
		//Window Surface
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(_device, Count, m_Surface, &presentSupport);

		if (presentSupport)
			indices.presentFamily = Count;
		///

		if (indices.isTakeData())
			break;

		++Count;
	}

	return indices;
}
///

///
//Window Surface
void Graphic_Vulkan::createSurface()
{
	if (glfwCreateWindowSurface(m_Vk, m_Window, nullptr, &m_Surface) != VK_SUCCESS)
		throw std::runtime_error("failed to create window surface!");
}
///

///
//Swap Chain
bool Graphic_Vulkan::checkDeviceExtensionSupport(VkPhysicalDevice _device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(_device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(_device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const VkExtensionProperties& extension : availableExtensions)
		requiredExtensions.erase(extension.extensionName);

	return requiredExtensions.empty();
}

SwapChainSupportDetails Graphic_Vulkan::querySwapChainSupport(VkPhysicalDevice _device)
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_device, m_Surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(_device, m_Surface, &formatCount, nullptr);

	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(_device, m_Surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(_device, m_Surface, &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(_device, m_Surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

VkSurfaceFormatKHR Graphic_Vulkan::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& _availableFormats)
{
	for (const VkSurfaceFormatKHR& availableFormat : _availableFormats)
	{
		if ((availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB) && (availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR))
			return availableFormat;
	}

	return _availableFormats[0];
}

VkPresentModeKHR Graphic_Vulkan::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& _avaliablePresentModes)
{
	for (const VkPresentModeKHR& availablePresentMode : _avaliablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			return availablePresentMode;
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Graphic_Vulkan::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& _capabilities)
{
	if (_capabilities.currentExtent.width != UINT32_MAX)
		return _capabilities.currentExtent;
	else
	{
		int width, height;

		glfwGetFramebufferSize(m_Window, &width, &height);

		//VkExtent2D actualExtent = {m_WindowWidth, m_WindowHeight};

		VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

		actualExtent.width = std::max(_capabilities.minImageExtent.width, std::min(_capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(_capabilities.minImageExtent.height, std::min(_capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

void Graphic_Vulkan::createSwapChain()
{
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_PhysicalDevice);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

	if ((swapChainSupport.capabilities.maxImageCount > 0) && (imageCount > swapChainSupport.capabilities.maxImageCount))
		imageCount = swapChainSupport.capabilities.maxImageCount;

	VkSwapchainCreateInfoKHR swapChainInfo = {};

	swapChainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapChainInfo.surface = m_Surface;

	swapChainInfo.minImageCount = imageCount;
	swapChainInfo.imageFormat = surfaceFormat.format;
	swapChainInfo.imageColorSpace = surfaceFormat.colorSpace;
	swapChainInfo.imageExtent = extent;
	swapChainInfo.imageArrayLayers = 1;
	swapChainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = findQueueFamilies(m_PhysicalDevice);
	uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

	if (indices.graphicsFamily != indices.presentFamily)
	{
		swapChainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapChainInfo.queueFamilyIndexCount = 2;
		swapChainInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		swapChainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		//swapChainInfo.queueFamilyIndexCount = 0; //optional
		//swapChainInfo.pQueueFamilyIndices = nullptr; //optional
	}

	swapChainInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	swapChainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapChainInfo.presentMode = presentMode;
	swapChainInfo.clipped = VK_TRUE;

	//swapChainInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(m_Device, &swapChainInfo, nullptr, &m_SwapChain) != VK_SUCCESS)
		throw std::runtime_error("failed to create swap chain!");

	vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, nullptr);
	m_SwapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, m_SwapChainImages.data());

	m_SwapChainImageFormat = surfaceFormat.format;
	m_SwapChainExtent = extent;
}

///
//Swap Chain Recreation
void Graphic_Vulkan::recreateSwapChain()
{
	int width, height = 0;
	glfwGetFramebufferSize(m_Window, &width, &height);
	while ((width == 0) || height == 0)
	{
		glfwGetFramebufferSize(m_Window, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(m_Device);

	cleanupSwapChain();

	createSwapChain();
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
	///
	//Depth bufferings
	createDepthResource();
	///
	createFramebuffers();

	///
	//uniform Buffer
	createUniformBuffers();
	createDescriptorPool();
	createDescriptorSets();
	///

	createCommandBuffers();
}

void Graphic_Vulkan::cleanupSwapChain()
{
	///
	//Depth bufferings
	vkDestroyImageView(m_Device, m_DepthImageView, nullptr);
	vkDestroyImage(m_Device, m_DepthImage, nullptr);
	vkFreeMemory(m_Device, m_DepthImageMemory, nullptr);
	///

	for (size_t Count = 0; m_SwapChainFramebuffers.size() > Count; ++Count)
		vkDestroyFramebuffer(m_Device, m_SwapChainFramebuffers[Count], nullptr);

	vkFreeCommandBuffers(m_Device, m_CommandPool, static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());

	vkDestroyPipeline(m_Device, m_GraphicsPipeline, nullptr);
	vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);

	for (size_t Count = 0; m_SwapChainImageViews.size() > Count; ++Count)
		vkDestroyImageView(m_Device, m_SwapChainImageViews[Count], nullptr);

	vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);

	///
	//Uniform Buffers
	for (size_t Count = 0; m_SwapChainImages.size() > Count; ++Count)
	{
		vkDestroyBuffer(m_Device, m_UniformBuffers[Count], nullptr);
		vkFreeMemory(m_Device, m_UniformBuffersMemory[Count], nullptr);
	}

	vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr);
	///
}

void Graphic_Vulkan::framebufferResizeCallback(GLFWwindow* _window, int _width, int _height)
{
	Graphic_Vulkan::GetInstance()->m_FramebufferResized = true;
}
///

///
//Image Views
void Graphic_Vulkan::createImageViews()
{
	m_SwapChainImageViews.resize(m_SwapChainImages.size());

	//for (size_t Count = 0; m_SwapChainImages.size() > Count; ++Count)
	//{
	//	VkImageViewCreateInfo ImageViewInfo = {};

	//	ImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	//	ImageViewInfo.image = m_SwapChainImages[Count];

	//	ImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	//	ImageViewInfo.format = m_SwapChainImageFormat;

	//	ImageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	//	ImageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	//	ImageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	//	ImageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

	//	ImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	//	ImageViewInfo.subresourceRange.baseMipLevel = 0;
	//	ImageViewInfo.subresourceRange.layerCount = 1;
	//	ImageViewInfo.subresourceRange.baseArrayLayer = 0;
	//	ImageViewInfo.subresourceRange.layerCount = 1;

	//	if (vkCreateImageView(m_Device, &ImageViewInfo, nullptr, &m_SwapChainImageViews[Count]) != VK_SUCCESS)
	//		throw std::runtime_error("failed to create image views!");
	//}

	///
	//Imageview
	//for (uint32_t Count = 0; m_SwapChainImages.size() > Count; ++Count)
	//	m_SwapChainImageViews[Count] = createImageView(m_SwapChainImages[Count], m_SwapChainImageFormat);
	///

	///
	//Depth buffers
	for (uint32_t Count = 0; m_SwapChainImages.size() > Count; ++Count)
		m_SwapChainImageViews[Count] = createImageView(m_SwapChainImages[Count], m_SwapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	///
}
///

///
//Graphic Pipeline
void Graphic_Vulkan::createGraphicsPipeline()
{
	//char curDir[1000];
	//_getcwd(curDir, 1000);
	//printf(curDir);

	std::vector<char> vertShaderCode = readFile("shaders/vert.spv");
	std::vector<char> fragShaderCode = readFile("shaders/frag.spv");

	VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};


	///
	//Fixed function
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	///
	//Vertex Input
	VkVertexInputBindingDescription Description = Vertex::getBindingDescription();
	//std::array<VkVertexInputAttributeDescription, 2U> Attribute = Vertex::getAttributeDescriptions();

	///
	//Combined Image
	std::array<VkVertexInputAttributeDescription, 3U> Attribute = Vertex::getAttributeDescriptions();
	///


	//vertexInputInfo.vertexBindingDescriptionCount = 0;
	//vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
	//vertexInputInfo.vertexAttributeDescriptionCount = 0;
	//vertexInputInfo.pVertexAttributeDescriptions = nullptr;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(Attribute.size());
	vertexInputInfo.pVertexBindingDescriptions = &Description;
	vertexInputInfo.pVertexAttributeDescriptions = Attribute.data();

	///
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;
	
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float) m_SwapChainExtent.width;
	viewport.height = (float) m_SwapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = m_SwapChainExtent;

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

	rasterizer.depthBiasEnable = VK_FALSE;
	//rasterizer.depthBiasConstantFactor = 0.0f;
	//rasterizer.depthBiasClamp = 0.0f;
	//rasterizer.depthBiasSlopeFactor = 0.0f;

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	//multisampling.minSampleShading = 1.0f; // Optional
	//multisampling.pSampleMask = nullptr; // Optional
	//multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	//multisampling.alphaToOneEnable = VK_FALSE; // Optional

	///
	//Depth buffering
	VkPipelineDepthStencilStateCreateInfo depthStencil = {};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;

	depthStencil.depthBoundsTestEnable = VK_FALSE;
	//depthStencil.minDepthBounds = 0.0f; // Optional
	//depthStencil.maxDepthBounds = 1.0f; // Optional

	depthStencil.stencilTestEnable = VK_FALSE;
	//depthStencil.front = {}; // Optional
	//depthStencil.back = {}; // Optional
	///

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	//colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	//colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	//colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	//colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	//colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	//colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	//colorBlendAttachment.blendEnable = VK_TRUE;
	//colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	//colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	//colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	//colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	//colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	//colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	//VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_LINE_WIDTH};

	//VkPipelineDynamicStateCreateInfo dynamicState = {};
	//dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	//dynamicState.dynamicStateCount = 2;
	//dynamicState.pDynamicStates = dynamicStates;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

	//pipelineLayoutInfo.setLayoutCount = 0; // Optional
	//pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
	///
	//uniform buffers
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
	///
	//pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	//pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

	if (vkCreatePipelineLayout(m_Device, &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		throw std::runtime_error("failed to create pipeline layout!");
	///

	///
	//Conclusion
	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;

	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	//pipelineInfo.pDepthStencilState = nullptr; // Optional
	///
	//Depth buffering
	pipelineInfo.pDepthStencilState = &depthStencil;
	///
	pipelineInfo.pColorBlendState = &colorBlending;
	//pipelineInfo.pDynamicState = nullptr; // Optional

	pipelineInfo.layout = m_PipelineLayout;

	pipelineInfo.renderPass = m_RenderPass;
	pipelineInfo.subpass = 0;

	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	//pipelineInfo.basePipelineIndex = -1; // Optional

	if (vkCreateGraphicsPipelines(m_Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline) != VK_SUCCESS)
		throw std::runtime_error("failed to create graphics pipeline!");
	///

	vkDestroyShaderModule(m_Device, fragShaderModule, nullptr);
	vkDestroyShaderModule(m_Device, vertShaderModule, nullptr);
}

VkShaderModule Graphic_Vulkan::createShaderModule(const std::vector<char>& _code)
{
	VkShaderModuleCreateInfo ShaderInfo = {};
	ShaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	ShaderInfo.codeSize = _code.size();
	ShaderInfo.pCode = reinterpret_cast<const uint32_t*>(_code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(m_Device, &ShaderInfo, nullptr, &shaderModule) != VK_SUCCESS)
		throw std::runtime_error("failed to create shader module!");

	return shaderModule;
}
///

///
//Render Passes
void Graphic_Vulkan::createRenderPass()
{
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = m_SwapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	//VkAttachmentReference colorAttachmentRef = {};
	//colorAttachmentRef.attachment = 0;
	//colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	//VkSubpassDescription subpass = {};
	//subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	//subpass.colorAttachmentCount = 1;
	//subpass.pColorAttachments = &colorAttachmentRef;

	///
	//Depth buffering
	VkAttachmentDescription depthAttachment = {};
	depthAttachment.format = findDepthFormat();
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef = {};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;
	
	std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	///

	///
	//Draw
	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	///

	//VkRenderPassCreateInfo renderPassInfo = {};
	//renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	//renderPassInfo.attachmentCount = 1;
	//renderPassInfo.pAttachments = &colorAttachment;
	//renderPassInfo.subpassCount = 1;
	//renderPassInfo.pSubpasses = &subpass;
	///
	//Draw
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;
	///

	if (vkCreateRenderPass(m_Device, &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
		throw std::runtime_error("failed to create render pass!");
}
///

///
//Framebuffers
void Graphic_Vulkan::createFramebuffers()
{
	m_SwapChainFramebuffers.resize(m_SwapChainImageViews.size());

	for (size_t Count = 0; m_SwapChainImageViews.size() > Count; ++Count)
	{

		//VkImageView attachements[] = { m_SwapChainImageViews[Count] };

		std::array<VkImageView, 2> attachemnts = {m_SwapChainImageViews[Count], m_DepthImageView };

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_RenderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachemnts.size());
		framebufferInfo.pAttachments = attachemnts.data();
		framebufferInfo.width = m_SwapChainExtent.width;
		framebufferInfo.height = m_SwapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(m_Device, &framebufferInfo, nullptr, &m_SwapChainFramebuffers[Count]) != VK_SUCCESS)
			throw std::runtime_error("failed to create framebuffer!");
	}
}
///

///
//Command Buffer
void Graphic_Vulkan::createCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(m_PhysicalDevice);

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
	poolInfo.flags = 0; // Optional

	if (vkCreateCommandPool(m_Device, &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS)
		throw std::runtime_error("failed to create command pool!");
}

void Graphic_Vulkan::createCommandBuffers()
{
	m_CommandBuffers.resize(m_SwapChainFramebuffers.size());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_CommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)m_CommandBuffers.size();

	if (vkAllocateCommandBuffers(m_Device, &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
		throw std::runtime_error("failed to allocate command buffers!");

	for (size_t Count = 0; m_CommandBuffers.size() > Count; ++Count)
	{
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		//beginInfo.flags = 0; // Optional
		//beginInfo.pInheritanceInfo = nullptr; // Optional

		if (vkBeginCommandBuffer(m_CommandBuffers[Count], &beginInfo) != VK_SUCCESS)
			throw std::runtime_error("failed to begin recoding command buffer!");

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_RenderPass;
		renderPassInfo.framebuffer = m_SwapChainFramebuffers[Count];

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_SwapChainExtent;

		//VkClearValue clearColor = {0.f, 0.f, 0.f, 1.f};
		//renderPassInfo.clearValueCount = 1;
		//renderPassInfo.pClearValues = &clearColor;

		///
		//Depth bufferings
		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
		clearValues[1].depthStencil = {1.0f, 0};

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();
		///

		vkCmdBeginRenderPass(m_CommandBuffers[Count], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindPipeline(m_CommandBuffers[Count], VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);

			VkBuffer vertexBuffers[] = { m_VertexBuffer };
			VkDeviceSize offsets[] = {0};
			vkCmdBindVertexBuffers(m_CommandBuffers[Count], 0, 1, vertexBuffers, offsets);

			///
			//Index Buffer
			vkCmdBindIndexBuffer(m_CommandBuffers[Count], m_IndexBuffer, 0, VK_INDEX_TYPE_UINT16);

				///
				//uniform buffer
				vkCmdBindDescriptorSets(m_CommandBuffers[Count], VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_DescriptorSets[Count], 0, nullptr);
				///

			vkCmdDrawIndexed(m_CommandBuffers[Count], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
			//vkCmdDraw(m_CommandBuffers[Count], static_cast<uint32_t>(vertices.size()), 1, 0, 0);
			///


		vkCmdEndRenderPass(m_CommandBuffers[Count]);

		if (vkEndCommandBuffer(m_CommandBuffers[Count]) != VK_SUCCESS)
			throw std::runtime_error("failed to record command buffer!");
	}
}
///

///
//Draw
void Graphic_Vulkan::createSyncObjects()
{
	m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_InfFlightFence.resize(MAX_FRAMES_IN_FLIGHT);
	m_ImagesInFlight.resize(m_SwapChainImages.size(), VK_NULL_HANDLE);

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t Count = 0; MAX_FRAMES_IN_FLIGHT > Count; ++Count)
	{
		if ((vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[Count]) != VK_SUCCESS) || (vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[Count]) != VK_SUCCESS) || (vkCreateFence(m_Device, &fenceInfo, nullptr, &m_InfFlightFence[Count]) != VK_SUCCESS))
			throw std::runtime_error("failed to create synchronization objects for a frame!");
	}
}

void Graphic_Vulkan::DrawFrame()
{
	vkWaitForFences(m_Device, 1, &m_InfFlightFence[m_CurrentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;

	///
	//SwapChain Recreation
	VkResult ImageResult = vkAcquireNextImageKHR(m_Device, m_SwapChain, UINT64_MAX, m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &imageIndex);

	if ((ImageResult == VK_ERROR_OUT_OF_DATE_KHR) || (ImageResult == VK_SUBOPTIMAL_KHR) || m_FramebufferResized)
	{
		recreateSwapChain();
		return;
	}
	else if ((ImageResult != VK_SUCCESS) && (ImageResult != VK_SUBOPTIMAL_KHR))
		throw std::runtime_error("failed to acquire swap chain image");
	///

	///
	//uniform Buffer
	updateUniformBuffer(imageIndex);
	///

	if (m_ImagesInFlight[imageIndex] != VK_NULL_HANDLE)
		vkWaitForFences(m_Device, 1, &m_ImagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);

	m_ImagesInFlight[imageIndex] = m_InfFlightFence[m_CurrentFrame];

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[m_CurrentFrame] };
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_CommandBuffers[imageIndex];

	VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphores[m_CurrentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(m_Device, 1, &m_InfFlightFence[m_CurrentFrame]);

	if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_InfFlightFence[m_CurrentFrame]) != VK_SUCCESS)
		throw std::runtime_error("failed to submit draw command buffer!");

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = {m_SwapChain};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional

	///
	//Swap Chain Recreation
	ImageResult = vkQueuePresentKHR(m_PresentQueue, &presentInfo);

	if ((ImageResult == VK_ERROR_OUT_OF_DATE_KHR) || (ImageResult == VK_SUBOPTIMAL_KHR))
		recreateSwapChain();
	else if (ImageResult != VK_SUCCESS)
		throw std::runtime_error("failed to present swap chain image!");
	///

	m_CurrentFrame = (++m_CurrentFrame) % MAX_FRAMES_IN_FLIGHT;
}
///

///
//Vertex Buffer Creation
void Graphic_Vulkan::createVertexBuffer()
{
	///
	//staging Buffer
	VkDeviceSize BufferSize = sizeof(vertices[0]) * vertices.size();
	VkBuffer StagingBuffer = VK_NULL_HANDLE;
	VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;
	createBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, StagingBuffer, stagingBufferMemory);

	void* Data;
	vkMapMemory(m_Device, stagingBufferMemory, 0, BufferSize, 0, &Data);
		memcpy(Data, vertices.data(), (size_t)BufferSize);
	vkUnmapMemory(m_Device, stagingBufferMemory);

	createBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VertexBuffer, m_VertexBufferMemory);

	copyBuffer(StagingBuffer, m_VertexBuffer, BufferSize);

	vkDestroyBuffer(m_Device, StagingBuffer, nullptr);
	vkFreeMemory(m_Device, stagingBufferMemory, nullptr);
	///
}

uint32_t Graphic_Vulkan::findMemroyType(uint32_t _typeFilter, VkMemoryPropertyFlags _properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memProperties);

	for (uint32_t Count = 0; memProperties.memoryTypeCount > Count; ++Count)
	{
		if ((_typeFilter & (1 << Count)) && ((memProperties.memoryTypes[Count].propertyFlags & _properties) == _properties))
		{
			return Count;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}
///

///
//Staging Buffer
void Graphic_Vulkan::createBuffer(VkDeviceSize _size, VkBufferUsageFlags _usage, VkMemoryPropertyFlags _properties, VkBuffer& _buffer, VkDeviceMemory& _bufferMemory)
{
	VkBufferCreateInfo bufferInfo = {};

	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = _size;
	bufferInfo.usage = _usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(m_Device, &bufferInfo, nullptr, &_buffer) != VK_SUCCESS)
		throw std::runtime_error("failed to create vertex buffer!");

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(m_Device, _buffer, &memRequirements);

	VkMemoryAllocateInfo allInfo = {};
	allInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allInfo.allocationSize = memRequirements.size;
	allInfo.memoryTypeIndex = findMemroyType(memRequirements.memoryTypeBits, _properties);

	if (vkAllocateMemory(m_Device, &allInfo, nullptr, &_bufferMemory) != VK_SUCCESS)
		throw std::runtime_error("failed to allocate vertex buffer memory!");

	vkBindBufferMemory(m_Device, _buffer, _bufferMemory, 0);
}

void Graphic_Vulkan::copyBuffer_old(VkBuffer _srcBuffer, VkBuffer _dstBuffer, VkDeviceSize _size)
{
	VkCommandBufferAllocateInfo AllocInfo = {};
	AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	AllocInfo.commandPool = m_CommandPool;
	AllocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(m_Device, &AllocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	//Comand Queue Writing Start
	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion = {};
	copyRegion.srcOffset = 0;//Optional
	copyRegion.dstOffset = 0;//Optional
	copyRegion.size = _size;

	//Command Queue Writing
	vkCmdCopyBuffer(commandBuffer, _srcBuffer, _dstBuffer, 1, &copyRegion);


	//Command Queue Writing End
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	//Command Queue Execute In GPU
	vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(m_GraphicsQueue);


	//release commandBuffer In m_CommandPool - CommandPool is Command Buffer Array
	vkFreeCommandBuffers(m_Device, m_CommandPool, 1, &commandBuffer);
}

void Graphic_Vulkan::copyBuffer(VkBuffer _srcBuffer, VkBuffer _dstBuffer, VkDeviceSize _size)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkBufferCopy copyRegion = {};
	copyRegion.size = _size;
	vkCmdCopyBuffer(commandBuffer, _srcBuffer, _dstBuffer, 1, &copyRegion);

	endSingleTimeCommands(commandBuffer);
}
///

///
//Index Buffer
void Graphic_Vulkan::createIndexBuffer()
{
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
	VkBuffer StagingBuffer = VK_NULL_HANDLE;
	VkDeviceMemory StagingBufferMemory = VK_NULL_HANDLE;

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, StagingBuffer, StagingBufferMemory);

	void* Data;
	vkMapMemory(m_Device, StagingBufferMemory, 0, bufferSize, 0, &Data);
	memcpy(Data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(m_Device, StagingBufferMemory);

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_IndexBuffer, m_IndexBufferMemory);

	copyBuffer(StagingBuffer, m_IndexBuffer, bufferSize);

	vkDestroyBuffer(m_Device, StagingBuffer, nullptr);
	vkFreeMemory(m_Device, StagingBufferMemory, nullptr);
}
///

///
//Uniform Buffer
void Graphic_Vulkan::createDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

	uboLayoutBinding.pImmutableSamplers = nullptr;//Optional
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	//VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	//layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	//layoutInfo.bindingCount = 1;
	//layoutInfo.pBindings = &uboLayoutBinding;

	///
	//CombindImageSampler
	VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();
	///

	if (vkCreateDescriptorSetLayout(m_Device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
		throw std::runtime_error("failed to create descriptor set layout!");
}

void Graphic_Vulkan::createUniformBuffers()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	m_UniformBuffers.resize(m_SwapChainImages.size());
	m_UniformBuffersMemory.resize(m_SwapChainImages.size());

	for (size_t Count = 0; m_SwapChainImages.size() > Count; ++Count)
		createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_UniformBuffers[Count], m_UniformBuffersMemory[Count]);
}

void Graphic_Vulkan::updateUniformBuffer(uint32_t _currentImage)
{
	static std::chrono::steady_clock::time_point startTime = std::chrono::high_resolution_clock::now();

	std::chrono::steady_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	///캐릭터 객체가 World 행렬 제공
	///카메라 객체가 view, proj 행렬을 제공할것
	UniformBufferObject ubo = {};
	ubo.world = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), m_SwapChainExtent.width / (float)m_SwapChainExtent.height, 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;

	void* Data;
	vkMapMemory(m_Device, m_UniformBuffersMemory[_currentImage], 0, sizeof(ubo), 0, &Data);
	memcpy(Data, &ubo, sizeof(ubo));
	vkUnmapMemory(m_Device, m_UniformBuffersMemory[_currentImage]);
}

void Graphic_Vulkan::createDescriptorPool()
{
	//VkDescriptorPoolSize poolSize = {};
	//poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	//poolSize.descriptorCount = static_cast<uint32_t>(m_SwapChainImages.size());

	//VkDescriptorPoolCreateInfo poolInfo = {};
	//poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	//poolInfo.poolSizeCount = 1;
	//poolInfo.pPoolSizes = &poolSize;

	///
	//Combined Image Sampler
	std::array<VkDescriptorPoolSize, 2> poolSizes = {};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(m_SwapChainImages.size());
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(m_SwapChainImages.size());
	
	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(m_SwapChainImages.size());
	///

	poolInfo.maxSets = static_cast<uint32_t>(m_SwapChainImages.size());

	if (vkCreateDescriptorPool(m_Device, &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS)
		throw std::runtime_error("failed to create descriptor pool!");
}

void Graphic_Vulkan::createDescriptorSets()
{
	std::vector<VkDescriptorSetLayout> layouts(m_SwapChainImages.size(), descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_DescriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(m_SwapChainImages.size());
	allocInfo.pSetLayouts = layouts.data();

	m_DescriptorSets.resize(m_SwapChainImages.size());
	if (vkAllocateDescriptorSets(m_Device, &allocInfo, m_DescriptorSets.data()) != VK_SUCCESS)
		throw std::runtime_error("failed to allocate descriptor sets!");

	for (size_t Count = 0; m_SwapChainImages.size() > Count; ++Count)
	{
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = m_UniformBuffers[Count];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		//VkWriteDescriptorSet descriptorWrite = {};
		//descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		//descriptorWrite.dstSet = m_DescriptorSets[Count];
		//descriptorWrite.dstBinding = 0;
		//descriptorWrite.dstArrayElement = 0;

		//descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		//descriptorWrite.descriptorCount = 1;

		//descriptorWrite.pBufferInfo = &bufferInfo;
		//descriptorWrite.pImageInfo = nullptr; // Optional
		//descriptorWrite.pTexelBufferView = nullptr; // Optional
		//vkUpdateDescriptorSets(m_Device, 1, &descriptorWrite, 0, nullptr);

		///
		//Combind Image Sampler
		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = m_TextureImageView;
		imageInfo.sampler = m_TextureSampler;

		std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = m_DescriptorSets[Count];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = m_DescriptorSets[Count];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(m_Device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		///
	}
}
///

///
//Image
void Graphic_Vulkan::createTextureImage()
{
	//char curDir[1000];
	//_getcwd(curDir, 1000);
	//printf(curDir);

	int texWidth = 0, texHeight = 0, texChannels = 0;
	stbi_uc* pixels = stbi_load("textures/image0.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize ImageSize = texWidth * texHeight * 4;

	if (!pixels)
		throw std::runtime_error("failed to load texture images!");

	VkBuffer stagingBuffer = VK_NULL_HANDLE;
	VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;

	createBuffer(ImageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* Data;
	vkMapMemory(m_Device, stagingBufferMemory, 0, ImageSize, 0, &Data);
	memcpy(Data, pixels, static_cast<size_t>(ImageSize));
	vkUnmapMemory(m_Device, stagingBufferMemory);

	stbi_image_free(pixels);

	createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_TextureImage, m_TextureImageMemory);

	transitionImageLayout(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(stagingBuffer, m_TextureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
	transitionImageLayout(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vkDestroyBuffer(m_Device, stagingBuffer, nullptr);
	vkFreeMemory(m_Device, stagingBufferMemory, nullptr);
}

void Graphic_Vulkan::createImage(uint32_t _width, uint32_t _height, VkFormat _format, VkImageTiling _tiling, VkImageUsageFlags _usage, VkMemoryPropertyFlags _properties, VkImage& _image, VkDeviceMemory& _imageMemory)
{
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = _width;
	imageInfo.extent.height = _height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = _format;
	imageInfo.tiling = _tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = _usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(m_Device, &imageInfo, nullptr, &_image) != VK_SUCCESS)
		throw std::runtime_error("failed to create image!");

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(m_Device, _image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemroyType(memRequirements.memoryTypeBits, _properties);

	if (vkAllocateMemory(m_Device, &allocInfo, nullptr, &_imageMemory) != VK_SUCCESS)
		throw std::runtime_error("failed to allocate image memory!");

	vkBindImageMemory(m_Device, _image, _imageMemory, 0);
}

VkCommandBuffer Graphic_Vulkan::beginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = m_CommandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer result;
	vkAllocateCommandBuffers(m_Device, &allocInfo, &result);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(result, &beginInfo);

	return result;
}

void Graphic_Vulkan::endSingleTimeCommands(VkCommandBuffer _commandBuffer)
{
	vkEndCommandBuffer(_commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &_commandBuffer;

	vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(m_GraphicsQueue);

	vkFreeCommandBuffers(m_Device, m_CommandPool, 1, &_commandBuffer);
}

void Graphic_Vulkan::transitionImageLayout(VkImage _image, VkFormat _format, VkImageLayout _oldLayout, VkImageLayout _newLayout)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkImageMemoryBarrier barrier = {};

	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = _oldLayout;
	barrier.newLayout = _newLayout;

	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	barrier.image = _image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	barrier.srcAccessMask = 0; //TODO
	barrier.dstAccessMask = 0; //TODO

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if ((_oldLayout == VK_IMAGE_LAYOUT_UNDEFINED) && (_newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL))
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if ((_oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) && (_newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL))
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if ((_oldLayout == VK_IMAGE_LAYOUT_UNDEFINED) && (_newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL))
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else
	{
		throw std::invalid_argument("unsupported layout transition!");
	}

	vkCmdPipelineBarrier(commandBuffer
						, sourceStage, destinationStage
						, 0
						, 0, nullptr
						, 0, nullptr
						, 1, &barrier);

	endSingleTimeCommands(commandBuffer);
}

void Graphic_Vulkan::copyBufferToImage(VkBuffer _buffer, VkImage _image, uint32_t _width, uint32_t _height)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = { 0, 0, 0};
	region.imageExtent = { _width, _height, 1 };

	vkCmdCopyBufferToImage(commandBuffer, _buffer, _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	endSingleTimeCommands(commandBuffer);
}
///

///
//ImageView
void Graphic_Vulkan::createTextureImageView()
{
	//m_TextureImageView = createImageView(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB);
	///
	//Depth buffer
	m_TextureImageView = createImageView(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
	///
}

VkImageView Graphic_Vulkan::createImageView(VkImage _image, VkFormat _format)
{
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = _image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = _format;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView = VK_NULL_HANDLE;
	if (vkCreateImageView(m_Device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
		throw std::runtime_error("failed to create texture image view!");

	return imageView;
}

void Graphic_Vulkan::createTextureSampler()
{
	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;

	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 16;

	//samplerInfo.anisotropyEnable = VK_FALSE;
	//samplerInfo.maxAnisotropy = 1;

	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

	samplerInfo.unnormalizedCoordinates = VK_FALSE;

	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	if (vkCreateSampler(m_Device, &samplerInfo, nullptr, &m_TextureSampler) != VK_SUCCESS)
		throw std::runtime_error("failed to create texture sampler!");
}
///

///
//Depth buffering
void Graphic_Vulkan::createDepthResource()
{
	VkFormat depthFormat = findDepthFormat();

	createImage(m_SwapChainExtent.width, m_SwapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_DepthImage, m_DepthImageMemory);
	m_DepthImageView = createImageView(m_DepthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
}

VkFormat Graphic_Vulkan::findSupportedFormat(const std::vector<VkFormat>& _candidates, VkImageTiling _tiling, VkFormatFeatureFlags _features)
{
	for (VkFormat format : _candidates)
	{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, format, &props);
	
		if (_tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & _features) == _features)
			return format;
		else if (_tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & _features) == _features)
			return format;
	}

	throw std::runtime_error("failed to find supported format!");
}

VkFormat Graphic_Vulkan::findDepthFormat()
{
	return findSupportedFormat(	{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }
								, VK_IMAGE_TILING_OPTIMAL
								, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

bool Graphic_Vulkan::hasStancilComponent(VkFormat _format)
{
	return (_format == VK_FORMAT_D32_SFLOAT_S8_UINT) || (_format == VK_FORMAT_D24_UNORM_S8_UINT);
}

VkImageView Graphic_Vulkan::createImageView(VkImage _image, VkFormat _format, VkImageAspectFlags _aspectFlages)
{
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = _image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = _format;
	viewInfo.subresourceRange.aspectMask = _aspectFlages;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView = VK_NULL_HANDLE;
	if (vkCreateImageView(m_Device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
		throw std::runtime_error("failed to create texture image view!");

	return imageView;
}
///
