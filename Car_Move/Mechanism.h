#ifndef CAR_MOVE_MECHANISM_H
#define CAR_MOVE_MECHANISM_H

#include <stdint.h>

/* CN6: step > 0 raises; step < 0 lowers. */
void Car_Lift(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed);

/* CN8: step > 0 extends; step < 0 retracts. */
void Car_Extend(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed);

#endif
