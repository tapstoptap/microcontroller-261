#ifndef INC_TRAFFIC_LIGHT_H_
#define INC_TRAFFIC_LIGHT_H_

#include "global.h"

#define DEFAULT_RED_S      5    /* seconds; red = amber + green must hold */
#define DEFAULT_AMBER_S    2
#define DEFAULT_GREEN_S    3

void traffic_light_init(void);
void fsm_traffic_light(void);   /* call in the super loop */

/* read-only access (used by the host-side tests / debugging) */
Mode traffic_get_mode(void);
int  traffic_get_duration(LightColor color);

#endif /* INC_TRAFFIC_LIGHT_H_ */
