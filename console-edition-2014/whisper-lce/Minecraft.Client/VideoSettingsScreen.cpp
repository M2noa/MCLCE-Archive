#include "stdafx.h"
#include "VideoSettingsScreen.h"
#include "SmallButton.h"
#include "SlideButton.h"
#include "Options.h"
#include "ControlsScreen.h"
#include "..\Minecraft.World\net.minecraft.locale.h"
#ifdef _WINDOWS64
#include "KeyboardMouseInput.h"
#endif

VideoSettingsScreen::VideoSettingsScreen(Screen *lastScreen, Options *options)
{
	this->title = L"Video Settings"; // 4J - added
    this->lastScreen = lastScreen;
    this->options = options;
    this->scrollOffset = 0;
}

void VideoSettingsScreen::init()
{
    Language *language = Language::getInstance();
    this->title = language->getElement(L"options.videoTitle");

	// 4J - this was as static array but moving it into the function to remove any issues with static initialisation order
	const Options::Option *items[8] = {
			        Options::Option::GRAPHICS, Options::Option::RENDER_DISTANCE_SLIDER, Options::Option::AMBIENT_OCCLUSION, Options::Option::FRAMERATE_LIMIT, Options::Option::ANAGLYPH, Options::Option::VIEW_BOBBING,
			        Options::Option::GUI_SCALE, Options::Option::ADVANCED_OPENGL
	};

	for (int position = 0; position < 8; position++)
	{
		const Options::Option *item = items[position];
        if (!item->isProgress())
		{
            buttons.push_back(new SmallButton(item->getId(), width / 2 - 155 + position % 2 * 160, height / 6 + 24 * (position >> 1) + scrollOffset, item, options->getMessage(item)));
        }
		else
		{
            buttons.push_back(new SlideButton(item->getId(), width / 2 - 155 + position % 2 * 160, height / 6 + 24 * (position >> 1) + scrollOffset, item, options->getMessage(item), options->getProgressValue(item)));
        }
    }

//        buttons.add(new Button(VIDEO_BUTTON_ID, width / 2 - 100, height / 6 + 24 * 4 + 12, language.getElement("options.video")));
//        buttons.add(new Button(CONTROLS_BUTTON_ID, width / 2 - 100, height / 6 + 24 * 5 + 12, language.getElement("options.controls")));
    buttons.push_back(new Button(200, width / 2 - 100, height / 6 + 24 * 7 + scrollOffset, language->getElement(L"gui.done")));

}

void VideoSettingsScreen::buttonClicked(Button *button)
{
    if (!button->active) return;
    if (button->id < 100 && (dynamic_cast<SmallButton *>(button) != NULL))
	{
        options->toggle(((SmallButton *) button)->getOption(), 1);
        button->msg = options->getMessage(Options::Option::getItem(button->id));
    }
    if (button->id == 200)
	{
        minecraft->options->save();
        minecraft->setScreen(lastScreen);
    }

    ScreenSizeCalculator ssc(minecraft->options, minecraft->width, minecraft->height);
    int screenWidth = ssc.getWidth();
    int screenHeight = ssc.getHeight();
    Screen::init(minecraft, screenWidth, screenHeight);	// 4J - was this.init
}

void VideoSettingsScreen::render(int xm, int ym, float a)
{
    renderBackground();
    drawCenteredString(font, title, width / 2, 20, 0xffffff);
    
    // Display current FPS in the menu
    if (minecraft && !minecraft->fpsString.empty())
    {
        drawString(font, minecraft->fpsString, 2, 2, 0xffffff);
    }
    
    Screen::render(xm, ym, a);
}

void VideoSettingsScreen::mouseEvent()
{
#ifdef _WINDOWS64
    // Handle mouse wheel scrolling
    int wheelDelta = g_KBMInput.GetMouseWheel();
    if (wheelDelta != 0)
    {
        scrollOffset += (wheelDelta > 0) ? 20 : -20;
        
        // Clamp scroll offset
        if (scrollOffset > 0) scrollOffset = 0;
        if (scrollOffset < -200) scrollOffset = -200;
        
        // Reinitialize buttons with new scroll offset
        buttons.clear();
        init();
    }
#endif
    Screen::mouseEvent();
}
