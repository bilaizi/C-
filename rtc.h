// https://github.com/torvalds/linux/blob/master/include/uapi/linux/rtc.h
/*
 * The struct used to pass data via the following ioctl. 
 * Similar to the struct tm in <time.h>, but it needs to be here 
 * so that the kernel source is self contained, allowing cross-compiles, etc.
 */
struct rtc_time {
	int tm_sec;  /* seconds after the minute */
	int tm_min;  /* minutes after the hour - [0, 59] */
	int tm_hour; /* hours since midnight - [0, 23] */
	int tm_mday; /* day of the month - [1, 31] */
	int tm_mon;  /* months since January - [0, 11] */
	int tm_year; /* years since 1900 */
	int tm_wday; /* days since Sunday - [0, 6] */
	int tm_yday; /* days since January 1 - [0, 365] */
	int tm_isdst;/* Daylight saving time flag */
};

/*
 * For these RTC methods the device parameter is the physical device
 * on whatever bus holds the hardware (I2C, Platform, SPI, etc), which
 * was passed to rtc_device_register().  Its driver_data normally holds
 * device state, including the rtc_device pointer for the RTC.
 *
 * Most of these methods are called with rtc_device.ops_lock held,
 * through the rtc_*(struct rtc_device *, ...) calls.
 *
 * The (current) exceptions are mostly filesystem hooks:
 *   - the proc() hook for procfs
 *   - non-ioctl() chardev hooks:  open(), release(), read_callback()
 *
 * REVISIT those periodic irq calls *do* have ops_lock when they're
 * issued through ioctl() ...
 */
struct rtc_class_ops {
	int (*ioctl)(struct device *, unsigned int, unsigned long);
	int (*read_time)(struct device *, struct rtc_time *);
	int (*set_time)(struct device *, struct rtc_time *);
	int (*read_alarm)(struct device *, struct rtc_wkalrm *);
	int (*set_alarm)(struct device *, struct rtc_wkalrm *);
	int (*proc)(struct device *, struct seq_file *);
	int (*set_mmss64)(struct device *, time64_t secs);
	int (*set_mmss)(struct device *, unsigned long secs);
	int (*read_callback)(struct device *, int data);
	int (*alarm_irq_enable)(struct device *, unsigned int enabled);
	int (*read_offset)(struct device *, long *offset);
	int (*set_offset)(struct device *, long offset);
};

/* flags */
#define RTC_DEV_BUSY 0
struct rtc_device {
	struct device dev;
	struct module *owner;
	int id;
	const struct rtc_class_ops *ops;
	struct mutex ops_lock;
	struct cdev char_dev;
	unsigned long flags;
	unsigned long irq_data;
	spinlock_t irq_lock;
	wait_queue_head_t irq_queue;
	struct fasync_struct *async_queue;
	struct rtc_task *irq_task;
	spinlock_t irq_task_lock;
	int irq_freq;
	int max_user_freq;
	struct timerqueue_head timerqueue;
	struct rtc_timer aie_timer;
	struct rtc_timer uie_rtctimer;
	struct hrtimer pie_timer; /* sub second exp, so needs hrtimer */
	int pie_enabled;
	struct work_struct irqwork;
	/* Some hardware can't support UIE mode */
	int uie_unsupported;
	/* Number of nsec it takes to set the RTC clock. This influences when
	 * the set ops are called. An offset:
	 *   - of 0.5 s will call RTC set for wall clock time 10.0 s at 9.5 s
	 *   - of 1.5 s will call RTC set for wall clock time 10.0 s at 8.5 s
	 *   - of -0.5 s will call RTC set for wall clock time 10.0 s at 10.5 s
	 */
	long set_offset_nsec;
	bool registered;
	struct nvmem_config *nvmem_config;
	struct nvmem_device *nvmem;
	/* Old ABI support */
	bool nvram_old_abi;
	struct bin_attribute *nvram;
#ifdef CONFIG_RTC_INTF_DEV_UIE_EMUL
	struct work_struct uie_task;
	struct timer_list uie_timer;
	/* Those fields are protected by rtc->irq_lock */
	unsigned int oldsecs;
	unsigned int uie_irq_active:1;
	unsigned int stop_uie_polling:1;
	unsigned int uie_task_active:1;
	unsigned int uie_timer_active:1;
#endif
};
#define to_rtc_device(d) container_of(d, struct rtc_device, dev)
