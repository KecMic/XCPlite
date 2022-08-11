#pragma once
/* xcpLite.h */

/* Copyright(c) Vector Informatik GmbH.All rights reserved.
   Licensed under the MIT license.See LICENSE file in the project root for details. */

#include "xcp_cfg.h"    // Protocol layer configuration
#include "xcptl_cfg.h"  // Transport layer configuration
#include "xcp.h"        // XCP protocol defines
#include "xcpTl.h"      // Transport layer interface



/****************************************************************************/
/* DAQ event information                                                    */
/****************************************************************************/

#ifdef XCP_ENABLE_DAQ_EVENT_LIST

typedef struct {
    const char* name;
    uint32_t size; // ext event size
    uint8_t timeUnit; // timeCycle unit, 1ns=0, 10ns=1, 100ns=2, 1us=3, ..., 1ms=6, ...
    uint8_t timeCycle; // cycletime in units, 0 = sporadic or unknown
    uint16_t sampleCount; // packed event sample count
    uint16_t daqList; // associated DAQ list
    uint8_t priority; // priority 0 = queued, 1 = pushing, 2 = realtime
#ifdef XCP_ENABLE_MULTITHREAD_EVENTS
    MUTEX mutex;
#endif
#ifdef XCP_ENABLE_TEST_CHECKS
    uint64_t time; // last event time stamp
#endif
} tXcpEvent;

#endif


/****************************************************************************/
/* Protocol layer interface                                                 */
/****************************************************************************/

/* Initialization for the XCP Protocol Layer */
extern void XcpInit(void);
extern void XcpStart(void);
extern void XcpDisconnect();

/* Trigger a XCP data acquisition or stimulation event */
extern void XcpEvent(uint16_t event);
extern void XcpEventExt(uint16_t event, uint8_t* base);
extern void XcpEventAt(uint16_t event, uint64_t clock);

/* XCP command processor */
extern void XcpCommand( const uint32_t* pCommand, uint16_t len );

/* Send an XCP event message */
extern void XcpSendEvent(uint8_t evc, const uint8_t* d, uint8_t l);

/* Check status */
extern BOOL XcpIsStarted();
extern BOOL XcpIsConnected();
extern BOOL XcpIsDaqRunning();
extern BOOL XcpIsDaqEventRunning(uint16_t event);
extern uint64_t XcpGetDaqStartTime();
extern uint32_t XcpGetDaqOverflowCount();

/* Time synchronisation */
#ifdef XCP_ENABLE_DAQ_CLOCK_MULTICAST
extern uint16_t XcpGetClusterId();
#endif
#ifdef XCP_ENABLE_PTP
extern void XcpSetGrandmasterClockInfo(uint8_t* id, uint8_t epoch, uint8_t stratumLevel);
#endif

// Event list
#ifdef XCP_ENABLE_DAQ_EVENT_LIST

#define XCP_INVALID_EVENT 0xFFFF

// Clear event list
extern void XcpClearEventList();
// Add a measurement event to event list, return event number (0..MAX_EVENT-1)
extern uint16_t XcpCreateEvent(const char* name, uint32_t cycleTimeNs /* ns */, uint8_t priority /* 0-normal, >=1 realtime*/, uint16_t sampleCount, uint32_t size);
// Get event list
extern tXcpEvent* XcpGetEventList(uint16_t* eventCount);
// Lookup event
extern tXcpEvent* XcpGetEvent(uint16_t event);

#endif


/****************************************************************************/
/* Protocol layer external dependencies                                     */
/****************************************************************************/

// All functions must be thread save

/* Callbacks */
extern BOOL ApplXcpConnect();
extern BOOL ApplXcpPrepareDaq();
extern BOOL ApplXcpStartDaq();
extern void ApplXcpStopDaq();

/* Generate a native pointer from XCP address extension and address */
extern uint8_t* ApplXcpGetPointer(uint8_t addr_ext, uint32_t addr);
extern uint32_t ApplXcpGetAddr(uint8_t* p);
extern uint8_t *ApplXcpGetBaseAddr();

/* Switch calibration page */
#ifdef XCP_ENABLE_CAL_PAGE
extern uint8_t ApplXcpGetCalPage(uint8_t segment, uint8_t mode);
extern uint8_t ApplXcpSetCalPage(uint8_t segment, uint8_t page, uint8_t mode);
#endif

/* DAQ clock */
extern uint64_t ApplXcpGetClock64();
#define CLOCK_STATE_SYNCH_IN_PROGRESS                  (0 << 0)
#define CLOCK_STATE_SYNCH                              (1 << 0)
#define CLOCK_STATE_FREE_RUNNING                       (7 << 0)
#define CLOCK_STATE_GRANDMASTER_STATE_SYNC_IN_PROGRESS (0 << 3)
#define CLOCK_STATE_GRANDMASTER_STATE_SYNC             (1 << 3)
extern uint8_t ApplXcpGetClockState();
#ifdef XCP_ENABLE_PTP
#define CLOCK_STRATUM_LEVEL_UNKNOWN   255
#define CLOCK_STRATUM_LEVEL_ARB       16   // unsychronized
#define CLOCK_STRATUM_LEVEL_UTC       0    // Atomic reference clock
#define CLOCK_EPOCH_TAI 0 // Atomic monotonic time since 1.1.1970 (TAI)
#define CLOCK_EPOCH_UTC 1 // Universal Coordinated Time (with leap seconds) since 1.1.1970 (UTC)
#define CLOCK_EPOCH_ARB 2 // Arbitrary (epoch unknown)
extern BOOL ApplXcpGetClockInfoGrandmaster(uint8_t* uuid, uint8_t* epoch, uint8_t* stratum);
#endif

/* Info (for GET_ID) */
extern uint32_t ApplXcpGetId(uint8_t id, uint8_t* buf, uint32_t bufLen);
#ifdef XCP_ENABLE_IDT_A2L_UPLOAD // Enable GET_ID: A2L content upload to host
extern BOOL ApplXcpReadA2L(uint8_t size, uint32_t addr, uint8_t* data);
#endif

