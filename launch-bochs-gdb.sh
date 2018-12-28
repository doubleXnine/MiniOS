# Run bochs, with gdb support
# You should have compiled bochs with gdb-stub enabled, and
# have added gdbstub option in bochsrc
# Added by xw, 18/12

gnome-terminal -x bash -c "echo 'type in gdb: target remote :2345';echo '';gdb -s kernel.gdb.bin" &
~/os/bochs/bochs-2.6.9-gdb/bochs -f bochsrc-gdb
