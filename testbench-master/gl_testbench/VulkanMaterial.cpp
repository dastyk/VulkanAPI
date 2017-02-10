#include "VulkanMaterial.h"

#include "VulkanHelpers.h"
#include <array>
#include <assert.h>
#include <fstream>

using namespace std;

VulkanMaterial::VulkanMaterial(VkDevice device, VkPipelineLayout pipelineLayout, VkRenderPass renderPass) : _device(device), _pipelineLayout(pipelineLayout), _renderPass(renderPass)
{

}

VulkanMaterial::~VulkanMaterial()
{
	if (_pipeline)
	{
		vkDestroyPipeline(_device, _pipeline, nullptr);
		_pipeline = VK_NULL_HANDLE;
	}

	for (auto& buffer : constantBuffers)
	{
		delete buffer.second;
		buffer.second = nullptr;
	}
}

void VulkanMaterial::setShader(const std::string& shaderFileName, ShaderType type)
{
	if (shaderFileNames.find(type) != shaderFileNames.end())
	{
		removeShader(type);
	}

	shaderFileNames[type] = shaderFileName;
}

void VulkanMaterial::removeShader(ShaderType type)
{
	VkShaderModule shader = _shaderObjects[static_cast<uint32_t>(type)];

	// check if name exists (if it doesn't there should not be a shader here.
	if (shaderFileNames.find(type) == shaderFileNames.end())
	{
		assert(shader == 0);
		return;
	}
	else if (shader != VK_NULL_HANDLE)
	{
		if (_pipeline)
		{
			vkDestroyPipeline(_device, _pipeline, nullptr);
			_pipeline = VK_NULL_HANDLE;
		}

		vkDestroyShaderModule(_device, shader, nullptr);
		_shaderObjects[static_cast<uint32_t>(type)] = VK_NULL_HANDLE;
	}
}

void VulkanMaterial::setDiffuse(Color c)
{

}

int VulkanMaterial::compileMaterial(std::string& errString)
{
	// Remove pre-existing shaders
	removeShader(ShaderType::VS);
	removeShader(ShaderType::PS);

	// Create shader modules
	if (!_compileShader(ShaderType::VS))
	{
		exit(-1);
		//return -1;
	}
	if (!_compileShader(ShaderType::PS))
	{
		exit(-1);
		//return -1;
	}

	// Put the shaders together in a pipeline. Wohoo! Explicitness!
	array<VkPipelineShaderStageCreateInfo, 2> stages = {};
	stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stages[0].pNext = nullptr;
	stages[0].flags = 0;
	stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	stages[0].module = _shaderObjects[static_cast<uint32_t>(ShaderType::VS)];
	stages[0].pName = "main";
	stages[0].pSpecializationInfo = nullptr;
	stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stages[1].pNext = nullptr;
	stages[1].flags = 0;
	stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	stages[1].module = _shaderObjects[static_cast<uint32_t>(ShaderType::PS)];
	stages[1].pName = "main";
	stages[1].pSpecializationInfo = nullptr;

	VkPipelineVertexInputStateCreateInfo vertexInput = {};
	vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInput.pNext = nullptr;
	vertexInput.flags = 0;
	vertexInput.vertexBindingDescriptionCount = 0;
	vertexInput.pVertexBindingDescriptions = nullptr;
	vertexInput.vertexAttributeDescriptionCount = 0;
	vertexInput.pVertexAttributeDescriptions = nullptr;
	
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.pNext = nullptr;
	inputAssembly.flags = 0;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkPipelineViewportStateCreateInfo viewportInfo = {};
	viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportInfo.pNext = nullptr;
	viewportInfo.flags = 0;
	viewportInfo.viewportCount = 1;
	viewportInfo.pViewports = nullptr;
	viewportInfo.scissorCount = 1;
	viewportInfo.pScissors = nullptr;

	VkPipelineRasterizationStateCreateInfo rasterState = {};
	rasterState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterState.pNext = nullptr;
	rasterState.flags = 0;
	rasterState.depthClampEnable = VK_FALSE;
	rasterState.rasterizerDiscardEnable = VK_FALSE;
	rasterState.polygonMode = VK_POLYGON_MODE_FILL;
	rasterState.cullMode = VK_CULL_MODE_NONE;
	rasterState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterState.depthBiasEnable = VK_FALSE;
	rasterState.depthBiasConstantFactor = 0.0f;
	rasterState.depthBiasClamp = 0.0f;
	rasterState.depthBiasSlopeFactor = 0.0f;
	rasterState.lineWidth = 1.0f;

	VkPipelineMultisampleStateCreateInfo multisampleState = {};
	multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleState.pNext = nullptr;
	multisampleState.flags = 0;
	multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampleState.sampleShadingEnable = VK_FALSE;
	multisampleState.minSampleShading = 0.0f;
	multisampleState.pSampleMask = 0;
	multisampleState.alphaToCoverageEnable = VK_FALSE;
	multisampleState.alphaToOneEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState colorAttachment = {};
	colorAttachment.blendEnable = VK_FALSE;
	colorAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR;
	colorAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_DST_COLOR;
	colorAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
	colorAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
	colorAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo blendState = {};
	blendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	blendState.pNext = nullptr;
	blendState.flags = 0;
	blendState.logicOpEnable = VK_FALSE;
	blendState.logicOp = VK_LOGIC_OP_AND;
	blendState.attachmentCount = 1;
	blendState.pAttachments = &colorAttachment;
	blendState.blendConstants[0] = 0.0f;
	blendState.blendConstants[1] = 0.0f;
	blendState.blendConstants[2] = 0.0f;
	blendState.blendConstants[3] = 0.0f;

	array<VkDynamicState, 2> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState = {};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.pNext = nullptr;
	dynamicState.flags = 0;
	dynamicState.dynamicStateCount = dynamicStates.size();
	dynamicState.pDynamicStates = dynamicStates.data();

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.pNext = nullptr;
	pipelineInfo.flags = 0;
	pipelineInfo.stageCount = stages.size();
	pipelineInfo.pStages = stages.data();
	pipelineInfo.pVertexInputState = &vertexInput;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pTessellationState = nullptr;
	pipelineInfo.pViewportState = &viewportInfo;
	pipelineInfo.pRasterizationState = &rasterState;
	pipelineInfo.pMultisampleState = &multisampleState;
	pipelineInfo.pDepthStencilState = nullptr;
	pipelineInfo.pColorBlendState = &blendState;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = _pipelineLayout;
	pipelineInfo.renderPass = _renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_pipeline) != VK_SUCCESS)
	{
		throw runtime_error("Failed to create pipeline for material!");
	}

	return 0;
}

// TODO: not really sure how this should map to Vulkan
void VulkanMaterial::addConstantBuffer(std::string name, unsigned int location)
{
	constantBuffers[location] = new ConstantBufferVk(name, location);
}

void VulkanMaterial::updateConstantBuffer(const void* data, size_t size, unsigned int location)
{
	//constantBuffers[location]->setData(data, size, this, location);
}

int VulkanMaterial::enable()
{
	// What!?
	// Vulkan is stateless, it's not like we enable a material by itself

	return 0;
}

void VulkanMaterial::disable()
{
	// Nothing to disable...
}

bool VulkanMaterial::_compileShader(ShaderType type)
{
	ifstream file(shaderFileNames[type], ios::binary | ios::ate);
	if (!file)
	{
		return false;
	}

	streampos size = file.tellg();
	file.seekg(0);
	char* spirv = new char[size];

	file.read(spirv, size);
	file.close();

	VkShaderModuleCreateInfo shaderInfo = {};
	shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderInfo.pNext = nullptr;
	shaderInfo.flags = 0;
	shaderInfo.codeSize = size;
	shaderInfo.pCode = (uint32_t*)spirv;

	VkResult result = vkCreateShaderModule(_device, &shaderInfo, nullptr, &_shaderObjects[static_cast<uint32_t>(type)]);
	if (result != VK_SUCCESS)
	{
		return false;
	}

	return true;
}
