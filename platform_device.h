struct platform_device {
        const char      *name;
	int		id;
	bool		id_auto;
	struct device	dev;
	u32		num_resources;
	struct resource *resource;
	const struct platform_device_id *id_entry;
	char  *driver_override; /* Driver name to force a match */
	/* MFD cell pointer */
	struct mfd_cell *mfd_cell;
	/* arch specific additions */
	struct pdev_archdata	archdata;
};
struct platform_driver {
	int (*probe)(struct platform_device *);
	int (*remove)(struct platform_device *);
	void (*shutdown)(struct platform_device *);
	int (*suspend)(struct platform_device *, pm_message_t state);
	int (*resume)(struct platform_device *);
	struct device_driver driver;
	const struct platform_device_id *id_table;
	bool prevent_deferred_probe;
};
extern struct resource *platform_get_resource(struct platform_device *, unsigned int, unsigned int);
extern int platform_get_irq(struct platform_device *, unsigned int);
