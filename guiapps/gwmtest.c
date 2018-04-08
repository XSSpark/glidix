/*
	Glidix GUI

	Copyright (c) 2014-2017, Madd Games.
	All rights reserved.
	
	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:
	
	* Redistributions of source code must retain the above copyright notice, this
	  list of conditions and the following disclaimer.
	
	* Redistributions in binary form must reproduce the above copyright notice,
	  this list of conditions and the following disclaimer in the documentation
	  and/or other materials provided with the distribution.
	
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
	FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
	SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
	CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
	OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <libddi.h>
#include <libgwm.h>
#include <stdio.h>
#include <assert.h>

enum
{
	SYM_BUTTON1 = GWM_SYM_USER,
	SYM_BUTTON2,
	SYM_BUTTON3,
	
	SYM_CHECKBOX,
	
	SYM_BUTTON_FRAME1,
	SYM_BUTTON_FRAME2,
};

int myCommandHandler(GWMCommandEvent *ev)
{
	switch (ev->symbol)
	{
	case SYM_BUTTON2:
		gwmMessageBox(NULL, "Example", "You clicked button 2!", GWM_MBUT_OK);
		return GWM_EVSTATUS_CONT;
	case SYM_CHECKBOX:
		if (gwmMessageBox(NULL, "Example", "Are you sure you want to flip this checkbox?", GWM_MBUT_YESNO | GWM_MBICON_QUEST) == GWM_SYM_YES)
		{
			return GWM_EVSTATUS_CONT;
		};
		return GWM_EVSTATUS_OK;
	case SYM_BUTTON_FRAME2:
		gwmMessageBox(NULL, "Example", "You clicked button 2 inside the frame!", GWM_MBUT_OK | GWM_MBICON_WARN);
		return GWM_EVSTATUS_CONT;
	default:
		return GWM_EVSTATUS_CONT;
	};
};

int myHandler(GWMEvent *ev, GWMWindow *win, void *context)
{
	switch (ev->type)
	{
	case GWM_EVENT_COMMAND:
		return myCommandHandler((GWMCommandEvent*) ev);
	default:
		return GWM_EVSTATUS_CONT;
	};
};

int main()
{
	if (gwmInit() != 0)
	{
		fprintf(stderr, "Failed to initialize GWM!\n");
		return 1;
	};
	
	GWMWindow *topWindow = gwmCreateWindow(
		NULL,						// window parent (none)
		"Tutorial Part 3",				// window caption
		GWM_POS_UNSPEC, GWM_POS_UNSPEC,			// window coordinates unspecified
		0, 0,						// window size (will be set later by gwmFit() )
		GWM_WINDOW_HIDDEN | GWM_WINDOW_NOTASKBAR	// hidden, no taskbar icon
	);
	assert(topWindow != NULL);
	
	gwmPushEventHandler(topWindow, myHandler, NULL);
	
	GWMLayout *boxLayout = gwmCreateBoxLayout(GWM_BOX_VERTICAL);
	gwmSetWindowLayout(topWindow, boxLayout);

	GWMWindow *button1 = gwmCreateButtonWithLabel(
		topWindow,			// the parent window
		SYM_BUTTON1,			// the symbol
		"Button 1"			// and finally the label
	);

	gwmBoxLayoutAddWindow(
		boxLayout,			// the layout to add the button to
		button1,			// the button
		0,				// the proportion (use minimum size)
		0,				// the border width
		0				// flags
	);

	GWMWindow *button2, *button3;
	gwmBoxLayoutAddWindow(boxLayout, button2 = gwmCreateButtonWithLabel(topWindow, SYM_BUTTON2, "Button 2"), 0, 0, 0);
	gwmBoxLayoutAddWindow(boxLayout, button3 = gwmCreateButtonWithLabel(topWindow, SYM_BUTTON3, "Button 3"), 0, 0, 0);

	GWMWindow *checkbox = gwmNewCheckbox(topWindow);
	gwmSetCheckboxLabel(checkbox, "Play with me");
	gwmSetCheckboxSymbol(checkbox, SYM_CHECKBOX);
	gwmBoxLayoutAddWindow(boxLayout, checkbox, 0, 0, 0);
	
	GWMWindow *frame = gwmNewFrame(topWindow);
	gwmSetFrameCaption(frame, "Example frame");
	gwmBoxLayoutAddWindow(boxLayout, frame, 0, 0, 0);
	
	GWMWindow *panel = gwmGetFramePanel(frame);
	GWMLayout *panelLayout = gwmCreateBoxLayout(GWM_BOX_HORIZONTAL);
	gwmSetWindowLayout(panel, panelLayout);
	
	gwmBoxLayoutAddWindow(panelLayout, gwmCreateButtonWithLabel(panel, SYM_BUTTON_FRAME1, "Frame button 1"), 0, 0, 0);
	gwmBoxLayoutAddWindow(panelLayout, gwmCreateButtonWithLabel(panel, SYM_BUTTON_FRAME2, "Frame button 2"), 0, 0, 0);
	
	gwmFit(topWindow);
	gwmSetWindowFlags(topWindow, GWM_WINDOW_MKFOCUSED);

	gwmMainLoop();
	gwmDestroyButton(button1);
	gwmDestroyButton(button2);
	gwmDestroyButton(button3);
	gwmDestroyCheckbox(checkbox);
	gwmDestroyBoxLayout(boxLayout);
	gwmDestroyWindow(topWindow);
	gwmQuit();
	return 0;
};
