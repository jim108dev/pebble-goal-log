# Pebble Goal Log

This is a learning project in order to create a Pebble app which logs countable goals on a daily basis. This can be fitness goals like lifting weights or logging the time spend on the smartphone.

The Pebble app works without a smartphone. The data is exchanged with a pc. Necessary functions for this exchange can be found in [pebble-pc-communication-example](https://github.com/jim108dev/pebble-pc-communication-example).

## Screenshots

## Install

1. Install [pebble-pc-communication-example](https://github.com/jim108dev/pebble-pc-communication-example).

1. Install repository 's software

    ```sh
    git clone https://github.com/jim108dev/pebble-goal-log.git
    ```

1. Create a Python 3 environment

    ```sh
    cd evaluation/
    virtualenv --python=/usr/bin/python3.7 venv
    bash
    source venv/bin/activate
    pip install -r requirements.txt
    deactivate
    exit
    ```

1. Adjust paths in `Makefile` accordingly.

1. Setup goals in `data/reference.csv`. An example is given in `data/reference.example.csv`.

1. Align the log file

    ```sh
    make align_log
    ```

## Usage

### On the Emulator

```sh
# Create `upload.csv`
make prepare_next_session

make install_emu

make upload_emu

# Gather feedback on the device

make download_emu

# Merge current feedback with history
make merge_feedback
```

### On the Watch

```sh
# Create `upload.csv`
make prepare_next_session

make install_watch

make upload_watch

# Gather feedback on the device

make download_watch

# Merge current feedback with history
make merge_feedback
```

## Limitations

1. The persistent storage on the pebble is limit to 4k. Therefore ids are chosen to be limited to 20 characters, texts to 78 characters and the number of questions/answers is 20 in one session.
