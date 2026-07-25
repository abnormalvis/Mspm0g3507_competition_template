#include "corner_detect.h"

/* ====================================================================
 * Tunable constants
 * ==================================================================== */
#define CNR_VOID_DEBOUNCE     3     /* consecutive all-white ticks to enter Stage 2 */
#define CNR_STAGE_TIMEOUT   150     /* max ticks in any stage before reset to IDLE */
#define CNR_STRAIGHTEN_HOLD   5     /* ticks Stage 4 must hold before confirming */

/* ====================================================================
 * Zone definitions — scaled by GRAY_CHANNEL_COUNT
 *
 *   LEFT zone  : outer sensors that go all-black when entering a corner
 *   CENTER zone: innermost 1-2 sensors, used for Stage 4 straighten check
 *   RIGHT zone : mirror of LEFT on the opposite side
 *   MIDDLE zone: broad band used for Stage 3 re-acquire check
 *
 * Channel 6 and 9 on 16ch are intentional dead zones between regions.
 * ==================================================================== */
#if defined(GRAY_SENSOR_16CH)
    #define CNR_LEFT_END     5
    #define CNR_CENTER_START 7
    #define CNR_CENTER_END   8
    #define CNR_RIGHT_START 10
    #define CNR_MIDDLE_START 4
    #define CNR_MIDDLE_END  11
#elif defined(GRAY_SENSOR_12CH)
    #define CNR_LEFT_END     4
    #define CNR_CENTER_START 5
    #define CNR_CENTER_END   6
    #define CNR_RIGHT_START  7
    #define CNR_MIDDLE_START 3
    #define CNR_MIDDLE_END   8
#else  /* 8ch default */
    #define CNR_LEFT_END     2
    #define CNR_CENTER_START 3
    #define CNR_CENTER_END   4
    #define CNR_RIGHT_START  5
    #define CNR_MIDDLE_START 2
    #define CNR_MIDDLE_END   5
#endif

/* ---- bitmask builder (all args compile-time constants -> folded to immediates) ---- */
#define CNR_MASK_RANGE(start, end) \
    ((((uint16_t)1u << ((end) + 1)) - 1u) ^ (((uint16_t)1u << (start)) - 1u))

#define CNR_LEFT_MASK   CNR_MASK_RANGE(0, CNR_LEFT_END)
#define CNR_CENTER_MASK CNR_MASK_RANGE(CNR_CENTER_START, CNR_CENTER_END)
#define CNR_RIGHT_MASK  CNR_MASK_RANGE(CNR_RIGHT_START, (GRAY_CHANNEL_COUNT - 1))
#define CNR_MIDDLE_MASK CNR_MASK_RANGE(CNR_MIDDLE_START, CNR_MIDDLE_END)
#define CNR_OUTER_MASK  (CNR_LEFT_MASK | CNR_RIGHT_MASK)

/* ====================================================================
 * Predicate helpers — each inspects gray_state.state bitmask
 * ==================================================================== */

/* Stage 1: LEFT edge sensors all black */
static uint8_t cnr_left_all_black(uint16_t state)
{
    return ((state & CNR_LEFT_MASK) == CNR_LEFT_MASK) ? 1U : 0U;
}

/* Stage 1: RIGHT edge sensors all black */
static uint8_t cnr_right_all_black(uint16_t state)
{
    return ((state & CNR_RIGHT_MASK) == CNR_RIGHT_MASK) ? 1U : 0U;
}

/* Stage 2: every sensor white */
static uint8_t cnr_all_white(uint16_t state)
{
    return (state == 0U) ? 1U : 0U;
}

/* Stage 3: any sensor in the middle band sees black */
static uint8_t cnr_middle_seen(uint16_t state)
{
    return ((state & CNR_MIDDLE_MASK) != 0U) ? 1U : 0U;
}

/* Stage 4: outer zones clear AND at least one center sensor on the line */
static uint8_t cnr_is_straight(uint16_t state)
{
    uint8_t outer_clear = ((state & CNR_OUTER_MASK) == 0U) ? 1U : 0U;
    uint8_t center_on   = ((state & CNR_CENTER_MASK) != 0U) ? 1U : 0U;
    return (outer_clear && center_on) ? 1U : 0U;
}

/* ====================================================================
 * Public API
 * ==================================================================== */

void corner_detect_init(CornerDetector *d)
{
    d->stage         = CORNER_STAGE_IDLE;
    d->pending_event = CORNER_EVENT_NONE;
    d->turn_direction = 0;
    d->stage_ticks   = 0;
    d->void_ticks    = 0;
}

void corner_detect_reset(CornerDetector *d)
{
    corner_detect_init(d);
}

CornerEvent corner_detect_update(CornerDetector *d, uint16_t sensor_state,
                                 uint8_t on_line, uint8_t sensor_count)
{
    CornerEvent result = CORNER_EVENT_NONE;

    /* time-based guards: any stage that lingers too long resets the machine */
    if (d->stage_ticks >= CNR_STAGE_TIMEOUT) {
        d->stage       = CORNER_STAGE_IDLE;
        d->stage_ticks = 0;
        d->void_ticks  = 0;
    }

    switch (d->stage) {

    /* ---- IDLE: waiting for corner entry condition ---- */
    case CORNER_STAGE_IDLE:
        d->void_ticks = 0;
        if (cnr_left_all_black(sensor_state)) {
            d->stage          = CORNER_STAGE_ENTER;
            d->stage_ticks    = 0;
            d->turn_direction = 1;   /* left edge black -> line curves right -> RIGHT turn */
        } else if (cnr_right_all_black(sensor_state)) {
            d->stage          = CORNER_STAGE_ENTER;
            d->stage_ticks    = 0;
            d->turn_direction = -1;   /* right edge black -> line curves left -> LEFT turn */
        }
        break;

    /* ---- ENTER: edge sensors went all-black; now waiting for all-white gap ---- */
    case CORNER_STAGE_ENTER:
        if (cnr_all_white(sensor_state)) {
            d->void_ticks++;
            if (d->void_ticks >= CNR_VOID_DEBOUNCE) {
                d->stage       = CORNER_STAGE_VOID;
                d->stage_ticks = 0;
            }
        } else {
            /* line still visible — keep counting, still in ENTER */
            d->void_ticks = 0;
            d->stage_ticks++;
        }
        break;

    /* ---- VOID: all sensors white; car is crossing the corner gap ---- */
    case CORNER_STAGE_VOID:
        if (cnr_middle_seen(sensor_state)) {
            d->stage       = CORNER_STAGE_REACQUIRE;
            d->stage_ticks = 0;
            d->void_ticks  = 0;
        } else {
            d->stage_ticks++;
            if (!cnr_all_white(sensor_state)) {
                /* non-middle sensor appeared — not a clean re-acquire, reset */
                d->stage       = CORNER_STAGE_IDLE;
                d->stage_ticks = 0;
            }
        }
        break;

    /* ---- REACQUIRE: middle band re-detects the line; waiting for straighten ---- */
    case CORNER_STAGE_REACQUIRE:
        if (cnr_is_straight(sensor_state)) {
            d->stage_ticks++;
            if (d->stage_ticks >= CNR_STRAIGHTEN_HOLD) {
                /* confirmed: corner completed */
                d->stage = CORNER_STAGE_STRAIGHTEN;
                if (d->turn_direction > 0) {
                    result = CORNER_EVENT_RIGHT_CONFIRMED;
                } else {
                    result = CORNER_EVENT_LEFT_CONFIRMED;
                }
                d->pending_event = result;
                /* one-shot: next tick clears the event so the task only sees it once */
            }
        } else {
            d->stage_ticks++;
        }
        break;

    /* ---- STRAIGHTEN: corner confirmed; return to IDLE ---- */
    case CORNER_STAGE_STRAIGHTEN:
        d->stage       = CORNER_STAGE_IDLE;
        d->stage_ticks = 0;
        d->void_ticks  = 0;
        break;

    default:
        d->stage = CORNER_STAGE_IDLE;
        break;
    }

    /* if a confirmed event was already latched, expose it exactly once */
    if (result != CORNER_EVENT_NONE) {
        return result;
    }
    return CORNER_EVENT_NONE;
}
