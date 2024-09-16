/*
 * This file is part of Cleanflight and Betaflight.
 *
 * Cleanflight and Betaflight are free software. You can redistribute
 * this software and/or modify this software under the terms of the
 * GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Cleanflight and Betaflight are distributed in the hope that they
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <stdint.h>

//#include "pg/pg.h"
#include "CoreMinimal.h"
#include "controlrate_profile.generated.h"

#define MAX_RATE_PROFILE_NAME_LENGTH 8u
#define CONTROL_RATE_CONFIG_RC_EXPO_MAX  100
#define CONTROL_RATE_CONFIG_RC_RATES_MAX  255
#define CONTROL_RATE_CONFIG_RATE_MAX  255
#define CONTROL_RATE_CONFIG_RATE_LIMIT_MIN  200
#define CONTROL_RATE_CONFIG_RATE_LIMIT_MAX  1998
#define SETPOINT_RATE_LIMIT_MIN -1998.0f
#define SETPOINT_RATE_LIMIT_MAX 1998.0f
#define RC_RATE_INCREMENTAL 14.54f
#define PWM_RANGE_MIN 1000
#define PWM_RANGE_MAX 2000
#define PWM_RANGE (PWM_RANGE_MAX - PWM_RANGE_MIN)
#define PWM_RANGE_MIDDLE (PWM_RANGE_MIN + (PWM_RANGE / 2))
#define THROTTLE_LOOKUP_LENGTH 12

typedef enum {
    RATES_TYPE_BETAFLIGHT = 0,
    RATES_TYPE_RACEFLIGHT,
    RATES_TYPE_KISS,
    RATES_TYPE_ACTUAL,
    RATES_TYPE_QUICK,
    RATES_TYPE_COUNT    // must be the final entry
} ratesType_e;

typedef struct ratesSettingsLimits_s {
    uint8_t rc_rate_limit;
    uint8_t srate_limit;
    uint8_t expo_limit;
} ratesSettingsLimits_t;

typedef enum {
    THROTTLE_LIMIT_TYPE_OFF = 0,
    THROTTLE_LIMIT_TYPE_SCALE,
    THROTTLE_LIMIT_TYPE_CLIP,
    THROTTLE_LIMIT_TYPE_COUNT   // must be the last entry
} throttleLimitType_e;


// See http://en.wikipedia.org/wiki/Flight_dynamics
typedef enum {
	FD_ROLL = 0,
	FD_PITCH,
	FD_YAW
} flight_dynamics_index_t;

USTRUCT()
struct FcontrolRateConfig_s {
	GENERATED_BODY()
	uint8_t thrMid8;
	uint8_t thrExpo8;
	uint8_t rates_type;
	uint8_t rcRates[3];
	uint8_t rcExpo[3];
	uint8_t rates[3];
	uint8_t throttle_limit_type;            // Sets the throttle limiting type - off, scale or clip
	uint8_t throttle_limit_percent;         // Sets the maximum pilot commanded throttle limit
	uint16_t rate_limit[3];                 // Sets the maximum rate for the axes
	char profileName[MAX_RATE_PROFILE_NAME_LENGTH + 1]; // Descriptive name for rate profile
	uint8_t quickRatesRcExpo;               // Sets expo on rc command for quick rates

	FcontrolRateConfig_s() {
		thrMid8 = 50;
		thrExpo8 = 0;
		rates_type = RATES_TYPE_ACTUAL;
		rcRates[FD_ROLL] = 7;
		rcRates[FD_PITCH] = 7;
		rcRates[FD_YAW] = 7;
		rcExpo[FD_ROLL] = 0;
		rcExpo[FD_PITCH] = 0;
		rcExpo[FD_YAW] = 0;
		rates[FD_ROLL] = 67;
		rates[FD_PITCH] = 67;
		rates[FD_YAW] = 67;
		throttle_limit_type = THROTTLE_LIMIT_TYPE_OFF;
		throttle_limit_percent = 100;
		rate_limit[FD_ROLL] = CONTROL_RATE_CONFIG_RATE_LIMIT_MAX;
		rate_limit[FD_PITCH] = CONTROL_RATE_CONFIG_RATE_LIMIT_MAX;
		rate_limit[FD_YAW] = CONTROL_RATE_CONFIG_RATE_LIMIT_MAX;
		//profileName.Empty();
		quickRatesRcExpo = 0;
	};
};

static const ratesSettingsLimits_t ratesSettingLimits[RATES_TYPE_COUNT] = {
	{ 255, 100, 100 },
	{ 200, 255, 100 },
	{ 255,  99, 100 },
	{ 200, 200, 100 },
	{ 255, 200, 100 }
};

UCLASS()
class ULegacyRates : public  UObject {
	GENERATED_BODY()
public:
	UPROPERTY()
	FcontrolRateConfig_s currentControlRateProfile;
	int16_t lookupThrottleRC[THROTTLE_LOOKUP_LENGTH];    // lookup table for expo & mid THROTTLE

	ULegacyRates() {
		currentControlRateProfile = FcontrolRateConfig_s();
	};
	~ULegacyRates() {};

	int16_t rcLookupThrottle(int32_t tmp) {
		const int32_t tmp2 = tmp / 100;
		// [0;1000] -> expo -> [MINTHROTTLE;MAXTHROTTLE]
		return lookupThrottleRC[tmp2] + (tmp - tmp2 * 100) * (lookupThrottleRC[tmp2 + 1] - lookupThrottleRC[tmp2]) / 100;
	}

	float applyBetaflightRates(const int axis, float rcCommandf, const float rcCommandfAbs) {
		if (currentControlRateProfile.rcExpo[axis]) {
			const float expof = currentControlRateProfile.rcExpo[axis] / 100.0f;
			rcCommandf = rcCommandf * FMath::Pow(rcCommandfAbs, 3.f) * expof + rcCommandf * (1 - expof);
		}

		float rcRate = currentControlRateProfile.rcRates[axis] / 100.0f;
		if (rcRate > 2.0f) {
			rcRate += RC_RATE_INCREMENTAL * (rcRate - 2.0f);
		}
		float angleRate = 200.0f * rcRate * rcCommandf;
		if (currentControlRateProfile.rates[axis]) {
			const float rcSuperfactor = 1.0f / (FMath::Clamp(1.0f - (rcCommandfAbs * (currentControlRateProfile.rates[axis] / 100.0f)), 0.01f, 1.00f));
			angleRate *= rcSuperfactor;
		}

		return angleRate;
	}
	float applyRaceFlightRates(const int axis, float rcCommandf, const float rcCommandfAbs) {
		// -1.0 to 1.0 ranged and curved
		rcCommandf = ((1.0f + 0.01f * currentControlRateProfile.rcExpo[axis] * (rcCommandf * rcCommandf - 1.0f)) * rcCommandf);
		// convert to -2000 to 2000 range using acro+ modifier
		float angleRate = 10.0f * currentControlRateProfile.rcRates[axis] * rcCommandf;
		angleRate = angleRate * (1 + rcCommandfAbs * (float)currentControlRateProfile.rates[axis] * 0.01f);

		return angleRate;
	}

	float applyKissRates(const int axis, float rcCommandf, const float rcCommandfAbs) {
		const float rcCurvef = currentControlRateProfile.rcExpo[axis] / 100.0f;

		float kissRpyUseRates = 1.0f / (FMath::Clamp(1.0f - (rcCommandfAbs * (currentControlRateProfile.rates[axis] / 100.0f)), 0.01f, 1.00f));
		float kissRcCommandf = (FMath::Pow(rcCommandf, 3.f) * rcCurvef + rcCommandf * (1 - rcCurvef)) * (currentControlRateProfile.rcRates[axis] / 1000.0f);
		float kissAngle = FMath::Clamp(((2000.0f * kissRpyUseRates) * kissRcCommandf), SETPOINT_RATE_LIMIT_MIN, SETPOINT_RATE_LIMIT_MAX);

		return kissAngle;
	}

	float applyActualRates(const int axis, float rcCommandf, const float rcCommandfAbs) {
		float expof = currentControlRateProfile.rcExpo[axis] / 100.0f;
		expof = rcCommandfAbs * (FMath::Pow(rcCommandf, 5.f) * expof + rcCommandf * (1 - expof));

		const float centerSensitivity = currentControlRateProfile.rcRates[axis] * 10.0f;
		const float stickMovement = FMath::Max(0.f, currentControlRateProfile.rates[axis] * 10.0f - centerSensitivity);
		const float angleRate = rcCommandf * centerSensitivity + stickMovement * expof;

		return angleRate;
	}

	float applyQuickRates(const int axis, float rcCommandf, const float rcCommandfAbs) {
		const auto rcRate = currentControlRateProfile.rcRates[axis] * 2;
		const uint16_t maxDPS = FMath::Max(currentControlRateProfile.rates[axis] * 10, rcRate);
		const float expof = currentControlRateProfile.rcExpo[axis] / 100.0f;
		const float superFactorConfig = ((float)maxDPS / rcRate - 1) / ((float)maxDPS / rcRate);

		float curve;
		float superFactor;
		float angleRate;

		if (currentControlRateProfile.quickRatesRcExpo) {
			curve = FMath::Pow(rcCommandf, 3.f) * expof + rcCommandf * (1 - expof);
			superFactor = 1.0f / (FMath::Clamp(1.0f - (rcCommandfAbs * superFactorConfig), 0.01f, 1.00f));
			angleRate = FMath::Clamp(curve * rcRate * superFactor, SETPOINT_RATE_LIMIT_MIN, SETPOINT_RATE_LIMIT_MAX);
		}
		else {
			curve = FMath::Pow(rcCommandfAbs, 3.f) * expof + rcCommandfAbs * (1 - expof);
			superFactor = 1.0f / (FMath::Clamp(1.0f - (curve * superFactorConfig), 0.01f, 1.00f));
			angleRate = FMath::Clamp(rcCommandf * rcRate * superFactor, SETPOINT_RATE_LIMIT_MIN, SETPOINT_RATE_LIMIT_MAX);
		}

		return angleRate;
	}
};
