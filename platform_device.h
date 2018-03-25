struct platform_device {
    const char	*name;
	  int		id;
	  bool		id_auto;
	  struct device	dev;
	  u32		num_resources;
	  struct resource	*resource;
	  const struct platform_device_id	*id_entry;
	  char *driver_override; /* Driver name to force a match */
	  /* MFD cell pointer */
	  struct mfd_cell *mfd_cell;
	  /* arch specific additions */
	  struct pdev_archdata	archdata;
};
