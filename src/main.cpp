#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

#include "PCAP_Parser.hpp"

int main(int argc, char* argv[])
{
	std::string pcapDumpFile = "";
	std::string outputFile = "output.json";

	for (int i = 1; i < argc; ++i)
	{
	    std::string arg = argv[i];
	    if ((arg == "-p" || arg == "--pcap_dump") && i + 1 < argc)
		{
	        pcapDumpFile = argv[++i];
	    }
	    else if ((arg == "-o" || arg == "--out") && i + 1 < argc)
		{
	        outputFile = argv[++i];
	    }
	}

	if (pcapDumpFile.empty() || outputFile.empty()) {
	    std::cerr << "Usage: " << std::endl << " -p [pcap file] " << std::endl << " -o [output file]" << std::endl;
	    return EXIT_FAILURE;
	}

	std::cout << "Start processing file: " << pcapDumpFile << std::endl;

	try
	{
		PCAPParser parser(pcapDumpFile, outputFile);
		parser.parse();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}