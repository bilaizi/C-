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
