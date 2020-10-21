#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define NDEBUG

#define VKBIND_IMPLEMENTATION
#include "vkbind.h"

#define VK(result) \
	if (VK_SUCCESS != (result)) { fprintf(stderr, "Failure at %u %s\n", __LINE__, __FILE__); exit(-1); }


VkResult vkGetBestComputeQueueNPH(VkPhysicalDevice physicalDevice, uint32_t* queueFamilyIndex) {
	uint32_t queueFamilyPropertiesCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertiesCount, 0);

	VkQueueFamilyProperties* const queueFamilyProperties = (VkQueueFamilyProperties*)malloc(
			sizeof(VkQueueFamilyProperties) * queueFamilyPropertiesCount);

	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertiesCount, queueFamilyProperties);

	// first try and find a queue that has just the compute bit set
	for (uint32_t i = 0; i < queueFamilyPropertiesCount; i++) {
		// mask out the sparse binding bit that we aren't caring about (yet!) and the transfer bit
		const VkQueueFlags maskedFlags = (~(VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT) &
				queueFamilyProperties[i].queueFlags);

		if (!(VK_QUEUE_GRAPHICS_BIT & maskedFlags) && (VK_QUEUE_COMPUTE_BIT & maskedFlags)) {
			*queueFamilyIndex = i;
			return VK_SUCCESS;
		}
	}

	// lastly get any queue that'll work for us
	for (uint32_t i = 0; i < queueFamilyPropertiesCount; i++) {
		// mask out the sparse binding bit that we aren't caring about (yet!) and the transfer bit
		const VkQueueFlags maskedFlags = (~(VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT) &
				queueFamilyProperties[i].queueFlags);

		if (VK_QUEUE_COMPUTE_BIT & maskedFlags) {
			*queueFamilyIndex = i;
			return VK_SUCCESS;
		}
	}

	return VK_ERROR_INITIALIZATION_FAILED;
}

int main(void)
{
	VK(vkbInit(NULL));

	VkApplicationInfo application_info;
	application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	application_info.pNext = NULL;
	application_info.pApplicationName = "VkCompute";
	application_info.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
	application_info.pEngineName = "No name";
	application_info.engineVersion = VK_MAKE_VERSION(0, 0, 1);
	application_info.apiVersion = VK_MAKE_VERSION(1, 1, 0);

	static char const * instance_layers [] = {"VK_LAYER_KHRONOS_validation"};

	VkInstanceCreateInfo instance_create_info;
	instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instance_create_info.pNext = 0;
	instance_create_info.flags = 0;
	instance_create_info.pApplicationInfo = &application_info;
#ifdef NDEBUG
	instance_create_info.enabledLayerCount = 0;
#else
	instance_create_info.enabledLayerCount = sizeof(instance_layers) / sizeof(instance_layers[0]);
	instance_create_info.ppEnabledLayerNames = instance_layers;
#endif
	instance_create_info.enabledExtensionCount = 0;
	instance_create_info.ppEnabledExtensionNames = NULL;

	VkInstance instance;
	VK(vkCreateInstance(&instance_create_info, 0, &instance));
	uint32_t devices;
	VK(vkEnumeratePhysicalDevices(instance, &devices, NULL));
	
	VkPhysicalDevice physical_device;
	VK(vkEnumeratePhysicalDevices(instance, &devices, &physical_device));

	uint32_t queues;
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queues, NULL);

	VkQueueFamilyProperties queue_family_prop[queues];
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &devices, queue_family_prop);

	/*
	uint32_t queueu_family_index;

	for (uint32_t i = 0; i < queues; i++)
	{
		const VkQueueFlags maskedFlags = (~(VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT) &
				queue_family_prop[i].queueFlags);

		if (VK_QUEUE_COMPUTE_BIT & maskedFlags)
		{
			queueu_family_index = i;
		}
	}
	*/



	vkDestroyInstance(instance, NULL);

	vkbUninit();

	return 0;
}

