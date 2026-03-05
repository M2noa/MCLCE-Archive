/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

class InitOnce {
public:
	InitOnce(const std::function<void()>& initFunction = std::function<void()>());

	void setInitAction(const std::function<void()>& initFunction);
	void initOnce();
private:
	bool mInitialized;
	std::function<void()> mInitFunction;
};