#ifndef __CORNER_DETECT_H__
#define __CORNER_DETECT_H__

#include "hal_gray.h"

/* ---- Corner detection state machine ----
 *
 * 4-stage sequence for right-angle corner detection:
 *   Stage 1 (ENTER):    edge sensors all black -> line curves away
 *   Stage 2 (VOID):     all sensors white -> car crossing the gap
 *   Stage 3 (REACQUIRE): middle sensors re-detect line -> car is turning back
 *   Stage 4 (STRAIGHTEN): outer clear, center on line -> corner confirmed
 *
 * Only when all 4 stages complete in order is a corner "confirmed."
 * Each stage has a timeout guard; if the next condition does not arrive
 * in time the state machine resets to IDLE to prevent false positives. */

typedef enum {
    CORNER_STAGE_IDLE = 0,       /* waiting for corner entry condition */
    CORNER_STAGE_ENTER,          /* stage 1: edge sensors all black */
    CORNER_STAGE_VOID,           /* stage 2: all sensors white (crossing gap) */
    CORNER_STAGE_REACQUIRE,      /* stage 3: middle sensors re-detect line */
    CORNER_STAGE_STRAIGHTEN      /* stage 4: corner confirmed, returning to straight */
} CornerStage;

typedef enum {
    CORNER_EVENT_NONE = 0,
    CORNER_EVENT_LEFT_CONFIRMED,    /* right-angle LEFT turn completed */
    CORNER_EVENT_RIGHT_CONFIRMED    /* right-angle RIGHT turn completed */
} CornerEvent;

typedef struct {
    CornerStage stage;           /* current stage in the 4-stage sequence */
    CornerEvent pending_event;   /* confirmed corner event (one-shot, cleared next tick) */
    int8_t      turn_direction;  /* +1 = right turn, -1 = left turn, 0 = unknown */
    uint16_t    stage_ticks;     /* ticks elapsed in current stage */
    uint16_t    void_ticks;      /* consecutive ticks with all-white in Stage 2 */
} CornerDetector;

/* ---- public API ---- */
void        corner_detect_init(CornerDetector *d);
void        corner_detect_reset(CornerDetector *d);
CornerEvent corner_detect_update(CornerDetector *d, uint16_t sensor_state,
                                 uint8_t on_line, uint8_t sensor_count);

#endif /* __CORNER_DETECT_H__ */
