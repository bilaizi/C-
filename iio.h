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
	bool (*validate_scan_mask)(struct iio_dev *indio_dev, const unsigned long *scan_mask);
};

/**
 * struct iio_info - constant information about device
 * @event_attrs:	event control attributes
 * @attrs:		general purpose device attributes
 * @read_raw:		function to request a value from the device.
 *			mask specifies which value. Note 0 means a reading of
 *			the channel in question.  Return value will specify the
 *			type of value returned by the device. val and val2 will
 *			contain the elements making up the returned value.
 * @read_raw_multi:	function to return values from the device.
 *			mask specifies which value. Note 0 means a reading of
 *			the channel in question.  Return value will specify the
 *			type of value returned by the device. vals pointer
 *			contain the elements making up the returned value.
 *			max_len specifies maximum number of elements
 *			vals pointer can contain. val_len is used to return
 *			length of valid elements in vals.
 * @read_avail:		function to return the available values from the device.
 *			mask specifies which value. Note 0 means the available
 *			values for the channel in question.  Return value
 *			specifies if a IIO_AVAIL_LIST or a IIO_AVAIL_RANGE is
 *			returned in vals. The type of the vals are returned in
 *			type and the number of vals is returned in length. For
 *			ranges, there are always three vals returned; min, step
 *			and max. For lists, all possible values are enumerated.
 * @write_raw:		function to write a value to the device.
 *			Parameters are the same as for read_raw.
 * @write_raw_get_fmt:	callback function to query the expected
 *			format/precision. If not set by the driver, write_raw
 *			returns IIO_VAL_INT_PLUS_MICRO.
 * @read_event_config:	find out if the event is enabled.
 * @write_event_config:	set if the event is enabled.
 * @read_event_value:	read a configuration value associated with the event.
 * @write_event_value:	write a configuration value for the event.
 * @validate_trigger:	function to validate the trigger when the
 *			current trigger gets changed.
 * @update_scan_mode:	function to configure device and scan buffer when
 *			channels have changed
 * @debugfs_reg_access:	function to read or write register value of device
 * @of_xlate:		function pointer to obtain channel specifier index.
 *			When #iio-cells is greater than '0', the driver could
 *			provide a custom of_xlate function that reads the
 *			*args* and returns the appropriate index in registered
 *			IIO channels array.
 * @hwfifo_set_watermark: function pointer to set the current hardware
 *			fifo watermark level; see hwfifo_* entries in
 *			Documentation/ABI/testing/sysfs-bus-iio for details on
 *			how the hardware fifo operates
 * @hwfifo_flush_to_buffer: function pointer to flush the samples stored
 *			in the hardware fifo to the device buffer. The driver
 *			should not flush more than count samples. The function
 *			must return the number of samples flushed, 0 if no
 *			samples were flushed or a negative integer if no samples
 *			were flushed and there was an error.
 **/
struct iio_info {
	const struct attribute_group	*event_attrs;
	const struct attribute_group	*attrs;

	int (*read_raw)(struct iio_dev *indio_dev, struct iio_chan_spec const *chan, int *val, int *val2, long mask);
	int (*read_raw_multi)(struct iio_dev *indio_dev, struct iio_chan_spec const *chan, int max_len, int *vals, int *val_len, long mask);
	int (*read_avail)(struct iio_dev *indio_dev, struct iio_chan_spec const *chan, const int **vals, int *type, int *length, long mask);
	int (*write_raw)(struct iio_dev *indio_dev, struct iio_chan_spec const *chan, int val, int val2, long mask);
	int (*write_raw_get_fmt)(struct iio_dev *indio_dev,  struct iio_chan_spec const *chan, long mask);
	int (*read_event_config)(struct iio_dev *indio_dev, const struct iio_chan_spec *chan, enum iio_event_type type, enum iio_event_direction dir);
	int (*write_event_config)(struct iio_dev *indio_dev,
				  const struct iio_chan_spec *chan,
				  enum iio_event_type type,
				  enum iio_event_direction dir,
				  int state);

	int (*read_event_value)(struct iio_dev *indio_dev,
				const struct iio_chan_spec *chan,
				enum iio_event_type type,
				enum iio_event_direction dir,
				enum iio_event_info info, int *val, int *val2);

	int (*write_event_value)(struct iio_dev *indio_dev,
				 const struct iio_chan_spec *chan,
				 enum iio_event_type type,
				 enum iio_event_direction dir,
				 enum iio_event_info info, int val, int val2);

	int (*validate_trigger)(struct iio_dev *indio_dev, struct iio_trigger *trig);
	int (*update_scan_mode)(struct iio_dev *indio_dev, const unsigned long *scan_mask);
	int (*debugfs_reg_access)(struct iio_dev *indio_dev, unsigned reg, unsigned writeval, unsigned *readval);
	int (*of_xlate)(struct iio_dev *indio_dev, const struct of_phandle_args *iiospec);
	int (*hwfifo_set_watermark)(struct iio_dev *indio_dev, unsigned val);
	int (*hwfifo_flush_to_buffer)(struct iio_dev *indio_dev, unsigned count);
};
