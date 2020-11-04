#include <stdio.h>

#include <GV.h>

int main()
{

	try 
	{
		Command::Graphic::run();
	}
	catch (const std::exception& _excep)
	{
		std::cerr << _excep.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}