#pragma once

class ICommand
{
public:
	virtual ~ICommand() {}

	virtual int Execute() = 0;
	virtual int Undo() = 0;
};
