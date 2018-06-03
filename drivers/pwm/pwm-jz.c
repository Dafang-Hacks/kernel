/*
 *  Copyright (C) 2014,  King liuyang <liuyang@ingenic.cn>
 *  jz_PWM support
 */

#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/pwm.h>
#include <linux/mfd/core.h>
#include <linux/mfd/jz_tcu.h>
#include <soc/extal.h>
#include <soc/gpio.h>
#include <linux/slab.h>

struct jz_pwm_device{
	short id;
	const char *label;
	struct jz_tcu_chn *tcu_cha;
};

struct jz_pwm_chip {
	struct device *dev;
	const struct mfd_cell *cell;

	struct jz_pwm_device *pwm_chrs;
	struct pwm_chip chip;
};

static inline struct jz_pwm_chip *to_jz(struct pwm_chip *chip)
{
	return container_of(chip, struct jz_pwm_chip, chip);
}

static int jz_pwm_request(struct pwm_chip *chip, struct pwm_device *pwm)
{
	struct jz_pwm_chip *jz = to_jz(chip);
	int id = jz->pwm_chrs->tcu_cha->index;
	if (id < 0 || id > 3)
		return -ENODEV;

	printk("request pwm channel %d successfully\n", id);

	return 0;
}

static void jz_pwm_free(struct pwm_chip *chip, struct pwm_device *pwm)
{
	struct jz_pwm_chip *jz = to_jz(chip);
	struct jz_tcu_chn *tcu_pwm = jz->pwm_chrs->tcu_cha;
	kfree (tcu_pwm);
	kfree (jz);
}

static int jz_pwm_enable(struct pwm_chip *chip, struct pwm_device *pwm)
{
	struct jz_pwm_chip *jz = to_jz(chip);
	struct jz_tcu_chn *tcu_pwm = jz->pwm_chrs->tcu_cha;

	jz_tcu_enable_counter(tcu_pwm);

	jz_tcu_start_counter(tcu_pwm);

	jz_gpio_set_func(tcu_pwm->gpio,GPIO_FUNC_0);

	return 0;
}

static void jz_pwm_disable(struct pwm_chip *chip, struct pwm_device *pwm)
{
	struct jz_pwm_chip *jz = to_jz(chip);
	struct jz_tcu_chn *tcu_pwm = jz->pwm_chrs->tcu_cha;

	jz_tcu_stop_counter(tcu_pwm);

	jz_tcu_disable_counter(tcu_pwm);

}

static int jz_pwm_config(struct pwm_chip *chip, struct pwm_device *pwm,
			     int duty_ns, int period_ns)
{
	unsigned long long tmp;
	unsigned long period, duty;
	struct jz_pwm_chip *jz = to_jz(chip);
	int prescaler = 0; /*prescale = 0,1,2,3,4,5*/

	struct jz_tcu_chn *tcu_pwm = jz->pwm_chrs->tcu_cha;

	if (duty_ns < 0 || duty_ns > period_ns)
		return -EINVAL;
	if (period_ns < 200 || period_ns > 1000000000)
		return -EINVAL;

	tcu_pwm->irq_type = NULL_IRQ_MODE;
	tcu_pwm->is_pwm = pwm->flags;

#ifndef CONFIG_SLCD_SUSPEND_ALARM_WAKEUP_REFRESH
	tmp = JZ_EXTAL;
#else
	tmp = JZ_EXTAL;//32768 RTC CLOCK failure!
#endif
	tmp = tmp * period_ns;

	do_div(tmp, 1000000000);
	period = tmp;

	while (period > 0xffff && prescaler < 6) {
		period >>= 2;
		++prescaler;
	}
	if (prescaler == 6)
		return -EINVAL;

	tmp = (unsigned long long)period * duty_ns;
	do_div(tmp, period_ns);
	duty = tmp;

	if (duty >= period)
		duty = period - 1;
	tcu_pwm->full_num = period;
	tcu_pwm->half_num = (period - duty);
	tcu_pwm->divi_ratio = prescaler;
#ifdef CONFIG_SLCD_SUSPEND_ALARM_WAKEUP_REFRESH
	tcu_pwm->clk_src = TCU_CLKSRC_RTC;
#else
	tcu_pwm->clk_src = TCU_CLKSRC_EXT;
#endif
	tcu_pwm->shutdown_mode = 0;

	jz_tcu_config_chn(tcu_pwm);

	jz_tcu_set_period(tcu_pwm,tcu_pwm->full_num);

	jz_tcu_set_duty(tcu_pwm,tcu_pwm->half_num);


	return 0;
}

int jz_pwm_set_polarity(struct pwm_chip *chip,struct pwm_device *pwm, enum pwm_polarity polarity)
{
	struct jz_pwm_chip *jz = to_jz(chip);
	struct jz_tcu_chn *tcu_pwm = jz->pwm_chrs->tcu_cha;

	if(polarity == PWM_POLARITY_INVERSED)
		tcu_pwm->init_level = 1;

	if(polarity == PWM_POLARITY_NORMAL)
		tcu_pwm->init_level = 0;

	return 0;
}

static const struct pwm_ops jz_pwm_ops = {
	.request = jz_pwm_request,
	.free = jz_pwm_free,
	.config = jz_pwm_config,
	.set_polarity = jz_pwm_set_polarity,
	.enable = jz_pwm_enable,
	.disable = jz_pwm_disable,
	.owner = THIS_MODULE,
};

static int jz_pwm_probe(struct platform_device *pdev)
{
	struct jz_pwm_chip *jz;
	int ret;

	jz = kzalloc(sizeof(struct jz_pwm_chip), GFP_KERNEL);

	jz->pwm_chrs = kzalloc(sizeof(struct jz_pwm_device),GFP_KERNEL);
	if (!jz || !jz->pwm_chrs)
		return -ENOMEM;

	jz->cell = mfd_get_cell(pdev);
	if (!jz->cell) {
		ret = -ENOENT;
		dev_err(&pdev->dev, "Failed to get mfd cell\n");
		goto err_free;
	}

	jz->chip.dev = &pdev->dev;
	jz->chip.ops = &jz_pwm_ops;
	jz->chip.npwm = 2;
	jz->chip.base = -1;

	jz->pwm_chrs->tcu_cha = (struct jz_tcu_chn *)jz->cell->platform_data;

	ret = pwmchip_add(&jz->chip);
	if (ret < 0) {
		goto err_free;
	}

	platform_set_drvdata(pdev, jz);

	return 0;
err_free:
		kfree(jz->pwm_chrs);
		kfree(jz);
	return ret;
}

static int jz_pwm_remove(struct platform_device *pdev)
{
	struct jz_pwm_chip *jz = platform_get_drvdata(pdev);
	int ret;

	ret = pwmchip_remove(&jz->chip);
	if (ret < 0)
		return ret;


	return 0;
}

static struct platform_driver jz_pwm_driver = {
	.driver = {
		.name = "tcu_chn0",
		.owner = THIS_MODULE,
	},
	.probe = jz_pwm_probe,
	.remove = jz_pwm_remove,
};
module_platform_driver(jz_pwm_driver);

MODULE_AUTHOR("King liuyang <liuyang@ingenic.cn>");
MODULE_DESCRIPTION("Ingenic jz PWM driver");
MODULE_ALIAS("platform:jz-pwm");
MODULE_LICENSE("GPL");
