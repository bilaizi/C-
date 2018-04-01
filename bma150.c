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

