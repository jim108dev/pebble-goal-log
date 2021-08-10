
# Connection
COMM_PATH := ~/github/pebble-pc-communication-example/host_python
VENV_PATH := $(COMM_PATH)/venv/bin
PYTHON2 := $(VENV_PATH)/python

DOWNLOAD := $(PYTHON2) $(COMM_PATH)/pebble_download.py
UPLOAD := $(PYTHON2) $(COMM_PATH)/pebble_upload.py

CONN_PATH := ./connection
CONN_CONF_WATCH := $(CONN_PATH)/config_watch.ini
CONN_CONF_EMU := $(CONN_PATH)/config_emu.ini

# Evaluation
EVAL_PATH := ./evaluation
VENV_PATH := $(EVAL_PATH)/venv/bin
PYTHON3 := $(VENV_PATH)/python

ALIGN_LOG := $(PYTHON3) $(EVAL_PATH)/align_log.py
PREPARE := $(PYTHON3) $(EVAL_PATH)/prepare_next_session.py
MERGE := $(PYTHON3) $(EVAL_PATH)/merge_feedback.py
EVAL_CONF := $(EVAL_PATH)/config.ini

all:
	@echo "Please choose explicitly a target."

# Pebble
start_emu:
	cd pebble_app && pebble build && pebble install --logs --emulator aplite

start_watch:
	cd pebble_app && pebble build && pebble install --serial /dev/rfcomm0

# Connection

upload_emu:
	$(UPLOAD) $(CONN_CONF_EMU)

upload_watch:
	$(UPLOAD) $(CONN_CONF_WATCH)

download_emu:
	$(DOWNLOAD) $(CONN_CONF_EMU)

download_watch:
	$(DOWNLOAD) $(CONN_CONF_WATCH)

# Evaluation
align_log:
	$(ALIGN_LOG) $(EVAL_CONF)

prepare_next_session:
	$(PREPARE) $(EVAL_CONF)

merge_feedback:
	$(MERGE) $(EVAL_CONF)