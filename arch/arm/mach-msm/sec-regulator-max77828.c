/*
 * Copyright (C) 2013 Samsung Electronics, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/mfd/max77828.h>
#include <linux/mfd/max77828-private.h>
#include <linux/regulator/machine.h>

static struct regulator_consumer_supply safeout1_supply[] = {
       REGULATOR_SUPPLY("safeout1", NULL),
};

static struct regulator_consumer_supply safeout2_supply[] = {
       REGULATOR_SUPPLY("safeout2", NULL),
};

static struct regulator_consumer_supply charger_supply[] = {
       REGULATOR_SUPPLY("vinchg1", "charger-manager.0"),
       REGULATOR_SUPPLY("vinchg1", NULL),
};

static struct regulator_init_data safeout1_init_data = {
       .constraints    = {
               .name           = "safeout1 range",
               .valid_ops_mask = REGULATOR_CHANGE_STATUS,
               .always_on      = 0,
               .boot_on        = 1,
               .state_mem      = {
                       .enabled = 1,
               },
       },
       .num_consumer_supplies  = ARRAY_SIZE(safeout1_supply),
       .consumer_supplies      = safeout1_supply,
};

static struct regulator_init_data safeout2_init_data = {
       .constraints    = {
               .name           = "safeout2 range",
               .valid_ops_mask = REGULATOR_CHANGE_STATUS,
               .always_on      = 0,
               .boot_on        = 0,
               .state_mem      = {
                       .enabled = 1,
               },
       },
       .num_consumer_supplies  = ARRAY_SIZE(safeout2_supply),
       .consumer_supplies      = safeout2_supply,
};

static struct regulator_init_data charger_init_data = {
       .constraints    = {
               .name           = "CHARGER",
               .valid_ops_mask = REGULATOR_CHANGE_STATUS |
               REGULATOR_CHANGE_CURRENT,
               .always_on      = 1,
               .boot_on        = 1,
               .min_uA         = 60000,
               .max_uA         = 2580000,
       },
       .num_consumer_supplies  = ARRAY_SIZE(charger_supply),
       .consumer_supplies      = charger_supply,
};

struct max77828_regulator_data max77828_regulators[] = {
       {MAX77828_ESAFEOUT1, &safeout1_init_data,},
       {MAX77828_ESAFEOUT2, &safeout2_init_data,},
       {MAX77828_CHARGER, &charger_init_data,},
};
#if defined(CONFIG_EXTCON_MAX77828)
int max77828_muic_set_safeout(int path)
{
	struct regulator *regulator;
	int ret;

	pr_info("%s: MUIC safeout path=%d\n", __func__, path);

	if (path == CP_USB_MODE) {
		regulator = regulator_get(NULL, "safeout1");
		if (IS_ERR(regulator))
			return -ENODEV;
		if (regulator_is_enabled(regulator))
			regulator_force_disable(regulator);
		regulator_put(regulator);

		regulator = regulator_get(NULL, "safeout2");
		if (IS_ERR(regulator))
			return -ENODEV;
		if (!regulator_is_enabled(regulator))
			ret = regulator_enable(regulator);
		regulator_put(regulator);
	} else {
		/* AP_USB_MODE || AUDIO_MODE */
		regulator = regulator_get(NULL, "safeout1");
		if (IS_ERR(regulator))
			return -ENODEV;
		if (!regulator_is_enabled(regulator))
			ret = regulator_enable(regulator);
		regulator_put(regulator);

		regulator = regulator_get(NULL, "safeout2");
		if (IS_ERR(regulator))
			return -ENODEV;
		if (regulator_is_enabled(regulator))
			regulator_force_disable(regulator);
		regulator_put(regulator);
	}

	return 0;
}
EXPORT_SYMBOL(max77828_muic_set_safeout);
#endif
