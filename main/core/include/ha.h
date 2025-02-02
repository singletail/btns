#ifndef _ha_H
#define _ha_H

#include "include.h"

extern const char *HA_TOPIC_AVAIL;
extern const char *HA_TOPIC_STATE;
extern const char *HA_TOPIC_DISCOVERY;
extern const char *HA_TOPIC_CMD;

extern const char *HA_MSG_AVAIL_ONLINE;
extern const char *HA_MSG_AVAIL_OFFLINE;

void ha_init(void);
void ha_reset_network(void);
bool ha_publish_discovery(void);
bool ha_publish_availability(void);
bool ha_publish_state(void);

#endif
