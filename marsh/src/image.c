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

#include "helper/checks.h"
#include "helper/number.h"
#include "helper/log.h"
#include "color.h"
#include "dimension.h"
#include "canvas.h"
#include "widget.h"
#include "widget_owner.h"
#include "image.h"
#include "bitmap_data/bitmap_data.h"


struct s_image_instance
{
	bitmap_t *bitmap;

	my_log_t * log;
	widget_t *glyph;
	widget_owner_t *self_reference;

	MODULE_PRIVATE_DATA_DECLARATION;
};

#define SIGNATURE_IMAGE (ADDRESS_TO_SIGNATURE_CAST)&image_create

static BOOL ready_to_draw(image_t * obj)
{
	if (!dimension_good(widget_get_dimension(obj->glyph)))
		return FALSE;

	if (obj->bitmap == NULL)
		return FALSE;

	return TRUE;
}

static void draw(image_t * obj)
{
	PTR_CHECK(obj, "image");
	INSTANCE_CHECK(obj, SIGNATURE_IMAGE, "image");

	if (!ready_to_draw(obj))
	{
		my_log(ERROR, __FILE__, __LINE__, "Object not initialized properly, can't draw.", obj->log);
		return;
	}

	canvas_t *canv = canvas_create(widget_get_dimension(obj->glyph));

	if (obj->bitmap->bitmap_data_width == BITMAP_BUFFER_16BPP)
	{
		canvas_draw_bitmap(canv, (BUFFER_PTR_RDOLY)obj->bitmap->bitmap, 0, 0, obj->bitmap->width, obj->bitmap->height);
	}
	else
	{
		my_log(ERROR, __FILE__, __LINE__, "Bad bitmap_data_width", obj->log);
	}

	canvas_destroy(canv);
}

image_t * image_create()
{
	image_t * obj = (image_t *) malloc(sizeof(struct s_image_instance));
	MEMORY_ALLOC_CHECK(obj);
	INSTANCE_SET(obj, SIGNATURE_IMAGE);

	obj->log = my_log_create("Icon", MESSAGE);
	obj->self_reference = widget_owner_create(obj, (void(*)(void *))draw, (void(*)(void *))image_destroy);
	obj->glyph = widget_create(obj->self_reference);
	obj->bitmap = NULL;

	return obj;
}

void image_destroy(image_t * const obj)
{
	PTR_CHECK(obj, "image");
	INSTANCE_CHECK(obj, SIGNATURE_IMAGE, "image");

	my_log_destroy(obj->log);
	widget_destroy(obj->glyph);
	widget_owner_destroy(obj->self_reference);

	INSTANCE_CLEAR(obj);
	free(obj);
}

static void set_size(image_t * obj, dim_t width, dim_t height)
{
	PTR_CHECK(obj, "image");
	INSTANCE_CHECK(obj, SIGNATURE_IMAGE, "image");

	dimension_set_size(widget_get_dimension(obj->glyph), width, height);
	dimension_set_rest_if_possible(widget_get_dimension(obj->glyph));
}

static BOOL is_bitmap_image(bitmap_t* bitmap)
{
	if (bitmap->single_channel)
		return FALSE;

	return TRUE;
}

void image_set_bitmap(image_t * obj, bitmap_t * bitmap)
{
	PTR_CHECK(obj, "image");
	INSTANCE_CHECK(obj, SIGNATURE_IMAGE, "image");

	if (!is_bitmap_image(bitmap))
	{
		LOG_ERROR("image", "Selected bitmap is not an image");
		return;
	}

	obj->bitmap = bitmap;
	set_size(obj, bitmap->width, bitmap->height);
}

void image_set_position(image_t * obj, dim_t x, dim_t y)
{
	PTR_CHECK(obj, "image");
	INSTANCE_CHECK(obj, SIGNATURE_IMAGE, "image");

	dimension_set_start_position(widget_get_dimension(obj->glyph), x, y);
	dimension_set_rest_if_possible(widget_get_dimension(obj->glyph));
}

widget_t *image_get_widget(image_t * const obj)
{
	PTR_CHECK_RETURN (obj, "image", NULL);
	INSTANCE_CHECK_RETURN(obj, SIGNATURE_IMAGE, "image", NULL);

	return obj->glyph;
}