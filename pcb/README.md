# Mouse PCB
Repository for the mouse circuit board schematics.

## TODO
- Adjust capacitance of all capacitors
- Add external pull-ups (the right value)
- Debounce buttons (use internal schmitt)
- Simulate debouncing using <something>spice
- Simulate capacitance using pspice (LDO has a sim model for only pspice)
- Add comments
- Research USB_REENUM (if it's even needed - stm32f042 should handle this internally) and add USB ESD protection
