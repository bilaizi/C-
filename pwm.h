/**
 * enum pwm_polarity - polarity of a PWM signal
 * @PWM_POLARITY_NORMAL: a high signal for the duration of the duty-cycle, 
 	followed by a low signal for the remainder of the pulse period
 * @PWM_POLARITY_INVERSED: a low signal for the duration of the duty-period
 */
enum pwm_polarity {
	PWM_POLARITY_NORMAL,
	PWM_POLARITY_INVERSED,
};

/**
 * struct pwm_args - board-dependent PWM arguments
 * @period: reference period
 * @polarity: reference polarity
 *
 * This structure describes board-dependent arguments attached to a PWM device. 
 * These arguments are usually retrieved from the PWM lookup table or device tree.
 *
 * Do not confuse this with the PWM state: 
 * PWM arguments represent the initial configuration
 * that users want to use on this PWM device rather than the current PWM hardware state.
 */
struct pwm_args {
	unsigned int period;
	enum pwm_polarity polarity;
};

/*
 * struct pwm_state - state of a PWM channel
 * @period: PWM period (in nanoseconds)
 * @duty_cycle: PWM duty cycle (in nanoseconds)
 * @polarity: PWM polarity
 * @enabled: PWM enabled status
 */
struct pwm_state {
	unsigned int period;
	unsigned int duty_cycle;
	enum pwm_polarity polarity;
	bool enabled;
};

/**
 * struct pwm_device - PWM channel object
 * @label: name of the PWM device
 * @flags: flags associated with the PWM device
 * @hwpwm: per-chip relative index of the PWM device
 * @pwm: global index of the PWM device
 * @chip: PWM chip providing this PWM device
 * @chip_data: chip-private data associated with the PWM device
 * @args: PWM arguments
 * @state: curent PWM channel state
 */
struct pwm_device {
	const char *label;
	unsigned long flags;
	unsigned int hwpwm;
	unsigned int pwm;
	struct pwm_chip *chip;
	void *chip_data;
	struct pwm_args args;
	struct pwm_state state;
};

/**
 * struct pwm_ops - PWM controller operations
 * @request: optional hook for requesting a PWM
 * @free: optional hook for freeing a PWM
 * @config: configure duty cycles and period length for this PWM
 * @set_polarity: configure the polarity of this PWM
 * @capture: capture and report PWM signal
 * @enable: enable PWM output toggling
 * @disable: disable PWM output toggling
 * @apply: atomically apply a new PWM config. The state argument should be adjusted with the real hardware config
 *	    (if the approximate the period or duty_cycle value, state should reflect it)
 * @get_state: get the current PWM state. This function is only called once per PWM device when the PWM chip is registered.
 * @dbg_show: optional routine to show contents in debugfs
 * @owner: helps prevent removal of modules exporting active PWMs
 */
struct pwm_ops {
	int (*request)(struct pwm_chip *chip, struct pwm_device *pwm);
	void (*free)(struct pwm_chip *chip, struct pwm_device *pwm);
	int (*config)(struct pwm_chip *chip, struct pwm_device *pwm, int duty_ns, int period_ns);
	int (*set_polarity)(struct pwm_chip *chip, struct pwm_device *pwm, enum pwm_polarity polarity);
	int (*capture)(struct pwm_chip *chip, struct pwm_device *pwm, struct pwm_capture *result, unsigned long timeout);
	int (*enable)(struct pwm_chip *chip, struct pwm_device *pwm);
	void (*disable)(struct pwm_chip *chip, struct pwm_device *pwm);
	int (*apply)(struct pwm_chip *chip, struct pwm_device *pwm, struct pwm_state *state);
	void (*get_state)(struct pwm_chip *chip, struct pwm_device *pwm, struct pwm_state *state);
#ifdef CONFIG_DEBUG_FS
	void (*dbg_show)(struct pwm_chip *chip, struct seq_file *s);
#endif
	struct module *owner;
};

/**
 * struct pwm_chip - abstract a PWM controller
 * @dev: device providing the PWMs
 * @list: list node for internal use
 * @ops: callbacks for this PWM controller
 * @base: number of first PWM controlled by this chip
 * @npwm: number of PWMs controlled by this chip
 * @pwms: array of PWM devices allocated by the framework
 * @of_xlate: request a PWM device given a device tree PWM specifier
 * @of_pwm_n_cells: number of cells expected in the device tree PWM specifier
 */
struct pwm_chip {
	struct device *dev;
	struct list_head list;
	const struct pwm_ops *ops;
	int base;
	unsigned int npwm;
	struct pwm_device *pwms;
	struct pwm_device * (*of_xlate)(struct pwm_chip *pc, onst struct of_phandle_args *args);
	unsigned int of_pwm_n_cells;
};


