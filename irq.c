typedef irqreturn_t (*irq_handler_t)(int, void*);
int request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags, const char* name, void* dev)
int request_threaded_irq(unsigned int irq, irq_handler_t handler, irq_handler_t thread_fn, unsigned long flags, const char *name, void *dev);
