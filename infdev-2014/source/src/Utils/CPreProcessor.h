#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <istream>
#include <cppu/cgc/pointers.h>

#include "General/CommonTypes.h"

namespace Esteem
{

	struct HashCommand
	{
		std::string command;
		std::string value;
		std::string value2;
		std::string value3;

		HashCommand() = default;

		HashCommand(const std::string& command, const std::string& value)
			: command(command), value(value)
		{ }

		HashCommand(const std::string& command, const std::string& value, const std::string& value2)
			: command(command), value(value), value2(value2)
		{ }

		HashCommand(const std::string& command, const std::string& value, const std::string& value2, const std::string& value3)
			: command(command), value(value), value2(value2), value3(value3)
		{ }
	};

	/// \brief A preprocessor for use in the engine
	/// \author Y. Schaeffer
	/// \date June 2015
	class CPreProcessor
	{
	public:
		/// \brief	Process a file stream.
		/// \param stream				Stream to parse.
		/// \param path					Path of the file we are processing.
		/// \param workingDirectory		Which directory are we working from?
		/// \param defines				All defines.
		/// \param includes				All recent included files, anti cyclic inclusion: works in a hierarchal way, adds it when parsing and removes it when done parsing.
		/// \return						String, returns the stream fully processed.
		static std::string ProcessStreamToString(cgc::raw_ptr<std::istream> stream, std::string path, const std::string& workingDirectory, std::unordered_map<std::string, std::string>& defines, std::vector<std::string>& includes);

		/// \brief	Finds all has commands per line in a string and parses them.
		/// \param string	String to parse.
		/// \return			Returns a vector with HashCommand, vector will be empty if no valid commands are found.
		static std::vector<HashCommand> RetrieveHashCommands(const std::string& string);

		/// \brief	Finds a single command in the given string and returns it into an HashCommand.
		/// \param string	String to parse.
		/// \return			Returns an HashCommand with empty string fields, if a command is found the command field will at least be filled with it.
		static HashCommand RetrieveHashCommand(std::string& string);
	};
}