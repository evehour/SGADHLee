#pragma once

class ICommandPattern
{
public:
	enum ICOMMAND_PATTERN_TYPE
	{
		ICP_CREATE,
		ICP_REMOVE,
		ICP_MODIFY,
		ICP_MAX
	};

	virtual void execute() = 0;

	virtual void undo() = 0;
	virtual void redo() = 0;

	ICOMMAND_PATTERN_TYPE GetType() { return type; }

protected:
	ICOMMAND_PATTERN_TYPE type;
};
