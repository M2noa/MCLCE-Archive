#pragma once

// 4J Stu Richard has added the class like this to the stubs, although the original Java code
// had an enum with a "surrounding" data member, which is set to the value we are setting the enum to

class LightLayer
{
public:
	enum variety
	{
		Sky = 15,
		Block = 0,
	};
};