#include "VulkanBuffer.h"

/**
* Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
*
* @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete buffer range.
* @param offset (Optional) Byte offset from beginning
*
* @return vk::Result of the buffer mapping call
*/
vk::Result VulkanBuffer::Map(vk::DeviceSize size, vk::DeviceSize offset)
{
	return device.mapMemory(memory, offset, size, vk::MemoryMapFlagBits(), &mapped);
}

/**
* Unmap a mapped memory range
*
* @note Does not return a result as vkUnmapMemory can't fail
*/
void VulkanBuffer::Unmap()
{
	if (mapped)
	{
		vkUnmapMemory(device, memory);
		mapped = nullptr;
	}
}

/**
* Attach the allocated memory block to the buffer
*
* @param offset (Optional) Byte offset (from the beginning) for the memory region to bind
*
* @return vk::Result of the bindBufferMemory call
*/
void VulkanBuffer::Bind(vk::DeviceSize offset)
{
	device.bindBufferMemory(buffer, memory, offset);
}

/**
* Setup the default descriptor for this buffer
*
* @param size (Optional) Size of the memory range of the descriptor
* @param offset (Optional) Byte offset from beginning
*
*/
void VulkanBuffer::SetupDescriptor(vk::DeviceSize size, vk::DeviceSize offset)
{
	descriptor.offset = offset;
	descriptor.buffer = buffer;
	descriptor.range = size;
}

/**
* Copies the specified data to the mapped buffer
*
* @param data Pointer to the data to copy
* @param size Size of the data to copy in machine units
*
*/
void VulkanBuffer::CopyTo(void* data, vk::DeviceSize size)
{
	assert(mapped);
	memcpy(mapped, data, size);
}

/**
* Flush a memory range of the buffer to make it visible to the device
*
* @note Only required for non-coherent memory
*
* @param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the complete buffer range.
* @param offset (Optional) Byte offset from beginning
*
* @return vk::Result of the flush call
*/
vk::Result VulkanBuffer::Flush(vk::DeviceSize size, vk::DeviceSize offset)
{
	vk::MappedMemoryRange mappedRange;
	mappedRange.memory = memory;
	mappedRange.offset = offset;
	mappedRange.size = size;
	return device.flushMappedMemoryRanges(1, &mappedRange);
}

/**
* Invalidate a memory range of the buffer to make it visible to the host
*
* @note Only required for non-coherent memory
*
* @param size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate the complete buffer range.
* @param offset (Optional) Byte offset from beginning
*
* @return vk::Result of the invalidate call
*/
vk::Result VulkanBuffer::Invalidate(vk::DeviceSize size, vk::DeviceSize offset)
{
	vk::MappedMemoryRange mappedRange;
	mappedRange.memory = memory;
	mappedRange.offset = offset;
	mappedRange.size = size;
	return device.invalidateMappedMemoryRanges(1, &mappedRange);
}

/**
* Release all Vulkan resources held by this buffer
*/
void VulkanBuffer::Destroy()
{
	if (buffer)
	{
		device.destroyBuffer(buffer);
	}
	if (memory)
	{
		device.freeMemory(memory);
	}
}