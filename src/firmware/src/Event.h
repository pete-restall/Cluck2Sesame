#ifndef __CLUCK3SESAME_SRC_EVENT_H
#define __CLUCK3SESAME_SRC_EVENT_H
#include <stdint.h>

#ifdef TEST
#define EMPTY_EVENT_ARGS uint8_t nothingForCmock;
#else
#define EMPTY_EVENT_ARGS
#endif

#define EVENT_TYPE_SYSTEM_MASK ((EventType) 0b10000000)

typedef uint8_t EventType;

struct Event
{
	EventType type;
	void *state;
	void const *args;
};

typedef void (*EventHandler)(const struct Event *event);

struct EventSubscription
{
	EventType type;
	EventHandler handler;
	void *state;
};

extern void eventInitialise(void);
extern void eventSubscribe(const struct EventSubscription *const subscription);
extern void eventUnsubscribe(const struct EventSubscription *const subscription);
extern void eventPublish(EventType type, const void *const args);
extern int8_t eventDispatchNext(void);

#endif
