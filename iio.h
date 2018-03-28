/**
 * struct iio_dev - industrial I/O device
 * @id:			[INTERN] used to identify device internally
 * @driver_module:	[INTERN] used to make it harder to undercut users
 * @modes:		[DRIVER] operating modes supported by device
 * @currentmode:	[DRIVER] current operating mode
 * @dev:		[DRIVER] device structure, should be assigned a parent and owner
 * @event_interface:	[INTERN] event chrdevs associated with interrupt lines
 * @buffer:		[DRIVER] any buffer present
 * @buffer_list:	[INTERN] list of all buffers currently attached
 * @scan_bytes:		[INTERN] num bytes captured to be fed to buffer demux
 * @mlock:		[DRIVER] lock used to prevent simultaneous device state changes
 * @available_scan_masks: [DRIVER] optional array of allowed bitmasks
 * @masklength:		[INTERN] the length of the mask established from channels
 * @active_scan_mask:	[INTERN] union of all scan masks requested by buffers
 * @scan_timestamp:	[INTERN] set if any buffers have requested timestamp
 * @scan_index_timestamp:[INTERN] cache of the index to the timestamp
 * @trig:		[INTERN] current device trigger (buffer modes)
 * @trig_readonly:	[INTERN] mark the current trigger immutable
 * @pollfunc:		[DRIVER] function run on trigger being received
 * @pollfunc_event:	[DRIVER] function run on events trigger being received
 * @channels:		[DRIVER] channel specification structure table
 * @num_channels:	[DRIVER] number of channels specified in @channels.
 * @channel_attr_list:	[INTERN] keep track of automatically created channel attributes
 * @chan_attr_group:	[INTERN] group for all attrs in base directory
 * @name:		[DRIVER] name of the device.
 * @info:		[DRIVER] callbacks and constant info from driver
 * @clock_id:		[INTERN] timestamping clock posix identifier
 * @info_exist_lock:	[INTERN] lock to prevent use during removal
 * @setup_ops:		[DRIVER] callbacks to call before and after buffer enable/disable
 * @chrdev:		[INTERN] associated character device
 * @groups:		[INTERN] attribute groups
 * @groupcounter:	[INTERN] index of next attribute group
 * @flags:		[INTERN] file ops related flags including busy flag.
 * @debugfs_dentry:	[INTERN] device specific debugfs dentry.
 * @cached_reg_addr:	[INTERN] cached register address for debugfs reads.
 */
struct iio_dev {
	int				id;
	struct module			*driver_module;

	int				modes;
	int				currentmode;
	struct device			dev;

	struct iio_event_interface	*event_interface;

	struct iio_buffer		*buffer;
	struct list_head		buffer_list;
	int				scan_bytes;
	struct mutex			mlock;

	const unsigned long		*available_scan_masks;
	unsigned			masklength;
	const unsigned long		*active_scan_mask;
	bool				scan_timestamp;
	unsigned			scan_index_timestamp;
	struct iio_trigger		*trig;
	bool				trig_readonly;
	struct iio_poll_func		*pollfunc;
	struct iio_poll_func		*pollfunc_event;

	struct iio_chan_spec const	*channels;
	int				num_channels;

	struct list_head		channel_attr_list;
	struct attribute_group		chan_attr_group;
	const char			*name;
	const struct iio_info		*info;
	clockid_t			clock_id;
	struct mutex			info_exist_lock;
	const struct iio_buffer_setup_ops	*setup_ops;
	struct cdev			chrdev;
#define IIO_MAX_GROUPS 6
	const struct attribute_group	*groups[IIO_MAX_GROUPS + 1];
	int				groupcounter;

	unsigned long			flags;
#if defined(CONFIG_DEBUG_FS)
	struct dentry			*debugfs_dentry;
	unsigned			cached_reg_addr;
#endif
};

/**
 * struct iio_buffer_setup_ops - buffer setup related callbacks
 * @preenable:		[DRIVER] function to run prior to marking buffer enabled
 * @postenable:		[DRIVER] function to run after marking buffer enabled
 * @predisable:		[DRIVER] function to run prior to marking buffer disabled
 * @postdisable:	[DRIVER] function to run after marking buffer disabled
 * @validate_scan_mask: [DRIVER] function callback to check whether a given scan mask is valid for the device.
 */
struct iio_buffer_setup_ops {
	int (*preenable)(struct iio_dev *);
	int (*postenable)(struct iio_dev *);
	int (*predisable)(struct iio_dev *);
	int (*postdisable)(struct iio_dev *);
	bool (*validate_scan_mask)(struct iio_dev *indio_dev, onst unsigned long *scan_mask);
};
