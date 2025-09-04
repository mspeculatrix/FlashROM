# NOTES

From the data sheet.

## Read

The Read operation of the SST39SF010A/020A/040 is controlled by CE# and OE#, both have to be low for the system to obtain data from the outputs. CE# is used for device selection. When CE# is high, the chip is deselected and only standby power is consumed. OE# is the output control and is used to gate data from the output pins. The data bus is in high impedance state when either CE# or OE# is high. Refer to the Read cycle timing diagram (Figure 5) for further details.

## Byte-Program Operation

The SST39SF010A/020A/040 are programmed on a byte-by-byte basis. Before programming, the sector where the byte exists must be fully erased. The Program operation is accomplished in three steps. The first step is the three-byte load sequence for Software Data Protection. The second step is to load byte address and byte data. During the Byte-Program operation, the addresses are latched on the falling edge of either CE# or WE#, whichever occurs last. The data is latched on the rising edge of either CE# or WE#, whichever occurs first. The third step is the internal Program operation which is initiated after the rising edge of the fourth WE# or CE#, whichever occurs first. The Program operation, once initiated, will be completed, within 20 μs. During the Program operation, the only valid reads are Data Polling and Toggle Bit. During the internal Program operation, the host is free to perform additional tasks. Any commands written during the internal Program operation will be ignored.

## Sector-Erase Operation

The Sector-Erase operation allows the system to erase the device on a sector-by-sector basis. The sector architecture is based on uniform sector size of 4 KByte. The Sector-Erase operation is initiated by executing a six-byte command load sequence for Software Data Protection with Sector-Erase command (30H) and sector address (SA) in the last bus cycle. The sector address is latched on the falling edge of the sixth WE# pulse, while the command (30H) is latched on the rising edge of the sixth WE# pulse. The internal Erase operation begins after the sixth WE# pulse. The End-of-Erase can be determined using either Data# Polling or Toggle Bit methods. Any commands written during the Sector-Erase operation will be ignored.

## Chip-Erase Operation

The SST39SF010A/020A/040 provide Chip-Erase operation, which allows the user to erase the entire memory array to the “1s” state. This is useful when the entire device must be quickly erased.

The Chip-Erase operation is initiated by executing a six-byte Software Data Protection command sequence with Chip-Erase command (10H) with address 5555H in the last byte sequence. The internal Erase operation begins with the rising edge of the sixth WE# or CE#, whichever occurs first. During the internal Erase operation, the only valid read is Toggle Bit or Data# Polling. Any commands written during the Chip-Erase operation will be ignored.
