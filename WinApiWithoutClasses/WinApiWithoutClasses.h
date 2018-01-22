#pragma once

#include "resource.h"

enum Direction
{
	//! Indicates that the cursor moves to the left
	LEFT,

	//! Indicates that the cursor moves to the up
	UP,

	//! Indicates that the cursor moves to the right
	RIGHT,

	//! Indicates that the cursor moves to the down
	DOWN
};

//! Set of processed buttons
enum Key
{
	//! Indicates that the HOME button is being processed
	HOME,

	//! Indicates that the END button is being processed
	END,

	//! Indicates that the BACKSPASE button is being processed
	BACKSPASE,

	//! Indicates that the DEL button is being processed
	DEL
};