
//
// Created by tyll on 2022-10-04.
//

#ifndef UC_CORE_WELLKNOWNPIDS_H
#define UC_CORE_WELLKNOWNPIDS_H

#include <stdint.h>
#include "ObdValue.h"
#include "core/StringBuilder.h"

namespace core { namespace can { namespace obd {
    struct WellKnownPid {
        const uint8_t pid;
        const uint8_t length;
        const char* description;
        float (*toFloat)(ObdValue);
        void (*toString)(core::StringBuilder, ObdValue, float);
    };

    constexpr WellKnownPid wellKnownPids[] = {
        { 0x00, 4, "PIDs supported 01-20", [] (ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint32()); }},
        { 0x01, 4, "Monitor status", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint32()); }},
        { 0x02, 2, "Freeze DTC", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); }},
        { 0x03, 2, "Fuel system status", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); }},
        { 0x04, 1, "Calculated engine load", [](ObdValue val){ return float(100 * val.A) / 255.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb + f + "%"; }},
        { 0x05, 1, "Engine coolant temperature", [](ObdValue val){ return float(val.A - 40); }, [](core::StringBuilder sb, ObdValue val, float f) { sb + f + "°C"; }},
        { 0x06, 1, "Short term fuel trim Bank 1", [](ObdValue val){ return float((100*val.A))/128.0f-100.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb + f; }},
        { 0x07, 1, "Long term fuel trim Bank 1", [](ObdValue val){ return float((100*val.A))/128.0f-100.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb + f; }},
        { 0x08, 1, "Short term fuel trim Bank 2", [](ObdValue val){ return float((100*val.A))/128.0f-100.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb + f; }},
        { 0x09, 1, "Long term fuel trim Bank 2", [](ObdValue val){ return float((100*val.A))/128.0f-100.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb + f; }},
        { 0x0A, 1, "Fuel pressure", [](ObdValue val){ return float(3 * val.A); }, [](core::StringBuilder sb, ObdValue val, float f) { sb + f + "kPa"; }},
        { 0x0B, 1, "Intake manifold absolute pressure", [](ObdValue val){ return float(val.A); }, [](core::StringBuilder sb, ObdValue val, float f) { sb + f + "kPa"; }},
        { 0x0C, 2, "Engine speed", [](ObdValue val){ return float(val.asUint16() / 4); }, [](core::StringBuilder sb, ObdValue val, float f) { sb + f + "rpm"; }},
        { 0x0D, 1, "Vehicle speed", [](ObdValue val){ return float(val.asUint8()); }, [](core::StringBuilder sb, ObdValue val, float f) { sb + f + "km/h"; }},
        { 0x0E, 1, "Timing advance", [](ObdValue val){ return float(val.A)/ 2 - 64; }, [](core::StringBuilder sb, ObdValue val, float f) { sb + f + "°"; }},
        { 0x0F, 1, "Intake air temperature", [](ObdValue val){ return float(val.A) - 40; }, [](core::StringBuilder sb, ObdValue val, float f) { sb + f + "°C"; }},
        { 0x10, 2, "MAF air flow", [](ObdValue val){ return float(256 * val.A + val.B) / 100; }, [](core::StringBuilder sb, ObdValue val, float f) { sb + f + "g/s"; }},
        { 0x11, 1, "Throttle position", [](ObdValue val){ return float(100 * val.A) / 255; }, [](core::StringBuilder sb, ObdValue val, float f) { sb + f + "%"; }},
        { 0x12, 1, "Commanded sencondary air status", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); }},
        { 0x13, 1, "Oxygen sensors present", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); }},
        { 0x14, 2, "Oxygen Sensor 1", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); }},
        { 0x15, 2, "Oxygen Sensor 2", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); }},
        { 0x16, 2, "Oxygen Sensor 3", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); }},
        { 0x17, 2, "Oxygen Sensor 4", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); }},
        { 0x18, 2, "Oxygen Sensor 5", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); }},
        { 0x19, 2, "Oxygen Sensor 6", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); }},
        { 0x1A, 2, "Oxygen Sensor 7", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); }},
        { 0x1B, 2, "Oxygen Sensor 8", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); }},
        { 0x1C, 1, "OBD standards this vehicle conforms to", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); }},
        { 0x1D, 1, "Oxygen sensors present (4 banks)", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); }},
        { 0x1E, 1, "Auxiliary input status", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); }},
        { 0x1F, 2, "Run time since engine start", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb + f + "s"; }},
        { 0x20, 4, "PIDs supported 21-40", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); }},
        { 0x21, 2, "Distance traveled with malfunction indicator lamp on", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb + f + "km"; }},
        { 0x22, 2, "Fuel Rail Pressure (relative to MAP)", [](ObdValue val){ return 0.079f * float(val.asUint16()); }, [](core::StringBuilder sb, ObdValue val, float f) { sb + f + "kPa"; }},
        { 0x23, 2, "Fail Rail Gauge Pressure (direct injection)", [](ObdValue val){ return float(10 * val.asUint16()); }, [](core::StringBuilder sb, ObdValue val, float f) { sb + f + "kPa"; }},
        { 0x24, 4, "Oxygen Sensor 1", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); }},
        { 0x25, 4, "Oxygen Sensor 2", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); }},
        { 0x26, 4, "Oxygen Sensor 3", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); }},
        { 0x27, 4, "Oxygen Sensor 4", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); }},
        { 0x28, 4, "Oxygen Sensor 5", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); }},
        { 0x29, 4, "Oxygen Sensor 6", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); }},
        { 0x2A, 4, "Oxygen Sensor 7", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); }},
        { 0x2B, 4, "Oxygen Sensor 8", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); }},
        { 0x2C, 1, "Commanded EGR", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb + f + "%"; }},
        { 0x2D, 1, "EGR Error", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb + f + "%"; }},
        { 0x2E, 1, "Commanded evaporative purge", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb + f + "%"; }},
        { 0x2F, 1, "Fuel Tank Level Input", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb + f + "%"; }},
        { 0x30, 1, "Warm-ups since codes cleared", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb + f; }},
        { 0x31, 2, "Distance traveled since codes cleared", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb + f + "km"; }},
        { 0x32, 2, "Evap. System Vapor pressure", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb + f + "Pa"; }},
        { 0x33, 1, "Absolute Barometric Pressure", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb + f + "kPa"; }},
        { 0x34, 4, "Oxygen Sensor 1", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x35, 4, "Oxygen Sensor 2", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x36, 4, "Oxygen Sensor 3", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x37, 4, "Oxygen Sensor 4", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x38, 4, "Oxygen Sensor 5", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x39, 4, "Oxygen Sensor 6", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x3A , 4, "Oxygen Sensor 7", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x3B , 4, "Oxygen Sensor 8", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x3C , 2, "Catalyst Temperature: Bank 1, Sensor 1", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x3D , 2, "Catalyst Temperature: Bank 2, Sensor 1", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x3E , 2, "Catalyst Temperature: Bank 1, Sensor 2", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x3F , 2, "Catalyst Temperature: Bank 2, Sensor 2", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x40, 4, "PIDs supported [41 - 60]", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x41, 4, "Monitor status this drive cycle", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x42, 2, "Control module voltage", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb + f + "V"; } },
        { 0x43, 2, "Absolute load value", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb + f + "%"; } },
        { 0x44, 2, "Commanded Air-Fuel Equivalence Ratio (lambda,λ)", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb + f; } },
        { 0x45, 1, "Relative throttle position", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x46, 1, "Ambient air temperature", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x47, 1, "Absolute throttle position B", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x48, 1, "Absolute throttle position C", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x49, 1, "Accelerator pedal position D", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x4A , 1, "Accelerator pedal position E", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x4B , 1, "Accelerator pedal position F", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x4C , 1, "Commanded throttle actuator", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x4D , 2, "Time run with MIL on", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x4E , 2, "Time since trouble codes cleared", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x4F , 4, "Maximum value for Fuel–Air equivalence ratio, oxygen sensor voltage, oxygen sensor current, and intake manifold absolute pressure", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x50, 4, "Maximum value for air flow rate from mass air flow sensor", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x51, 1, "Fuel Type", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x52, 1, "Ethanol fuel percent", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x53, 2, "Absolute Evap system Vapor Pressure", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x54, 2, "Evap system vapor pressure", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x55, 2, "Short term secondary oxygen sensor trim, A: bank 1, B: bank 3", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x56, 2, "Long term secondary oxygen sensor trim, A: bank 1, B: bank 3", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x57, 2, "Short term secondary oxygen sensor trim, A: bank 2, B: bank 4", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x58, 2, "Long term secondary oxygen sensor trim, A: bank 2, B: bank 4", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x59, 2, "Fuel rail absolute pressure", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x5A , 1, "Relative accelerator pedal position", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x5B , 1, "Hybrid battery pack remaining life", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x5C , 1, "Engine oil temperature", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x5D , 2, "Fuel injection timing", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x5E , 2, "Engine fuel rate", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x5F , 1, "Emission requirements to which vehicle is designed", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x60, 4, "PIDs supported [61 - 80]", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x61, 1, "Driver's demand engine - percent torque", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x62, 1, "Actual engine - percent torque", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x63, 2, "Engine reference torque", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x64, 5, "Engine percent torque data", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x65, 2, "Auxiliary input / output supported", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x66, 5, "Mass air flow sensor", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x67, 3, "Engine coolant temperature", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x68, 3, "Intake air temperature sensor", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x69, 7, "Actual EGR, Commanded EGR, and EGR Error", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x6A , 5, "Commanded Diesel intake air flow control and relative intake air flow position", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x6B , 5, "Exhaust gas recirculation temperature", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x6C , 5, "Commanded throttle actuator control and relative throttle position", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x6D , 11, "Fuel pressure control system", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x6E , 9, "Injection pressure control system", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x6F , 3, "Turbocharger compressor inlet pressure", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x70, 10, "Boost pressure control", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x71, 6, "Variable Geometry turbo (VGT) control", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x72, 5, "Wastegate control", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x73, 5, "Exhaust pressure", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x74, 5, "Turbocharger RPM", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x75, 7, "Turbocharger temperature", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x76, 7, "Turbocharger temperature", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x77, 5, "Charge air cooler temperature (CACT)", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x78, 9, "Exhaust Gas temperature (EGT) Bank 1", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x79, 9, "Exhaust Gas temperature (EGT) Bank 2", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x7A , 7, "Diesel particulate filter (DPF)", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x7B , 7, "Diesel particulate filter (DPF)", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x7C , 9, "Diesel Particulate filter (DPF) temperature", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x7D , 1, "NOx NTE (Not-To-Exceed) control area status", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x7E , 1, "PM NTE (Not-To-Exceed) control area status", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x7F , 13, "Engine run time [b]", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x80, 4, "PIDs supported [81 - A0]", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x81, 41, "Engine run time for Auxiliary Emissions Control Device(AECD)", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x82, 41, "Engine run time for Auxiliary Emissions Control Device(AECD)", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x83, 9, "NOx sensor", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x84, 1, "Manifold surface temperature", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x85, 10, "NOx reagent system", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x86, 5, "Particulate matter (PM) sensor", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x87, 5, "Intake manifold absolute pressure", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x88, 13, "SCR Induce System", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x89, 41, "Run Time for AECD #11-#15", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x8A , 41, "Run Time for AECD #16-#20", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x8B , 7, "Diesel Aftertreatment", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x8C , 17, "O2 Sensor (Wide Range)", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x8D , 1, "Throttle Position G", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x8E , 1, "Engine Friction - Percent Torque", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x8F , 7, "PM Sensor Bank 1 & 2", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x90, 3, "WWH-OBD Vehicle OBD System Information", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x91, 5, "WWH-OBD Vehicle OBD System Information", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x92, 2, "Fuel System Control", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x93, 3, "WWH-OBD Vehicle OBD Counters support", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x94, 12, "NOx Warning And Inducement System", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x95, 4, "Unknown", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x96, 4, "Unknown", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x97, 4, "Unknown", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x98, 9, "Exhaust Gas Temperature Sensor", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x99, 9, "Exhaust Gas Temperature Sensor", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x9A , 6, "Hybrid/EV Vehicle System Data, Battery, Voltage", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x9B , 4, "Diesel Exhaust Fluid Sensor Data", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x9C , 17, "O2 Sensor Data", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x9D , 4, "Engine Fuel Rate", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x9E , 2, "Engine Exhaust Flow Rate", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0x9F , 9, "Fuel System Percentage Use", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0xA0 , 4, "PIDs supported [A1 - C0]", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0xA1 , 9, "NOx Sensor Corrected Data", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0xA2 , 2, "Cylinder Fuel Rate", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0xA3 , 9, "Evap System Vapor Pressure", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
        { 0xA4 , 4, "Transmission Actual Gear", [](ObdValue val){ return 0.0f; }, [](core::StringBuilder sb, ObdValue val, float f) { sb.appendHex(val.asUint16()); } },
    };



}}}
#endif //UC_CORE_WELLKNOWNPIDS_H
