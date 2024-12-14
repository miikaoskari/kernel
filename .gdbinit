# set correct target
target extended-remote :3333

# load kernel8.elf
load kernel8.elf

# load svd file
dashboard svd load dbg/svd/bcm2711_lpa.svd

# load gics
dashboard svd add GIC_DIST GICD_ISENABLER
dashboard svd add GIC_DIST GICD_ITARGETSR

dashboard svd add GIC_CPU GICC_IAR
dashboard svd add GIC_CPU GICC_EOIR

# set breakpoints
break kmain
break handle_irq

