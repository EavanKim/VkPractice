#ifndef GVFUNCTOR_H__
#define GVFUNCTOR_H__

static std::vector<char> readFile(const std::string& _filename)
{
	std::ifstream file(_filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);


	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

#endif // ! GVFUNCTOR_H__
