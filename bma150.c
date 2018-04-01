struct bma150_data {
	struct i2c_client *client;
	struct input_polled_dev *input_polled;
	struct input_dev *input;
	u8 mode;
};

/*
 * The settings for the given range, bandwidth and interrupt features
 * are stated and verified by Bosch Sensortec where they are configured
 * to provide a generic sensitivity performance.
 */
static const struct bma150_cfg default_cfg = {
	.any_motion_int = 1,
	.hg_int = 1,
	.lg_int = 1,
	.any_motion_dur = 0,
	.any_motion_thres = 0,
	.hg_hyst = 0,
	.hg_dur = 150,
	.hg_thres = 160,
	.lg_hyst = 0,
	.lg_dur = 150,
	.lg_thres = 20,
	.range = BMA150_RANGE_2G,
	.bandwidth = BMA150_BW_50HZ
};

static void bma150_report_xyz(struct bma150_data *bma150)
{
	u8 data[BMA150_XYZ_DATA_SIZE];
	s16 x, y, z;
	s32 ret;
	ret = i2c_smbus_read_i2c_block_data(bma150->client, BMA150_ACC_X_LSB_REG, BMA150_XYZ_DATA_SIZE, data);
	if (ret != BMA150_XYZ_DATA_SIZE)
		return;
	x = ((0xc0 & data[0]) >> 6) | (data[1] << 2);
	y = ((0xc0 & data[2]) >> 6) | (data[3] << 2);
	z = ((0xc0 & data[4]) >> 6) | (data[5] << 2);
	x = sign_extend32(x, 9);
	y = sign_extend32(y, 9);
	z = sign_extend32(z, 9);
	input_report_abs(bma150->input, ABS_X, x);
	input_report_abs(bma150->input, ABS_Y, y);
	input_report_abs(bma150->input, ABS_Z, z);
	input_sync(bma150->input);
}

static void bma150_init_input_device(struct bma150_data *bma150, struct input_dev *idev)
{
	idev->name = BMA150_DRIVER;
	idev->phys = BMA150_DRIVER "/input0";
	idev->id.bustype = BUS_I2C;
	idev->dev.parent = &bma150->client->dev;
	idev->evbit[0] = BIT_MASK(EV_ABS);
	input_set_abs_params(idev, ABS_X, ABSMIN_ACC_VAL, ABSMAX_ACC_VAL, 0, 0);
	input_set_abs_params(idev, ABS_Y, ABSMIN_ACC_VAL, ABSMAX_ACC_VAL, 0, 0);
	input_set_abs_params(idev, ABS_Z, ABSMIN_ACC_VAL, ABSMAX_ACC_VAL, 0, 0);
}

static int bma150_register_input_device(struct bma150_data *bma150)
{
	struct input_dev *idev;
	int error;
	idev = input_allocate_device();
	if (!idev)
		return -ENOMEM;
	bma150_init_input_device(bma150, idev);
	idev->open = bma150_irq_open;
	idev->close = bma150_irq_close;
	input_set_drvdata(idev, bma150);
	error = input_register_device(idev);
	if (error) {
		input_free_device(idev);
		return error;
	}
	bma150->input = idev;
	return 0;
}

static int bma150_register_polled_device(struct bma150_data *bma150)
{
	struct input_polled_dev *ipoll_dev;
	int error;
	ipoll_dev = input_allocate_polled_device();
	if (!ipoll_dev)
		return -ENOMEM;
	ipoll_dev->private = bma150;
	ipoll_dev->open = bma150_poll_open;
	ipoll_dev->close = bma150_poll_close;
	ipoll_dev->poll = bma150_poll;
	ipoll_dev->poll_interval = BMA150_POLL_INTERVAL;
	ipoll_dev->poll_interval_min = BMA150_POLL_MIN;
	ipoll_dev->poll_interval_max = BMA150_POLL_MAX;
	bma150_init_input_device(bma150, ipoll_dev->input);
	error = input_register_polled_device(ipoll_dev);
	if (error) {
		input_free_polled_device(ipoll_dev);
		return error;
	}
	bma150->input_polled = ipoll_dev;
	bma150->input = ipoll_dev->input;
	return 0;
}
