#include <xc.h>
#include <stdint.h>

#include "Event.h"

#define MAX_SUBSCRIPTIONS 32
#define MAX_EVENTS 16

static const struct EventSubscription *subscriptions[MAX_SUBSCRIPTIONS];

static struct EventQueueEntry
{
	EventType type;
	const void *args;
} events[MAX_EVENTS];

static uint8_t eventsReadIndex, eventsWriteIndex;

void eventInitialise(void)
{
	uint8_t i;
	for (i = 0; i < MAX_SUBSCRIPTIONS; i++)
		subscriptions[i] = (struct EventSubscription *) 0;

	eventsReadIndex = 0;
	eventsWriteIndex = 0;
}

void eventSubscribe(const struct EventSubscription *const subscription)
{
	int8_t freeIndex = -1;
	uint8_t i;
	for (i = 0; i < MAX_SUBSCRIPTIONS; i++)
	{
		if (
			(freeIndex < 0 && !subscriptions[i]) ||
			subscriptions[i] == subscription)
				freeIndex = i;
	}

	if (freeIndex >= 0)
		subscriptions[freeIndex] = subscription;
}

void eventUnsubscribe(const struct EventSubscription *const subscription)
{
	uint8_t i;
	for (i = 0; i < MAX_SUBSCRIPTIONS; i++)
	{
		if (subscriptions[i] == subscription)
		{
			subscriptions[i] = (const struct EventSubscription *) 0;
			return;
		}
	}
}

void eventPublish(EventType type, const void *const args)
{
	events[eventsWriteIndex].type = type;
	events[eventsWriteIndex].args = args;
	if (++eventsWriteIndex >= MAX_EVENTS)
		eventsWriteIndex = 0;
}

int8_t eventDispatchNext(void)
{
	if (eventsReadIndex == eventsWriteIndex)
		return 0;

	static struct Event event;
	event.type = events[eventsReadIndex].type;
	event.args = events[eventsReadIndex].args;

	uint8_t i;
	for (i = 0; i < MAX_SUBSCRIPTIONS; i++)
	{
		if (
			!subscriptions[i] ||
			subscriptions[i]->type != event.type ||
			!subscriptions[i]->handler)
				continue;

		event.state = subscriptions[i]->state;
		subscriptions[i]->handler(&event);
	}

	if (++eventsReadIndex >= MAX_EVENTS)
		eventsReadIndex = 0;

	return 1;
}
