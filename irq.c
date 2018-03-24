typedef irqreturn_t (*irq_handler_t)(int, void*);
int request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags, const char* name, void* dev)
