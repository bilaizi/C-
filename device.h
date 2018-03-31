/**
 * struct bus_type - The bus type of the device
 *
 * @name:	The name of the bus.
 * @dev_name:	Used for subsystems to enumerate devices like ("foo%u", dev->id).
 * @dev_root:	Default device to use as the parent.
 * @bus_groups:	Default attributes of the bus.
 * @dev_groups:	Default attributes of the devices on the bus.
 * @drv_groups: Default attributes of the device drivers on the bus.
 * @match:	Called, perhaps multiple times, whenever a new device or driver is added for this bus. 
 *		It should return a positive value if the given device can be handled by the given driver and zero otherwise. 
 *		It may also return error code if determining that the driver supports the device is not possible. 
 *		In case of -EPROBE_DEFER it will queue the device for deferred probing.
 * @uevent:	Called when a device is added, removed, or a few other things 
                that generate uevents to add the environment variables.
 * @probe:	Called when a new device or driver add to this bus, 
                and callback the specific driver's probe to initial the matched device.
 * @remove:	Called when a device removed from this bus.
 * @shutdown:	Called at shut-down time to quiesce the device.
 * @online:	Called to put the device back online (after offlining it).
 * @offline:	Called to put the device offline for hot-removal. May fail.
 * @suspend:	Called when a device on this bus wants to go to sleep mode.
 * @resume:	Called to bring a device on this bus out of sleep mode.
 * @num_vf:	Called to find out how many virtual functions a device on this bus supports.
 * @pm:		Power management operations of this bus, callback the specific device driver's pm-ops.
 * @iommu_ops:  IOMMU specific operations for this bus, used to attach IOMMU driver implementations to a bus 
                and allow the driver to do bus-specific setup
 * @p:		The private data of the driver core, only the driver core can touch this.
 * @lock_key:	Lock class key for use by the lock validator
 * @force_dma:	Assume devices on this bus should be set up by dma_configure()
 * 		even if DMA capability is not explicitly described by firmware.
 *
 * A bus is a channel between the processor and one or more devices. For the
 * purposes of the device model, all devices are connected via a bus, even if
 * it is an internal, virtual, "platform" bus. Buses can plug into each other.
 * A USB controller is usually a PCI device, for example. The device model
 * represents the actual connections between buses and the devices they control.
 * A bus is represented by the bus_type structure. It contains the name, the
 * default attributes, the bus' methods, PM operations, and the driver core's
 * private data.
 */
struct bus_type {
	const char		*name;
	const char		*dev_name;
	struct device		*dev_root;
	const struct attribute_group **bus_groups;
	const struct attribute_group **dev_groups;
	const struct attribute_group **drv_groups;
	int (*match)(struct device *dev, struct device_driver *drv);
	int (*uevent)(struct device *dev, struct kobj_uevent_env *env);
	int (*probe)(struct device *dev);
	int (*remove)(struct device *dev);
	void (*shutdown)(struct device *dev);
	int (*online)(struct device *dev);
	int (*offline)(struct device *dev);
	int (*suspend)(struct device *dev, pm_message_t state);
	int (*resume)(struct device *dev);
	int (*num_vf)(struct device *dev);
	const struct dev_pm_ops *pm;
	const struct iommu_ops *iommu_ops;
	struct subsys_private *p;
	struct lock_class_key lock_key;
	bool force_dma;
};

/**
 * struct device_driver - The basic device driver structure
 * @name:	Name of the device driver.
 * @bus:	The bus which the device of this driver belongs to.
 * @owner:	The module owner.
 * @mod_name:	Used for built-in modules.
 * @suppress_bind_attrs: Disables bind/unbind via sysfs.
 * @probe_type:	Type of the probe (synchronous or asynchronous) to use.
 * @of_match_table: The open firmware table.
 * @acpi_match_table: The ACPI match table.
 * @probe:	Called to query the existence of a specific device, 
                whether this driver can work with it, and bind the driver to a specific device.
 * @remove:	Called when the device is removed from the system to unbind a device from this driver.
 * @shutdown:	Called at shut-down time to quiesce the device.
 * @suspend:	Called to put the device to sleep mode. Usually to a low power state.
 * @resume:	Called to bring a device from sleep mode.
 * @groups:	Default attributes that get created by the driver core automatically.
 * @pm:		Power management operations of the device which matched this driver.
 * @p:		Driver core's private data, no one other than the driver core can touch this.
 *
 * The device driver-model tracks all of the drivers known to the system.
 * The main reason for this tracking is to enable the driver core to match up drivers with new devices.
 * Once drivers are known objects within the system, however, a number of other things become possible. 
 * Device drivers can export information and configuration variables that are independent of any specific device.
 */
struct device_driver {
	const char		*name;
	struct bus_type		*bus;
	struct module		*owner;
	const char		*mod_name;	/* used for built-in modules */
	bool suppress_bind_attrs;	/* disables bind/unbind via sysfs */
	enum probe_type probe_type;
	const struct of_device_id	*of_match_table;
	const struct acpi_device_id	*acpi_match_table;
	int (*probe) (struct device *dev);
	int (*remove) (struct device *dev);
	void (*shutdown) (struct device *dev);
	int (*suspend) (struct device *dev, pm_message_t state);
	int (*resume) (struct device *dev);
	const struct attribute_group **groups;
	const struct dev_pm_ops *pm;
	int (*coredump) (struct device *dev);
	struct driver_private *p;
};


/**
 * struct device - The basic device structure
 * @parent:	The device's "parent" device, the device to which it is attached.
 * 		In most cases, a parent device is some sort of bus or host
 * 		controller. If parent is NULL, the device, is a top-level device,
 * 		which is not usually what you want.
 * @p:		Holds the private data of the driver core portions of the device.
 * 		See the comment of the struct device_private for detail.
 * @kobj:	A top-level, abstract class from which other classes are derived.
 * @init_name:	Initial name of the device.
 * @type:	The type of device. This identifies the device type and carries type-specific information.
 * @mutex:	Mutex to synchronize calls to its driver.
 * @bus:	Type of bus device is on.
 * @driver:	Which driver has allocated this
 * @platform_data: Platform data specific to the device.
 * 		Example: For devices on custom boards, as typical of embedded
 * 		and SOC based hardware, Linux often uses platform_data to point
 * 		to board-specific structures describing devices and how they
 * 		are wired.  That can include what ports are available, chip
 * 		variants, which GPIO pins act in what additional roles, and so
 * 		on.  This shrinks the "Board Support Packages" (BSPs) and
 * 		minimizes board-specific #ifdefs in drivers.
 * @driver_data: Private pointer for driver specific info.
 * @links:	Links to suppliers and consumers of this device.
 * @power:	For device power management.
 *		See Documentation/driver-api/pm/devices.rst for details.
 * @pm_domain:	Provide callbacks that are executed during system suspend,
 * 		hibernation, system resume and during runtime PM transitions
 * 		along with subsystem-level and driver-level callbacks.
 * @pins:	For device pin management.
 *		See Documentation/driver-api/pinctl.rst for details.
 * @msi_list:	Hosts MSI descriptors
 * @msi_domain: The generic MSI domain this device is using.
 * @numa_node:	NUMA node this device is close to.
 * @dma_ops:    DMA mapping operations for this device.
 * @dma_mask:	Dma mask (if dma'ble device).
 * @coherent_dma_mask: Like dma_mask, but for alloc_coherent mapping as not all
 * 		hardware supports 64-bit addresses for consistent allocations
 * 		such descriptors.
 * @dma_pfn_offset: offset of DMA memory range relatively of RAM
 * @dma_parms:	A low level driver may set these to teach IOMMU code about
 * 		segment limitations.
 * @dma_pools:	Dma pools (if dma'ble device).
 * @dma_mem:	Internal for coherent mem override.
 * @cma_area:	Contiguous memory area for dma allocations
 * @archdata:	For arch-specific additions.
 * @of_node:	Associated device tree node.
 * @fwnode:	Associated device node supplied by platform firmware.
 * @devt:	For creating the sysfs "dev".
 * @id:		device instance
 * @devres_lock: Spinlock to protect the resource of the device.
 * @devres_head: The resources list of the device.
 * @knode_class: The node used to add the device to the class list.
 * @class:	The class of the device.
 * @groups:	Optional attribute groups.
 * @release:	Callback to free the device after all references have
 * 		gone away. This should be set by the allocator of the
 * 		device (i.e. the bus driver that discovered the device).
 * @iommu_group: IOMMU group the device belongs to.
 * @iommu_fwspec: IOMMU-specific properties supplied by firmware.
 *
 * @offline_disabled: If set, the device is permanently online.
 * @offline:	Set after successful invocation of bus type's .offline().
 * @of_node_reused: Set if the device-tree node is shared with an ancestor device.
 *
 * At the lowest level, every device in a Linux system is represented by an
 * instance of struct device. The device structure contains the information
 * that the device model core needs to model the system. Most subsystems,
 * however, track additional information about the devices they host. As a
 * result, it is rare for devices to be represented by bare device structures;
 * instead, that structure, like kobject structures, is usually embedded within
 * a higher-level representation of the device.
 */
struct device {
	struct device		*parent;
	struct device_private	*p;
	struct kobject          kobj;
	const char		*init_name; /* initial name of the device */
	const struct device_type *type;
	struct mutex		mutex;	/* mutex to synchronize calls to its driver. */
	struct bus_type	        *bus;		/* type of bus device is on */
	struct device_driver *driver;	/* which driver has allocated this device */
	void		*platform_data;	/* Platform specific data, device core doesn't touch it */
	void		*driver_data;	/* Driver data, set and get with dev_set/get_drvdata */
	struct dev_links_info	links;
	struct dev_pm_info	power;
	struct dev_pm_domain	*pm_domain;
#ifdef CONFIG_GENERIC_MSI_IRQ_DOMAIN
	struct irq_domain	*msi_domain;
#endif
#ifdef CONFIG_PINCTRL
	struct dev_pin_info	*pins;
#endif
#ifdef CONFIG_GENERIC_MSI_IRQ
	struct list_head	msi_list;
#endif
#ifdef CONFIG_NUMA
	int		numa_node;	/* NUMA node this device is close to */
#endif
	const struct dma_map_ops *dma_ops;
	u64		*dma_mask;	/* dma mask (if dma'able device) */
	u64		coherent_dma_mask;/* Like dma_mask, but for
					     alloc_coherent mappings as
					     not all hardware supports
					     64 bit addresses for consistent
					     allocations such descriptors. */
	unsigned long	dma_pfn_offset;
	struct device_dma_parameters *dma_parms;
	struct list_head	dma_pools;	/* dma pools (if dma'ble) */
	struct dma_coherent_mem	*dma_mem; /* internal for coherent mem override */
#ifdef CONFIG_DMA_CMA
	struct cma *cma_area;		/* contiguous memory area for dma allocations */
#endif
	/* arch specific additions */
	struct dev_archdata	archdata;
	struct device_node	*of_node; /* associated device tree node */
	struct fwnode_handle	*fwnode; /* firmware device node */
	dev_t			devt;	/* dev_t, creates the sysfs "dev" */
	u32			id;	/* device instance */
	spinlock_t		devres_lock;
	struct list_head	devres_head;
	struct klist_node	knode_class;
	struct class		*class;
	const struct attribute_group **groups;	/* optional groups */
	void	(*release)(struct device *dev);
	struct iommu_group	*iommu_group;
	struct iommu_fwspec	*iommu_fwspec;
	bool			offline_disabled:1;
	bool			offline:1;
	bool			of_node_reused:1;
};

/* interface for exporting device attributes */
struct device_attribute {
	struct attribute	attr;
	ssize_t (*show)(struct device *dev, struct device_attribute *attr, char *buf);
	ssize_t (*store)(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
};
