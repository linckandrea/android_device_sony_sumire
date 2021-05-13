/*
 * Copyright (C) 2008 The Android Open Source Project
 * Copyright (C) 2014 The CyanogenMod Project
 * Copyright (C) 2017 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>

#include <android-base/logging.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>
#include <sys/types.h>

#include "vendor_init.h"
#include "util.h"

using android::init::import_kernel_cmdline;

std::vector<std::string> ro_props_default_source_order = {
    "",
    "odm.",
    "product.",
    "system.",
    "system_ext.",
    "vendor.",
};

void property_override(char const prop[], char const value[], bool add = true)
{
    prop_info *pi;

    pi = (prop_info*) __system_property_find(prop);
    if (pi)
        __system_property_update(pi, value, strlen(value));
    else if (add)
        __system_property_add(prop, strlen(prop), value, strlen(value));
}

static void import_kernel_nv(const std::string& key,
        const std::string& value, bool for_emulator __attribute__((unused)))
{
    if (key.empty()) return;

    if (key == "oemandroidboot.phoneid") {
        // Dual Sim variant contains two IMEIs separated by comma.
        if ((count(value.begin(), value.end(),',')) > 0) {
            property_override("persist.multisim.config", "dsds");
            property_override("persist.radio.multisim.config", "dsds");
            property_override("ro.telephony.default_network", "9,1");
            property_override("ro.semc.product.model", "E6683");
            property_override("ro.semc.product.name", "Xperia Z5 Dual");
            for (const auto &source : ro_props_default_source_order) {
	        set_ro_build_prop("source", "model", "E6683");
	        set_ro_build_prop("source", "name", "sumire_dsds");
	        set_ro_build_prop("source", "device", "sumire_dsds");
	        set_ro_build_prop("source", "description", "sumire_dsds-user 7.1.1 N-MR1-KITAKAMI-170609-1025 1 dev-keys");
	        set_ro_build_prop("source", "fingerprint", "Sony/sumire_dsds/sumire_dsds:7.1.1/N-MR1-KITAKAMI-170609-1025/1:user/dev-keys");
            }
        } else {
            property_override("ro.telephony.default_network", "9");
            property_override("ro.semc.product.model", "E6653");
            property_override("ro.semc.product.name", "Xperia Z5");
            for (const auto &source : ro_props_default_source_order) {
                property_override_dual("source", "model", "E6653");
                property_override_dual("source", "name", "sumire");
                property_override_dual("source", "device", "sumire");
                property_override("source", "description",  "sumire-user 7.1.1 N-MR1-KITAKAMI-170609-1025 1 dev-keys");
                property_override_dual("source", "fingerprint", "Sony/sumire/sumire:7.1.1/N-MR1-KITAKAMI-170609-1025/1:user/dev-keys");
            }
        }
    }
}

void vendor_load_properties()
{
    import_kernel_cmdline(0, import_kernel_nv);
}
