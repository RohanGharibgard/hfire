### HFire-1.2.0 MacOS

## Instructions to compile
navigate to `HFire-1.2.0/src` directory and run the following command:<br>
`clang -std=c89 -O3 -DUSING_UNIX -DUSING_GD hfire/*.c hfirelib/*.c gd/*.c  -Ihfirelib -Igd -Ifuture -Icalcros -Iroth1972cmp -o hfire-arm`
