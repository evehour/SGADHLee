#pragma once

class ICommandPattern
{
public:
	virtual void execute() = 0;

	virtual void undo() = 0;
	virtual void redo() = 0;
};

class ICommandPatternTransform
{
public:
	virtual void execute() = 0;
	virtual void undo() = 0;
	virtual void redo() = 0;
};