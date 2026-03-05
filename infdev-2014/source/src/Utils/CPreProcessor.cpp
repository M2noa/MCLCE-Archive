#include "./CPreProcessor.h"

#include <sstream>

#include "Utils/Data.h"
#include "Utils/Debug.h"
#include "Utils/StringParser.h"

namespace Esteem
{
	std::string CPreProcessor::ProcessStreamToString(cgc::raw_ptr<std::istream> stream, std::string path, const std::string& workingDirectory, std::unordered_map<std::string, std::string>& defines, std::vector<std::string>& includes)
	{
		std::string output;
		std::string line;

		// lets parse first file, rest will be done by the #include command
		StringParser::StripAll(path, "./");

		// add this file to the includes list, to prevent cyclic inclusion
		includes.push_back(path);

		// write upcomming line
		bool writeLine = true;
		// are we in an if statement?
		bool inIf = false;
		// are we in a switch statement?
		bool inSwitch = false;
		// what are we checking against?
		std::string switchCondition = "";
		// if true we can skip all of the other cases
		bool switchSucceeded = false;
		// does this command really contains a command?
		bool validCommand = false;

		uint lineNumber = 0;
		while (std::getline(*stream, line))
		{
			// helpful with errors
			++lineNumber;

			HashCommand hashCommand = CPreProcessor::RetrieveHashCommand(line);
			validCommand = hashCommand.command != "";

			// check if we are in an if statement
			if (validCommand)
			{
				if (inIf)
				{
					if (hashCommand.command == "#else")
					{
						writeLine = !writeLine;
						continue;
					}
					else if (hashCommand.command == "#endif")
					{
						inIf = false;
						writeLine = true;
						continue;
					}
				}

				if (inSwitch)
				{
					if (hashCommand.command == "#case")
					{
						if (!switchSucceeded && switchCondition == hashCommand.value)
						{
							writeLine = true;
							switchSucceeded = true;
						}
						else
							writeLine = false;

						continue;
					}
					else if (hashCommand.command == "#default")
					{
						if (!switchSucceeded)
						{
							writeLine = true;
							switchSucceeded = true;
						}

						continue;
					}
					else if (hashCommand.command == "#endswitch")
					{
						inSwitch = false;
						writeLine = true;
						continue;
					}
				}
			}

			if (writeLine)
			{
				if (!validCommand)
				{
					StringParser::ReplaceAll(line, defines);
					output += line + "\n";
				}
				else if (hashCommand.command == "#pragma")
				{
					if (hashCommand.value == "once")
						defines.emplace(path, "included");
				}
				else if (hashCommand.command == "#include")
				{
					std::string includePath = workingDirectory + hashCommand.value;
					StringParser::StripAll(includePath, "./");

					if (defines.find(includePath) == defines.end())
					{
						if (std::find(includes.begin(), includes.end(), includePath) == includes.end())
						{
							size_t size;
							cgc::strong_ptr<std::istream> ifs = Data::StreamAsset(includePath, size);
							if (ifs)
								output += CPreProcessor::ProcessStreamToString(ifs, includePath, workingDirectory, defines, includes) + "\r\n";
							else
								Debug::LogError("Could not load file to include: " + includePath);
						}
						else
							Debug::LogError("CPreProcessor: cyclic inclusion detected in file: \"" + includePath + "\" line: " + std::to_string(lineNumber) + " \r\n" + line);
					}
				}
				else if (hashCommand.command == "#define")
				{
					defines.emplace(hashCommand.value, hashCommand.value2);
				}
				else if (hashCommand.command == "#undef")
				{
					auto found = defines.find(hashCommand.value);
					if (found != defines.end())
						defines.erase(found);
				}
				else if (hashCommand.command == "#ifdef")
				{
					inIf = true;

					if (defines.find(hashCommand.value) == defines.end())
						writeLine = false;
				}
				else if (hashCommand.command == "#ifndef")
				{
					inIf = true;

					if (defines.find(hashCommand.value) != defines.end())
						writeLine = false;
				}
				else if (hashCommand.command == "#if")
				{
					inIf = true;

					if (hashCommand.value2 != "")
					{
						StringParser::ReplaceAll(hashCommand.value, defines);
						StringParser::ReplaceAll(hashCommand.value3, defines);

						if (hashCommand.value2 == "==")
							writeLine = defines[hashCommand.value] == hashCommand.value3;
						else if (hashCommand.value2 == "!=")
							writeLine = defines[hashCommand.value] != hashCommand.value3;
						else if (hashCommand.value2 == ">")
							writeLine = stof(hashCommand.value) > stof(hashCommand.value3);
						else if (hashCommand.value2 == ">=")
							writeLine = stof(hashCommand.value) >= stof(hashCommand.value3);
						else if (hashCommand.value2 == "<")
							writeLine = stof(hashCommand.value) < stof(hashCommand.value3);
						else if (hashCommand.value2 == "<=")
							writeLine = stof(hashCommand.value) <= stof(hashCommand.value3);
					}
					else
						writeLine = defines[hashCommand.value] != "0" && defines[hashCommand.value] != "false" && defines[hashCommand.value] != "";
				}
				else if (hashCommand.command == "#switch")
				{
					inSwitch = true;
					switchSucceeded = false;
					switchCondition = hashCommand.value;
					StringParser::ReplaceAll(switchCondition, defines);
				}
				else
				{
					StringParser::ReplaceAll(line, defines);
					output += line + "\n";
				}
			}
		}

		// remove this file from the includes list, make sure we can reinclude this file
		includes.push_back(path);

		return output;
	}

	std::vector<HashCommand> CPreProcessor::RetrieveHashCommands(const std::string& string)
	{
		std::vector<HashCommand> commands;

		std::string line;
		std::ifstream stream(string.c_str());

		while (getline(stream, line))
		{
			HashCommand command = CPreProcessor::RetrieveHashCommand(line);

			if (command.command != "")
				commands.push_back(command);
		}

		// all is fine
		return commands;
	}

	HashCommand CPreProcessor::RetrieveHashCommand(std::string& string)
	{
		HashCommand command;

		uint hashCommandPos;
		// make sure this line is not commented
		uint commentPos = string.find("//");

		if ((hashCommandPos = string.find("#")) != std::string::npos && hashCommandPos < commentPos)
		{
			string = string.substr(hashCommandPos, string.length() - hashCommandPos);

			std::stringstream valuesStream(string.c_str());
			std::string item;
			while (getline(valuesStream, item, ' '))
			{
				// if this is an empty part, continue to the next
				if (item == "")
					continue;

				// if we find a comment we will ignore the rest of the line
				if (item.substr(0, 2) == "//")
					break;

				// get first container to fill with data
				std::string* value = &command.command;
				if (*value != "")
					value = command.value == "" ? &command.value : &command.value2;

				if (*value != "")
					value = command.value3 == "" ? &command.value3 : &command.value2;

				if (*value == "")
				{
					// trim off all the spaces
					StringParser::Trim(item);

					// remove quotes from the beginning
					if (item[0] == '"')
						item = item.substr(1, item.length() - 1);

					// remove quotes from the end
					if (item[item.length() - 1] == '"')
						item = item.substr(0, item.length() - 1);

					*value = item;
				}
				else
					break; // 2 values are already written, stop parsing this line
			}
		}

		return command;
	}
}