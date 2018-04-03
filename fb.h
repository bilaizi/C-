
// https://github.com/torvalds/linux/blob/master/include/uapi/linux/fb.h
struct fb_fix_screeninfo {
	char id[16];                	/* identification string eg "TT Builtin" */
	unsigned long smem_start;	/* Start of frame buffer mem(physical address) */
	__u32 smem_len;			/* Length of frame buffer mem 	*/
	__u32 type;			/* see FB_TYPE_*		*/
	__u32 type_aux;			/* Interleave for interleaved Planes */
	__u32 visual;			/* see FB_VISUAL_*		*/ 
	__u16 xpanstep;			/* zero if no hardware panning  */
	__u16 ypanstep;			/* zero if no hardware panning  */
	__u16 ywrapstep;		/* zero if no hardware ywrap    */
	__u32 line_length;		/* length of a line in bytes    */
	unsigned long mmio_start;	/* Start of Memory Mapped I/O(physical address) */
	__u32 mmio_len;			/* Length of Memory Mapped I/O  */
	__u32 accel;			/* Indicate to driver which specific chip/card we have	*/
	__u16 capabilities;		/* see FB_CAP_*			*/
	__u16 reserved[2];		/* Reserved for future compatibility */
};
struct fb_var_screeninfo {
	__u32 xres;			/* visible resolution */
	__u32 yres;
	__u32 xres_virtual;		/* virtual resolution */
	__u32 yres_virtual;
	__u32 xoffset;			/* offset from virtual to visible resolution */
	__u32 yoffset;			
	__u32 bits_per_pixel;		/* guess what */
	__u32 grayscale;		/* 0 = color, 1 = grayscale, >1 = FOURCC*/
	struct fb_bitfield red;		/* bitfield in fb mem if true color, */
	struct fb_bitfield green;	/* else only length is significant */
	struct fb_bitfield blue;
	struct fb_bitfield transp;	/* transparency	*/	
	__u32 nonstd;			/* != 0 Non standard pixel format */
	__u32 activate;			/* see FB_ACTIVATE_* */
	__u32 height;			/* height of picture in mm */
	__u32 width;			/* width of picture in mm  */
	__u32 accel_flags;		/* (OBSOLETE) see fb_info.flags */
	/* Timing: All values in pixclocks, except pixclock (of course) */
	__u32 pixclock;			/* pixel clock in ps (pico seconds) */
	__u32 left_margin;		/* time from sync to picture */
	__u32 right_margin;		/* time from picture to sync */
	__u32 upper_margin;		/* time from sync to picture */
	__u32 lower_margin;
	__u32 hsync_len;		/* length of horizontal sync */
	__u32 vsync_len;		/* length of vertical sync */
	__u32 sync;			/* see FB_SYNC_* */
	__u32 vmode;			/* see FB_VMODE_* */
	__u32 rotate;			/* angle we rotate counter clockwise */
	__u32 colorspace;		/* colorspace for FOURCC-based modes */
	__u32 reserved[4];		/* Reserved for future compatibility */
};
struct fb_cmap {
	__u32 start;			/* First entry	*/
	__u32 len;			/* Number of entries */
	__u16 *red;			/* Red values	*/
	__u16 *green;
	__u16 *blue;
	__u16 *transp;			/* transparency, can be NULL */
};


struct fb_info {
	atomic_t count;
	int node;
	int flags;
	/*
	 * -1 by default, set to a FB_ROTATE_* value by the driver, if it knows
	 * a lcd is not mounted upright and fbcon should rotate to compensate.
	 */
	int fbcon_rotate_hint;
	struct mutex lock;		/* Lock for open/release/ioctl funcs */
	struct mutex mm_lock;		/* Lock for fb_mmap and smem_* fields */
	struct fb_var_screeninfo var;	/* Current var */
	struct fb_fix_screeninfo fix;	/* Current fix */
	struct fb_monspecs monspecs;	/* Current Monitor specs */
	struct work_struct queue;	/* Framebuffer event queue */
	struct fb_pixmap pixmap;	/* Image hardware mapper */
	struct fb_pixmap sprite;	/* Cursor hardware mapper */
	struct fb_cmap cmap;		/* Current cmap */
	struct list_head modelist;      /* mode list */
	struct fb_videomode *mode;	/* current mode */
#ifdef CONFIG_FB_BACKLIGHT
	/* assigned backlight device, set before framebuffer registration, remove after unregister */
	struct backlight_device *bl_dev;
	/* Backlight level curve */
	struct mutex bl_curve_mutex;	
	u8 bl_curve[FB_BACKLIGHT_LEVELS];
#endif
#ifdef CONFIG_FB_DEFERRED_IO
	struct delayed_work deferred_work;
	struct fb_deferred_io *fbdefio;
#endif
	struct fb_ops *fbops;
	struct device *device;		/* This is the parent */
	struct device *dev;		/* This is this fb device */
	int class_flag;                    /* private sysfs flags */
#ifdef CONFIG_FB_TILEBLITTING
	struct fb_tile_ops *tileops;    /* Tile Blitting */
#endif
	union {
		char __iomem *screen_base;	/* Virtual address */
		char *screen_buffer;
	};
	unsigned long screen_size;	/* Amount of ioremapped VRAM or 0 */ 
	void *pseudo_palette;		/* Fake palette of 16 colors */ 
#define FBINFO_STATE_RUNNING	0
#define FBINFO_STATE_SUSPENDED	1
	u32 state;			/* Hardware state i.e suspend */
	void *fbcon_par;                /* fbcon use-only private area */
	/* From here on everything is device dependent */
	void *par;
	/* we need the PCI or similar aperture base/size not
	   smem_start/size as smem_start may just be an object
	   allocated inside the aperture so may not actually overlap */
	struct apertures_struct {
		unsigned int count;
		struct aperture {
			resource_size_t base;
			resource_size_t size;
		} ranges[0];
	} *apertures;
	bool skip_vt_switch; /* no VT switch on suspend/resume required */
};
/*
 * Frame buffer operations
 *
 * LOCKING NOTE: those functions must _ALL_ be called with the console semaphore held, 
 * this is the only suitable locking mechanism we have in 2.6. Some may be called at interrupt time at this point though.
 *
 * The exception to this is the debug related hooks.  Putting the fb into a debug state (e.g. flipping to the kernel console)  
 * and restoring it must be done in a lock-free manner, so low level drivers should keep track of the initial console (if applicable) 
 * and may need to perform direct, unlocked hardware writes in these hooks.
 */
struct fb_ops {
	/* open/release and usage marking */
	struct module *owner;
	int (*fb_open)(struct fb_info *info, int user);
	int (*fb_release)(struct fb_info *info, int user);
	/* For framebuffers with strange non linear layouts or that do not  work with normal memory mapped access */
	ssize_t (*fb_read)(struct fb_info *info, char __user *buf, size_t count, loff_t *ppos);
	ssize_t (*fb_write)(struct fb_info *info, const char __user *buf, size_t count, loff_t *ppos);
	/* checks var and eventually tweaks it to something supported, DO NOT MODIFY PAR */
	int (*fb_check_var)(struct fb_var_screeninfo *var, struct fb_info *info);
	/* set the video mode according to info->var */
	int (*fb_set_par)(struct fb_info *info);
	/* set color register */
	int (*fb_setcolreg)(unsigned regno, unsigned red, unsigned green, unsigned blue, unsigned transp, struct fb_info *info);
	/* set color registers in batch */
	int (*fb_setcmap)(struct fb_cmap *cmap, struct fb_info *info);
	/* blank display */
	int (*fb_blank)(int blank, struct fb_info *info);
	/* pan display */
	int (*fb_pan_display)(struct fb_var_screeninfo *var, struct fb_info *info);
	/* Draws a rectangle */
	void (*fb_fillrect) (struct fb_info *info, const struct fb_fillrect *rect);
	/* Copy data from area to another */
	void (*fb_copyarea) (struct fb_info *info, const struct fb_copyarea *region);
	/* Draws a image to the display */
	void (*fb_imageblit) (struct fb_info *info, const struct fb_image *image);
	/* Draws cursor */
	int (*fb_cursor) (struct fb_info *info, struct fb_cursor *cursor);
	/* wait for blit idle, optional */
	int (*fb_sync)(struct fb_info *info);
	/* perform fb specific ioctl (optional) */
	int (*fb_ioctl)(struct fb_info *info, unsigned int cmd, unsigned long arg);
	/* Handle 32bit compat ioctl (optional) */
	int (*fb_compat_ioctl)(struct fb_info *info, unsigned cmd, unsigned long arg);
	/* perform fb specific mmap */
	int (*fb_mmap)(struct fb_info *info, struct vm_area_struct *vma);
	/* get capability given var */
	void (*fb_get_caps)(struct fb_info *info, struct fb_blit_caps *caps, struct fb_var_screeninfo *var);
	/* teardown any resources to do with this framebuffer */
	void (*fb_destroy)(struct fb_info *info);
	/* called at KDB enter and leave time to prepare the console */
	int (*fb_debug_enter)(struct fb_info *info);
	int (*fb_debug_leave)(struct fb_info *info);
};
