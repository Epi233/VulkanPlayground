#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include <vector>

#define DEBUG_OPEN

class Application
{
public:
	static bool doLog;

public:
	const uint32_t WINDOW_WIDTH = 800;
	const uint32_t WINDOW_HEIGHT = 600;

	GLFWwindow* window;

	VkInstance instance;

	const std::vector<const char*> validationLayers =
	{
		"VK_LAYER_KHRONOS_validation"
	};

#ifdef DEBUG_OPEN
	const bool enableValidationLayer = true;
#else
	const bool enableValidationLayer = false;
#endif

	void run()
	{
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

private:
	void initWindow()
	{
		glfwInit();
		// ����glew������openGL����openGLES��api
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		// ���Ҳ���������resize
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Vulkan Project", nullptr, nullptr);
	}

	void initVulkan()
	{
		createInstance();
	}

	void mainLoop()
	{
		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();
		}
	}

	void cleanup()
	{
		// clear vk
		vkDestroyInstance(instance, nullptr);

		// clean window
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	bool checkValidationLayerSupport()
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : validationLayers)
		{
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (layerFound)
				std::cout << "[checkValidationLayer] Available Validation Layer: " << layerName << "\n";
			else
			{
				std::cerr << "[Error][checkValidationLayer] Available Validation Layer Not Found: " << layerName << "\n";
				return false;
			}
		}

		return true;
	}

	void createInstance()
	{
		// validation layer check
		if (enableValidationLayer && !checkValidationLayerSupport())
		{
			throw std::runtime_error("Validation Layers Requested, But Not Available!");
		}

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "First Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		{
			uint32_t extensionCount = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
			std::vector<VkExtensionProperties> vkExtensions(extensionCount);
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, vkExtensions.data());

			std::cout << "[VkCreateInstance] Available Extensions: " << extensionCount << "\n";
			for (const auto& extension : vkExtensions)
			{
				std::cout << "\t" << extension.extensionName << "\n";
			}
		}

		auto glfwExtensions = getRequiredExtensions();

		createInfo.enabledExtensionCount = static_cast<uint32_t>(glfwExtensions.size());
		createInfo.ppEnabledExtensionNames = glfwExtensions.data();

		if (enableValidationLayer)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
		}

		VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("vkCreateInstance failed");
		}
		else
			std::cout << "[VkCreateInstance] Vk Create Instance Success";
	}

	std::vector<const char*> getRequiredExtensions()
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		std::cout << "[getRequiredExtensions] GLFW Get Required Extensions: " << glfwExtensions << "\n";

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (enableValidationLayer)
		{
			// macro VK_EXT_DEBUG_UTILS_EXTENSION_NAME = "VK_EXT_debug_utils"
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallBack
	(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	)
	{
		std::cerr << "[Error][ValidationLayer] " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}
};

// Static Member Init
bool Application::doLog = true;

int main()
{
	Application app;

	try
	{
		app.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}