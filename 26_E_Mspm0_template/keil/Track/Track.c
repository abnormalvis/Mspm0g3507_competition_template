#include "Track.h"

uint8_t TrackValue[9];
int32_t Track_out[8];

/* Only 3 track sensors (TRACK_S1, S2, S3) are wired in this template's syscfg.
 * They are mapped to TrackValue[3..5] (left/center/right) so the pattern
 * comparisons in switch{} below still hit center-line cases. Bits 0..2 and
 * 6..7 are forced white (1) since no hardware is connected.
 * Black line -> 0, White -> 1 (TrackValue convention). */
void Get_Track(void)
{
    TrackValue[8] = 0;
    TrackValue[0] = 1;
    TrackValue[1] = 1;
    TrackValue[2] = 1;
    TrackValue[3] = (DL_GPIO_readPins(TRACK_S1_PORT, TRACK_S1_PIN) == 0) ? 1 : 0;
    TrackValue[4] = (DL_GPIO_readPins(TRACK_S2_PORT, TRACK_S2_PIN) == 0) ? 1 : 0;
    TrackValue[5] = (DL_GPIO_readPins(TRACK_S3_PORT, TRACK_S3_PIN) == 0) ? 1 : 0;
    TrackValue[6] = 1;
    TrackValue[7] = 1;
    for (int i = 0; i < 8; i++) {
        TrackValue[8] += TrackValue[i] << i;
    }
    switch (TrackValue[8]) {
        case 0xE7: Track_out[0]=30; Track_out[1]=30; break;
        case 0x7F: Track_out[0]=10; Track_out[1]=30; break;
        case 0xBF: Track_out[0]=10; Track_out[1]=30; break;
        case 0xDF: Track_out[0]=15; Track_out[1]=30; break;
        case 0xEF: Track_out[0]=20; Track_out[1]=30; break;
        case 0xF7: Track_out[0]=30; Track_out[1]=20; break;
        case 0xFB: Track_out[0]=30; Track_out[1]=15; break;
        case 0xFD: Track_out[0]=30; Track_out[1]=10; break;
        case 0xFE: Track_out[0]=30; Track_out[1]=10; break;
        case 0x9F: Track_out[0]=10; Track_out[1]=30; break;
        case 0xCF: Track_out[0]=10; Track_out[1]=30; break;
        case 0xF3: Track_out[0]=30; Track_out[1]=10; break;
        case 0xF9: Track_out[0]=30; Track_out[1]=10; break;
        case 0x07: Track_out[3]=1;                   break;
        case 0x0F: Track_out[3]=1;                   break;
        case 0x1F: Track_out[3]=1;                   break;
    }
}
