#include <xc.h>
#include <stdint.h>

#include "Event.h"

#define MAX_SUBSCRIPTIONS 32
#define MAX_EVENTS 8

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
	// TODO: IF NO MORE SUBSCRIPTIONS...THEN DON'T OVERWRITE ANYTHING (TEST)
	// TODO: IF HOLES IN SUBSCRIPTIONS...THEN FILL THE HOLE (TEST)

	uint8_t i;
	for (i = 0; i < MAX_SUBSCRIPTIONS; i++)
	{
		if (!subscriptions[i])
		{
			subscriptions[i] = subscription;
			return;
		}
	}
}

void eventUnsubscribe(const struct EventSubscription *const subscription)
{
	// TODO
}

void eventPublish(EventType type, const void *const args)
{
	events[eventsWriteIndex].type = type;
	events[eventsWriteIndex].args = args;
	if (++eventsWriteIndex >= MAX_EVENTS)
		eventsWriteIndex = 0;
}

void eventDispatchNext(void)
{
	if (eventsReadIndex == eventsWriteIndex)
		return;

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
}
