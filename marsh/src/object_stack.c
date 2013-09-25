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

#include "helper/fatal.h"
#include "helper/log.h"
#include "helper/checks.h"
#include "helper/stack.h"

#include "object_stack.h"
#include "widget.h"
#include "dimension.h"

struct s_object_stack
{
	my_log_t * log;
	my_stack_t *stack;
	MODULE_PRIVATE_DATA_DECLARATION;
};

#define SIGNATURE_OBJECT_STACK (ADDRESS_TO_SIGNATURE_CAST)&object_stack_create

object_stack_t * object_stack_create(void)
{
	object_stack_t * instance;

	instance = (object_stack_t *)calloc(1, sizeof(object_stack_t));
	MEMORY_ALLOC_CHECK(instance);

	INSTANCE_SET(instance, SIGNATURE_OBJECT_STACK);

	instance->log = my_log_create("objectStack", ERROR);
	instance->stack = stack_create(sizeof (widget_t *));

	return instance;
}

void object_stack_destroy(object_stack_t * const instance)
{
	PTR_CHECK(instance, "objectStack");
	INSTANCE_CHECK(instance, SIGNATURE_OBJECT_STACK, "objectStack");

	stack_destroy(instance->stack);
	my_log_destroy(instance->log);
	free(instance);
}

void object_stack_add(object_stack_t * instance, widget_t *graphic_obj)
{
	PTR_CHECK(instance, "objectStack");
	INSTANCE_CHECK(instance, SIGNATURE_OBJECT_STACK, "objectStack");

	stack_add(instance->stack, (BUFFER_PTR_RDOLY)&graphic_obj);
}

widget_t * object_stack_get_object(object_stack_t * instance, uint32_t index)
{
	widget_t * return_object;

	PTR_CHECK_RETURN (instance, "objectStack", NULL);
	INSTANCE_CHECK_RETURN(instance, SIGNATURE_OBJECT_STACK, "objectStack", NULL);

	if (index >= stack_size(instance->stack))
		return NULL;

	stack_get(instance->stack, (BUFFER_PTR)&return_object, index);

	return return_object;
}

widget_t * object_stack_get_top_object(object_stack_t * instance, dim_t x, dim_t y)
{
	uint32_t stack_iterator;
	widget_t * object;
	pos_t position;

	PTR_CHECK_RETURN (instance, "objectStack", NULL);
	INSTANCE_CHECK_RETURN(instance, SIGNATURE_OBJECT_STACK, "objectStack", NULL);

	stack_iterator = stack_size(instance->stack);

	while(stack_iterator--)
	{
		stack_get(instance->stack, (BUFFER_PTR)&object, stack_iterator);
		position = widget_get_position(object);

		if (position.start.x <= x && position.start.y <= y)
			if (position.end.x >= x && position.end.y >= y)
				return object;
	}

	return NULL;
}

uint32_t object_stack_get_object_index(object_stack_t * instance, const widget_t * object)
{
	uint32_t stack_iterator;
	widget_t * stack_object;

	PTR_CHECK_RETURN (instance, "objectStack", OBJECTSTACK_INVALID_INDEX);
	INSTANCE_CHECK_RETURN(instance, SIGNATURE_OBJECT_STACK, "objectStack", OBJECTSTACK_INVALID_INDEX);

	stack_iterator = stack_size(instance->stack);

	while(stack_iterator--)
	{
		stack_get(instance->stack, (BUFFER_PTR)&stack_object, stack_iterator);
		if (object == stack_object)
				return stack_iterator;
	}

	my_log(WARNING, __FILE__, __LINE__, "object_stack_get_object_index: object was not found", instance->log);
	return OBJECTSTACK_INVALID_INDEX;
}

uint32_t object_stack_size(object_stack_t * instance)
{
	PTR_CHECK_RETURN (instance, "objectStack", OBJECTSTACK_INVALID_INDEX);
	INSTANCE_CHECK_RETURN(instance, SIGNATURE_OBJECT_STACK, "objectStack", OBJECTSTACK_INVALID_INDEX);

	return stack_size(instance->stack);
}

widget_t *object_stack_get_object_on_top(object_stack_t* instance, dim_t x, dim_t y)
{
	uint32_t stack_iterator;
	widget_t *object;

	PTR_CHECK_RETURN (instance, "objectStack", NULL);
	INSTANCE_CHECK_RETURN(instance, SIGNATURE_OBJECT_STACK, "objectStack", NULL);

	stack_iterator = stack_size(instance->stack);

	while(stack_iterator--)
	{
		stack_get(instance->stack, (BUFFER_PTR)&object, stack_iterator);
		if (dimension_contains(widget_get_dimension(object), x, y))
				return object;
	}

	return NULL;
}
