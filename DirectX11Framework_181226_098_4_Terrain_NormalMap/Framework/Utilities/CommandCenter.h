#pragma once

class CommandCenter
{
public:
	CommandCenter();
	~CommandCenter();

	int ExecuteCommand(class ICommand* command);
	int Undo();
	int Redo();

private:
	vector<class ICommand *> commands;
	vector<class ICommand *> undos;
};