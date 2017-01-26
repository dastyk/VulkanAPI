#include "Console.h"

namespace DebugUtils
{

	DebugConsole::DebugConsole()
	{
	}


	DebugConsole::~DebugConsole()
	{
	}

	/*Copies the default Command Functions to the 0 slot in the commands map*/
	const void DebugConsole::Init(const Command_Structure* defaultCommandFunctions)
	{
#ifdef _DEBUG
		if (defaultCommandFunctions->commandFunction == nullptr)
			throw std::exception("Invalid command function pointer.");
		if (defaultCommandFunctions->commandHelpFunction == nullptr)
			throw std::exception("Invalid command help function pointer.");
#endif
		_commands[0] = *defaultCommandFunctions;
		return void();
	}
	const void DebugConsole::Shutdown()
	{
		return void();
	}
	/*Hash the identifier and add the command*/
	const void DebugConsole::AddCommand(const char* identifier, const Command_Structure * command)
	{

		uint32_t hash = std::hash<std::string>{}(identifier);

#ifdef _DEBUG
		if (command->commandFunction == nullptr)
			throw std::exception("Invalid command function pointer.");

#endif
		
			
		_commands[hash] = *command;
		if (command->commandHelpFunction == nullptr)
			_commands[hash].commandHelpFunction = _commands[0].commandHelpFunction;
		
		return void();
	}
	/* Adds the command with the already hashed identifer*/
	const void DebugConsole::AddCommand(uint32_t identifier, const Command_Structure * command)
	{
#ifdef _DEBUG
		if (command->commandFunction == nullptr)
			throw std::exception("Invalid command function pointer.");

#endif


		_commands[identifier] = *command;
		if (command->commandHelpFunction == nullptr)
			_commands[identifier].commandHelpFunction = _commands[0].commandHelpFunction;


		return void();
	}

	/*Splits the command and calls the correct command function */
	const void DebugConsole::InterpretCommand(char * command)
	{

		int argc = 0;
		char* argv[MAX_ARGUMENTS];
		_ParseCommandString(command, argc, argv);


		if (argc > 0)
		{
			uint32_t hash = std::hash<std::string>{}(argv[0]);

			auto& find = _commands.find(hash);
			if (find != _commands.end()) {
				for (int i = 1; i < argc; i++) {
					if (argv[i][0] == '-') {
						if (argv[i][1] == 'h') {
							find->second.commandHelpFunction(find->second.userData, argc, argv);
							return;
						}
					}
				}
				find->second.commandFunction(find->second.userData, argc, argv);
			}
			else
			{
				_commands[0].commandFunction(find->second.userData, argc, argv);
			}
		}

		return void();
	}
	/* Split the command string into the arg array*/
	const void DebugConsole::_ParseCommandString(char* command, int& argc, char** argv)
	{

		argc = 0;
		int j = 0;
		int i = 0;
		char c = command[i];
		while (c != '\0')
		{
			while (!(c == ' ' || c == '\0'))
			{
				i++;
				c = command[i];
			}
			argv[argc] = &command[j];
			argc++;
			if (command[i] != '\0')
			{
				command[i] = '\0';
				i++;
				j = i;
				c = command[i];
				if (argc >= MAX_ARGUMENTS)
				{
					break;
				}
				
			}
			
			
		}

	}

}