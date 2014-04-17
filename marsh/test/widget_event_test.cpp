/*
 *  Copyright (C) 2013 by Felipe Lavratti
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of
 *  this software and associated documentation files (the "Software"), to deal in the
 *  Software without restriction, including without limitation the rights to use,
 *  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
 *  Software, and to permit persons to whom the Software is furnished to do so,
 *  subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 *  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 *  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 *  AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <cstring>

#include "CppUTest/TestHarness.h"
#include "CppUTest/MemoryLeakDetector.h"

#include "mocks/terminal_intercepter.h"

extern "C" {
#include "types.h"
#include "widget_event.h"
#include "widget_event.c"
}

TEST_GROUP(widget_event)
{
	void setup()
	{
	}

	void teardown()
	{
		DISABLE_INTERCEPTION;
	}
};

TEST(widget_event, leaks)
{
	widget_t wid;
	widget_event_init(&wid.event_handler_list);

	CHECK_TRUE(0 == widget_event_install_handler(&wid, 13, NULL));
	CHECK_FALSE(-1 == widget_event_install_handler(&wid, 13, NULL));

	widget_event_deinit(&wid.event_handler_list);
}

static bool consume_handler(widget_t * wid, event_t * event)
{
	return true;
}

TEST(widget_event, consume)
{
	widget_t * wid = widget_new(NULL, NULL, NULL, NULL);
	widget_new(wid, NULL, NULL, NULL);

	widget_event_install_handler(widget_child(wid), event_code_click, consume_handler);

	event_t * event = event_new(event_code_click, NULL, NULL);

	enum e_event_result result = widget_event_commit(wid, event);
	CHECK_EQUAL(event_consumed, result);

	widget_delete(wid);
}

widget_t * widgets[100];
bool widgets_called[100];

static bool list_handler(widget_t * wid, event_t * event)
{
	for (int i = 0; i < 100; i++)
		if (wid == widgets[i])
			widgets_called[i] = true;

	return false;
}

TEST(widget_event, tree)
{
	memset(widgets, 0x00, sizeof(widgets));
	memset(widgets_called, 0x00, sizeof(widgets_called));

	int total_widgets = 0;

	widgets[total_widgets++] = widget_new(NULL, NULL, NULL, NULL);
	widgets[total_widgets++] = widget_new(widgets[0], NULL, NULL, NULL);
	widgets[total_widgets++] = widget_new(widgets[0], NULL, NULL, NULL);
	widgets[total_widgets++] = widget_new(widgets[0], NULL, NULL, NULL);
	widgets[total_widgets++] = widget_new(widgets[0], NULL, NULL, NULL);
	widgets[total_widgets++] = widget_new(widgets[1], NULL, NULL, NULL);
	widgets[total_widgets++] = widget_new(widgets[1], NULL, NULL, NULL);
	widgets[total_widgets++] = widget_new(widgets[1], NULL, NULL, NULL);
	widgets[total_widgets++] = widget_new(widgets[2], NULL, NULL, NULL);
	widgets[total_widgets++] = widget_new(widgets[3], NULL, NULL, NULL);
	widgets[total_widgets++] = widget_new(widgets[5], NULL, NULL, NULL);
	widgets[total_widgets++] = widget_new(widgets[5], NULL, NULL, NULL);
	widgets[total_widgets++] = widget_new(widgets[6], NULL, NULL, NULL);

	for (int i = 0; i < total_widgets; i++)
		widget_event_install_handler(widgets[i], event_code_press, list_handler);

	event_t * event = event_new(event_code_press, NULL, NULL);

	for (int i = 0; i < total_widgets; i++)
		CHECK_FALSE(widgets_called[i]);

	widget_event_commit(widgets[0], event);

	for (int i = 0; i < total_widgets; i++)
		CHECK_TRUE(widgets_called[i]);

	widget_delete(widgets[0]);
}

