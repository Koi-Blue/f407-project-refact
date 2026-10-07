#ifndef CAR_MOVE_CHASSIS_H
#define CAR_MOVE_CHASSIS_H

#include <stdint.h>

/*
 * Field point layout:
 *
 *     1 ----- 2 ----- 3     A: start area 1
 *     |       |       |
 *     4 ----- 5 ----- 6
 *     |       |       |
 *     7 ----- 8 ----- 9     B: start area 2
 *
 * A positive step follows the positive direction documented for each path.
 * A negative step moves along the same path in the opposite direction.
 */

/* Path01: positive 3 -> A; negative A -> 3. */
void Car_Path01(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed);

/* Path02: positive 2 -> 3; negative 3 -> 2. */
void Car_Path02(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed);

/* Path03: positive 6 -> 3; negative 3 -> 6. */
void Car_Path03(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed);

/* Path04: positive 9 -> 6; negative 6 -> 9. */
void Car_Path04(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed);

/* Path05: positive 5 -> 6; negative 6 -> 5. */
void Car_Path05(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed);

/* Path06: positive 5 -> 2; negative 2 -> 5. */
void Car_Path06(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed);

/* Path07: positive B -> 9; negative 9 -> B. */
void Car_Path07(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed);

/* Path08: positive 1 -> 2; negative 2 -> 1. */
void Car_Path08(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed);

/* Path09: positive 4 -> 1; negative 1 -> 4. */
void Car_Path09(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed);

/* Path10: positive 4 -> 5; negative 5 -> 4. */
void Car_Path10(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed);

/* Path11: positive 8 -> 5; negative 5 -> 8. */
void Car_Path11(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed);

/* Path12: positive 7 -> 8; negative 8 -> 7. */
void Car_Path12(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed);

/* Path13: positive 7 -> 4; negative 4 -> 7. */
void Car_Path13(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed);

/* Path14: positive 8 -> 9; negative 9 -> 8. */
void Car_Path14(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed);

#endif
