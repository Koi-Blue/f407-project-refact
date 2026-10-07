#ifndef __CHASSIS_MOVE_H
#define __CHASSIS_MOVE_H

#include <stdint.h>

/* step > 0: right; step < 0: left. */
void Chassis_Move_X(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed);

/* step > 0: forward; step < 0: backward. */
void Chassis_Move_Y(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed);

/* step > 0: rotate right; step < 0: rotate left. */
void Chassis_Move_Turn(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed);

#endif
