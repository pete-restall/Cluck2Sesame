#ifndef __CLUCK2SESAME_TESTS_UNITY_CONFIG_H
#define __CLUCK2SESAME_TESTS_UNITY_CONFIG_H

#define UNITY_EXCLUDE_SETJMP_H
#define UNITY_OUTPUT_START() unityBeforeRunHook()
#define UNITY_OUTPUT_COMPLETE() unityBreakpointHook()

extern void unityBeforeRunHook(void);
extern void unityBreakpointHook(void);

#endif
