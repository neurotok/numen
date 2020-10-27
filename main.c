#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


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

	VkApplicationInfo app_info;
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pNext = NULL;
	app_info.pApplicationName = "VkCompute";
	app_info.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
	app_info.pEngineName = "No name";
	app_info.engineVersion = VK_MAKE_VERSION(0, 0, 1);
	app_info.apiVersion = VK_MAKE_VERSION(1, 1, 0);

	static char const * instance_layers [] = {"VK_LAYER_KHRONOS_validation"};

	VkInstanceCreateInfo instance_create_info;
	instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instance_create_info.pNext = 0;
	instance_create_info.flags = 0;
	instance_create_info.pApplicationInfo = &app_info;
#ifdef NDEBUG
	instance_create_info.enabledLayerCount = 0;
#else
	instance_create_info.enabledLayerCount = sizeof(instance_layers) / sizeof(instance_layers[0]);
	instance_create_info.ppEnabledLayerNames = instance_layers;
#endif
	instance_create_info.enabledExtensionCount = 0;
	instance_create_info.ppEnabledExtensionNames = NULL;

	VkInstance instance = VK_NULL_HANDLE;
	VK(vkCreateInstance(&instance_create_info, 0, &instance));

	uint32_t physical_devices_count;
	VK(vkEnumeratePhysicalDevices(instance, &physical_devices_count, NULL));
	VkPhysicalDevice physical_devices[physical_devices_count];
	VK(vkEnumeratePhysicalDevices(instance, &physical_devices_count, physical_devices));

	for (uint32_t i = 0; i < physical_devices_count; ++i)
	{
		uint32_t queues_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physical_devices[i], &queues_count, NULL);
		VkQueueFamilyProperties queue_family_prop[queues_count];
		vkGetPhysicalDeviceQueueFamilyProperties(physical_devices[i], &physical_devices_count, queue_family_prop);

		const VkQueueFlags availible_mask =  (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT);

		printf("No of queyes families %d\n", queues_count);

		for (uint32_t k = 0; k < queues_count; ++k) {
			
			if (queue_family_prop[0].queueFlags & availible_mask) {
				printf("I got it all! %d\n", (uint8_t)queue_family_prop[0]);	

			}
		}

		const VkQueueFlags flags_mask = (~(VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT) &
				queue_family_prop[i].queueFlags);

		uint32_t queueu_family_index = 0;

		for (uint32_t j = 0; j < queues_count; ++j) {

			if (VK_QUEUE_COMPUTE_BIT & flags_mask)
			{
				queueu_family_index = i;
			}
		}

		const float queue_prioritory = 1.0f;

		VkDeviceQueueCreateInfo device_queue_create_info;

		device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		device_queue_create_info.pNext = NULL;
		device_queue_create_info.flags = 0;
		device_queue_create_info.queueFamilyIndex = queueu_family_index;
		device_queue_create_info.queueCount = 1;
		device_queue_create_info.pNext = &queue_prioritory;

	}





	vkDestroyInstance(instance, NULL);

	vkbUninit();

	return 0;
}

