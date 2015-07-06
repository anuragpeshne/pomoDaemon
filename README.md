# pomoDaemon
Pomodoro Daemon for Console/Tmux

## How-To
- Compile and execute `pomoDaemon.c`.
- Compile `pomoClient.c` and add the binary to `$PATH`.
- Get the timer status by executing `pomoClient`; add it to tmux status bar by appending `set -g status-right "#(pomoClient)"`.
- Reset the timer by sending `USR1` signal to `pomoDaemon`. Example: `ps -ae | grep "\d pomoDaemon" | awk '{print $1}' | xargs kill -USR1`.
