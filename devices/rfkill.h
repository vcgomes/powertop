#ifndef _INCLUDE_GUARD_RFKILL_H
#define _INCLUDE_GUARD_RFKILL_H


#include "device.h"

class rfkill: public device {
	int start_soft, end_soft;
	int start_hard, end_hard;
	char sysfs_path[4096];
	char name[4096];
public:

	rfkill(char *_name, char *path);

	virtual void start_measurement(void);
	virtual void end_measurement(void);

	virtual double	utilization(void); /* percentage */

	virtual const char * class_name(void) { return "rfkill";};

	virtual const char * device_name(void);
	virtual double power_usage(struct result_bundle *result, struct parameter_bundle *bundle);
};

extern void create_all_rfkills(void);


#endif