#include "Framework.h"
#include "CommandCenter.h"

CommandCenter::CommandCenter()
{
}

CommandCenter::~CommandCenter()
{
	for (UINT i = 0; i < commands.size(); i++)
		SAFE_DELETE(commands[i]);

	for (UINT i = 0; i < undos.size(); i++)
		SAFE_DELETE(undos[i]);
}

int CommandCenter::ExecuteCommand(ICommand * command)
{
	if (command == NULL)
	{
		assert(false);
	}

	if (undos.size() > 0)
	{
		vector<class ICommand *>::iterator it = undos.begin();

		while(it != undos.end())
		{
			SAFE_DELETE((*it));
			it = undos.erase(it);
		}
	}

	commands.push_back(command);

	command->Execute();

	return 0;
}

int CommandCenter::Undo()
{
	if (commands.size() < 1)
		return 1;

	ICommand* command = commands.back();
	commands.pop_back();

	undos.push_back(command);

	command->Undo();

	return 0;
}

int CommandCenter::Redo()
{
	if (undos.size() < 1)
		return 1;

	ICommand* command = undos.back();
	undos.pop_back();

	commands.push_back(command);

	command->Execute();

	return 0;
}
