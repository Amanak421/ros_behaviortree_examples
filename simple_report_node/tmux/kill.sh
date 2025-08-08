#!/bin/bash

# Absolute path to this script. /home/user/bin/foo.sh
SCRIPT=$(readlink -f $0)
# Absolute path this script is in. /home/user/bin
SCRIPTPATH=$(dirname $SCRIPT)
cd "$SCRIPTPATH"

export TMUX_SOCKET_NAME=ros_bt

# just attach to the session
tmux -L $TMUX_SOCKET_NAME new-window -n kill-tmux
tmux -L $TMUX_SOCKET_NAME send-keys "sleep 1; tmux list-panes -s -F \"#{pane_pid} #{window_name}\" | grep -v "kill-tmux" | cut -d\" \" -f1 | while read in; do killProcessRecursive \$in; done; exit" ENTER